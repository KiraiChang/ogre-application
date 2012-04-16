#include "physic.h"
#include "btBulletDynamicsCommon.h"
#include "..\score_system\game_system.h"
#include "..\score_system\score_system.h"
#include "..\score_system\score_object.h"

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
	gContactAddedCallback = GameSystem::MaterialCombinerCallback;
	gContactProcessedCallback = (ContactProcessedCallback)GameSystem::MaterialProcessedCallback;

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
		GameSystem::getInstance()->testCollision();
	}
	return timePass;
}

btDynamicsWorld* PhysicSimulation::getDynamicsWorld(void)
{
	return m_pDynamicsWorld; 
}