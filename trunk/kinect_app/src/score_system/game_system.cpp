#include "game_system.h"
#include "../ogre_physic/ogre_physic_shape.h"

GameSystem *GameSystem::g_instance = NULL;

GameSystem::GameSystem(void):
		m_pWorld(NULL),
		m_pSceneMgr(NULL)
{
}

GameSystem::~GameSystem(void)
{
	release();
}

GameSystem *GameSystem::getInstance(void)
{
	if(g_instance == NULL)
	{
		g_instance = new GameSystem();
	}
	return g_instance;
}

void GameSystem::init(btDynamicsWorld* world, Ogre::SceneManager *sceneMgr)
{
	if(m_pWorld == NULL)
		m_pWorld = world;

	if(m_pSceneMgr == NULL)
		m_pSceneMgr = sceneMgr;
}

void GameSystem::release(void)
{
	restart();
	m_pWorld = NULL;
	m_pSceneMgr = NULL;
}

void GameSystem::restart(void)
{
	V_RIGID_BODY::iterator rIte;
	V_SHAPE::iterator sIte;
	PhysicRigidBody *body;
	PhysicShapeBase *shape;

	for(rIte = m_vRigidBody.begin(); rIte != m_vRigidBody.end(); ++rIte)
	{
		body = *rIte;
		rIte = m_vRigidBody.erase(rIte);
		body->release();
		delete body;
	}

	for(sIte = m_vShape.begin(); sIte != m_vShape.end(); sIte++)
	{
		shape = *sIte;
		sIte = m_vShape.erase(sIte);
		shape->release();
		delete shape;
	}
}

void GameSystem::createShape(const char *modelName, float *scale, float *pos, float *quat)
{
	m_vShape.push_back(new OgreShapeBox(m_pSceneMgr));
	OgreShapeBox *shape  = (OgreShapeBox *)m_vShape.back();
	shape->init(modelName, scale);
	shape->update(pos, quat);
	
}

void GameSystem::initScene(void)
{
	float quat[4];
	float pos[3];
	float scale[3];
	for(int i = 0; i < 3; i++)
	{
		quat[i] = 0.0;
		pos[i] = 0.0;
		scale[i] = 1.0;
	}
	quat[3] = 0.0;
	quat[0] = 1.0;
	pos[2] = -50.0;
	createShape("pipeAndBrick.mesh", scale, pos, quat);

	pos[0] = 30;
	createShape("pipeAndBrick.mesh", scale, pos, quat);

	pos[0] = -30;
	createShape("pipeAndBrick.mesh", scale, pos, quat);

	pos[0] = -40;
	pos[2] = -60;
	createShape("rock.mesh", scale, pos, quat);
}