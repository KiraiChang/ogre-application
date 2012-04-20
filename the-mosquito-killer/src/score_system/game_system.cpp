#include "game_system.h"
#include "score_system.h"
#include "score_object.h"
#include "btBulletDynamicsCommon.h"
#include "../ogre_physic/ogre_physic_shape.h"
#include "../ogre_physic/ogre_physic_debug.h"
#include "../physic/physic_debug.h"
#include <Ogre.h>

#include <random>

const float HAND_CHECK_CLOSE_DIST = 2.0f;
const float HAND_CHECK_OPEN_DIST = 1.0f;
const float HAND_WAIT_ATTACK_TIMEOUT = 3.0f;

bool GameSystem::MaterialCombinerCallback(btManifoldPoint& cp,	const btCollisionObject* colObj0,int partId0,int index0,const btCollisionObject* colObj1,int partId1,int index1)
{

	float friction0 = colObj0->getFriction();
	float friction1 = colObj1->getFriction();
	float restitution0 = colObj0->getRestitution();
	float restitution1 = colObj1->getRestitution();

	if (colObj0->getCollisionFlags() & btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK)
	{
		friction0 = 1.0;//partId0,index0
		restitution0 = 0.f;
	}
	if (colObj1->getCollisionFlags() & btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK)
	{
		if (index1&1)
		{
			friction1 = 1.0f;//partId1,index1
		} else
		{
			friction1 = 0.f;
		}
		restitution1 = 0.f;
	}

	//cp.m_combinedFriction = calculateCombinedFriction(friction0,friction1);
	//cp.m_combinedRestitution = calculateCombinedRestitution(restitution0,restitution1);

	//this return value is currently ignored, but to be on the safe side: return false if you don't calculate friction
	return true;
}

void checkDestory(ScoreBase *object0, ScoreBase *object1)
{
	if(ScoreSystem::calcScore(object0, object1) != 0)
	{
		//object1->m_bDestory = true;
		GameSystem::getInstance()->setHandState(GameSystem::eOnHandClose);
		if(object1->getParent() != NULL)
		{
			switch(((MosquitoBase *)object1->getParent())->getType())
			{	
			case eMosquitoBase:
				{
					((MosquitoBase *)object1->getParent())->setDestory();
				}
				break;
			case eMosquitoSplit:
				{
					int number = ((MosquitoSplit *)object1->getParent())->getSplitNumber();
					((MosquitoSplit *)object1->getParent())->setDestory();
					//create "number" 
				}
				break;
			case eMosquitoFat:
				((MosquitoFat *)object1->getParent())->decreaseBlood();
				break;
			default:
				break;
			}
		}
	}
}

bool GameSystem::MaterialProcessedCallback(btManifoldPoint& cp,btCollisionObject* body0,btCollisionObject* body1)
{
	btRigidBody* rigidbody0 = dynamic_cast<btRigidBody*>(body0);
	btRigidBody* rigidbody1 = dynamic_cast<btRigidBody*>(body1);
	if(rigidbody0->getUserPointer() != NULL && rigidbody1->getUserPointer() != NULL)
	{
		//取出rigidbody內的指標...並檢查兩個的指標是否需要作用
		ScoreBase *object0 = (ScoreBase *)rigidbody0->getUserPointer();
		ScoreBase *object1 = (ScoreBase *)rigidbody1->getUserPointer();
		GameSystem::HandState eState = GameSystem::getInstance()->getHandState();
		if(/*object0->getType() == SCORE_TYPE_BODY ||*/ object0->getType() == SCORE_TYPE_HAND)
		{
			if(eState == eOnHandWaitAttack)
				checkDestory(object0, object1);
		}
		else if(object0->getType() == SCORE_TYPE_WEAPON)
		{
			//if(ScoreSystem::calcScore(object0, object1) != 0)
			//{
			//	object1->m_bDestory = true;
			//	GameSystem::getInstance()->setHandState(eOnHandClose);
			//	//rigidbody1->clearForces();
			//	//rigidbody1->setGravity(btVector3(0.0, -1.0, 0.0));
			//}
			checkDestory(object0, object1);
		}
	}
	return true;
}

GameSystem *GameSystem::g_instance = NULL;

GameSystem::GameSystem(void):
		m_pWorld(NULL),
		m_pSceneMgr(NULL),
		m_fFullTime(DEF_MAX_PLAY_TIME),
		m_fTimePass(0),
		m_bShoot(false),
		m_eState(eOnPlaying),
		m_eHandState(eOnHandOpen),
		m_fHandClose(0),
		m_iCurrentID(0)
{
	for(int i = 0; i < NUI_SKELETON_COUNT; i++)
	{
		m_vpPlayer[i] = NULL;
	}

		m_vfHandDebugPos[0][0] = 3.0;
		m_vfHandDebugPos[0][1] = 8.0;
		m_vfHandDebugPos[0][2] = 50.0;

		m_vfHandDebugPos[1][0] = -3.0;
		m_vfHandDebugPos[1][1] = 8.0;
		m_vfHandDebugPos[1][2] = 50.0;
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
	releaseCharacter();
	m_pWorld = NULL;
	m_pSceneMgr = NULL;
}

void GameSystem::releaseMosquito(void)
{
	V_MOSQUITO::iterator rIte;
	MosquitoBase *body;
	for(rIte = m_vMosquito.begin(); rIte != m_vMosquito.end(); rIte++)
	{
		body = *rIte;
		body->release();
		delete body;
	}
	m_vMosquito.clear();
}

void GameSystem::releaseCharacter()
{
	for(int i = 0; i < NUI_SKELETON_COUNT; i++)
	{
		if(m_vpPlayer[i] != NULL)
		{
			delete m_vpPlayer[i];
			m_vpPlayer[i] = NULL;
		}
	}
}

void GameSystem::restart(void)
{
	V_RIGID_BODY::iterator rIte;
	V_SHAPE::iterator sIte;
	PhysicRigidBody *body;
	PhysicShapeBase *shape;
	ScoreSystem::resetScore();
	m_fTimePass = 0.0f;
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

	releaseMosquito();
}

float GameSystem::getTimePass(void)const
{
	return m_fTimePass;
}

GameSystem::HandState GameSystem::getHandState(void)const
{
	return m_eHandState;
}

void GameSystem::setHandState(GameSystem::HandState state)
{
	m_eHandState = state;
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

void GameSystem::createMosquito(MOSQUITO_TYPE type, const char *modelName, float mass, float *scale, float *pos, float *quat, int score, int otherData)
{
	switch(type)
	{
	case eMosquitoSplit:
		m_vMosquito.push_back(new MosquitoSplit());
		((MosquitoSplit *)m_vMosquito.back())->setSplitNumber(otherData);
		break;
	case eMosquitoFat:
		m_vMosquito.push_back(new MosquitoFat());
		((MosquitoFat *)m_vMosquito.back())->setBloodNumber(otherData);
		break;
	default:
		m_vMosquito.push_back(new MosquitoBase());
		break;
	}
	m_vMosquito.back()->init(m_pSceneMgr, m_pWorld);
	m_vMosquito.back()->create(modelName, mass, scale, pos, quat, score);
}

void GameSystem::randomShoot(void)
{
	m_bShoot = true;
	int r = rand() % 3;
	float scale[3] = {1.0, 1.0, 1.0};
	float pos[3] = {0.0, 3, -45.0};
	float dir[3] = {0, 0.4, 1.0};
	float quat[4] = {1.0, 0.0, 0.0, 0.0};
	if(r == 0)
	{
		dir[0] = -0.4;
		pos[0] = 30;
		//quat[2] = -0.25;
	}
	else if(r == 1)
	{
		dir[0] = 0.4;
		pos[0] = -30;
		//quat[2] = 0.25;
	}

	float roat[3] = {0.0, 0.0, 0.0};
	float speed = 35;
	char modelName[64] = "";
	//int scoreType = rand() % 3 + 3;//3 - 5
	
	//switch(scoreType)
	//{
	//	case SCORE_TYPE_COIN:
	//		sprintf_s(modelName, "coin.mesh");
	//		break;
	//	case SCORE_TYPE_ARROW:
	//		sprintf_s(modelName, "arror.mesh");
	//		break;
	//	case SCORE_TYPE_BOMB:
	//		sprintf_s(modelName, "bomb.mesh");
	//		break;
	//	default:
	//		sprintf_s(modelName, "bomb.mesh");
	//		break;
	//}
	//OgrePhysicDebug *debug = new OgrePhysicDebug();
	//debug->init(m_pSceneMgr);
	sprintf_s(modelName, "mosquito01.mesh");
	//PhysicRigidBody *body = createRidigBody(modelName, 1.0, scale, pos, quat, debug, ScoreSystem::createScoreObject(SCORE_TYPE_ENEMY, 100), 8);
	//body->force(dir[0], dir[1], dir[2], roat[0], roat[1], roat[2], speed);
	createMosquito(eMosquitoBase, modelName, 1.0, scale, pos, quat, 100);
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

void GameSystem::initPlayer(void)
{
	for(int i = 0; i < NUI_SKELETON_COUNT; i++)
	{
		m_vpPlayer[i] = new PhysicKinect(m_pSceneMgr, m_pWorld);
	}

}

void GameSystem::initPlayer(unsigned int playerCount)
{
	for(int i = 0; i < playerCount; i++)
	{
		m_vpPlayer[i]->init(i);
	}
}

void GameSystem::update(float timePass)
{
	switch(m_eState)
	{
	case eOnPlaying:
		updatePlaying(timePass);
	break;

	default:
		break;
	}
}

void GameSystem::updatePlaying(float timePass)
{
	bool shoot = (int)(m_fTimePass + timePass)%5 == 0;
	if(shoot && !m_bShoot)
	{
		randomShoot();
	}
	else if(!shoot)
	{
		m_bShoot = false;
	}
	m_fTimePass += timePass;

	//V_RIGID_BODY::iterator rIte;
	//V_RIGID_BODY::iterator eraseIte;
	//PhysicRigidBody *body;
	//for(rIte = m_vRigidBody.begin(); rIte != m_vRigidBody.end();)
	//{
	//	body = *rIte;
	//	
	//	body->update();
	//	float pos[3];
	//	body->getPos(pos);
	//	if(pos[2] > 50)
	//	{
	//		eraseIte = rIte;
	//		rIte++;
	//		m_vRigidBody.erase(eraseIte);
	//		body->release();
	//		delete body;
	//	}
	//	else if(body->getUserPointer() != NULL)
	//	{
	//		ScoreBase *base = (ScoreBase *)body->getUserPointer();
	//		if(base->m_bDestory)
	//		{
	//			eraseIte = rIte;
	//			rIte++;
	//			m_vRigidBody.erase(eraseIte);
	//			body->release();
	//			delete body;
	//		}
	//		else
	//			rIte++;
	//	}
	//	else
	//		rIte++;
	//}
	updateMosquito(timePass);
	if(m_fTimePass >= m_fFullTime)
	{
		restart();
		initScene();
	}
	updateHandState(timePass);
}

void GameSystem::updateMosquito(float timePass)
{
	V_MOSQUITO::iterator rIte;
	V_MOSQUITO::iterator eraseIte;
	MosquitoBase *body;
	for(rIte = m_vMosquito.begin(); rIte != m_vMosquito.end();)
	{
		body = *rIte;
		body->update();

		if(body->isDestory())
		{
			eraseIte = rIte;
			rIte++;
			m_vMosquito.erase(eraseIte);
			body->release();
			delete body;
		}
		else
			rIte++;
	}
}

void GameSystem::updatePlayer(const NUI_SKELETON_FRAME &frame)
{
	for(int i = 0; i < NUI_SKELETON_COUNT; i++ )
	{
		if(frame.SkeletonData[i].eTrackingState != NUI_SKELETON_NOT_TRACKED)
		{
			if(m_vpPlayer[i]->getID() != frame.SkeletonData[i].dwTrackingID)
			{
				m_vpPlayer[i]->release();
				m_vpPlayer[i]->init(frame.SkeletonData[i].dwTrackingID);
			}
			m_iCurrentID = i;
			m_vpPlayer[i]->update(frame.SkeletonData[i]);
		}
		else
		{
			m_vpPlayer[i]->release();
		}
	}
}

void GameSystem::updatePlayerDebug(void)
{
	m_vpPlayer[0]->updateDebug(m_vfHandDebugPos);
}

void GameSystem::updateHandState(float timePass)
{
	float rightPos[3];
	float leftPos[3];
	switch(m_eHandState)
	{
	case eOnHandOpen:
		{
			m_vpPlayer[m_iCurrentID]->getPartPos(eKinectRightHand, rightPos);
			m_vpPlayer[m_iCurrentID]->getPartPos(eKinectLeftHand, leftPos);
			Ogre::Vector3 right(rightPos);
			Ogre::Vector3 left(leftPos);
			float dist = left.distance(right);
			if(dist < HAND_CHECK_CLOSE_DIST)
				m_eHandState = eOnHandWaitAttack;
		}
		break;
	case eOnHandClose:
		{
			m_vpPlayer[m_iCurrentID]->getPartPos(eKinectRightHand, rightPos);
			m_vpPlayer[m_iCurrentID]->getPartPos(eKinectLeftHand, leftPos);
			Ogre::Vector3 right(rightPos);
			Ogre::Vector3 left(leftPos);
			float dist = left.distance(right);
			if(dist > HAND_CHECK_OPEN_DIST)
				m_eHandState = eOnHandOpen;
		}
		break;
	case eOnHandWaitAttack:
		{
			m_fHandClose += timePass;
			m_vpPlayer[m_iCurrentID]->getPartPos(eKinectRightHand, rightPos);
			m_vpPlayer[m_iCurrentID]->getPartPos(eKinectLeftHand, leftPos);
			Ogre::Vector3 right(rightPos);
			Ogre::Vector3 left(leftPos);
			float dist = left.distance(right);
			if(dist > HAND_CHECK_OPEN_DIST)
				m_eHandState = eOnHandOpen;
			else if(m_fHandClose > HAND_WAIT_ATTACK_TIMEOUT)
			{
				m_eHandState = eOnHandClose;
				m_fHandClose = 0.0;
			}
		}
		break;
	case eOnHandAttacked:
		{
			m_vpPlayer[m_iCurrentID]->getPartPos(eKinectRightHand, rightPos);
			m_vpPlayer[m_iCurrentID]->getPartPos(eKinectLeftHand, leftPos);
			Ogre::Vector3 right(rightPos);
			Ogre::Vector3 left(leftPos);
			float dist = left.distance(right);
			if(dist > HAND_CHECK_OPEN_DIST)
				m_eHandState = eOnHandOpen;
			else if(dist < HAND_CHECK_CLOSE_DIST)
				m_eHandState = eOnHandClose;
		}
		break;
	case eOnHandWaitShoot:
		break;
	default:
		break;
	}
}

void GameSystem::testCollision()
{
	int numManifolds = m_pWorld->getDispatcher()->getNumManifolds();
	for (int i=0;i<numManifolds;i++)
	{
		btPersistentManifold* contactManifold =  m_pWorld->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* obA = static_cast<btCollisionObject*>(contactManifold->getBody0());
		btCollisionObject* obB = static_cast<btCollisionObject*>(contactManifold->getBody1());

		//int flagsA = obA->getCompanionId();
		//int flagsB = obB->getCompanionId();
		//int numContacts = contactManifold->getNumContacts();
		//for (int j=0;j<numContacts;j++)
		//{
		//	btManifoldPoint& pt = contactManifold->getContactPoint(j);
		//	if (pt.getDistance()<0.f)
		//	{
		//		const btVector3& ptA = pt.getPositionWorldOnA();
		//		const btVector3& ptB = pt.getPositionWorldOnB();
		//		const btVector3& normalOnB = pt.m_normalWorldOnB;
		//	}
		//}

		//if(obA->getUserPointer() != NULL && obB->getUserPointer() != NULL)
		//{
		//	//取出rigidbody內的指標...並檢查兩個的指標是否需要作用
		//	ScoreBase *object0 = (ScoreBase *)obA->getUserPointer();
		//	ScoreBase *object1 = (ScoreBase *)obB->getUserPointer();
		//	if(object0->getType() == SCORE_TYPE_BODY || object0->getType() == SCORE_TYPE_HAND)
		//	{
		//		if(ScoreSystem::calcScore(object0, object1) != 0)
		//			object1->m_bDestory = true;
		//	}
		//}
	}
}