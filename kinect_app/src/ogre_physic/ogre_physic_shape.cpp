#include "ogre_physic_shape.h"
#include <OgreSceneManager.h>
#include <OgreEntity.h>

//*******************************************************
//********************  BOX  ****************************
//*******************************************************
OgreShapeBox::OgreShapeBox(Ogre::SceneManager *scene):
		PhysicShapeBox(), 
		m_pSceneMgr(scene),
		m_pBodyNode(NULL),
		m_pBodyEnt(NULL)
{
}

OgreShapeBox::~OgreShapeBox(void)
{
	release();
}

void OgreShapeBox::init(unsigned int id, const char *meshName, float *scale)
{	
	if(m_pBodyNode == NULL)
	{
		char name[64];
		sprintf(name, "m_pBodyEnt_%d", id);
		m_pBodyEnt = m_pSceneMgr->createEntity(name, meshName);

		sprintf(name, "m_pBodyNode%d", id);
		m_pBodyNode = m_pSceneMgr->getRootSceneNode()->
			createChildSceneNode(name, Ogre::Vector3(0.0f, 0.0f, 0.0f));
		m_pBodyNode->attachObject(m_pBodyEnt);

		m_pBodyNode->setScale(Ogre::Vector3(scale));

		Ogre::Vector3 size = m_pBodyEnt->getBoundingBox().getSize();
		setData(SIZE_X, size.x * scale[0]);
		setData(SIZE_Y, size.y * scale[1]);
		setData(SIZE_Z, size.z * scale[2]);
	}
}

void OgreShapeBox::release(void)
{
	if(m_pSceneMgr)
	{
		if(m_pBodyEnt != NULL)
		{
			if(m_pBodyNode)
				m_pBodyNode->detachObject(m_pBodyEnt);

			m_pSceneMgr->destroyEntity(m_pBodyEnt);
			m_pBodyEnt = NULL;
		}

		if(m_pBodyNode!= NULL)
		{
			m_pSceneMgr->destroySceneNode(m_pBodyNode);
			m_pBodyNode = NULL;
		}
	}
}

void OgreShapeBox::update(float *pos, float *qua)
{
	m_pBodyNode->setOrientation(*((Ogre::Quaternion *)qua));
	m_pBodyNode->setPosition(*((Ogre::Vector3 *)pos));
}
//*******************************************************
//********************  CONE  ***************************
//*******************************************************
OgreShapeCone::OgreShapeCone(Ogre::SceneManager *scene, unsigned int id, const char *meshName):
		PhysicShapeCone(), 
		m_pSceneMgr(scene),
		m_pBodyNode(NULL),
		m_pBodyEnt(NULL)
{
	char name[64];
	if(m_pBodyNode == NULL)
	{
		sprintf(name, "m_pBodyEnt_%d", id);
		m_pBodyEnt = m_pSceneMgr->createEntity(name, meshName);

		sprintf(name, "m_pBodyNode%d", id);
		m_pBodyNode = m_pSceneMgr->getRootSceneNode()->
			createChildSceneNode(name, Ogre::Vector3(0.0f, 0.0f, 0.0f));
		m_pBodyNode->attachObject(m_pBodyEnt);
	}
}

OgreShapeCone::~OgreShapeCone(void)
{
	release();
}

void OgreShapeCone::release(void)
{
	if(m_pSceneMgr)
	{
		if(m_pBodyEnt != NULL)
		{
			if(m_pBodyNode)
				m_pBodyNode->detachObject(m_pBodyEnt);

			m_pSceneMgr->destroyEntity(m_pBodyEnt);
			m_pBodyEnt = NULL;
		}

		if(m_pBodyNode!= NULL)
		{
			m_pSceneMgr->destroySceneNode(m_pBodyNode);
			m_pBodyNode = NULL;
		}
	}
}

void OgreShapeCone::update(float *pos, float *qua)
{
	m_pBodyNode->setOrientation(*((Ogre::Quaternion *)qua));
	m_pBodyNode->setPosition(*((Ogre::Vector3 *)pos));
}
//*******************************************************
//******************  CYLINDER  *************************
//*******************************************************
OgreShapeCylinder::OgreShapeCylinder(Ogre::SceneManager *scene, unsigned int id, const char *meshName):
		PhysicShapeCylinder(), 
		m_pSceneMgr(scene),
		m_pBodyNode(NULL),
		m_pBodyEnt(NULL)
{
	char name[64];
	if(m_pBodyNode == NULL)
	{
		sprintf(name, "m_pBodyEnt_%d", id);
		m_pBodyEnt = m_pSceneMgr->createEntity(name, meshName);

		sprintf(name, "m_pBodyNode%d", id);
		m_pBodyNode = m_pSceneMgr->getRootSceneNode()->
			createChildSceneNode(name, Ogre::Vector3(0.0f, 0.0f, 0.0f));
		m_pBodyNode->attachObject(m_pBodyEnt);
	}
}

OgreShapeCylinder::~OgreShapeCylinder(void)
{
	release();
}

void OgreShapeCylinder::release(void)
{
	if(m_pSceneMgr)
	{
		if(m_pBodyEnt != NULL)
		{
			if(m_pBodyNode)
				m_pBodyNode->detachObject(m_pBodyEnt);

			m_pSceneMgr->destroyEntity(m_pBodyEnt);
			m_pBodyEnt = NULL;
		}

		if(m_pBodyNode!= NULL)
		{
			m_pSceneMgr->destroySceneNode(m_pBodyNode);
			m_pBodyNode = NULL;
		}
	}
}

void OgreShapeCylinder::update(float *pos, float *qua)
{
	m_pBodyNode->setOrientation(*((Ogre::Quaternion *)qua));
	m_pBodyNode->setPosition(*((Ogre::Vector3 *)pos));
}
//*******************************************************
//*******************  SPHERE  **************************
//*******************************************************
OgreShapeSphere::OgreShapeSphere(Ogre::SceneManager *scene, unsigned int id, const char *meshName):
		PhysicShapeSphere(), 
		m_pSceneMgr(scene),
		m_pBodyNode(NULL),
		m_pBodyEnt(NULL)
{
	char name[64];
	if(m_pBodyNode == NULL)
	{
		sprintf(name, "m_pBodyEnt_%d", id);
		m_pBodyEnt = m_pSceneMgr->createEntity(name, meshName);

		sprintf(name, "m_pBodyNode%d", id);
		m_pBodyNode = m_pSceneMgr->getRootSceneNode()->
			createChildSceneNode(name, Ogre::Vector3(0.0f, 0.0f, 0.0f));
		m_pBodyNode->attachObject(m_pBodyEnt);
	}
}

OgreShapeSphere::~OgreShapeSphere(void)
{
	release();
}

void OgreShapeSphere::release(void)
{
	if(m_pSceneMgr)
	{
		if(m_pBodyEnt != NULL)
		{
			if(m_pBodyNode)
				m_pBodyNode->detachObject(m_pBodyEnt);

			m_pSceneMgr->destroyEntity(m_pBodyEnt);
			m_pBodyEnt = NULL;
		}

		if(m_pBodyNode!= NULL)
		{
			m_pSceneMgr->destroySceneNode(m_pBodyNode);
			m_pBodyNode = NULL;
		}
	}
}

void OgreShapeSphere::update(float *pos, float *qua)
{
	m_pBodyNode->setOrientation(*((Ogre::Quaternion *)qua));
	m_pBodyNode->setPosition(*((Ogre::Vector3 *)pos));
}
//*******************************************************
//********************  END  ****************************
//*******************************************************