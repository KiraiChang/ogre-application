#include "game_system.h"
#include "score_system.h"
#include "../ogre_physic/ogre_physic_shape.h"
#include "../physic/physic_debug.h"

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

	if(m_vRigidBody.size() > 1)
	{
		for(rIte = m_vRigidBody.begin(); rIte != m_vRigidBody.end(); rIte++)
		{
			body = *rIte;
			rIte = m_vRigidBody.erase(rIte);
			body->release();
			delete body;
		}
	}
	else if(m_vRigidBody.size() == 1)
	{
		body = m_vRigidBody.back();
		body->release();
		delete body;
		m_vRigidBody.clear();
	}

	if(m_vShape.size() > 1)
	{
		for(sIte = m_vShape.begin(); sIte != m_vShape.end(); sIte++)
		{
			shape = *sIte;
			sIte = m_vShape.erase(sIte);
			shape->release();
			delete shape;
		}
	}
	else if(m_vShape.size() == 1)
	{
		for(sIte = m_vShape.begin(); sIte != m_vShape.end(); sIte++)
		{
			shape = m_vShape.back();
			shape->release();
			delete shape;
			m_vShape.clear();
		}
	}

}

void GameSystem::createShape(const char *modelName, float *scale, float *pos, float *quat)
{
	m_vShape.push_back(new OgreShapeBox(m_pSceneMgr));
	OgreShapeBox *shape  = (OgreShapeBox *)m_vShape.back();
	shape->init(modelName, scale);
	shape->update(pos, quat);
	
}

PhysicRigidBody *GameSystem::createRidigBody(const char *modelName, float mass, float *scale, float *pos, PhysicDebug *debug, void *userPoint, int flag)
{
	OgreShapeBox *shape  = new OgreShapeBox(m_pSceneMgr);
	shape->init(modelName, scale);
	m_vRigidBody.push_back(new PhysicRigidBody(m_pWorld));
	PhysicRigidBody *body = m_vRigidBody.back();
	body->init(shape, debug, mass, pos, userPoint, flag);
	
	return body;
}

void GameSystem::randomShoot(void)
{
	float scale[3] = {1.0, -1.0, 1.0};
	float pos[3] = {0.0, 3, -45.0};
	float dir[3] = {0, 1.0, 2.5};
	float roat[3] = {-0.1, 0, 0};
	float speed = 35;
	char modelName[64] = "";
	int scoreType = 5;//3 - 5
	sprintf(modelName, "bomb.mesh");


	PhysicRigidBody *body = createRidigBody(modelName, 1.0, scale, pos, NULL, ScoreSystem::createScoreObject(scoreType), 8);
	body->force(dir[0], dir[1], dir[2], roat[0], roat[1], roat[2], speed);

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

void GameSystem::update(void)
{
	V_RIGID_BODY::iterator rIte;
	PhysicRigidBody *body;

	for(rIte = m_vRigidBody.begin(); rIte != m_vRigidBody.end(); rIte++)
	{
		body = *rIte;
		body->update();
	}
}