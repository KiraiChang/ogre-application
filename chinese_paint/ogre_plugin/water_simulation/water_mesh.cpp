#include "water_mesh.h"

#define ANIMATIONS_PER_SECOND 100.0f


WaterMesh::WaterMesh(const std::string& inMeshName, float planeSize, int inComplexity):m_eSimulationMode(eVTFWMode)
{
	int x,y,b; // I prefer to initialize for() variables inside it, but VC doesn't like it ;(
	Ogre::ColourValue color;
	image.load("dampening.tga", "Popular");
	for(int y = 0;y <DAMPEN_SIZE;y++)
	{
		for(int x = 0;x <DAMPEN_SIZE;x++)
		{
			color = image.getColourAt(x, y, 0);
			m_vfDampening[x][y] = color.r;
		}
	}
	this->meshName = inMeshName ;
	this->complexity = inComplexity ;
	numFaces = 2 * complexity * complexity;
	numVertices = (complexity + 1) * (complexity + 1) ;
	lastTimeStamp = 0 ;
	lastAnimationTimeStamp = 0;
	lastFrameTime = 0 ;

	// initialize algorithm parameters
	PARAM_C = 0.3f ; // ripple speed
	PARAM_D = 0.4f ; // distance
	PARAM_U = 0.05f ; // viscosity
	PARAM_T = 0.13f ; // time
	useFakeNormals = false ;

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
WaterMesh::~WaterMesh ()
{
 	delete[] vertexBuffers[0];
 	delete[] vertexBuffers[1];
 	delete[] vertexBuffers[2];

	delete[] vNormals;

	Ogre::MeshManager::getSingleton().remove(meshName);
}
/* ========================================================================= */
void WaterMesh::push(float x, float y, float depth, bool absolute)
{
	float *buf = vertexBuffers[currentBuffNumber]+1 ;
	// scale pressure according to time passed
	depth = depth * lastFrameTime * ANIMATIONS_PER_SECOND ;
#define _PREP(addx,addy) { \
	float *vertex=buf+3*((int)(y+addy)*(complexity+1)+(int)(x+addx)) ; \
	float diffy = y - floor(y+addy); \
	float diffx = x - floor(x+addx); \
	float dist=sqrt(diffy*diffy + diffx*diffx) ; \
	float power = 1 - dist ; \
	if (power<0)  \
		power = 0; \
	if (absolute) \
		*vertex = depth*power ;  \
	else \
		*vertex += depth*power ;  \
} /* #define */
	_PREP(0,0);
	_PREP(0,1);
	_PREP(1,0);
	_PREP(1,1);
#undef _PREP
}
/* ========================================================================= */
float WaterMesh::getHeight(float x, float y)
{
#define hat(_x,_y) buf[3*((int)_y*(complexity+1)+(int)(_x))]
	float *buf = vertexBuffers[currentBuffNumber] ;
	float xa = floor(x);
	float xb = xa + 1 ;
	float ya = floor(y);
	float yb = ya + 1 ;
	float yaxavg = hat(xa,ya) * (1.0f-fabs(xa-x)) + hat(xb,ya) * (1.0f-fabs(xb-x));
	float ybxavg = hat(xa,yb) * (1.0f-fabs(xa-x)) + hat(xb,yb) * (1.0f-fabs(xb-x));
	float yavg = yaxavg * (1.0f-fabs(ya-y)) + ybxavg * (1.0f-fabs(yb-y)) ;
	return yavg ;
}
/* ========================================================================= */
void WaterMesh::calculateFakeNormals()
{
	int x,y;
	float *buf = vertexBuffers[currentBuffNumber] + 1;
	float *pNormals = (float*) normVertexBuffer->lock(
		0,normVertexBuffer->getSizeInBytes(), Ogre::HardwareBuffer::HBL_DISCARD);
	for(y=1;y<complexity;y++) {
		float *nrow = pNormals + 3*y*(complexity+1);
		float *row = buf + 3*y*(complexity+1) ;
		float *rowup = buf + 3*(y-1)*(complexity+1) ;
		float *rowdown = buf + 3*(y+1)*(complexity+1) ;
		for(x=1;x<complexity;x++) {
			float xdiff = row[3*x+3] - row[3*x-3] ;
			float ydiff = rowup[3*x] - rowdown[3*x-3] ;
			Ogre::Vector3 norm(xdiff,30,ydiff);
			norm.normalise();
			nrow[3*x+0] = norm.x;
			nrow[3*x+1] = norm.y;
			nrow[3*x+2] = norm.z;
		}
	}
	normVertexBuffer->unlock();
}
/* ========================================================================= */
void WaterMesh::calculateNormals()
{
	int i,x,y;
	float *buf = vertexBuffers[currentBuffNumber] + 1;
	// zero normals
	for(i=0;i<numVertices;i++) {
		vNormals[i] = Ogre::Vector3::ZERO;
	}
	// first, calculate normals for faces, add them to proper vertices
	buf = vertexBuffers[currentBuffNumber] ;
	unsigned short* vinds = (unsigned short*) indexBuffer->lock(
		0, indexBuffer->getSizeInBytes(), Ogre::HardwareBuffer::HBL_READ_ONLY);
	float *pNormals = (float*) normVertexBuffer->lock(
		0, normVertexBuffer->getSizeInBytes(), Ogre::HardwareBuffer::HBL_DISCARD);
	for(i=0;i<numFaces;i++) {
		int p0 = vinds[3*i] ;
		int p1 = vinds[3*i+1] ;
		int p2 = vinds[3*i+2] ;
		Ogre::Vector3 v0(buf[3*p0], buf[3*p0+1], buf[3*p0+2]);
		Ogre::Vector3 v1(buf[3*p1], buf[3*p1+1], buf[3*p1+2]);
		Ogre::Vector3 v2(buf[3*p2], buf[3*p2+1], buf[3*p2+2]);
		Ogre::Vector3 diff1 = v2 - v1 ;
		Ogre::Vector3 diff2 = v0 - v1 ;
		Ogre::Vector3 fn = diff1.crossProduct(diff2);
		vNormals[p0] += fn ;
		vNormals[p1] += fn ;
		vNormals[p2] += fn ;
	}
	// now normalize vertex normals
	for(y=0;y<=complexity;y++) {
		for(x=0;x<=complexity;x++) {
			int numPoint = y*(complexity+1) + x ;
			Ogre::Vector3 n = vNormals[numPoint] ;
			n.normalise() ;
			float* normal = pNormals + 3*numPoint ;
			normal[0]=n.x;
			normal[1]=n.y;
			normal[2]=n.z;
		}
	}
	indexBuffer->unlock();
	normVertexBuffer->unlock();
}

void WaterMesh::ogreSimulation(float timepass)
{
	int x, y ;
	// switch buffer numbers
	currentBuffNumber = (currentBuffNumber + 1) % 3 ;
	float *buf = vertexBuffers[currentBuffNumber] + 1 ; // +1 for Y coordinate
	float *buf1 = vertexBuffers[(currentBuffNumber+2)%3] + 1 ;
	float *buf2 = vertexBuffers[(currentBuffNumber+1)%3] + 1;

	///* we use an algorithm from
	// * http://collective.valve-erc.com/index.php?go=water_simulation
	// * The params could be dynamically changed every frame ofcourse
	// */
	double C = PARAM_C; // ripple speed
	double D = PARAM_D; // distance
	double U = PARAM_U; // viscosity
	double T = PARAM_T; // time
	float TERM1 = ( 4.0f - 8.0f*C*C*T*T/(D*D) ) / (U*T+2) ;
	float TERM2 = ( U*T-2.0f ) / (U*T+2.0f) ;
	float TERM3 = ( 2.0f * C*C*T*T/(D*D) ) / (U*T+2) ;
	for(y=1;y<complexity;y++) { // don't do anything with border values
		float *row = buf + 3*y*(complexity+1) ;
		float *row1 = buf1 + 3*y*(complexity+1) ;
		float *row1up = buf1 + 3*(y-1)*(complexity+1) ;
		float *row1down = buf1 + 3*(y+1)*(complexity+1) ;
		float *row2 = buf2 + 3*y*(complexity+1) ;
		for(x=1;x<complexity;x++) {
			row[3*x] = TERM1 * row1[3*x]
			+ TERM2 * row2[3*x]
			+ TERM3 * ( row1[3*x-3] + row1[3*x+3] + row1up[3*x]+row1down[3*x] ) ;
		}
	}
}

void WaterMesh::vtfwSimulation(float timepass)
{
	// switch buffer numbers
	int x, y;
	Ogre::ColourValue color;
	currentBuffNumber = (currentBuffNumber + 1) % 3 ;
	float *buf = vertexBuffers[currentBuffNumber] + 1 ; // +1 for Y coordinate
	float *buf1 = vertexBuffers[(currentBuffNumber+2)%3] + 1 ;
	float *buf2 = vertexBuffers[(currentBuffNumber+1)%3] + 1;
	float dt = timepass * timepass;
	for(y=1;y<complexity;y++) // don't do anything with border values
	{ 
		float *row = buf + 3*y*(complexity+1) ;
		float *row1 = buf1 + 3*y*(complexity+1) ;
		float *row1up = buf1 + 3*(y-1)*(complexity+1) ;
		float *row1down = buf1 + 3*(y+1)*(complexity+1) ;
		float *row2 = buf2 + 3*y*(complexity+1) ;
		for(x=1;x<complexity;x++) 
		{
			float force = 2 * m_vfDampening[x][y] *  (row1[3*x-3] + row1[3*x+3] + row1up[3*x]+row1down[3*x] - (4 * row[3*x]));
			float newHight =  (1.99 * row1[3*x]) - (0.99 * row2[3*x]) + (0.5 * force * dt);
			row[3*x] = newHight;
		}
	}
}
/* ========================================================================= */
void WaterMesh::updateMesh(float timeSinceLastFrame)
{
	lastFrameTime = timeSinceLastFrame ;
	lastTimeStamp += timeSinceLastFrame ;

	// do rendering to get ANIMATIONS_PER_SECOND
	while(lastAnimationTimeStamp <= lastTimeStamp) 
	{
		switch(m_eSimulationMode)
		{
		case eOGREMode:
			ogreSimulation(timeSinceLastFrame);
			break;
		case eVTFWMode:
			vtfwSimulation(timeSinceLastFrame);
			break;
		}
		lastAnimationTimeStamp += (1.0f / ANIMATIONS_PER_SECOND);
	}

	if (useFakeNormals) 
	{
		calculateFakeNormals();
	}
	else 
	{
		calculateNormals();
	}

	// set vertex buffer
	posVertexBuffer->writeData(0,
		posVertexBuffer->getSizeInBytes(), // size
		vertexBuffers[currentBuffNumber], // source
		true); // discard?
}