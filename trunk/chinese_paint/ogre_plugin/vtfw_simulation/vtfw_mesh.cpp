#define ANIMATIONS_PER_SECOND 100.0f
#include "vtfw_mesh.h"

VTFWMesh::VTFWMesh(const std::string& inMeshName, float planeSize, int inComplexity, Ogre::RenderWindow *win):m_pWindow(win),
	m_imageBox(0.0, 0.0, 128.0, 128.0)
{
	// create previous texture
	Ogre::TexturePtr texutre = Ogre::TextureManager::getSingleton().createManual("heightSampler", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D, inComplexity, inComplexity, 0, Ogre::PF_FLOAT32_RGBA, Ogre::TU_RENDERTARGET);
	mHeightBuf = texutre->getBuffer();
	// create previous texture
	texutre = Ogre::TextureManager::getSingleton().createManual("previousHeightSampler", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D, inComplexity, inComplexity, 0, Ogre::PF_FLOAT32_RGBA, Ogre::TU_RENDERTARGET);
	mPreviousHeightBuf = texutre->getBuffer();

	float *data = new float[128*128*4];
	memset(data, 0, 128*128*4*sizeof(float));
	m_pPixelBox = new Ogre::PixelBox(128, 128, 1, Ogre::PF_FLOAT32_RGBA, data);
	//data = new float[128*128*4];
	//memset(data, 0, 128*128*4*sizeof(float));
	//m_pPrePixelBox = new Ogre::PixelBox(128, 128, 1, Ogre::PF_FLOAT32_RGBA, data);
	//Ogre::Image::Box box(0.0, 0.0, 128.0, 128.0);
	mHeightBuf->blitFromMemory(*m_pPixelBox, m_imageBox);
	mPreviousHeightBuf->blitFromMemory(*m_pPixelBox, m_imageBox);
	Ogre::MaterialManager::getSingleton().getByName("ChinesePaint/Water")->load();


	Ogre::CompositorManager::getSingleton().addCompositor(m_pWindow->getViewport(0), "ChinesePaint");
	Ogre::CompositorManager::getSingleton().setCompositorEnabled(m_pWindow->getViewport(0), "ChinesePaint", true);
	Ogre::CompositorInstance *ins = Ogre::CompositorManager::getSingleton().getCompositorChain(m_pWindow->getViewport(0))->getCompositor("ChinesePaint");
	ins->addListener(this);
	//ins->getTextureInstance("previousHeightSampler", 0)->getBuffer()->blitFromMemory(*m_pPixelBox, box);
	//ins->getTextureInstance("heightSampler", 0)->getBuffer()->blitFromMemory(*m_pPixelBox, box);
	ins->getTextureInstance("curHeightSampler", 0)->getBuffer()->getRenderTarget()->addListener(this);

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
			float u = (float)x / complexity ;
			float v = 1.0f - ((float)y / (complexity)) ;
			texcoordsBufData[2*(y*(complexity+1)+x)+0] = u;
			texcoordsBufData[2*(y*(complexity+1)+x)+1] = v;
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

	//delete[] m_pPrePixelBox->data;
	//delete m_pPrePixelBox;
	//m_pPrePixelBox = NULL;

	Ogre::TextureManager::getSingleton().remove("heightSampler");
	Ogre::TextureManager::getSingleton().remove("previousHeightSampler");
	Ogre::MaterialManager::getSingleton().getByName("ChinesePaint/Water")->unload();

	Ogre::MeshManager::getSingleton().remove(meshName);
	Ogre::CompositorInstance *ins = Ogre::CompositorManager::getSingleton().getCompositorChain(m_pWindow->getViewport(0))->getCompositor("ChinesePaint");
	ins->setEnabled(false);
	ins->removeListener(this);
	ins->getTextureInstance("curHeightSampler", 0)->getBuffer()->getRenderTarget()->removeAllListeners();
}
/* ========================================================================= */
void VTFWMesh::push(float x, float y, float depth, bool absolute)
{
	Ogre::CompositorInstance *ins = Ogre::CompositorManager::getSingleton().getCompositorChain(m_pWindow->getViewport(0))->getCompositor("ChinesePaint");
	//Ogre::Image::Box box(0.0, 0.0, 128.0, 128.0);
	//ins->getTextureInstance("curHeightSampler", 0)->getBuffer()->blitToMemory(m_imageBox, *m_pPixelBox);
	mHeightBuf->blitToMemory(m_imageBox, *m_pPixelBox);
	float *buf = (float *)m_pPixelBox->data;
	// scale pressure according to time passed
	depth = depth * lastFrameTime;// * ANIMATIONS_PER_SECOND ;
#define _PREP(addx,addy) { \
	float *vertex=buf+4*((int)(y+addy)*(complexity)+(int)(x+addx)) ; \
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
	//ins->getTextureInstance("curHeightSampler", 0)->getBuffer()->blitFromMemory(*m_pPixelBox, box);
	mHeightBuf->blitFromMemory(*m_pPixelBox, m_imageBox);
}

/* ========================================================================= */
void VTFWMesh::updateMesh(float timeSinceLastFrame)
{
	lastFrameTime = timeSinceLastFrame ;
	lastTimeStamp += timeSinceLastFrame ;

	//Ogre::CompositorInstance *ins = Ogre::CompositorManager::getSingleton().getCompositorChain(m_pWindow->getViewport(0))->getCompositor("ChinesePaint");
	//float float2[2] = {-0.0078125, 0.0};
	//ins->getTechnique()->getTargetPass(1)->getPass(0)->getMaterial()->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("psSimulationTexCoordDelta_x0y1", float2, 2);
	//float2[0] = 0.0078125;
	//float2[1] = 0.0;
	//ins->getTechnique()->getTargetPass(1)->getPass(0)->getMaterial()->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("psSimulationTexCoordDelta_x2y1", float2, 2);
	//float2[0] = 0.0;
	//float2[1] = -0.0078125;
	//ins->getTechnique()->getTargetPass(1)->getPass(0)->getMaterial()->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("psSimulationTexCoordDelta_x1y0", float2, 2);
	//float2[0] = 0.0;
	//float2[1] = 0.0078125;
	//ins->getTechnique()->getTargetPass(1)->getPass(0)->getMaterial()->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("psSimulationTexCoordDelta_x1y2", float2, 2);
	//float2[0] = 1.99;
	//float2[1] = 0.99;
	//ins->getTechnique()->getTargetPass(1)->getPass(0)->getMaterial()->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("psSimulationPositionWeighting", float2, 2);
	//ins->getTechnique()->getTargetPass(1)->getPass(0)->getMaterial()->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("psSimulationWaveSpeedSquared", 10);
	//ins->getTechnique()->getTargetPass(1)->getPass(0)->getMaterial()->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("psSimulationOnefloatTimesDeltaTimeSquared", 15*timeSinceLastFrame*timeSinceLastFrame);
	//float float3[3] = {23.4375, 0.0, 23.4375};
	//ins->getTechnique()->getTargetPass(1)->getPass(0)->getMaterial()->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("psSimulationGridSize", float3, 3);
	//mHeightBuf->blitToMemory(m_imageBox, *m_pPixelBox);
	//mPreviousHeightBuf->blitFromMemory(*m_pPixelBox, m_imageBox);
	//
	//ins->getTextureInstance("curHeightSampler", 0)->getBuffer()->blitToMemory(m_imageBox, *m_pPixelBox);
	//mHeightBuf->blitFromMemory(*m_pPixelBox, m_imageBox);


	//float *pData = (float *)m_pPixelBox->data;
	//while(lastAnimationTimeStamp <= lastTimeStamp) 
	//{
	//		// switch buffer numbers
	//	int x, y;
	//	float *buf = vertexBuffers[currentBuffNumber] + 1 ; // +1 for Y coordinate
	//	for(y=0;y<complexity;y++) // don't do anything with border values
	//	{ 
	//		float *row = buf + 3*y*(complexity) ;
	//		float *pixelRow = pData + 4 * y * (complexity) ;
	//		for(x=0;x<complexity;x++) 
	//		{
	//			float newHigh = pixelRow[(4*x)];
	//			row[3*x] = newHigh;
	//			//row[3*x] = 0;
	//		}
	//	}
	//	lastAnimationTimeStamp += (1.0f / ANIMATIONS_PER_SECOND);
	//}
	//// set vertex buffer
	//posVertexBuffer->writeData(0,
	//	posVertexBuffer->getSizeInBytes(), // size
	//	vertexBuffers[currentBuffNumber], // source
	//	true); // discard?
}

//Ogre::TexturePtr VTFWMesh::getTexture(const std::string &name)
//{
//	return mTexture[name];
//}

//void VTFWMesh::preRenderTargetUpdate(const Ogre::RenderTargetEvent& evt)
//{
//	Ogre::CompositorInstance *ins = Ogre::CompositorManager::getSingleton().getCompositorChain(m_pWindow->getViewport(0))->getCompositor("ChinesePaint");
//	ins->getTechnique()->getTargetPass(1)->getPass(0)->getMaterial()->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("psSimulationOnefloatTimesDeltaTimeSquared", 15*lastTimeStamp*lastTimeStamp);
//	lastTimeStamp = 0.0;
//}
//
//void VTFWMesh::postRenderTargetUpdate(const Ogre::RenderTargetEvent& evt)
//{
//	Ogre::CompositorInstance *ins = Ogre::CompositorManager::getSingleton().getCompositorChain(m_pWindow->getViewport(0))->getCompositor("ChinesePaint");
//	mHeightBuf->blitToMemory(m_imageBox, *m_pPixelBox);
//	mPreviousHeightBuf->blitFromMemory(*m_pPixelBox, m_imageBox);
//	ins->getTextureInstance("curHeightSampler", 0)->getBuffer()->blitToMemory(m_imageBox, *m_pPixelBox);
//	mHeightBuf->blitFromMemory(*m_pPixelBox, m_imageBox);
//
//
//	float *pData = (float *)m_pPixelBox->data;
//	while(lastAnimationTimeStamp <= lastTimeStamp) 
//	{
//		// switch buffer numbers
//		int x, y;
//		float *buf = vertexBuffers[currentBuffNumber] + 1 ; // +1 for Y coordinate
//		for(y=0;y<complexity;y++) // don't do anything with border values
//		{ 
//			float *row = buf + 3*y*(complexity) ;
//			float *pixelRow = pData + 4 * y * (complexity) ;
//			for(x=0;x<complexity;x++) 
//			{
//				float newHigh = pixelRow[(4*x)];
//				row[3*x] = newHigh;
//				//row[3*x] = 0;
//			}
//		}
//		lastAnimationTimeStamp += (1.0f / ANIMATIONS_PER_SECOND);
//	}
//	// set vertex buffer
//	posVertexBuffer->writeData(0,
//		posVertexBuffer->getSizeInBytes(), // size
//		vertexBuffers[currentBuffNumber], // source
//		true); // discard?
//}

// Implementation of Ogre::CompositorInstance::Listener
void VTFWMesh::notifyMaterialSetup(Ogre::uint32 passId, Ogre::MaterialPtr& material)
{
}

// Implementation of Ogre::RenderTargetListener
void VTFWMesh::preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt)
{
	Ogre::CompositorInstance *ins = Ogre::CompositorManager::getSingleton().getCompositorChain(m_pWindow->getViewport(0))->getCompositor("ChinesePaint");
	Ogre::GpuProgramParametersSharedPtr fragParams = ins->getTechnique()->getTargetPass(1)->getPass(0)->getMaterial()->
														getTechnique(0)->getPass(0)->getFragmentProgramParameters();
	if((!fragParams.isNull())&&(fragParams->_findNamedConstantDefinition("psSimulationOnefloatTimesDeltaTimeSquared")))
		fragParams->setNamedConstant("psSimulationOnefloatTimesDeltaTimeSquared", 15*lastTimeStamp*lastTimeStamp);
	lastTimeStamp = 0.0;

	Ogre::RenderQueue* queue = evt.source->getCamera()->getSceneManager()->getRenderQueue();
	queue->setRenderableListener(this);
}

void VTFWMesh::postViewportUpdate(const Ogre::RenderTargetViewportEvent& evt)
{
	Ogre::CompositorInstance *ins = Ogre::CompositorManager::getSingleton().getCompositorChain(m_pWindow->getViewport(0))->getCompositor("ChinesePaint");
	mHeightBuf->blitToMemory(m_imageBox, *m_pPixelBox);
	mPreviousHeightBuf->blitFromMemory(*m_pPixelBox, m_imageBox);
	ins->getTextureInstance("curHeightSampler", 0)->getBuffer()->blitToMemory(m_imageBox, *m_pPixelBox);
	mHeightBuf->blitFromMemory(*m_pPixelBox, m_imageBox);


	float *pData = (float *)m_pPixelBox->data;
	// switch buffer numbers
	int x, y;
	float *buf = vertexBuffers[currentBuffNumber] + 1 ; // +1 for Y coordinate
	for(y=1;y<complexity;y++) // don't do anything with border values
	{ 
		float *row = buf + 3*y*(complexity+1) ;
		float *pixelRow = pData + 4 * y * (complexity) ;
		for(x=1;x<complexity;x++) 
		{
			float newHigh = pixelRow[(4*x)];
			//row[3*x] = newHigh * 100;
			row[3*x] = 0;
		}
	}

	// set vertex buffer
	posVertexBuffer->writeData(0,
		posVertexBuffer->getSizeInBytes(), // size
		vertexBuffers[currentBuffNumber], // source
		true); // discard?
	Ogre::RenderQueue* queue = evt.source->getCamera()->getSceneManager()->getRenderQueue();
	queue->setRenderableListener(0);
}


// Implementation of Ogre::RenderQueue::RenderableListener
bool VTFWMesh::renderableQueued(Ogre::Renderable* rend, Ogre::uint8 groupID, 
	Ogre::ushort priority, Ogre::Technique** ppTech, Ogre::RenderQueue* pQueue)
{
	//*ppTech = mDepthTechnique;
	return true;
}