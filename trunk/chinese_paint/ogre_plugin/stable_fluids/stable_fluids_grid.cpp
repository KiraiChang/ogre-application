#include "stable_fluids_grid.h"
#include "../ogre_tool/ogre_mesh_tool.h"
#include <iostream>

extern void dens_step ( int N, float * x, float * x0, float * u, float * v, float diff, float dt );
extern void vel_step ( int N, float * u, float * v, float * u0, float * v0, float visc, float dt );
extern void set_bnd ( int N, int b, float * x );
extern void wave_step ( int N, float * buf, float * buf1, float * buf2, float *dampening, float timePass );

static unsigned int MAX_HEIGHT_MAP_COUNT = 3;
static unsigned int MAX_VERTEX_MAP_COUNT = 3;

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
	m_sVertexCount(0),
	m_sIndex_count(0),
	m_vIndices(NULL),
	m_iCurrentVertex(0),
	m_vfEnforceU(0),
	m_viEnforceUCount(0),
	m_vfEnforceV(0),
	m_viEnforceVCount(0),
	m_pSceneMgr(NULL)
{
	m_iGridSize = (m_iGridNumber+2)*(m_iGridNumber+2);
	for(int i = 0; i < MAX_HEIGHT_MAP_COUNT;i++)
	{
		m_vfHeightMap[i] = 0;
		if(i < MAX_VERTEX_MAP_COUNT)
			m_vVertices[i] = 0;
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

	m_pPS = m_pSceneMgr->createParticleSystem();
	m_pPSNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
	m_pPSNode->attachObject(m_pPS);
	m_pPSNode->setPosition(32, 0, 40);
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
		if(m_vfHeightMap[i] != NULL)
		{
			delete m_vfHeightMap[i];
			m_vfHeightMap[i] = 0;
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

	for(int i = 0;i < MAX_VERTEX_MAP_COUNT;i++)
	{
		if(m_vVertices[i] != NULL)
		{
			delete[] m_vVertices[i];
			m_vVertices[i] = NULL;
		}
	}

	if(m_vIndices != NULL)
	{
		delete[] m_vIndices;
		m_vIndices = NULL;
	}

	Ogre::TextureManager::getSingleton().remove("heightSampler");
}

void StableFluidsGrid::updateParticle(float timePass)
{
	Ogre::Particle* particle = NULL;
	unsigned int index;
	Ogre::ParticleIterator ite = m_pPS->_getIterator();
	while(!ite.end())
	{
		particle = ite.getNext();
		Ogre::Vector3 pos = particle->position;
		index = ((int)pos.x)+(m_iGridNumber+2)*((int)pos.z);
		if(index > m_iGridSize)
			break;
		pos.x += m_vfU[index];
		pos.z += m_vfV[index];
		particle->position = pos;
	}
	particle = m_pPS->createParticle();
	if(particle != NULL)
	{
		particle->totalTimeToLive = 5;
		particle->setDimensions (0.5, 0.5);
		particle->position = m_pPS->getParentSceneNode()->getPosition();
	}
}

void StableFluidsGrid::updateMesh(float timePass)
{
	vel_step ( m_iGridNumber, m_vfU, m_vfV, m_vfUPrev, m_vfVPrev, m_fVisc, timePass);
	//dens_step ( m_iGridNumber, m_vfDens, m_vfDensPrev, m_vfU, m_vfV, m_fDiff, timePass);

	m_iCurrentMap = (m_iCurrentMap + 1) % 3 ;
	float *buf = m_vfHeightMap[m_iCurrentMap] ; // +1 for Y coordinate
	float *buf1 = m_vfHeightMap[(m_iCurrentMap+2)%3] ;
	float *buf2 = m_vfHeightMap[(m_iCurrentMap+1)%3] ;
	wave_step (m_iGridNumber+2, buf, buf1, buf2, m_vfDumpening, timePass);
	updateDebug();
	//m_pPixelBox->data =  m_vfHeightMap[m_iCurrentMap];
	m_pPixelBox->data =  m_vbIntersectGrid;
	m_heightMap->blitFromMemory(*m_pPixelBox);

	setMeshBoundary();
	setMeshEnforce(timePass);
	updateParticle(timePass);
}

void StableFluidsGrid::updateDebug()
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
		
		x = (i-0.5f)*h;
		for(j = 1;j <= m_iGridNumber;j++)
		{
			y = (j-0.5f)*h;
			index = (i)+(m_iGridNumber+2)*(j);
			Ogre::Vector3 origin(x*m_iGridNumber, 0, y*m_iGridNumber);
			Ogre::Vector3 to((x+m_vfU[index])*m_iGridNumber, 0, (y+m_vfV[index])*m_iGridNumber);
			m_pManuObj->position(origin);
			m_pManuObj->position(to);
		}
	}
	m_pManuObj->end();

	//m_pManuObj->begin("ChinesePaint/Debug", Ogre::RenderOperation::OT_LINE_LIST);
	//m_pManuObj->position(0.0, 0.0, 0.0);
	//m_pManuObj->position(100.0, 0.0, 100.0);
	//m_pManuObj->end();
}

void push(int N, float x, float y, float addx, float addy, float depth, bool absolute, float *current)
{
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

void StableFluidsGrid::push(float x, float y, float depth, bool absolute)
{
	unsigned int index = (x)+(m_iGridNumber+2)*(y);
	if(index > m_iGridSize)
		return;
	m_vfU[index] = 0;
	m_vfV[index] = -m_fForce;

	::push(m_iGridNumber+2, x, y, 0, 0, depth, absolute, m_vfHeightMap[m_iCurrentMap]);
	::push(m_iGridNumber+2, x, y, 0, 1, depth, absolute, m_vfHeightMap[m_iCurrentMap]);
	::push(m_iGridNumber+2, x, y, 1, 0, depth, absolute, m_vfHeightMap[m_iCurrentMap]);
	::push(m_iGridNumber+2, x, y, 1, 1, depth, absolute, m_vfHeightMap[m_iCurrentMap]);
}


void StableFluidsGrid::clear(void)
{
	int i = 0;
	for ( i=0 ; i<m_iGridSize ; i++ ) 
	{
		m_vfU[i] = m_vfV[i] = m_vfUPrev[i] = m_vfVPrev[i] = /*m_vfDens[i] = m_vfDensPrev[i] =*/ 0.0f;
	}
}

void StableFluidsGrid::updateMeshData(Ogre::SceneNode *node, Ogre::Entity *entity)
{
	//set particle system before fish head
	//Ogre::Vector3 pos = node->getPosition();
	//pos = pos + ( node->getOrientation() * Ogre::Vector3(-1.0, 0.0, 0.0) ) * 5;
	//m_pPSNode->setPosition(pos);
	m_iCurrentVertex = (m_iCurrentVertex + 1) % 3 ;
	getMeshInformation(entity, 
		m_sVertexCount, 
		m_vVertices[m_iCurrentVertex], 
		m_sIndex_count,
		m_vIndices, 
		node->getPosition(), 
		node->getOrientation(), 
		node->getScale());

	calcMeshFace();
	calcMeshEnforce();
}

void StableFluidsGrid::calcMeshFace()
{
	memset(m_vbIntersectGrid, 0, m_iGridSize * sizeof(m_vbIntersectGrid));
	int i, x, y, ite;
	unsigned int index;
	bool hasEdge = false;
	int front, back;
	for(i = 0;i < m_sVertexCount;i++)
	{
		if(m_vVertices[m_iCurrentVertex][i].y < 1.0 && m_vVertices[m_iCurrentVertex][i].y > -1.0)
		{
			x = ((int)m_vVertices[m_iCurrentVertex][i].x);
			if(m_vVertices[m_iCurrentVertex][i].x - x > 0.5)
				x++;
			y = ((int)m_vVertices[m_iCurrentVertex][i].z);
			if( m_vVertices[m_iCurrentVertex][i].z - y > 0.5)
				y++;

			index = x +  (y*(m_iGridNumber+2));
			if(index < m_iGridSize)
				m_vbIntersectGrid[index] = true;//draw contour of object
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
					for(ite = front; ite < back; ite++)
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

void StableFluidsGrid::calcMeshEnforce()
{
	memset(m_vfEnforceU, 0, m_iGridSize * sizeof(m_vfEnforceU));
	memset(m_viEnforceUCount, 0, m_iGridSize * sizeof(m_viEnforceUCount));
	memset(m_vfEnforceV, 0, m_iGridSize * sizeof(m_vfEnforceV));
	memset(m_viEnforceVCount, 0, m_iGridSize * sizeof(m_viEnforceVCount));
	unsigned int index;
	int prev  = (m_iCurrentVertex + 2) %3;
	if(m_vVertices[prev] == NULL)
		return;
	int i;
	for(i = 0;i < m_sVertexCount;i++)
	{
		if(m_vVertices[prev][i].y < 1.0 && m_vVertices[prev][i].y > -1.0)
		{
			index = ((int)m_vVertices[prev][i].x)  +  ((int)m_vVertices[prev][i].z)*(m_iGridNumber+2);
			m_vfEnforceU[index] = m_vVertices[m_iCurrentVertex][i].x - m_vVertices[prev][i].x;
			m_viEnforceUCount[index]++;
			m_vfEnforceV[index] = m_vVertices[m_iCurrentVertex][i].z - m_vVertices[prev][i].z;
			m_viEnforceVCount[index]++;
		}
	}
}

void StableFluidsGrid::setMeshBoundary()
{
	int x, y;
	unsigned int index, up, down, left, right;
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
				m_vfHeightMap[m_iCurrentMap][index] = 0;
				if(!m_vbIntersectGrid[left] && !m_vbIntersectGrid[right])
				{
					if(!m_vbIntersectGrid[down] && !m_vbIntersectGrid[up])
					{
						m_vfU[index] = -((m_vfU[down]+m_vfU[down] + m_vfU[left] + m_vfU[right])*0.25);
						m_vfV[index] = -((m_vfV[down]+m_vfV[down] + m_vfV[left] + m_vfV[right])*0.25);
					}
					else if(!m_vbIntersectGrid[up])
					{
						m_vfU[index] = -((m_vfU[up]*0.5)+((m_vfU[left] + m_vfU[right])*0.25));
						m_vfV[index] = -((m_vfV[up]*0.5)+((m_vfV[left] + m_vfV[right])*0.25));
					}
					else if(!m_vbIntersectGrid[down])
					{
						m_vfU[index] = -((m_vfU[down]*0.5)+((m_vfU[left] + m_vfU[right])*0.25));
						m_vfV[index] = -((m_vfV[down]*0.5)+((m_vfV[left] + m_vfV[right])*0.25));
					}
					else
					{
						m_vfU[index] = -((m_vfU[left] + m_vfU[right])*0.5);
						m_vfV[index] = -((m_vfV[left] + m_vfV[right])*0.5);
					}
				}
				else if(!m_vbIntersectGrid[left])
				{
					if(!m_vbIntersectGrid[down] && !m_vbIntersectGrid[up])
					{
						m_vfU[index] = -(((m_vfU[down]+m_vfU[down])*0.25) + (m_vfU[left]*0.5));
						m_vfV[index] = -(((m_vfU[down]+m_vfU[down])*0.25) + (m_vfV[left]*0.5));
					}
					else if(!m_vbIntersectGrid[up])
					{
						m_vfU[index] = -((m_vfU[up]+m_vfU[left])*0.5);
						m_vfV[index] = -((m_vfV[up]+m_vfV[left])*0.5);
					}
					else if(!m_vbIntersectGrid[down])
					{
						m_vfU[index] = -((m_vfU[down]+m_vfU[left])*0.5);
						m_vfV[index] = -((m_vfV[down]+m_vfV[left])*0.5);
					}
					else
					{
						m_vfU[index] = -m_vfU[left];
						m_vfV[index] = -m_vfV[left];
					}
				}
				else if(!m_vbIntersectGrid[right])
				{
					if(!m_vbIntersectGrid[down] && !m_vbIntersectGrid[up])
					{
						m_vfU[index] = -(((m_vfU[down]+m_vfU[down])*0.25) + (m_vfU[right]*0.5));
						m_vfV[index] = -(((m_vfU[down]+m_vfU[down])*0.25) + (m_vfV[right]*0.5));
					}
					else if(!m_vbIntersectGrid[up])
					{
						m_vfU[index] = -((m_vfU[up]+m_vfU[right])*0.5);
						m_vfV[index] = -((m_vfV[up]+m_vfV[right])*0.5);
					}
					else if(!m_vbIntersectGrid[down])
					{
						m_vfU[index] = -((m_vfU[down]+m_vfU[right])*0.5);
						m_vfV[index] = -((m_vfV[down]+m_vfV[right])*0.5);
					}
					else
					{
						m_vfU[index] = -m_vfU[right];
						m_vfV[index] = -m_vfV[right];
					}
				}
				else
				{
					if(!m_vbIntersectGrid[up])
					{
						m_vfU[index] = -m_vfU[up];
						m_vfV[index] = -m_vfV[up];
					}
					else if(!m_vbIntersectGrid[down])
					{
						m_vfU[index] = -m_vfU[down];
						m_vfV[index] = -m_vfV[down];
					}
					else
					{
						m_vfU[index] = 0;
						m_vfV[index] = 0;
					}
				}
			}
		}
	}
}

void StableFluidsGrid::setMeshEnforce(float timePass)
{
	int x, y;
	unsigned int index;
	for(y = 1; y < m_iGridNumber; y++)
	{
		for(x = 1; x < m_iGridNumber; x++)
		{
			index = x + (y*(m_iGridNumber+2));
			if(m_viEnforceUCount[index] > 0)
				m_vfU[index] += m_vfEnforceU[index] / m_viEnforceUCount[index];
			if(m_viEnforceVCount[index] > 0)
				m_vfV[index] += m_vfEnforceV[index] / m_viEnforceVCount[index];
		}
	}
}