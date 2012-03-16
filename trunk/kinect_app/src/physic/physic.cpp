#include "physic.h"
#include "btBulletDynamicsCommon.h"

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

void PhysicSimulation::init(void)
{
	m_pCollisionConfiguration = new btDefaultCollisionConfiguration();

	m_pDispatcher = new btCollisionDispatcher(m_pCollisionConfiguration);

	btVector3 worldAabbMin(-10000,-10000,-10000);
	btVector3 worldAabbMax(10000,10000,10000);
	m_pBroadphase = new btAxisSweep3 (worldAabbMin, worldAabbMax);

	m_pSolver = new btSequentialImpulseConstraintSolver;

	m_pDynamicsWorld = new btDiscreteDynamicsWorld(m_pDispatcher, m_pBroadphase, m_pSolver, m_pCollisionConfiguration);

	m_pClock = new btClock();
}

void PhysicSimulation::release(void)
{
	if(NULL != m_pClock)
	{
		delete m_pClock;
		m_pClock = NULL;
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

void PhysicSimulation::update(const float &timePass)
{
	if (NULL != m_pDynamicsWorld)
	{
		if(timePass == 0)
		{
			float ms = getDeltaTimeMicroseconds();

			float minFPS = 1000000.f/60.f;
			if (ms > minFPS)
				ms = minFPS;

			m_pDynamicsWorld->stepSimulation(ms / 1000000.f);
		}
		else if(timePass > 0)
			m_pDynamicsWorld->stepSimulation(timePass);
		else
			m_pDynamicsWorld->stepSimulation(0);

		//optional but useful: debug drawing
		m_pDynamicsWorld->debugDrawWorld();
	}
}