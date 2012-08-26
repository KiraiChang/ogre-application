#include "stable_fluids_grid.h"

#include <iostream>
#include <OgreManualObject.h>
#include <OgreSceneManager.h>

extern void dens_step ( int N, float * x, float * x0, float * u, float * v, float diff, float dt );
extern void vel_step ( int N, float * u, float * v, float * u0, float * v0, float visc, float dt );

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
	m_pManuObj(NULL),
	m_pManualNode(NULL),
	m_pSceneMgr(NULL)
{
	m_iGridSize = (m_iGridNumber+2)*(m_iGridNumber+2);
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
	//m_vfDens		= new float[m_iGridSize];
	//m_vfDensPrev	= new float[m_iGridSize];
	clear();
	m_pManuObj = m_pSceneMgr->createManualObject();
	m_pManualNode = m_pSceneMgr->getRootSceneNode()->
		createChildSceneNode(Ogre::Vector3(0.0f, 0.0f, 0.0f));
	m_pManualNode->attachObject(m_pManuObj);
}

void StableFluidsGrid::release(void)
{
	if ( m_vfU ) delete ( m_vfU );
	if ( m_vfV ) delete ( m_vfV );
	if ( m_vfUPrev ) delete ( m_vfUPrev );
	if ( m_vfVPrev ) delete ( m_vfVPrev );
	//if ( m_vfDens ) delete ( m_vfDens );
	//if ( m_vfDensPrev ) delete ( m_vfDensPrev );
	if(m_pSceneMgr != NULL)
	{
		if(m_pManualNode != NULL)
		{
			m_pSceneMgr->destroySceneNode(m_pManualNode);
			m_pManualNode = NULL;
		}
		if(m_pManuObj != NULL)
		{
			m_pSceneMgr->destroyManualObject(m_pManuObj);
			m_pManuObj = NULL;
		}
	}
}

void StableFluidsGrid::updateMesh(float timePass)
{
	vel_step ( m_iGridNumber, m_vfU, m_vfV, m_vfUPrev, m_vfVPrev, m_fVisc, timePass);
	//dens_step ( m_iGridNumber, m_vfDens, m_vfDensPrev, m_vfU, m_vfV, m_fDiff, timePass);
	updateDebug();

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
			Ogre::Vector3 to((x+m_vfU[index])*m_iGridNumber, 0, (y+m_vfU[index])*m_iGridNumber);
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

void StableFluidsGrid::push(float x, float y, float depth, bool absolute)
{
	unsigned int index = (x)+(m_iGridNumber+2)*(y);
	if(index > m_iGridSize)
		return;
	m_vfU[index] = m_fForce;
	m_vfU[index] = -m_fForce;
}

void StableFluidsGrid::clear(void)
{
	int i = 0;
	for ( i=0 ; i<m_iGridSize ; i++ ) 
	{
		m_vfU[i] = m_vfV[i] = m_vfUPrev[i] = m_vfVPrev[i] = /*m_vfDens[i] = m_vfDensPrev[i] =*/ 0.0f;
	}
}