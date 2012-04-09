#include "physic.h"
#include "btBulletDynamicsCommon.h"
#include "..\score_system\score_system.h"
#include "..\score_system\score_object.h"

static bool MaterialCombinerCallback(btManifoldPoint& cp,	const btCollisionObject* colObj0,int partId0,int index0,const btCollisionObject* colObj1,int partId1,int index1)
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

static bool MaterialProcessedCallback(btManifoldPoint& cp,btCollisionObject* body0,btCollisionObject* body1)
{
	btRigidBody* rigidbody0 = dynamic_cast<btRigidBody*>(body0);
	btRigidBody* rigidbody1 = dynamic_cast<btRigidBody*>(body1);
	if(rigidbody0->getUserPointer() != NULL && rigidbody1->getUserPointer() != NULL)
	{
		//取出rigidbody內的指標...並檢查兩個的指標是否需要作用
		ScoreBase *object0 = (ScoreBase *)rigidbody0->getUserPointer();
		ScoreBase *object1 = (ScoreBase *)rigidbody1->getUserPointer();
		if(object0->getType() == SCORE_TYPE_BODY || object0->getType() == SCORE_TYPE_HAND)
		{
			ScoreSystem::calcScore(object0, object1);
		}
	}
	return true;
}

extern ContactAddedCallback		gContactAddedCallback;
extern ContactProcessedCallback gContactProcessedCallback;

PhysicSimulation::PhysicSimulation(void):
		m_pDynamicsWorld(NULL), 
		m_pBroadphase(NULL), 
		m_pDispatcher(NULL), 
		m_pSolver(NULL), 
		m_pCollisionConfiguration(NULL),
		m_pClock(NULL)
{

}

PhysicSimulation::~PhysicSimulation(void)
{
	release();
}

void PhysicSimulation::init(const int &x, const int &y, const int &z)
{
	gContactAddedCallback = MaterialCombinerCallback;
	gContactProcessedCallback = (ContactProcessedCallback)MaterialProcessedCallback;

	m_pCollisionConfiguration = new btDefaultCollisionConfiguration();

	m_pDispatcher = new btCollisionDispatcher(m_pCollisionConfiguration);

	//m_pBroadphase = new btDbvtBroadphase();

	btVector3 worldAabbMin(-x,-y,-z);
	btVector3 worldAabbMax(x, y, z);
	m_pBroadphase = new btAxisSweep3 (worldAabbMin, worldAabbMax);

	m_pSolver = new btSequentialImpulseConstraintSolver;

	m_pDynamicsWorld = new btDiscreteDynamicsWorld(m_pDispatcher, m_pBroadphase, m_pSolver, m_pCollisionConfiguration);
	m_pDynamicsWorld->setGravity(btVector3(0, -10, 0));
	m_pClock = new btClock();


	//setup terrain
	btTransform Transform;
	Transform.setIdentity();
	Transform.setOrigin(btVector3(0, 0, 0));

	// Give it to the motion state
	btDefaultMotionState *MotionState = new btDefaultMotionState(Transform);

	m_pTerrainShape = new btStaticPlaneShape(btVector3(0,1,0),0);

	// Add Mass
	btVector3 LocalInertia;
	m_pTerrainShape->calculateLocalInertia(0, LocalInertia);

	// CReate the rigid body object
	m_pTerrainBody = new btRigidBody(0, MotionState, m_pTerrainShape, LocalInertia);

	// Add it to the physics world
	m_pDynamicsWorld->addRigidBody(m_pTerrainBody);
}

void PhysicSimulation::release(void)
{
	if(NULL != m_pClock)
	{
		delete m_pClock;
		m_pClock = NULL;
	}

	if(NULL != m_pTerrainBody)
	{
		m_pDynamicsWorld->removeRigidBody(m_pTerrainBody);

		delete m_pTerrainBody->getMotionState();

		delete m_pTerrainBody; 
		m_pTerrainBody = 0;
	}

	if( NULL != m_pTerrainShape)
	{
		delete m_pTerrainShape; 
		m_pTerrainShape = 0;
	}

	//delete dynamics world
	if(NULL != m_pDynamicsWorld)
	{
		delete m_pDynamicsWorld;
		m_pDynamicsWorld = NULL;
	}

	//delete solver
	if(NULL != m_pSolver)
	{
		delete m_pSolver;
		m_pSolver = NULL;
	}

	//delete broadphase
	if(NULL != m_pBroadphase)
	{
		delete m_pBroadphase;
		m_pBroadphase = NULL;
	}

	//delete dispatcher
	if(NULL != m_pDispatcher)
	{
		delete m_pDispatcher;
		m_pDispatcher = NULL;
	}

	if(NULL != m_pCollisionConfiguration)
	{
		delete m_pCollisionConfiguration;
		m_pCollisionConfiguration = NULL;
	}
}

float PhysicSimulation::getDeltaTimeMicroseconds()
{
	btScalar dt = (btScalar)m_pClock->getTimeMicroseconds();
	m_pClock->reset();
	return dt;
}

float PhysicSimulation::update(const float &timePass)
{
	if (NULL != m_pDynamicsWorld)
	{
		if(timePass == 0)
		{
			float ms = getDeltaTimeMicroseconds();

			float minFPS = 1000000.f/60.f;
			if (ms > minFPS)
				ms = minFPS;

			m_pDynamicsWorld->stepSimulation(ms / 1000000.f, 60);
			return ms / 1000000.f;
		}
		else if(timePass > 0)
		{
			m_pDynamicsWorld->stepSimulation(timePass, 60);
			return timePass;
		}
		else
			m_pDynamicsWorld->stepSimulation(0, 60);

		//optional but useful: debug drawing
		m_pDynamicsWorld->debugDrawWorld();

		//test object collision
		collisionTest();
	}
	return timePass;
}

void PhysicSimulation::collisionTest()
{
	int numManifolds = m_pDynamicsWorld->getDispatcher()->getNumManifolds();
	for (int i=0;i<numManifolds;i++)
	{
		btPersistentManifold* contactManifold =  m_pDynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
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

		if(obA->getUserPointer() != NULL && obB->getUserPointer() != NULL)
		{
			//取出rigidbody內的指標...並檢查兩個的指標是否需要作用
			ScoreBase *object0 = (ScoreBase *)obA->getUserPointer();
			ScoreBase *object1 = (ScoreBase *)obB->getUserPointer();
			if(object0->getType() == SCORE_TYPE_BODY || object0->getType() == SCORE_TYPE_HAND)
			{
				if(ScoreSystem::calcScore(object0, object1) != 0)
					object1->m_bDestory = true;
			}
		}
	}
}

btDynamicsWorld* PhysicSimulation::getDynamicsWorld(void)
{
	return m_pDynamicsWorld; 
}