#define ANIMATIONS_PER_SECOND 100.0f
#include "vtfw_mesh.h"

//Constants (copied as is from sample)

// Grid sizes (in vertices)
#define X_SIZE_LOG2		6
#define Y_SIZE_LOG2		6
#define Z_SIZE_LOG2		6
#define TOTAL_POINTS	(1<<(X_SIZE_LOG2 + Y_SIZE_LOG2 + Z_SIZE_LOG2))
#define CELLS_COUNT		(((1<<X_SIZE_LOG2) - 1) * ((1<<Y_SIZE_LOG2) - 1) * ((1<<Z_SIZE_LOG2) - 1))

#define SWIZZLE	1

#define MAKE_INDEX(x, y, z, sizeLog2)	(int)((x) | ((y) << sizeLog2[0]) | ((z) << (sizeLog2[0] + sizeLog2[1])))

//--------------------------------------------------------------------------------------
// Fills pPos with x, y, z de-swizzled from index with bitsizes in sizeLog2
//
//  Traversing the grid in a swizzled fashion improves locality of reference,
// and this is very beneficial when sampling a texture.
//--------------------------------------------------------------------------------------
void UnSwizzle(Ogre::uint index, Ogre::uint sizeLog2[3], Ogre::uint * pPos)
{

    // force index traversal to occur in 2x2x2 blocks by giving each of x, y, and z one
    // of the bottom 3 bits
	pPos[0] = index & 1;
    index >>= 1;
    pPos[1] = index & 1;
    index >>= 1;
    pPos[2] = index & 1;
    index >>= 1;

    // Treat the rest of the index like a row, collumn, depth array
    // Each dimension needs to grab sizeLog2 - 1 bits
    // This will make the blocks traverse the grid in a raster style order
    index <<= 1;
    pPos[0] = pPos[0] | (index &  ( (1 << sizeLog2[0]) - 2));
    index >>=  sizeLog2[0] - 1;
    pPos[1] = pPos[1] | ( index &  ( (1 << sizeLog2[1]) - 2));
    index >>= sizeLog2[1] - 1;
    pPos[2] = pPos[2] | ( index &  ( (1 << sizeLog2[2]) - 2));
}

VTFWMesh::VTFWMesh(const std::string& inMeshName, float planeSize, int inComplexity)
{
	this->meshName = inMeshName ;
	lastTimeStamp = 0 ;
	lastAnimationTimeStamp = 0;
	lastFrameTime = 0 ;

	Ogre::uint sizeLog2[3] = { X_SIZE_LOG2, Y_SIZE_LOG2, Z_SIZE_LOG2 };
	Ogre::uint nTotalBits = sizeLog2[0] + sizeLog2[1] + sizeLog2[2];
	Ogre::uint nPointsTotal = 1 << nTotalBits;

	// create mesh and submesh
	mesh = Ogre::MeshManager::getSingleton().createManual(meshName,
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	subMesh = mesh->createSubMesh();
	subMesh->operationType = Ogre::RenderOperation::OT_LINE_LIST;
	subMesh->setMaterialName("Ogre/IsoSurf/TessellateTetrahedra");
	subMesh->useSharedVertices = false;
	subMesh->vertexData = new Ogre::VertexData;
	subMesh->indexData = new Ogre::IndexData;

	subMesh->vertexData->vertexDeclaration->addElement(0, 0, 
		Ogre::VET_FLOAT4, Ogre::VES_POSITION);

	posVertexBuffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
		subMesh->vertexData->vertexDeclaration->getVertexSize(0), 
		nPointsTotal, 
		Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

	indexBuffer = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
		Ogre::HardwareIndexBuffer::IT_32BIT, 
		CELLS_COUNT * sizeof(Ogre::uint) * 24, 
		Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	
	subMesh->vertexData->vertexBufferBinding->setBinding(0, posVertexBuffer);
	subMesh->vertexData->vertexCount = nPointsTotal;
	subMesh->vertexData->vertexStart = 0;
	
	subMesh->indexData->indexBuffer = indexBuffer;
	
	float* positions = static_cast<float*>(posVertexBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD));
	
	//Generate positions
	for(Ogre::uint i=0; i<nPointsTotal; i++) {
		Ogre::uint pos[3];
		pos[0] = i & ((1<<X_SIZE_LOG2)-1);
		pos[1] = (i >> X_SIZE_LOG2) & ((1<<Y_SIZE_LOG2)-1);
		pos[2] = (i >> (X_SIZE_LOG2+Y_SIZE_LOG2)) & ((1<<Z_SIZE_LOG2)-1);

		*positions++ = (float(pos[0]) / float(1<<X_SIZE_LOG2))*2.0-1.0;
		*positions++ = (float(pos[1]) / float(1<<Y_SIZE_LOG2))*2.0-1.0;
		*positions++ = (float(pos[2]) / float(1<<Z_SIZE_LOG2))*2.0-1.0;
		*positions++ = 1.0f;
	}
	posVertexBuffer->unlock();
	
	Ogre::uint numIndices = 0;

	//Generate indices
	Ogre::uint32* indices = static_cast<Ogre::uint32*>(indexBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD));

	for (Ogre::uint i = 0; i<nPointsTotal; i++) {

		Ogre::uint pos[3];
#if SWIZZLE
		UnSwizzle(i, sizeLog2, pos);	// un-swizzle current index to get x, y, z for the current sampling point
#else
		pos[0] = i & ((1<<X_SIZE_LOG2)-1);
		pos[1] = (i >> X_SIZE_LOG2) & ((1<<Y_SIZE_LOG2)-1);
		pos[2] = (i >> (X_SIZE_LOG2+Y_SIZE_LOG2)) & ((1<<Z_SIZE_LOG2)-1);
#endif
		if ((int)pos[0] == (1 << sizeLog2[0]) - 1 || (int)pos[1] == (1 << sizeLog2[1]) - 1 || (int)pos[2] == (1 << sizeLog2[2]) - 1)
			continue;	// skip extra cells

		numIndices += 24; //Got to this point, adding 24 indices

		// NOTE: order of vertices matters! important for backface culling

		// T0
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1], pos[2], sizeLog2);
		*indices++ = MAKE_INDEX(pos[0], pos[1], pos[2], sizeLog2);
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1] + 1, pos[2], sizeLog2);
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1] + 1, pos[2] + 1, sizeLog2);

		// T1
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1] + 1, pos[2] + 1, sizeLog2);
		*indices++ = MAKE_INDEX(pos[0], pos[1], pos[2], sizeLog2);
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1] + 1, pos[2], sizeLog2);
		*indices++ = MAKE_INDEX(pos[0], pos[1] + 1, pos[2], sizeLog2);

		// T2
		*indices++ = MAKE_INDEX(pos[0], pos[1] + 1, pos[2], sizeLog2);
		*indices++ = MAKE_INDEX(pos[0], pos[1], pos[2], sizeLog2);
		*indices++ = MAKE_INDEX(pos[0], pos[1] + 1, pos[2] + 1, sizeLog2);
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1] + 1, pos[2] + 1, sizeLog2);

		// T3
		*indices++ = MAKE_INDEX(pos[0], pos[1], pos[2], sizeLog2);
		*indices++ = MAKE_INDEX(pos[0], pos[1], pos[2] + 1, sizeLog2);
		*indices++ = MAKE_INDEX(pos[0], pos[1] + 1, pos[2] + 1, sizeLog2);
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1] + 1, pos[2] + 1, sizeLog2);

		// T4
		*indices++ = MAKE_INDEX(pos[0], pos[1], pos[2] + 1, sizeLog2);
		*indices++ = MAKE_INDEX(pos[0], pos[1], pos[2], sizeLog2);
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1], pos[2] + 1, sizeLog2);
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1] + 1, pos[2] + 1, sizeLog2);

		// T5
		*indices++ = MAKE_INDEX(pos[0], pos[1], pos[2], sizeLog2);
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1], pos[2], sizeLog2);
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1], pos[2] + 1, sizeLog2);
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1] + 1, pos[2] + 1, sizeLog2);
	}
	
	indexBuffer->unlock();

	subMesh->indexData->indexCount = numIndices;
	subMesh->indexData->indexStart = 0;

	Ogre::AxisAlignedBox meshBounds;
	meshBounds.setMinimum(-1,-1,-1);
	meshBounds.setMaximum(1,1,1);
	mesh->_setBounds(meshBounds);
	mesh->_setBoundingSphereRadius(2);
}
/* ========================================================================= */
VTFWMesh::~VTFWMesh ()
{
	Ogre::MeshManager::getSingleton().remove(meshName);
}
/* ========================================================================= */
void VTFWMesh::push(float x, float y, float depth, bool absolute)
{

}

/* ========================================================================= */
void VTFWMesh::updateMesh(float timeSinceLastFrame)
{
	lastFrameTime = timeSinceLastFrame ;
	lastTimeStamp += timeSinceLastFrame ;

	// do rendering to get ANIMATIONS_PER_SECOND
	while(lastAnimationTimeStamp <= lastTimeStamp) 
	{

		lastAnimationTimeStamp += (1.0f / ANIMATIONS_PER_SECOND);
	}
}