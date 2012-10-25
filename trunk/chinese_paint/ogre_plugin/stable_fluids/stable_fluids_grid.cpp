#include "stable_fluids_grid.h"
#include "../ogre_tool/ogre_mesh_tool.h"
#include "solid_mesh.h"
#include <iostream>

extern void dens_step ( int N, float * x, float * x0, float * u, float * v, float diff, float dt );
extern void vel_step ( int N, float * u, float * v, float * u0, float * v0, float visc, float dt );
extern void set_bnd ( int N, int b, float * x );
extern void wave_step ( int N, float * buf, float * buf1, float * buf2, float *dampening, float timePass, float *velocity[2]);

static unsigned int MAX_HEIGHT_MAP_COUNT = 3;
static unsigned int MAX_VERTEX_MAP_COUNT = 3;
static float FISH_DEPTH = 0.01;
static float ANIMATIONS_PER_SECOND = 1.0f;
static float FORCE_STRENGTH = 0.04 / FISH_SCALE_SIZE;//0.02;
#define PARTICLE_LIVE_TIME 20
#define PARTICLE_SIZE_X 0.5
#define PARTICLE_SIZE_Y 0.5
#define PARTICLE_MOVE_SPEED 100
#define EXTERNAL_FORCE_Y_POS_DIFF -5
#define PARTICLE_COUNT 5
#define PARTICLE_X_BEG -4.5
#define PARTICLE_X_DIFF 0.25
#define PARTICLE_Z_BEG -0.8
#define PARTICLE_Z_DIFF 0.4

StableFluidsGrid::StableFluidsGrid(unsigned int number):
	m_iGridNumber(number),
	//m_fTimeSpeed(0.1f), 
	//m_vfDens(NULL),
	//m_vfDensPrev(NULL),
	m_fDiff(0.0f), 
	m_fVisc(0.0f),
	m_fForce(5.0f), 
	m_fSource(100.0f),
	m_vfU(NULL), 
	m_vfV(NULL), 
	m_vfUPrev(NULL), 
	m_vfVPrev(NULL),
	m_vfDumpening(NULL),
	m_vbIntersectGrid(NULL),
	m_iCurrentMap(0),
	m_pManuObj(NULL),
	m_pManualNode(NULL),
	m_pPixelBox(NULL),
	m_pMiniScreenNode(NULL),
	m_pMiniScreen(NULL),
	m_pPS(NULL),
	m_pPSNode(NULL),
	//m_sVertexCount(0),
	//m_sIndex_count(0),
	//m_vIndices(NULL),
	//m_iCurrentVertex(0),
	m_vfBoundaryU(NULL),
	m_vfBoundaryV(NULL),
	m_vfEnforceU(0),
	m_viEnforceUCount(0),
	m_vfEnforceV(0),
	m_viEnforceVCount(0),
	m_eMapDisplayType(DISPLAY_DENSITY_MAP),
	m_eVelocityType(DISPLAY_ORIGIN),
	m_fLastTimeStamp(0),
	m_fLastFrameTime(0),
	m_bExternlForce(false),
	m_bAddForce(false),
	m_pSceneMgr(NULL)
{
	int i;
	m_iGridSize = (m_iGridNumber+2)*(m_iGridNumber+2);
	for( i = 0; i < MAX_HEIGHT_MAP_COUNT;i++)
	{
		m_vfHeightMap[i] = 0;
		//if(i < MAX_VERTEX_MAP_COUNT)
		//	m_vVertices[i] = 0;
	}

	for( i = 0; i < VELOCITY_COUNT; i++)
	{
		m_vfWaveVelocity[i] = 0;
	}
	
}

StableFluidsGrid::~StableFluidsGrid(void)
{
	release();
}

void StableFluidsGrid::init(Ogre::SceneManager *mgr)
{
	m_pSceneMgr = mgr;
	m_vfU			= new float[m_iGridSize];
	m_vfV			= new float[m_iGridSize];
	m_vfUPrev		= new float[m_iGridSize];
	m_vfVPrev		= new float[m_iGridSize];
	m_vbIntersectGrid = new float[m_iGridSize];
	//m_vfDens		= new float[m_iGridSize];
	//m_vfDensPrev	= new float[m_iGridSize];

	m_vfEnforceU	= new float[m_iGridSize];
	m_viEnforceUCount = new int[m_iGridSize];
	m_vfEnforceV = new float[m_iGridSize];
	m_viEnforceVCount = new int[m_iGridSize];

	m_vfBoundaryU	= new float[m_iGridSize];
	m_vfBoundaryV	= new float[m_iGridSize];

	m_vfWaveVelocity[VELOCITY_U] = new float[m_iGridSize];
	m_vfWaveVelocity[VELOCITY_V] = new float[m_iGridSize];
	memset(m_vfWaveVelocity[VELOCITY_U], 0, m_iGridSize * sizeof(m_vfWaveVelocity[VELOCITY_U]));
	memset(m_vfWaveVelocity[VELOCITY_V], 0, m_iGridSize * sizeof(m_vfWaveVelocity[VELOCITY_V]));
	memset(m_vbIntersectGrid, 0, m_iGridSize * sizeof(m_vbIntersectGrid));

	clear();
	int y ,x;
	for(int i = 0; i < MAX_HEIGHT_MAP_COUNT;i++)
		if(m_vfHeightMap[i] == NULL)
		{
			m_vfHeightMap[i] = new float[m_iGridSize];
			//memset(m_vfHeightMap[i], 0, m_iGridSize*sizeof(float));//init height map to 0
			for(y=0;y<m_iGridNumber+2;y++) {
				for(x=0;x<m_iGridNumber+2;x++) {
					int numPoint = y*(m_iGridNumber+2) + x ;
					float* vertex = m_vfHeightMap[i] + numPoint ;
					*vertex = 0.0f ; // rand() % 30 ;
				}
			}
		}
	m_vfDumpening = new float[m_iGridSize];
	//memset(m_vfDumpening, 1, m_iGridSize*sizeof(float));//init height map to 1
	for(y=0;y<m_iGridNumber+2;y++) {
		for(x=0;x<m_iGridNumber+2;x++) {
			int numPoint = y*(m_iGridNumber+2) + x ;
			float* vertex = m_vfDumpening + numPoint ;
			*vertex = 1.0f ; // rand() % 30 ;
		}
	}
	set_bnd(m_iGridNumber, 1, m_vfDumpening);
	set_bnd(m_iGridNumber, 2, m_vfDumpening);
	m_pPixelBox = new Ogre::PixelBox(m_iGridNumber+2, m_iGridNumber+2, 1, Ogre::PF_FLOAT32_R);

	m_pManuObj = m_pSceneMgr->createManualObject();
	m_pManualNode = m_pSceneMgr->getRootSceneNode()->
		createChildSceneNode(Ogre::Vector3(0.0f, 0.0f, 0.0f));
	m_pManualNode->attachObject(m_pManuObj);

	// create previous texture
	Ogre::TexturePtr texutre = Ogre::TextureManager::getSingleton().createManual("heightSampler", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D, m_iGridNumber+2, m_iGridNumber+2, 0, Ogre::PF_FLOAT32_R, Ogre::TU_RENDERTARGET);
	m_heightMap = texutre->getBuffer();

	m_pMiniScreen = new Ogre::Rectangle2D(true);
	m_pMiniScreen->setCorners(0.5f, -0.5f, 1.0f, -1.0f);
	m_pMiniScreen->setBoundingBox(Ogre::AxisAlignedBox(-100000.0f * Ogre::Vector3::UNIT_SCALE, 100000.0f * Ogre::Vector3::UNIT_SCALE));

	m_pMiniScreenNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
	m_pMiniScreenNode->attachObject(m_pMiniScreen);

	m_pMiniScreen->setMaterial("ChinesePaint/Texture");

	m_pPS = m_pSceneMgr->createParticleSystem(20000U);
	m_pPS->setMaterialName("ChinesePaint/InkBlock");
	m_pPSNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
	m_pPSNode->attachObject(m_pPS);
	m_pPSNode->setPosition(32, 0, 42);
}

void StableFluidsGrid::release(void)
{
	if ( m_vfU ) delete ( m_vfU );
	if ( m_vfV ) delete ( m_vfV );
	if ( m_vfUPrev ) delete ( m_vfUPrev );
	if ( m_vfVPrev ) delete ( m_vfVPrev );
	//if ( m_vfDens ) delete ( m_vfDens );
	//if ( m_vfDensPrev ) delete ( m_vfDensPrev );

	if ( m_vbIntersectGrid ) 
	{
		delete [] m_vbIntersectGrid;
		m_vbIntersectGrid = NULL;
	}

	
	if ( m_vfEnforceU ) 
	{
		delete [] m_vfEnforceU;
		m_vfEnforceU = NULL;
	}

	if ( m_vfBoundaryU ) 
	{
		delete [] m_vfBoundaryU;
		m_vfBoundaryU = NULL;
	}

	if ( m_vfBoundaryV ) 
	{
		delete [] m_vfBoundaryU;
		m_vfBoundaryU = NULL;
	}

	if ( m_viEnforceUCount ) 
	{
		delete [] m_viEnforceUCount;
		m_viEnforceUCount = NULL;
	}

	if ( m_vfEnforceV ) 
	{
		delete [] m_vfEnforceV;
		m_vfEnforceV = NULL;
	}

	if ( m_viEnforceVCount ) 
	{
		delete [] m_viEnforceVCount;
		m_viEnforceVCount = NULL;
	}

	for(int i = 0; i < MAX_HEIGHT_MAP_COUNT;i++)
	{
		if(m_vfHeightMap[i] != NULL)
		{
			delete m_vfHeightMap[i];
			m_vfHeightMap[i] = 0;
		}
	}

	for(int i = 0; i < VELOCITY_COUNT;i++)
	{
		if(m_vfWaveVelocity[i] != NULL)
		{
			delete m_vfWaveVelocity[i];
			m_vfWaveVelocity[i] = 0;
		}
	}

	if(m_pSceneMgr != NULL)
	{
		if(m_pManualNode != NULL)
		{
			m_pManualNode->detachObject(m_pManuObj);
			m_pSceneMgr->destroySceneNode(m_pManualNode);
			m_pManualNode = NULL;
		}
		if(m_pManuObj != NULL)
		{
			m_pSceneMgr->destroyManualObject(m_pManuObj);
			m_pManuObj = NULL;
		}
		
		if(m_pMiniScreenNode != NULL)
		{
			m_pMiniScreenNode->detachObject(m_pMiniScreen);
			m_pSceneMgr->destroySceneNode(m_pMiniScreenNode);
			m_pMiniScreenNode = NULL;
		}
		if(m_pMiniScreen != NULL)
		{
			delete m_pMiniScreen;
			m_pMiniScreen = NULL;
		}

		if(m_pPSNode != NULL)
		{
			m_pPSNode->detachObject(m_pPS);
			m_pSceneMgr->destroySceneNode(m_pPSNode);
			m_pPSNode = NULL;
		}
		if(m_pPS != NULL)
		{
			m_pSceneMgr->destroyParticleSystem(m_pPS);
			m_pPS = NULL;
		}
	}
	if(m_pPixelBox != NULL)
	{
		delete m_pPixelBox;
		m_pPixelBox = NULL;
	}

	//for(int i = 0;i < MAX_VERTEX_MAP_COUNT;i++)
	//{
	//	if(m_vVertices[i] != NULL)
	//	{
	//		delete[] m_vVertices[i];
	//		m_vVertices[i] = NULL;
	//	}
	//}

	//if(m_vIndices != NULL)
	//{
	//	delete[] m_vIndices;
	//	m_vIndices = NULL;
	//}

	Ogre::TextureManager::getSingleton().remove("heightSampler");
}

void StableFluidsGrid::updateParticle(float timePass)
{
	Ogre::Particle* particle = NULL;

	unsigned int index;
	float u0, u1, v0, v1, u, v;
	int x0, x1, y0, y1;
	Ogre::ParticleIterator ite = m_pPS->_getIterator();
	while(!ite.end())
	{
		particle = ite.getNext();
		Ogre::Vector3 pos = particle->position;
		//index = ((int)pos.x)+(m_iGridNumber+2)*((int)pos.z);
		//if(index > m_iGridSize)
		//	break;
		x0 = (int)pos.x; y0 = (int)pos.z;
		x1 = x0 + 1; y1 = y0 + 1;
		u1 = pos.x-x0; u0 = 1-u1; v1 = pos.z-y0; v0 = 1-v1;
		u = u0*(v0*m_vfU[IX(x0,y0)]+v1*m_vfU[IX(x0,y1)])+
			u1*(v0*m_vfU[IX(x1,y0)]+v1*m_vfU[IX(x1,y1)]);

		v = u0*(v0*m_vfV[IX(x0,y0)]+v1*m_vfV[IX(x0,y1)])+
			u1*(v0*m_vfV[IX(x1,y0)]+v1*m_vfV[IX(x1,y1)]);

		pos.x += u * timePass * PARTICLE_MOVE_SPEED; pos.z += v * timePass * PARTICLE_MOVE_SPEED;
		//Ogre::Vector2 nor(m_vfU[index], m_vfV[index]);
		//nor.normalise();
		//nor *= timePass * PARTICLE_MOVE_SPEED;
		//pos.x += nor.x;
		//pos.z += nor.y;

		//index = ((int)pos.x)+(m_iGridNumber+2)*((int)pos.z);
		//if(m_vbIntersectGrid[index])
		//{
		//	Ogre::Vector3 dir = pos - m_v3FishPos;
		//	dir.normalise();
		//	dir *= timePass * PARTICLE_MOVE_SPEED;
		//	pos.x += dir.x;
		//	pos.z += dir.z;
		//}
		particle->position = pos;
	}
	if(m_bAddForce)
	{
		float dirX;
		float dirZ;
		float halfCount = PARTICLE_COUNT / 2;
		for(int i = 0; i < PARTICLE_COUNT; i++)
		{
			particle = m_pPS->createParticle();
			if(particle != NULL)
			{
				if(halfCount >= i)
					dirX = PARTICLE_X_BEG + ((halfCount-i) * PARTICLE_X_DIFF);
				else
					dirX = PARTICLE_X_BEG + ((i-halfCount) * PARTICLE_X_DIFF);
				dirZ = PARTICLE_Z_BEG + (i * PARTICLE_Z_DIFF);
				particle->timeToLive = PARTICLE_LIVE_TIME;
				particle->setDimensions (PARTICLE_SIZE_X, PARTICLE_SIZE_Y);
				particle->position = m_pPS->getParentSceneNode()->getPosition() + m_pPS->getParentSceneNode()->getOrientation() * Ogre::Vector3(dirX, 0.0, dirZ) * FISH_SCALE_SIZE;
			}
		}

		//particle = m_pPS->createParticle();
		//if(particle != NULL)
		//{
		//	particle->timeToLive = PARTICLE_LIVE_TIME;
		//	particle->setDimensions (PARTICLE_SIZE_X, PARTICLE_SIZE_Y);
		//	particle->position = m_pPS->getParentSceneNode()->getPosition() + m_pPS->getParentSceneNode()->getOrientation() * Ogre::Vector3(-4.0, 0.0, -0.75) * FISH_SCALE_SIZE;
		//}
		//particle = m_pPS->createParticle();
		//if(particle != NULL)
		//{
		//	particle->timeToLive = PARTICLE_LIVE_TIME;
		//	particle->setDimensions (PARTICLE_SIZE_X, PARTICLE_SIZE_Y);
		//	particle->position = m_pPS->getParentSceneNode()->getPosition() + m_pPS->getParentSceneNode()->getOrientation() * Ogre::Vector3(-4.25, 0.0, -0.5) * FISH_SCALE_SIZE;
		//}
		//particle = m_pPS->createParticle();
		//if(particle != NULL)
		//{
		//	particle->timeToLive = PARTICLE_LIVE_TIME;
		//	particle->setDimensions (PARTICLE_SIZE_X, PARTICLE_SIZE_Y);
		//	particle->position = m_pPS->getParentSceneNode()->getPosition() + m_pPS->getParentSceneNode()->getOrientation() * Ogre::Vector3(-4.5, 0.0, 0) * FISH_SCALE_SIZE;
		//}
		//particle = m_pPS->createParticle();
		//if(particle != NULL)
		//{
		//	particle->timeToLive = PARTICLE_LIVE_TIME;
		//	particle->setDimensions (PARTICLE_SIZE_X, PARTICLE_SIZE_Y);
		//	particle->position = m_pPS->getParentSceneNode()->getPosition() + m_pPS->getParentSceneNode()->getOrientation() * Ogre::Vector3(-4.25, 0.0, 0.5) * FISH_SCALE_SIZE;
		//}
		//particle = m_pPS->createParticle();
		//if(particle != NULL)
		//{
		//	particle->timeToLive = PARTICLE_LIVE_TIME;
		//	particle->setDimensions (PARTICLE_SIZE_X, PARTICLE_SIZE_Y);
		//	particle->position = m_pPS->getParentSceneNode()->getPosition() + m_pPS->getParentSceneNode()->getOrientation() * Ogre::Vector3(-4.0, 0.0, 0.75) * FISH_SCALE_SIZE;
		//}
	}

	if(m_bExternlForce)
	{
		//particle = m_pPS->createParticle();
		//if(particle != NULL)
		//{
		//	particle->timeToLive = PARTICLE_LIVE_TIME;
		//	particle->setDimensions (PARTICLE_SIZE_X, PARTICLE_SIZE_Y);
		//	particle->position.x = m_iGridNumber/2;
		//	particle->position.y = 0;
		//	particle->position.z = m_iGridNumber - 20;
		//}
		for(int i = m_iGridNumber/2 - 2; i < m_iGridNumber/2 + 2;i++)
		{
			particle = m_pPS->createParticle();
			if(particle != NULL)
			{
				particle->timeToLive = PARTICLE_LIVE_TIME;
				particle->setDimensions (PARTICLE_SIZE_X, PARTICLE_SIZE_Y);
				particle->position.x = i;
				particle->position.y = 0;
				particle->position.z = m_iGridNumber + EXTERNAL_FORCE_Y_POS_DIFF;
			}
		}
	}
}

void StableFluidsGrid::updateMesh(float timePass)
{
	m_fLastFrameTime = timePass ;
	m_fLastTimeStamp += timePass ;

	if(m_bExternlForce)
		setExternalForce();

	vel_step ( m_iGridNumber, m_vfU, m_vfV, m_vfUPrev, m_vfVPrev, m_fVisc, timePass);
	//dens_step ( m_iGridNumber, m_vfDens, m_vfDensPrev, m_vfU, m_vfV, m_fDiff, timePass);

	m_iCurrentMap = (m_iCurrentMap + 1) % 3 ;
	float *buf = m_vfHeightMap[m_iCurrentMap] ; //new map
	float *buf1 = m_vfHeightMap[(m_iCurrentMap+2)%3] ;//current map
	float *buf2 = m_vfHeightMap[(m_iCurrentMap+1)%3] ;//prev map
	wave_step (m_iGridNumber+2, buf, buf1, buf2, m_vfDumpening, timePass, m_vfWaveVelocity);

	float *vfU = NULL, *vfV = NULL;

	switch(m_eVelocityType)
	{

	case DISPLAY_ORIGIN:
		vfU = m_vfU;
		vfV = m_vfV;
		break;
	case DISPLAY_ADD_FORCE:
		vfU = m_vfEnforceU;
		vfV = m_vfEnforceV;
		break;
	case DISPLAY_BOUNDARY:
		vfU = m_vfBoundaryU;
		vfV = m_vfBoundaryV;
		break;
	case DISPLAY_WAVE_EQUATION:
		vfU = m_vfWaveVelocity[VELOCITY_U];
		vfV = m_vfWaveVelocity[VELOCITY_V];
	default:
		break;
	}
	//if(vfU != NULL && vfV != NULL)
		updateDebug(vfU, vfV);
	//else
	//{
	//	if(m_pManuObj != NULL)
	//		m_pManuObj->clear();
	//}

	switch(m_eMapDisplayType)
	{
	case DISPLAY_BOOLEAN_GRID:
		m_pPixelBox->data =  m_vbIntersectGrid;
		m_heightMap->blitFromMemory(*m_pPixelBox);
		m_pMiniScreenNode->setVisible(true);
		break;
	case DISPLAY_DENSITY_MAP:
		m_pPixelBox->data =  m_vfHeightMap[m_iCurrentMap];
		m_heightMap->blitFromMemory(*m_pPixelBox);
		m_pMiniScreenNode->setVisible(true);
		break;
	default:
		m_pMiniScreenNode->setVisible(false);
		break;
	}

	setMeshBoundary();
	if(m_bAddForce)
	{
		setMeshEnforce(timePass);
		//vel_step ( m_iGridNumber, m_vfU, m_vfV, m_vfUPrev, m_vfVPrev, m_fVisc, timePass);
		setMeshBoundary();
	}
	
	updateParticle(timePass);
}

void StableFluidsGrid::updateDebug(float *vfU, float *vfV)
{
	int i, j;
	float x, y, h;
	int index;

	h = 1.0f/m_iGridNumber;
	if(m_pManuObj == NULL)
		return;
	m_pManuObj->clear();
	m_pManuObj->begin("ChinesePaint/Debug", Ogre::RenderOperation::OT_LINE_LIST);
	for(i = 1;i <= m_iGridNumber;i++)
	{
		
		x = i * h;//(i-0.5f)*h;
		for(j = 1;j <= m_iGridNumber;j++)
		{
			y = j * h;//(j-0.5f)*h;
			index = (i)+(m_iGridNumber+2)*(j);
			Ogre::Vector3 origin(x*m_iGridNumber, 0, y*m_iGridNumber);
			Ogre::Vector3 to((x+vfU[index])*m_iGridNumber, 0, (y+vfV[index])*m_iGridNumber);
			m_pManuObj->position(origin);
			m_pManuObj->position(to);
		}
	}
	m_pManuObj->end();
}

void StableFluidsGrid::push(int N, float x, float y, float addx, float addy, float depth, bool absolute, float *current)
{
	depth = depth * m_fLastFrameTime * ANIMATIONS_PER_SECOND ;
	float *vertex=current+((int)(y+addy)*N+(int)(x+addx));
	float diffy = y - floor(y+addy);
	float diffx = x - floor(x+addx);
	float dist=sqrt(diffy*diffy + diffx*diffx) ;
	float power = 1 - dist ;
	if (power<0)
		power = 0;
	if (absolute) 
		*vertex = depth*power ;
	else
		*vertex += depth*power ;
}

void StableFluidsGrid::push(float x, float y, float depth, bool absolute)//this function just for test
{
	unsigned int index = (x)+(m_iGridNumber+2)*(y);
	if(index > m_iGridSize)
		return;
	m_vfU[index] = 0;
	m_vfV[index] = -m_fForce * m_fLastFrameTime;
}


void StableFluidsGrid::clear(void)
{
	int i = 0;
	for ( i=0 ; i<m_iGridSize ; i++ ) 
	{
		m_vfU[i] = m_vfV[i] = m_vfUPrev[i] = m_vfVPrev[i] = /*m_vfDens[i] = m_vfDensPrev[i] =*/ 0.0f;
	}
}

//void StableFluidsGrid::updateMeshData(Ogre::SceneNode *node, Ogre::Entity *entity)
//{
//	//set particle system before fish head
//	m_v3FishPos = node->getPosition();
//	Ogre::Vector3 pos = m_v3FishPos + ( node->getOrientation() * Ogre::Vector3(-5.0, 0.0, 0.0) ) * FISH_SCALE_SIZE;
//	m_pPSNode->setPosition( node->getPosition());
//	m_pPSNode->setOrientation(node->getOrientation());
//
//	//node->setVisible(false);
//	//m_pPSNode->setVisible(false);
//
//	//when fish move front fluid will give back force
//	//Ogre::Vector3 dir = node->getOrientation() * Ogre::Vector3(1.0, 0.0, 0.0);
//	//dir.normalise();
//	//unsigned int index = ((int)pos.x)+((int)pos.z * (m_iGridNumber+2));
//	//m_vfU[index] = dir.x * m_fForce * 2 * m_fLastFrameTime;
//	//m_vfV[index] = dir.z * m_fForce * 2 * m_fLastFrameTime;
//	size_t prev_count = m_sVertexCount;
//	m_iCurrentVertex = (m_iCurrentVertex + 1) % 3 ;
//	getMeshInformation(entity, 
//		m_sVertexCount, 
//		m_vVertices[m_iCurrentVertex], 
//		m_sIndex_count,
//		m_vIndices, 
//		node->getPosition(), 
//		node->getOrientation(), 
//		node->getScale());
//
//	calcMeshFace();
//	//if(prev_count == m_sVertexCount)
//	//	calcMeshEnforce();
//}

void StableFluidsGrid::updateMeshData(SolidMesh *mesh, bool reset)
{
	//set particle system before fish head
	m_v3FishPos = mesh->getNode()->getPosition();
	Ogre::Vector3 pos = m_v3FishPos + ( mesh->getNode()->getOrientation() * Ogre::Vector3(-5.0, 0.0, 0.0) ) * FISH_SCALE_SIZE;
	m_pPSNode->setPosition( mesh->getNode()->getPosition());
	m_pPSNode->setOrientation(mesh->getNode()->getOrientation());

	calcMeshFace(mesh->getVertexCount(), mesh->getVertices(), reset);
	//if(m_bAddForce)
	//{
	//	//when fish move front fluid will give back force
	//	Ogre::Vector3 dir = mesh->getNode()->getOrientation() * Ogre::Vector3(1.0, 0.0, 0.0);
	//	dir.normalise();
	//	unsigned int index = ((int)pos.x)+((int)pos.z * (m_iGridNumber+2));
	//	m_vfU[index] = dir.x * m_fForce * m_fLastFrameTime;
	//	m_vfV[index] = dir.z * m_fForce * m_fLastFrameTime;
	//}
	calcMeshEnforce(mesh->getVertexCount(), mesh->getVertices(), mesh->getPrevVertices(), reset);
}

//void StableFluidsGrid::calcMeshFace()
//{
//	memset(m_vbIntersectGrid, 0, m_iGridSize * sizeof(m_vbIntersectGrid));
//	int i, x, y, ite;
//	unsigned int index;
//	bool hasEdge = false;
//	int front, back;
//	for(i = 0;i < m_sVertexCount;i++)
//	{
//		if(m_vVertices[m_iCurrentVertex][i].y < 1.0 && m_vVertices[m_iCurrentVertex][i].y > -1.0)//the y position at surface
//		{
//			x = ((int)m_vVertices[m_iCurrentVertex][i].x);
//			//if(m_vVertices[m_iCurrentVertex][i].x - x > 0.5)
//			//	x++;
//			y = ((int)m_vVertices[m_iCurrentVertex][i].z);
//			//if( m_vVertices[m_iCurrentVertex][i].z - y > 0.5)
//			//	y++;
//
//			index = x +  (y*(m_iGridNumber+2));
//			if(index < m_iGridSize)
//				m_vbIntersectGrid[index] = 1.0;//draw contour of object
//		}
//	}
//
//	for(y = 1; y < m_iGridNumber; y++)//fill contour hole
//	{
//		hasEdge = false;
//		for(x = 1; x < m_iGridNumber; x++)
//		{
//			index = x + (y*(m_iGridNumber+2));
//			if(m_vbIntersectGrid[index])
//			{
//				if(!hasEdge)
//				{
//					front = x;
//					hasEdge = true;
//				}
//				else
//				{
//					back = x;
//					for(ite = front+1; ite < back; ite++)
//					{
//						index = ite + (y*(m_iGridNumber+2));
//						m_vbIntersectGrid[index] = true;
//					}
//					front = x;
//				}
//			}
//		}
//	}
//}

void StableFluidsGrid::calcMeshFace(size_t verticesCount, Ogre::Vector3 *vertices, bool reset)
{
	if(reset)
		memset(m_vbIntersectGrid, 0, m_iGridSize * sizeof(m_vbIntersectGrid));

	if(vertices == NULL)
		return;
	int i, x, y, ite;
	unsigned int index;
	bool hasEdge = false;
	int front, back;
	for(i = 0;i < verticesCount;i++)
	{
		if(vertices[i].y < 1.0 && vertices[i].y > -1.0)//the y position at surface
		{
			x = ((int)vertices[i].x);

			y = ((int)vertices[i].z);

			index = x +  (y*(m_iGridNumber+2));
			if(index < m_iGridSize)
				m_vbIntersectGrid[index] = 1.0;//draw contour of object
		}
	}

	for(y = 1; y < m_iGridNumber; y++)//fill contour hole
	{
		hasEdge = false;
		for(x = 1; x < m_iGridNumber; x++)
		{
			index = x + (y*(m_iGridNumber+2));
			if(m_vbIntersectGrid[index])
			{
				if(!hasEdge)
				{
					front = x;
					hasEdge = true;
				}
				else
				{
					back = x;
					for(ite = front+1; ite < back; ite++)
					{
						index = ite + (y*(m_iGridNumber+2));
						m_vbIntersectGrid[index] = true;
					}
					front = x;
				}
			}
		}
	}
}

//void StableFluidsGrid::calcMeshEnforce()
//{
//	memset(m_vfEnforceU, 0, m_iGridSize * sizeof(m_vfEnforceU));
//	memset(m_viEnforceUCount, 0, m_iGridSize * sizeof(m_viEnforceUCount));
//	memset(m_vfEnforceV, 0, m_iGridSize * sizeof(m_vfEnforceV));
//	memset(m_viEnforceVCount, 0, m_iGridSize * sizeof(m_viEnforceVCount));
//	unsigned int index;
//	int prev  = (m_iCurrentVertex + 2) %3;
//	if(m_vVertices[prev] == NULL)
//		return;
//	int i;
//	for(i = 0;i < m_sVertexCount;i++)
//	{
//		if(m_vVertices[prev][i].y < 1.0 && m_vVertices[prev][i].y > -1.0)
//		{
//			index = ((int)m_vVertices[prev][i].x)  +  ((int)m_vVertices[prev][i].z)*(m_iGridNumber+2);
//			m_vfEnforceU[index] = m_vVertices[m_iCurrentVertex][i].x - m_vVertices[prev][i].x;
//			m_viEnforceUCount[index]++;
//			m_vfEnforceV[index] = m_vVertices[m_iCurrentVertex][i].z - m_vVertices[prev][i].z;
//			m_viEnforceVCount[index]++;
//		}
//	}
//}

void StableFluidsGrid::calcMeshEnforce(size_t verticesCount, Ogre::Vector3 *vertices, Ogre::Vector3 *prev, bool reset)
{
	unsigned int index;
	if(reset)
	{
		memset(m_vfEnforceU, 0, m_iGridSize * sizeof(m_vfEnforceU));
		memset(m_viEnforceUCount, 0, m_iGridSize * sizeof(m_viEnforceUCount));
		memset(m_vfEnforceV, 0, m_iGridSize * sizeof(m_vfEnforceV));
		memset(m_viEnforceVCount, 0, m_iGridSize * sizeof(m_viEnforceVCount));
	}


	if(prev == NULL)
		return;
	int i;
	for(i = 0;i < verticesCount;i++)
	{
		if(prev[i].y < 1.0 && prev[i].y > -1.0)
		{
			index = ((int)prev[i].x)  +  ((int)prev[i].z)*(m_iGridNumber+2);
			m_vfEnforceU[index] = vertices[i].x - prev[i].x;
			m_viEnforceUCount[index]++;
			m_vfEnforceV[index] = vertices[i].z - prev[i].z;
			m_viEnforceVCount[index]++;
		}
	}

	//for(i = 0;i < m_iGridSize;i++)
	//{
	//	if(m_viEnforceUCount[i] > 0)
	//		m_vfEnforceU[i] = m_vfEnforceU[i] / m_viEnforceUCount[i];
	//	if(m_viEnforceVCount[i] > 0)
	//		m_vfEnforceV[i] = m_vfEnforceV[i] / m_viEnforceVCount[i];
	//}
}

void StableFluidsGrid::setMeshBoundary(bool setDenstity)
{
	memset(m_vfBoundaryU, 0, m_iGridSize * sizeof(m_vfBoundaryU));
	memset(m_vfBoundaryV, 0, m_iGridSize * sizeof(m_vfBoundaryV));
	int x, y;
	unsigned int index, up, down, left, right;
	for(y = 1; y < m_iGridNumber; y++)
	{
		for(x = 1; x < m_iGridNumber; x++)
		{
			index = x + (y*(m_iGridNumber+2));
			if(m_vbIntersectGrid[index])
			{
				//set fish model to density field
				if(setDenstity)
				{
					push(m_iGridNumber+2, x, y, 0, 0, FISH_DEPTH, false, m_vfHeightMap[m_iCurrentMap]);
					push(m_iGridNumber+2, x, y, 0, 1, FISH_DEPTH, false, m_vfHeightMap[m_iCurrentMap]);
					push(m_iGridNumber+2, x, y, 1, 0, FISH_DEPTH, false, m_vfHeightMap[m_iCurrentMap]);
					push(m_iGridNumber+2, x, y, 1, 1, FISH_DEPTH, false, m_vfHeightMap[m_iCurrentMap]);
				}

				//set the velocity inside model to zero
				m_vfU[index] = 0;
				m_vfV[index] = 0;
			}
		}
	}

	for(y = 1; y < m_iGridNumber; y++)
	{
		for(x = 1; x < m_iGridNumber; x++)
		{
			up = x + ((y-1)*(m_iGridNumber+2));
			index = x + (y*(m_iGridNumber+2));
			down = x + ((y+1)*(m_iGridNumber+2));
			left = (x-1) + ((y)*(m_iGridNumber+2));
			right =  (x+1) + ((y)*(m_iGridNumber+2));
			if(m_vbIntersectGrid[index])
			{
				//set the velocity by neighbor velocity
				//m_vfBoundaryU[index] = (/*m_vfU[up]+m_vfU[down]+*/-m_vfU[left]+m_vfU[right]);
				//m_vfBoundaryV[index] = (-m_vfV[up]+m_vfV[down]/*+m_vfV[left]+m_vfV[right]*/);
				m_vfBoundaryU[index] = (m_vfU[up]+m_vfU[down]-(m_vfU[left]+m_vfU[right]));
				m_vfBoundaryV[index] = (-(m_vfV[up]+m_vfV[down])+m_vfV[left]+m_vfV[right]);
				//	x[IX(0  ,i)] = b==1 ? -x[IX(1,i)] : x[IX(1,i)];
				//	x[IX(N+1,i)] = b==1 ? -x[IX(N,i)] : x[IX(N,i)];
			}
		}
	}

	for(y = 1; y < m_iGridNumber; y++)
	{
		for(x = 1; x < m_iGridNumber; x++)
		{
			index = x + (y*(m_iGridNumber+2));
			if(m_vbIntersectGrid[index])
			{
				//set boundary velocity to velocity field
				m_vfU[index] = m_vfBoundaryU[index];
				m_vfV[index] = m_vfBoundaryV[index];
			}
		}
	}
}

void StableFluidsGrid::setMeshEnforce(float timePass)
{
	//return;
	//int x, y, count;
	//float uforce, vforce;
	//Ogre::Vector2 diff;
	//unsigned int index, dIndex;
	//for(y = 1; y < m_iGridNumber; y++)
	//{
	//	for(x = 1; x < m_iGridNumber; x++)
	//	{
	//		index = x + (y*(m_iGridNumber+2));
	//		uforce = 0;//m_vfU[index];
	//		vforce = 0;//m_vfV[index];
	//		diff.x = 0;
	//		diff.y = 0;
	//		if(m_vfEnforceU[index] != 0)
	//		{
	//			//count = m_viEnforceUCount[index];
	//			//force = m_vfEnforceU[index];
	//			//force = force / count;
	//			//m_vfU[index] += force;
	//			count = m_viEnforceUCount[index];
	//			uforce = m_vfEnforceU[index] / count * timePass;
	//			diff.x = uforce;
	//			//uforce *= 10; 
	//		}
	//		if(m_vfEnforceV[index] != 0)
	//		{
	//			//count = m_viEnforceVCount[index];
	//			//force = m_vfEnforceV[index];
	//			//force = force / count;
	//			//m_vfV[index] += force;
	//			count = m_viEnforceVCount[index];
	//			vforce = m_vfEnforceV[index] / count * timePass;
	//			diff.y = vforce;
	//			//vforce *= 10;
	//		}
	//		diff.normalise();
	//		//if(diff.x > 0.5)
	//		//	diff.x = 1;
	//		//else if(diff.x < -0.5)
	//		//	diff.x = -1;
	//		//if(diff.y > 0.5)
	//		//	diff.y = 1;
	//		//else if(diff.y < -0.5)
	//		//	diff.y = -1;
	//		diff.x += x;
	//		diff.y += y;
	//		dIndex = (int)diff.x + ((int)diff.y*(m_iGridNumber+2));
	//		m_vfU[dIndex] += -uforce;
	//		m_vfV[dIndex] += -vforce;
	//	}
	//}

	int x, y;
	float up, down, left, right;
	unsigned int index, uIndex, dIndex, rIndex, lIndex;
	for(y = 1; y < m_iGridNumber; y++)
	{
		for(x = 1; x < m_iGridNumber; x++)
		{
			index = x + (y*(m_iGridNumber+2));
			uIndex = x + ((y-1)*(m_iGridNumber+2));
			dIndex = x + ((y+1)*(m_iGridNumber+2));
			lIndex = (x-1) + ((y)*(m_iGridNumber+2));
			rIndex =  (x+1) + ((y)*(m_iGridNumber+2));
			//U
			up = down = left = right = 0;
			if(m_viEnforceUCount[uIndex] > 0)
				up  = m_vfEnforceU[uIndex] / m_viEnforceUCount[uIndex];
			if(m_viEnforceUCount[dIndex] > 0)
				down  = m_vfEnforceU[dIndex] / m_viEnforceUCount[dIndex];
			if(m_viEnforceUCount[lIndex] > 0)
				up  = m_vfEnforceU[lIndex] / m_viEnforceUCount[lIndex];
			if(m_viEnforceUCount[rIndex] > 0)
				down  = m_vfEnforceU[rIndex] / m_viEnforceUCount[rIndex];
			if(up || down || left || right)
				m_vfU[index] = ((up+down)-(left+right)) / timePass * FORCE_STRENGTH;
			//V
			up = down = left = right = 0;
			if(m_viEnforceVCount[uIndex] > 0)
				up  = m_vfEnforceV[uIndex] / m_viEnforceVCount[uIndex];
			if(m_viEnforceVCount[dIndex] > 0)
				down  = m_vfEnforceV[dIndex] / m_viEnforceVCount[dIndex];
			if(m_viEnforceVCount[lIndex] > 0)
				up  = m_vfEnforceV[lIndex] / m_viEnforceVCount[lIndex];
			if(m_viEnforceVCount[rIndex] > 0)
				down  = m_vfEnforceV[rIndex] / m_viEnforceVCount[rIndex];
			if(up || down || left || right)
				m_vfV[index] = ((up+down)+(left+right)) / timePass * FORCE_STRENGTH;
		}
	}
}

void StableFluidsGrid::setExternalForce()
{
	unsigned int index = m_iGridNumber/2 + ((m_iGridNumber + EXTERNAL_FORCE_Y_POS_DIFF) * (m_iGridNumber+2));
	//m_vfU[index] = -1 * m_fForce * m_fLastFrameTime;
	m_vfV[index] = -1 * m_fForce * m_fLastFrameTime;

	//for(int i = m_iGridNumber/2 - 2; i < m_iGridNumber/2 + 2;i++)
	//{
	//	unsigned int index = i + ((m_iGridNumber - 20) * (m_iGridNumber+2));
	//	//m_vfU[index] = -1 * m_fForce * m_fLastFrameTime;
	//	m_vfV[index] = -1 * m_fForce * m_fLastFrameTime;
	//}
}

unsigned int StableFluidsGrid::IX(int x, int y)
{
	return x+(m_iGridNumber+2)*y;
}