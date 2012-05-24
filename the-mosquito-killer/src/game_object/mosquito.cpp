#include "mosquito.h"
//*******************************************************
//***************  MOSQUITO_BASE  ***********************
//*******************************************************
#include "../physic/physic_rigid_body.h"
#include "../move_system/move_system.h"
#include "../ogre_physic/ogre_physic_shape.h"
#include "../ogre_physic/ogre_physic_debug.h"
#include "../score_system/score_system.h"
#include "../score_system/score_object.h"

MosquitoBase::MosquitoBase(void):
		m_pScore(NULL),
		m_pBody(NULL),
		m_pMove(NULL),
		m_pSceneMgr(NULL),
		m_pWorld(NULL),
		m_bDestory(FALSE)
{
}

MosquitoBase::~MosquitoBase(void)
{
	release();
}

void MosquitoBase::init(Ogre::SceneManager *scene, btDynamicsWorld *world)
{
	m_pSceneMgr = scene;
	m_pWorld = world;
}

void MosquitoBase::create(const char *modelName, unsigned int moveType, float speed, float mass, float *scale, float *pos, float *quat, int score)
{
	m_fMoveSpeed = speed;
	//create shape
	OgreShapeBox *shape  = new OgreShapeBox(m_pSceneMgr);
	shape->init(modelName, scale);
	shape->update(0.0, pos, quat);

	//create score object
	m_pScore = ScoreSystem::createScoreObject(SCORE_TYPE_ENEMY, score);
	m_pScore->regParent(this);

	//create physic body
	m_pBody = new PhysicRigidBody(m_pWorld);
	PhysicDebug *debug = new OgrePhysicDebug();
	((OgrePhysicDebug *)debug)->init(m_pSceneMgr);
	m_pBody->init(shape, debug, mass, pos, quat, m_pScore, 8);

	//create move type
	switch(moveType)
	{
	case eMoveStraight:
		m_pMove = new MoveStraight(shape->getNode());
		break;
	default:
		m_pMove = new MoveRandom(shape->getNode());
		break;
	}
}

void MosquitoBase::release(void)
{
	if(m_pMove != NULL)
	{
		delete m_pMove;
		m_pMove = NULL;
	}

	if(m_pBody != NULL)
	{
		m_pBody->release();
		delete m_pBody;
		m_pBody = NULL;
	}

	m_pSceneMgr = NULL;
	m_pWorld = NULL;
	m_pScore = NULL;//release in rigidbody
}

void MosquitoBase::update(float timepass)
{
	if(m_pBody != NULL)
	{
		if(m_pMove != NULL)
		{
			m_pMove->update(m_bDestory, timepass * m_fMoveSpeed);
		
			Ogre::Vector3 pos= m_pMove->getPosition();
			Ogre::Quaternion q= m_pMove->getOrientation();
			m_pBody->update(timepass, (float *)&pos, (float *)&q);
			//m_pBody->update(timepass);
		}
	}
}

void MosquitoBase::setAnimation(const char *aniName)
{
	if(m_pBody != NULL)
		m_pBody->setAnimation(aniName);
}

void MosquitoBase::setParticle(const char *fileName)
{
	if(m_pBody != NULL)
		m_pBody->setParticle(fileName);
}

void MosquitoBase::setVisible(bool visible)
{
	if(m_pBody != NULL)
		m_pBody->setVisible(visible);
}

void MosquitoBase::getPos(float *pos)
{
	if(m_pMove != NULL)
	{
		Ogre::Vector3 p= m_pMove->getPosition();
		pos[0] = p.x;
		pos[1] = p.y;
		pos[2] = p.z;
	}
}
//*******************************************************
//***************  MOSQUITO_SPLIT  **********************
//*******************************************************
MosquitoSplit::MosquitoSplit(void):
	MosquitoBase(), 
	m_iSplitNumber(0)
{
}

MosquitoSplit::~MosquitoSplit(void)
{
	release();
}

void MosquitoSplit::init(Ogre::SceneManager *scene, btDynamicsWorld *world)
{
	MosquitoBase::init(scene, world);
}

void MosquitoSplit::create(const char *modelName, unsigned int moveType, float speed, float mass, float *scale, float *pos, float *quat, int score)
{
	MosquitoBase::create(modelName, moveType, speed, mass, scale, pos, quat, score);
}

void MosquitoSplit::release(void)
{

	MosquitoBase::release();
}

void MosquitoSplit::update(float timepass)
{
	MosquitoBase::update(timepass);
}
//*******************************************************
//****************  MOSQUITO_FAT  ***********************
//*******************************************************
MosquitoFat::MosquitoFat(void):
	MosquitoBase(), 
	m_iBloodNumber(0)
{
}

MosquitoFat::~MosquitoFat(void)
{
	release();
}

void MosquitoFat::init(Ogre::SceneManager *scene, btDynamicsWorld *world)
{
	MosquitoBase::init(scene, world);
}

void MosquitoFat::create(const char *modelName, unsigned int moveType, float speed, float mass, float *scale, float *pos, float *quat, int score)
{
	MosquitoBase::create(modelName, moveType, speed, mass, scale, pos, quat, score);
}

void MosquitoFat::release(void)
{

	MosquitoBase::release();
}

void MosquitoFat::update(float timepass)
{
	MosquitoBase::update(timepass);
}
//*******************************************************
//********************  END  ****************************
//*******************************************************