#include "weapon.h"

#include "../physic/physic_rigid_body.h"
#include "../move_system/shoot_system.h"
#include "../ogre_physic/ogre_physic_shape.h"
#include "../ogre_physic/ogre_physic_debug.h"
#include "../score_system/score_system.h"
#include "../score_system/score_object.h"
extern bool MOSQUITO_DEBUG_MODE;
WeaponKnife::WeaponKnife(void):
		m_pScore(NULL),
		m_pBody(NULL),
		m_pMove(NULL),
		m_pSceneMgr(NULL),
		m_pWorld(NULL),
		m_bDestory(FALSE)
{
}

WeaponKnife::~WeaponKnife(void)
{
	release();
}

void WeaponKnife::init(Ogre::SceneManager *scene, btDynamicsWorld *world)
{
	m_pSceneMgr = scene;
	m_pWorld = world;
}

void WeaponKnife::create(const char *modelName, int moveType, float mass, float *scale, float *pos, float *size, float *quat, int score,float *tar)
{
	//create shape
	OgreShapeBox *shape  = new OgreShapeBox(m_pSceneMgr);
	shape->init(modelName, scale);
	shape->update(0.0, pos, quat);

	ScoreType stype = SCORE_TYPE_ICON;
	//create move type
	switch(moveType)
	{
	case eShootBase:
		m_pMove = new ShootBase(shape->getNode());
		break;
	case eShootNormal:
		m_pMove = new ShootNormal(shape->getNode(),tar);
		stype = SCORE_TYPE_WEAPON;
		break;
	default:
		break;
	}

	//create score object
	m_pScore = ScoreSystem::createScoreObject(stype, score);
	m_pScore->regParent(this);

	//create physic body
	m_pBody = new PhysicRigidBody(m_pWorld);
	PhysicDebug *debug = NULL;
	if(MOSQUITO_DEBUG_MODE)
	{
		debug = new OgrePhysicDebug();
		((OgrePhysicDebug *)debug)->init(m_pSceneMgr);
	}
	m_pBody->init(shape, debug, mass, pos, size, quat, m_pScore, 8);
}

void WeaponKnife::release(void)
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

void WeaponKnife::update(float timePass)
{
	if(m_pBody != NULL)
	{
		if(m_pMove != NULL)
		{
			m_pMove->update(m_bDestory, timePass);
		
			Ogre::Vector3 pos= m_pMove->getPosition();
			Ogre::Quaternion q= m_pMove->getOrientation();
			m_pBody->update(timePass, (float *)&pos, (float *)&q);
			m_pBody->update(timePass);
		}
	}
}

void WeaponKnife::setVisible(bool v)
{
	if(m_pBody != NULL)
		m_pBody->setVisible(v);
}

//*******************************************************
WeaponBook::WeaponBook(void):
	WeaponKnife()
{
}

WeaponBook::~WeaponBook(void)
{
	release();
}
//*******************************************************
WeaponBomb::WeaponBomb(void):
	WeaponKnife()
{
}

WeaponBomb::~WeaponBomb(void)
{
	release();
}
//*******************************************************