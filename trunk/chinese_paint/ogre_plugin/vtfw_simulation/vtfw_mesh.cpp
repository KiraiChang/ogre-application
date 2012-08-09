#define ANIMATIONS_PER_SECOND 100.0f
#include "vtfw_mesh.h"

VTFWMesh::VTFWMesh(const std::string& inMeshName, float planeSize, int inComplexity, Ogre::RenderWindow *win):m_pWindow(win)
{
	// create height texture
	Ogre::TexturePtr texHight = Ogre::TextureManager::getSingleton().createManual("heightSampler", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D, inComplexity, inComplexity, 0, Ogre::PF_R8G8B8A8, Ogre::TU_RENDERTARGET);

	mHeightBuf = texHight->getBuffer();  // save off the texture buffer

	Ogre::uint8* data = new Ogre::uint8[inComplexity * inComplexity * 3];
	memset(data, 0xff, inComplexity * inComplexity * 3 * sizeof(Ogre::uint8));
	m_pPixelBox = new Ogre::PixelBox(mHeightBuf->getWidth(), mHeightBuf->getHeight(), 1, Ogre::PF_R8G8B8A8, data);
	mHeightBuf->blitFromMemory(*m_pPixelBox);

	////copy the actual pixels over to the texture
	//mHeightBuf->lock(Ogre::HardwareBuffer::LockOptions::HBL_DISCARD);
	//Ogre::PixelBox pixelBox = mHeightBuf->getCurrentLock();
	//memset(pixelBox.data, 0x0f, mHeightBuf->getSizeInBytes());
	//mHeightBuf->unlock();

	// create previous texture
	Ogre::TexturePtr texPrevious = Ogre::TextureManager::getSingleton().createManual("previousHeightSampler", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D, inComplexity, inComplexity, 0, Ogre::PF_R8G8B8A8, Ogre::TU_RENDERTARGET);

	mPreviousHeightBuf = texPrevious->getBuffer();  // save off the texture buffer
	mPreviousHeightBuf->blitFromMemory(*m_pPixelBox);

	int x,y,b; // I prefer to initialize for() variables inside it, but VC doesn't like it ;(
	this->meshName = inMeshName ;
	this->complexity = inComplexity ;
	numFaces = 2 * complexity * complexity;
	numVertices = (complexity + 1) * (complexity + 1) ;
	lastTimeStamp = 0 ;
	lastAnimationTimeStamp = 0;
	lastFrameTime = 0 ;

	// allocate space for normal calculation
	vNormals = new Ogre::Vector3[numVertices];

	// create mesh and submesh
	mesh = Ogre::MeshManager::getSingleton().createManual(meshName,
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	subMesh = mesh->createSubMesh();
	subMesh->useSharedVertices=false;

	// Vertex buffers
	subMesh->vertexData = new Ogre::VertexData();
	subMesh->vertexData->vertexStart = 0;
	subMesh->vertexData->vertexCount = numVertices;

	Ogre::VertexDeclaration* vdecl = subMesh->vertexData->vertexDeclaration;
	Ogre::VertexBufferBinding* vbind = subMesh->vertexData->vertexBufferBinding;


	vdecl->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
	vdecl->addElement(1, 0, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
	vdecl->addElement(2, 0, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);

	// Prepare buffer for positions - todo: first attempt, slow
	posVertexBuffer =
         Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
            3*sizeof(float),
			numVertices,
			Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
	vbind->setBinding(0, posVertexBuffer);

	// Prepare buffer for normals - write only
	normVertexBuffer =
         Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
            3*sizeof(float),
			numVertices,
			Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
	vbind->setBinding(1, normVertexBuffer);

	// Prepare texture coords buffer - static one
	// todo: optimize to write directly into buffer
	float *texcoordsBufData = new float[numVertices*2];
	for(y=0;y<=complexity;y++) {
		for(x=0;x<=complexity;x++) {
			texcoordsBufData[2*(y*(complexity+1)+x)+0] = (float)x / complexity ;
			texcoordsBufData[2*(y*(complexity+1)+x)+1] = 1.0f - ((float)y / (complexity)) ;
		}
	}
	texcoordsVertexBuffer =
         Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
            2*sizeof(float),
			numVertices,
			Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	texcoordsVertexBuffer->writeData(0,
		texcoordsVertexBuffer->getSizeInBytes(),
		texcoordsBufData,
		true); // true?
	delete [] texcoordsBufData;
    vbind->setBinding(2, texcoordsVertexBuffer);

	// Prepare buffer for indices
	indexBuffer =
		Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
			Ogre::HardwareIndexBuffer::IT_16BIT,
			3*numFaces,
			Ogre::HardwareBuffer::HBU_STATIC, true);
	unsigned short *faceVertexIndices = (unsigned short*)
		indexBuffer->lock(0, numFaces*3*2, Ogre::HardwareBuffer::HBL_DISCARD);
	for(y=0 ; y<complexity ; y++) {
		for(x=0 ; x<complexity ; x++) {
			unsigned short *twoface = faceVertexIndices + (y*complexity+x)*2*3;
			int p0 = y*(complexity+1) + x ;
			int p1 = y*(complexity+1) + x + 1 ;
			int p2 = (y+1)*(complexity+1) + x ;
			int p3 = (y+1)*(complexity+1) + x + 1 ;
			twoface[0]=p2; //first tri
			twoface[1]=p1;
			twoface[2]=p0;
			twoface[3]=p2; //second tri
			twoface[4]=p3;
			twoface[5]=p1;
		}
	}
	indexBuffer->unlock();
	// Set index buffer for this submesh
	subMesh->indexData->indexBuffer = indexBuffer;
	subMesh->indexData->indexStart = 0;
	subMesh->indexData->indexCount = 3*numFaces;

	/*	prepare vertex positions
	 *	note - we use 3 vertex buffers, since algorighm uses two last phases
	 *	to calculate the next one
	 */
	for(b=0;b<3;b++) {
		vertexBuffers[b] = new float[numVertices * 3] ;
		for(y=0;y<=complexity;y++) {
			for(x=0;x<=complexity;x++) {
				int numPoint = y*(complexity+1) + x ;
				float* vertex = vertexBuffers[b] + 3*numPoint ;
				vertex[0]=(float)(x) / (float)(complexity) * (float) planeSize ;
				vertex[1]= 0 ; // rand() % 30 ;
				vertex[2]=(float)(y) / (float)(complexity) * (float) planeSize ;
			}
		}
	}

	Ogre::AxisAlignedBox meshBounds(0,0,0,
		planeSize,0, planeSize);
	mesh->_setBounds(meshBounds);

	currentBuffNumber = 0 ;
	posVertexBuffer->writeData(0,
		posVertexBuffer->getSizeInBytes(), // size
		vertexBuffers[currentBuffNumber], // source
		true); // discard?

    mesh->load();
    mesh->touch();
}
/* ========================================================================= */
VTFWMesh::~VTFWMesh ()
{
	delete[] vertexBuffers[0];
 	delete[] vertexBuffers[1];
 	delete[] vertexBuffers[2];

	delete[] vNormals;

	delete[] m_pPixelBox->data;
	delete m_pPixelBox;
	m_pPixelBox = NULL;

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
	calcWaveToTexture();
	//mHeightBuf->blitToMemory(*m_pPixelBox);

	//Ogre::uint8* data = (Ogre::uint8*)m_pPixelBox->data;

	while(lastAnimationTimeStamp <= lastTimeStamp) 
	{
			// switch buffer numbers
		int x, y;
		float *buf = vertexBuffers[currentBuffNumber] + 1 ; // +1 for Y coordinate
		for(y=1;y<complexity;y++) // don't do anything with border values
		{ 
			float *row = buf + 3*y*(complexity+1) ;
			//Ogre::uint8 *pixelRow = data + 3 * y * (complexity+1) ;
			for(x=1;x<complexity;x++) 
			{
				//Ogre::uint8 newHigh = pixelRow[3*x];
				//row[3*x] = newHigh;
			}
		}
		lastAnimationTimeStamp += (1.0f / ANIMATIONS_PER_SECOND);
	}
	// set vertex buffer
	posVertexBuffer->writeData(0,
		posVertexBuffer->getSizeInBytes(), // size
		vertexBuffers[currentBuffNumber], // source
		true); // discard?
}

void VTFWMesh::calcWaveToTexture()
{
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("ChinesePaint/Water");
	Ogre::Root::getSingleton().getRenderSystem()->_setRenderTarget(mHeightBuf->getRenderTarget());
	Ogre::Root::getSingleton().getRenderSystem()->bindGpuProgram(mat.getPointer()->getTechnique(0)->getPass(0)->getVertexProgram().getPointer());
	Ogre::Root::getSingleton().getRenderSystem()->bindGpuProgram(mat.getPointer()->getTechnique(0)->getPass(0)->getFragmentProgram().getPointer());
	mHeightBuf->getRenderTarget()->writeContentsToFile("height.png");
	Ogre::Root::getSingleton().getRenderSystem()->_setRenderTarget(m_pWindow);
}