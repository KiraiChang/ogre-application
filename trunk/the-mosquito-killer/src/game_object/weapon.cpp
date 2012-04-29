#include "weapon.h"

#include "../physic/physic_rigid_body.h"
#include "../move_system/shoot_system.h"
#include "../ogre_physic/ogre_physic_shape.h"
#include "../score_system/score_system.h"
#include "../score_system/score_object.h"

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

void WeaponKnife::create(const char *modelName, float mass, float *scale, float *pos, float *quat, int score)
{
	//create shape
	OgreShapeBox *shape  = new OgreShapeBox(m_pSceneMgr);
	shape->init(modelName, scale);
	shape->update(pos, quat);

	//create score object
	m_pScore = ScoreSystem::createScoreObject(SCORE_TYPE_WEAPON, score);
	m_pScore->regParent(this);

	//create physic body
	m_pBody = new PhysicRigidBody(m_pWorld);
	PhysicDebug *debug = NULL;
	m_pBody->init(shape, debug, mass, pos, quat, m_pScore, 8);

	//create move type
	m_pMove = new ShootBase(shape->getNode());
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

void WeaponKnife::update(void)
{
	if(m_pBody != NULL)
	{
		if(m_pMove != NULL)
		{
			m_pMove->update(m_bDestory);
		
			Ogre::Vector3 pos= m_pMove->getPosition();
			Ogre::Quaternion q= m_pMove->getOrientation();
			m_pBody->update((float *)&pos, (float *)&q);
		}
	}
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