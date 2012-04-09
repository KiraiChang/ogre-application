#include "game_system.h"
#include "score_system.h"
#include "score_object.h"
#include "../ogre_physic/ogre_physic_shape.h"
#include "../physic/physic_debug.h"

#include <random>

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
	ScoreSystem::resetScore();
	for(rIte = m_vRigidBody.begin(); rIte != m_vRigidBody.end(); rIte++)
	{
		body = *rIte;
		body->release();
		delete body;
	}
	m_vRigidBody.clear();

	for(sIte = m_vShape.begin(); sIte != m_vShape.end(); sIte++)
	{
		shape = *sIte;
		shape->release();
		delete shape;
	}
	m_vShape.clear();

}

void GameSystem::createShape(const char *modelName, float *scale, float *pos, float *quat)
{
	m_vShape.push_back(new OgreShapeBox(m_pSceneMgr));
	OgreShapeBox *shape  = (OgreShapeBox *)m_vShape.back();
	shape->init(modelName, scale);
	shape->update(pos, quat);
	
}

PhysicRigidBody *GameSystem::createRidigBody(const char *modelName, float mass, float *scale, float *pos, float *quat, PhysicDebug *debug, void *userPoint, int flag)
{
	OgreShapeBox *shape  = new OgreShapeBox(m_pSceneMgr);
	shape->init(modelName, scale);
	m_vRigidBody.push_back(new PhysicRigidBody(m_pWorld));
	PhysicRigidBody *body = m_vRigidBody.back();
	body->init(shape, debug, mass, pos, quat, userPoint, flag);
	
	return body;
}

void GameSystem::randomShoot(void)
{
	int r = rand() % 3;
	float scale[3] = {1.0, -1.0, 1.0};
	float pos[3] = {0.0, 3, -45.0};
	float dir[3] = {0, 0.4, 1.0};
	float quat[4] = {1.0, 0.0, 0.0, 0.0};
	if(r == 0)
	{
		dir[0] = -0.4;
		pos[0] = 30;
		quat[2] = -0.25;
	}
	else if(r == 1)
	{
		dir[0] = 0.4;
		pos[0] = -30;
		quat[2] = 0.25;
	}

	float roat[3] = {0.0, 0.0, 0.0};
	float speed = 35;
	char modelName[64] = "";
	int scoreType = rand() % 3 + 3;//3 - 5
	switch(scoreType)
	{
		case SCORE_TYPE_COIN:
			sprintf_s(modelName, "coin.mesh");
			break;
		case SCORE_TYPE_ARROW:
			sprintf_s(modelName, "arror.mesh");
			break;
		case SCORE_TYPE_BOMB:
			sprintf_s(modelName, "bomb.mesh");
			break;
		default:
			sprintf_s(modelName, "bomb.mesh");
			break;
	}


	PhysicRigidBody *body = createRidigBody(modelName, 1.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(scoreType), 8);
	body->force(dir[0], dir[1], dir[2], roat[0], roat[1], roat[2], speed);

}

void GameSystem::initScene(void)
{
	float quat[4] = {1.0, 0.0, 0.0, 0.0};
	float pos[3] = {0.0, 0.0, -50.0};
	float scale[3] = {1.0, 1.0, 1.0};
	createShape("pipeAndBrick.mesh", scale, pos, quat);

	pos[0] = 30;
	quat[2] = -0.25;
	createShape("pipeAndBrick.mesh", scale, pos, quat);

	pos[0] = -30;
	quat[2] = 0.25;
	createShape("pipeAndBrick.mesh", scale, pos, quat);

	pos[0] = -40;
	pos[2] = -60;
	createShape("rock.mesh", scale, pos, quat);

	pos[0] = -20;
	pos[2] = -80;
	scale[0] = 2.0;
	scale[1] = 3.0;
	scale[2] = 4.0;
	quat[2] = 0.05;
	createShape("rock.mesh", scale, pos, quat);

	pos[0] = 40;
	pos[2] = -60;
	scale[0] = 2.0;
	scale[1] = 3.0;
	scale[2] = 1.0;
	quat[2] = -0.1;
	createShape("rock.mesh", scale, pos, quat);

	pos[0] = 30;
	pos[2] = -100;
	scale[0] = 5.0;
	scale[1] = 6.0;
	scale[2] = 4.0;
	quat[2] = -0.15;
	createShape("rock.mesh", scale, pos, quat);

	pos[0] = 130;
	pos[2] = -300;
	scale[0] = 20.0;
	scale[1] = 22.0;
	scale[2] = 24.0;
	quat[2] = -0.1;
	createShape("rock.mesh", scale, pos, quat);

	pos[0] = -100;
	pos[2] = -300;
	scale[0] = 20.0;
	scale[1] = 24.0;
	scale[2] = 22.0;
	quat[2] = 0.2;
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