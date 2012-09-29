#include "solid_mesh.h"
#include "../ogre_tool/ogre_mesh_tool.h"

SolidMesh::SolidMesh(const std::string &solidName, 
	const std::string &meshName, 
	Ogre::SceneManager *mgr):m_pEnt(NULL),
								m_pNode(NULL),
								m_vIndices(NULL),
								m_pAniState(NULL),
								m_pSceneMgr(mgr)
{
	for(int i = 0; i < MAX_MAP_COUNT; i++)
	{
		m_vVertices[i] = NULL;
		//m_vIndices[i] = NULL;
	}
	init(solidName, meshName);
}

SolidMesh::~SolidMesh(void)
{
	release();
}

void SolidMesh::init(const std::string &solidName, 
	const std::string &meshName)
{
	release();
	m_pNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
	m_pEnt = m_pSceneMgr->createEntity(solidName, meshName);
	m_pNode->attachObject(m_pEnt);
}

void SolidMesh::release(void)
{
	if(m_pNode != NULL)
	{
		m_pNode->detachObject(m_pEnt);
		m_pSceneMgr->destroySceneNode(m_pNode);
		m_pNode = NULL;
	}
	if(m_pEnt != NULL)
	{
		m_pSceneMgr->destroyEntity( m_pEnt );
		m_pEnt = NULL;
	}

	m_sVertexCount = 0;
	m_sIndexCount = 0;
	m_iCurrentMap = 0;
	if(m_vIndices != NULL)
	{
		delete [] m_vIndices;
		m_vIndices = NULL;
	}
	for(int i = 0; i < MAX_MAP_COUNT; i++)
	{
		if(m_vVertices[i] != NULL)
		{
			delete [] m_vVertices[i];
			m_vVertices[i] = NULL;
		}
		//if(m_vIndices[i] != NULL)
		//{
		//	delete [] m_vIndices[i];
		//	m_vIndices[i] = NULL;
		//}
	}
}

void SolidMesh::update(float timePass)
{
	if(m_pAniState != NULL)
		m_pAniState->addTime(timePass);

	m_iCurrentMap = (m_iCurrentMap + 1) % 3 ;
	getMeshInformation(m_pEnt, 
		m_sVertexCount, 
		m_vVertices[m_iCurrentMap], 
		m_sIndexCount,
		m_vIndices,//[m_iCurrentMap], 
		m_pNode->getPosition(), 
		m_pNode->getOrientation(), 
		m_pNode->getScale());
}

void SolidMesh::setAnimation(const std::string &aniName, bool enable, bool loop)
{
	m_pAniState = m_pEnt->getAnimationState(aniName);
	m_pAniState->setEnabled(enable);
	m_pAniState->setLoop(loop);
}

size_t SolidMesh::getVertexCount(void)
{
	return m_sVertexCount;
}

size_t SolidMesh::getIndexCount(void)
{
	return m_sVertexCount;
}

Ogre::Vector3* SolidMesh::getVertices(void)
{
	return m_vVertices[m_iCurrentMap];
}

Ogre::Vector3* SolidMesh::getPrevVertices(void)
{
	int prev  = (m_iCurrentMap + 2) %3;
	return m_vVertices[prev];
}

unsigned long* SolidMesh::getIndices(void)
{
	return m_vIndices;//[m_iCurrentMap];
}

//unsigned long* SolidMesh::getPrevIndices(void)
//{
//	int prev  = (m_iCurrentMap + 2) %3;
//	return m_vIndices[prev];
//}

Ogre::SceneNode * SolidMesh::getNode(void)
{
	return m_pNode;
}

