#include "ogre_physic_shape.h"
#include <Ogre.h>

Ogre::FloatRect	OgreShapeBox::m_vTexCoordArray[MAX_TEXTURE_COORD] = {
	Ogre::FloatRect(0.0, 0.0, 0.33, 0.33),
	Ogre::FloatRect(0.33, 0.00, 0.66, 0.33),
	Ogre::FloatRect(0.66, 0.0, 1.0, 0.33),
	Ogre::FloatRect(0.0, 0.33, 0.33, 0.66),
	Ogre::FloatRect(0.33, 0.33, 0.66, 0.66),
	Ogre::FloatRect(0.66, 0.33, 1.0, 0.66),
	Ogre::FloatRect(0.0, 0.66, 0.33, 1.0),
	Ogre::FloatRect(0.33, 0.66, 0.66, 1.0),
	Ogre::FloatRect(0.66, 0.66, 1.0, 1.0)
};
const int BILLBOARD_TIMEPASS = 16;
unsigned int gCurrentID = 0;
//*******************************************************
//********************  BOX  ****************************
//*******************************************************
OgreShapeBox::OgreShapeBox(Ogre::SceneManager *scene):
		PhysicShapeBox(), 
		m_pSceneMgr(scene),
		m_pBodyNode(NULL),
		m_pBodyEnt(NULL),
		m_pParticleSystem(NULL),
		m_uiID(0),
		m_pBillboardSet(NULL),
		m_pBillboard(NULL),
		m_fBillboardTime(0),
		m_pAnimationState(NULL)
{
	//m_vTexCoordArray[0] = Ogre::FloatRect(0.0, 0.0, 0.33, 0.33);
	//m_vTexCoordArray[1] = Ogre::FloatRect(0.33, 0.00, 0.66, 0.33);
	//m_vTexCoordArray[2] = Ogre::FloatRect(0.66, 0.0, 1.0, 0.33);
	//m_vTexCoordArray[3] = Ogre::FloatRect(0.0, 0.33, 0.33, 0.66);
	//m_vTexCoordArray[4] = Ogre::FloatRect(0.33, 0.33, 0.66, 0.66);
	//m_vTexCoordArray[5] = Ogre::FloatRect(0.66, 0.33, 1.0, 0.66);
	//m_vTexCoordArray[6] = Ogre::FloatRect(0.0, 0.66, 0.33, 1.0);
	//m_vTexCoordArray[7] = Ogre::FloatRect(0.33, 0.66, 0.66, 1.0);
	//m_vTexCoordArray[8] = Ogre::FloatRect(0.66, 0.66, 1.0, 1.0);
}

OgreShapeBox::~OgreShapeBox(void)
{
	release();
}

void OgreShapeBox::init(const char *meshName, float *scale)
{	
	if(m_pBodyNode == NULL)
	{
		char name[64];
		m_uiID = gCurrentID;
		sprintf(name, "m_pShapeBodyEnt%d", gCurrentID);
		m_pBodyEnt = m_pSceneMgr->createEntity(name, meshName);
		m_pBodyEnt->setCastShadows(true);
		sprintf(name, "m_pShapeBodyNode%d", gCurrentID);
		m_pBodyNode = m_pSceneMgr->getRootSceneNode()->
			createChildSceneNode(name, Ogre::Vector3(0.0f, 0.0f, 0.0f));
		m_pBodyNode->attachObject(m_pBodyEnt);

		m_pBodyNode->setScale(Ogre::Vector3(scale));

		Ogre::Vector3 size = m_pBodyEnt->getBoundingBox().getSize();
		setData(SIZE_X, size.x * scale[0]);
		setData(SIZE_Y, size.y * scale[1]);
		setData(SIZE_Z, size.z * scale[2]);
		gCurrentID++;
	}
}

void OgreShapeBox::release(void)
{
	if(m_pSceneMgr)
	{
		if(m_pBillboardSet != NULL)
		{
			if(m_pBodyNode)
				m_pBodyNode->detachObject(m_pBillboardSet);

			m_pSceneMgr->destroyBillboardSet(m_pBillboardSet);
			m_pBillboardSet = NULL;
		}

		if(m_pBodyEnt != NULL)
		{
			if(m_pBodyNode)
				m_pBodyNode->detachObject(m_pBodyEnt);

			m_pSceneMgr->destroyEntity(m_pBodyEnt);
			m_pBodyEnt = NULL;
		}

		if(m_pParticleSystem != NULL)
		{
			if(m_pBodyNode)
				m_pBodyNode->detachObject(m_pParticleSystem);

			m_pSceneMgr->destroyParticleSystem(m_pParticleSystem);
			m_pParticleSystem = NULL;
		}

		if(m_pBodyNode!= NULL)
		{
			m_pSceneMgr->destroySceneNode(m_pBodyNode);
			m_pBodyNode = NULL;
		}
	}
}

void OgreShapeBox::update(float timePass, float *pos, float *qua)
{
	m_pBodyNode->setOrientation(*((Ogre::Quaternion *)qua));
	m_pBodyNode->setPosition(*((Ogre::Vector3 *)pos));
	if(m_pAnimationState != NULL)
		m_pAnimationState->addTime(timePass);

	updateBillboard(timePass);
}


void OgreShapeBox::update(float timePass)
{
	if(m_pAnimationState != NULL)
		m_pAnimationState->addTime(timePass);

	updateBillboard(timePass);
}

void OgreShapeBox::updateBillboard(float timePass)
{
	if(m_pBillboardSet != NULL)
	{
		if(m_pBillboard == NULL)
			m_pBillboard = m_pBillboardSet->createBillboard(Ogre::Vector3(0, 0, 0));
		m_fBillboardTime += timePass * BILLBOARD_TIMEPASS;
		if(m_fBillboardTime < MAX_TEXTURE_COORD + 1)
		{
			
			int index = (int)m_fBillboardTime % MAX_TEXTURE_COORD;
			m_pBillboard->setTexcoordIndex(index);
		}
		else
		{
			m_fBillboardTime = 0.0;
			m_pBillboardSet->setVisible(false);
		}
	}
}

void OgreShapeBox::setAnimation(const char *aniName, bool loop, bool blend)
{
	if(m_pAnimationState != NULL)
	{
		if(!blend)
		{
			m_pAnimationState->setWeight(0.0);
			m_pAnimationState->setEnabled(false);
		}
	}
	m_pAnimationState = m_pBodyEnt->getAnimationState(aniName);
	m_pAnimationState->setLoop(loop);
	m_pAnimationState->setEnabled(true);
	if(!blend)
	{
		m_pAnimationState->setTimePosition(0.0);
		m_pAnimationState->setWeight(1.0);
	}
}

void OgreShapeBox::setParticle(const char *fileName)
{
	if(m_pParticleSystem != NULL)
	{
		if(m_pBodyNode)
			m_pBodyNode->detachObject(m_pParticleSystem);

		m_pSceneMgr->destroyParticleSystem(m_pParticleSystem);
		m_pParticleSystem = NULL;
		return ;
	}
	char name[64];
	sprintf(name, "m_pShapeParticle_%s_%d", fileName, m_uiID);
	m_pParticleSystem = m_pSceneMgr->createParticleSystem(name, fileName);
	m_pBodyNode->attachObject(m_pParticleSystem);
}

void OgreShapeBox::setVisible(bool visible)
{
	if(m_pBodyNode != NULL)
	{
		m_pBodyNode->setVisible(visible);
	}
	if(m_pParticleSystem != NULL)
		m_pParticleSystem->setVisible(visible);
	if(m_pBillboardSet != NULL)
		m_pBillboardSet->setVisible(visible);
}

void OgreShapeBox::setQueryMask(int id)
{
	m_pBodyEnt->setQueryFlags(id);
}

void OgreShapeBox::setUserData(void *data)
{
	m_pBodyEnt->setUserAny((Ogre::Any)data);
}

void OgreShapeBox::setBillboard(const char *name)
{
	if(m_pBillboardSet == NULL)
	{
		m_pBillboardSet = m_pSceneMgr->createBillboardSet();
		m_pBodyNode->attachObject(m_pBillboardSet);
	}
	m_pBillboardSet->setMaterialName(name);
	m_pBillboardSet->setTextureCoords(m_vTexCoordArray, MAX_TEXTURE_COORD);
	m_pBillboardSet->setVisible(true);
}

/*
//*******************************************************
//********************  CONE  ***************************
//*******************************************************
OgreShapeCone::OgreShapeCone(Ogre::SceneManager *scene, unsigned int id, const char *meshName):
		PhysicShapeCone(), 
		m_pSceneMgr(scene),
		m_pBodyNode(NULL),
		m_pBodyEnt(NULL),
		m_pAnimationState(NULL)
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

void OgreShapeCone::update(float timePass, float *pos, float *qua)
{
	m_pBodyNode->setOrientation(*((Ogre::Quaternion *)qua));
	m_pBodyNode->setPosition(*((Ogre::Vector3 *)pos));
	if(m_pAnimationState != NULL)
		m_pAnimationState->addTime(timePass);
}

void OgreShapeCone::setAnimation(const char *aniName)
{
	m_pAnimationState = m_pBodyEnt->getAnimationState(aniName);
	m_pAnimationState->setLoop(true);
	m_pAnimationState->setEnabled(true);
}
//*******************************************************
//******************  CYLINDER  *************************
//*******************************************************
OgreShapeCylinder::OgreShapeCylinder(Ogre::SceneManager *scene, unsigned int id, const char *meshName):
		PhysicShapeCylinder(), 
		m_pSceneMgr(scene),
		m_pBodyNode(NULL),
		m_pBodyEnt(NULL),
		m_pAnimationState(NULL)
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

void OgreShapeCylinder::update(float timePass, float *pos, float *qua)
{
	m_pBodyNode->setOrientation(*((Ogre::Quaternion *)qua));
	m_pBodyNode->setPosition(*((Ogre::Vector3 *)pos));
	if(m_pAnimationState != NULL)
		m_pAnimationState->addTime(timePass);
}

void OgreShapeCylinder::setAnimation(const char *aniName)
{
	m_pAnimationState = m_pBodyEnt->getAnimationState(aniName);
	m_pAnimationState->setLoop(true);
	m_pAnimationState->setEnabled(true);
}
//*******************************************************
//*******************  SPHERE  **************************
//*******************************************************
OgreShapeSphere::OgreShapeSphere(Ogre::SceneManager *scene):
		PhysicShapeSphere(), 
		m_pSceneMgr(scene),
		m_pBodyNode(NULL),
		m_pBodyEnt(NULL),
		m_pAnimationState(NULL)
{
}

OgreShapeSphere::~OgreShapeSphere(void)
{
	release();
}

void OgreShapeSphere::init(const char *meshName, float *scale)
{
	char name[64];
	if(m_pBodyNode == NULL)
	{
		sprintf(name, "m_pBodyEnt_%d", gCurrentID);
		m_pBodyEnt = m_pSceneMgr->createEntity(name, meshName);
		m_pBodyEnt->setCastShadows(true);
		sprintf(name, "m_pBodyNode%d", gCurrentID);
		m_pBodyNode = m_pSceneMgr->getRootSceneNode()->
			createChildSceneNode(name, Ogre::Vector3(0.0f, 0.0f, 0.0f));
		m_pBodyNode->attachObject(m_pBodyEnt);
		m_pBodyNode->setScale(Ogre::Vector3(scale));

		Ogre::Vector3 size = m_pBodyEnt->getBoundingBox().getSize();
		setData(RADIUS, size.x * scale[0]);

		gCurrentID++;
	}
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

void OgreShapeSphere::update(float timePass, float *pos, float *qua)
{
	m_pBodyNode->setOrientation(*((Ogre::Quaternion *)qua));
	m_pBodyNode->setPosition(*((Ogre::Vector3 *)pos));
	if(m_pAnimationState != NULL)
		m_pAnimationState->addTime(timePass);
}

void OgreShapeSphere::setAnimation(const char *aniName)
{
	m_pAnimationState = m_pBodyEnt->getAnimationState(aniName);
	m_pAnimationState->setLoop(true);
	m_pAnimationState->setEnabled(true);
}
*/
//*******************************************************
//********************  END  ****************************
//*******************************************************