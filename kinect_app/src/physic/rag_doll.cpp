#include "rag_doll.h"
#include "LinearMath/btIDebugDraw.h"
#include "btBulletDynamicsCommon.h"
#include "physic_debug.h"


#define CONSTRAINT_DEBUG_SIZE 0.2f

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2     1.57079632679489661923
#endif

#ifndef M_PI_4
#define M_PI_4     0.785398163397448309616
#endif

btRigidBody* RagDoll::localCreateRigidBody (float mass, const btTransform& startTransform, btCollisionShape* shape)
{
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0,0,0);
	if (isDynamic)
		shape->calculateLocalInertia(mass,localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,shape,localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

	m_pWorld->addRigidBody(body);

	return body;
}


RagDoll::RagDoll(btDynamicsWorld* pWorld):
		m_pWorld(pWorld),
		m_pDebug(NULL),
		m_pP2p(NULL)
{
	int i = 0;
	for(int i = 0; i < BODYPART_COUNT; i ++)
	{
		m_pShapes[i] = NULL;
		m_pBodies[i] = NULL;
	}

	for(int i = 0; i < JOINT_COUNT; i ++)
	{
		m_pJoints[i] = NULL;
	}
}

RagDoll::~RagDoll()
{
	release();
}

void RagDoll::update()
{
	if(m_pBodies[BODYPART_SPINE] != NULL)
	{
		//m_pBodies[BODYPART_SPINE]->setActivationState(DISABLE_DEACTIVATION);
		//btTransform trans = m_pBodies[BODYPART_LEFT_UPPER_ARM]->getCenterOfMassTransform();
		//m_pBodies[BODYPART_SPINE]->getMotionState()->getWorldTransform(trans);
		//m_pBodies[BODYPART_LEFT_UPPER_ARM]->translate(btVector3(0.5, 0.0, 0.0));

		m_pBodies[BODYPART_SPINE]->translate(btVector3(0.0, 0.1, 0.0));
		m_pBodies[BODYPART_PELVIS]->translate(btVector3(0.0, 0.1, 0.0));//移動
		
		m_pBodies[BODYPART_LEFT_UPPER_ARM]->translate(btVector3(0.1, 0.1, 0.0));
		m_pBodies[BODYPART_RIGHT_UPPER_ARM]->translate(btVector3(-0.1, 0.1, 0.0));


		//m_pBodies[BODYPART_SPINE]->getWorldTransform().setOrigin(btVector3(0.0, 270.0, 0.0));//設定位置

		//btTransform transform;
		//transform.setIdentity();
		//transform.setOrigin(btVector3(trans.getOrigin().getX(), btScalar(270.0f/* y */), trans.getOrigin().getZ()));
		//m_pBodies[BODYPART_SPINE]->setCenterOfMassTransform(transform);
	}
	if(NULL != m_pDebug)
	{
		m_pDebug->beginDraw();
		for(int i = 0; i < BODYPART_COUNT; i ++)
			m_pDebug->draw(i, m_pBodies[i]);
		m_pDebug->endDraw();
	}
}

void RagDoll::init(const int &x, const int &y, const int &z)
{
	btVector3 positionOffset(x, y, z);
	// Setup the geometry
	m_pShapes[BODYPART_PELVIS] = new btCapsuleShape(btScalar(0.15), btScalar(0.20));
	m_pShapes[BODYPART_SPINE] = new btCapsuleShape(btScalar(0.15), btScalar(0.28));
	m_pShapes[BODYPART_HEAD] = new btCapsuleShape(btScalar(0.10), btScalar(0.05));
	m_pShapes[BODYPART_LEFT_UPPER_LEG] = new btCapsuleShape(btScalar(0.07), btScalar(0.45));
	m_pShapes[BODYPART_LEFT_LOWER_LEG] = new btCapsuleShape(btScalar(0.05), btScalar(0.37));
	m_pShapes[BODYPART_RIGHT_UPPER_LEG] = new btCapsuleShape(btScalar(0.07), btScalar(0.45));
	m_pShapes[BODYPART_RIGHT_LOWER_LEG] = new btCapsuleShape(btScalar(0.05), btScalar(0.37));
	m_pShapes[BODYPART_LEFT_UPPER_ARM] = new btCapsuleShape(btScalar(0.05), btScalar(0.33));
	m_pShapes[BODYPART_LEFT_LOWER_ARM] = new btCapsuleShape(btScalar(0.04), btScalar(0.25));
	m_pShapes[BODYPART_RIGHT_UPPER_ARM] = new btCapsuleShape(btScalar(0.05), btScalar(0.33));
	m_pShapes[BODYPART_RIGHT_LOWER_ARM] = new btCapsuleShape(btScalar(0.04), btScalar(0.25));

	// Setup all the rigid bodies
	btTransform offset; offset.setIdentity();
	offset.setOrigin(positionOffset);

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(0.), btScalar(1.), btScalar(0.)));
	m_pBodies[BODYPART_PELVIS] = localCreateRigidBody(btScalar(1.), offset*transform, m_pShapes[BODYPART_PELVIS]);

	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(0.), btScalar(1.2), btScalar(0.)));
	m_pBodies[BODYPART_SPINE] = localCreateRigidBody(btScalar(1.), offset*transform, m_pShapes[BODYPART_SPINE]);

	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(0.), btScalar(1.6), btScalar(0.)));
	m_pBodies[BODYPART_HEAD] = localCreateRigidBody(btScalar(1.), offset*transform, m_pShapes[BODYPART_HEAD]);

	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(-0.18), btScalar(0.65), btScalar(0.)));
	m_pBodies[BODYPART_LEFT_UPPER_LEG] = localCreateRigidBody(btScalar(1.), offset*transform, m_pShapes[BODYPART_LEFT_UPPER_LEG]);

	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(-0.18), btScalar(0.2), btScalar(0.)));
	m_pBodies[BODYPART_LEFT_LOWER_LEG] = localCreateRigidBody(btScalar(1.), offset*transform, m_pShapes[BODYPART_LEFT_LOWER_LEG]);

	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(0.18), btScalar(0.65), btScalar(0.)));
	m_pBodies[BODYPART_RIGHT_UPPER_LEG] = localCreateRigidBody(btScalar(1.), offset*transform, m_pShapes[BODYPART_RIGHT_UPPER_LEG]);

	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(0.18), btScalar(0.2), btScalar(0.)));
	m_pBodies[BODYPART_RIGHT_LOWER_LEG] = localCreateRigidBody(btScalar(1.), offset*transform, m_pShapes[BODYPART_RIGHT_LOWER_LEG]);

	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(-0.35), btScalar(1.45), btScalar(0.)));
	transform.getBasis().setEulerZYX(0,0,M_PI_2);
	m_pBodies[BODYPART_LEFT_UPPER_ARM] = localCreateRigidBody(btScalar(1.), offset*transform, m_pShapes[BODYPART_LEFT_UPPER_ARM]);

	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(-0.7), btScalar(1.45), btScalar(0.)));
	transform.getBasis().setEulerZYX(0,0,M_PI_2);
	m_pBodies[BODYPART_LEFT_LOWER_ARM] = localCreateRigidBody(btScalar(1.), offset*transform, m_pShapes[BODYPART_LEFT_LOWER_ARM]);

	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(0.35), btScalar(1.45), btScalar(0.)));
	transform.getBasis().setEulerZYX(0,0,-M_PI_2);
	m_pBodies[BODYPART_RIGHT_UPPER_ARM] = localCreateRigidBody(btScalar(1.), offset*transform, m_pShapes[BODYPART_RIGHT_UPPER_ARM]);

	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(0.7), btScalar(1.45), btScalar(0.)));
	transform.getBasis().setEulerZYX(0,0,-M_PI_2);
	m_pBodies[BODYPART_RIGHT_LOWER_ARM] = localCreateRigidBody(btScalar(1.), offset*transform, m_pShapes[BODYPART_RIGHT_LOWER_ARM]);

	// Setup some damping on the m_pBodies
	for (int i = 0; i < BODYPART_COUNT; ++i)
	{
		m_pBodies[i]->setDamping(0.05, 0.85);
		m_pBodies[i]->setDeactivationTime(0.8);
		m_pBodies[i]->setSleepingThresholds(1.6, 2.5);
	}

	// Now setup the constraints
	btHingeConstraint* hingeC;
	btConeTwistConstraint* coneC;

	btTransform localA, localB;

	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0,M_PI_2,0); localA.setOrigin(btVector3(btScalar(0.), btScalar(0.15), btScalar(0.)));
	localB.getBasis().setEulerZYX(0,M_PI_2,0); localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.15), btScalar(0.)));
	hingeC =  new btHingeConstraint(*m_pBodies[BODYPART_PELVIS], *m_pBodies[BODYPART_SPINE], localA, localB);
	hingeC->setLimit(btScalar(-M_PI_4), btScalar(M_PI_2));
	m_pJoints[JOINT_PELVIS_SPINE] = hingeC;
	hingeC->setDbgDrawSize(CONSTRAINT_DEBUG_SIZE);

	m_pWorld->addConstraint(m_pJoints[JOINT_PELVIS_SPINE], true);


	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0,0,M_PI_2); localA.setOrigin(btVector3(btScalar(0.), btScalar(0.30), btScalar(0.)));
	localB.getBasis().setEulerZYX(0,0,M_PI_2); localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.14), btScalar(0.)));
	coneC = new btConeTwistConstraint(*m_pBodies[BODYPART_SPINE], *m_pBodies[BODYPART_HEAD], localA, localB);
	coneC->setLimit(M_PI_4, M_PI_4, M_PI_2);
	m_pJoints[JOINT_SPINE_HEAD] = coneC;
	coneC->setDbgDrawSize(CONSTRAINT_DEBUG_SIZE);

	m_pWorld->addConstraint(m_pJoints[JOINT_SPINE_HEAD], true);


	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0,0,-M_PI_4*5); localA.setOrigin(btVector3(btScalar(-0.18), btScalar(-0.10), btScalar(0.)));
	localB.getBasis().setEulerZYX(0,0,-M_PI_4*5); localB.setOrigin(btVector3(btScalar(0.), btScalar(0.225), btScalar(0.)));
	coneC = new btConeTwistConstraint(*m_pBodies[BODYPART_PELVIS], *m_pBodies[BODYPART_LEFT_UPPER_LEG], localA, localB);
	coneC->setLimit(M_PI_4, M_PI_4, 0);
	m_pJoints[JOINT_LEFT_HIP] = coneC;
	coneC->setDbgDrawSize(CONSTRAINT_DEBUG_SIZE);

	m_pWorld->addConstraint(m_pJoints[JOINT_LEFT_HIP], true);

	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0,M_PI_2,0); localA.setOrigin(btVector3(btScalar(0.), btScalar(-0.225), btScalar(0.)));
	localB.getBasis().setEulerZYX(0,M_PI_2,0); localB.setOrigin(btVector3(btScalar(0.), btScalar(0.185), btScalar(0.)));
	hingeC =  new btHingeConstraint(*m_pBodies[BODYPART_LEFT_UPPER_LEG], *m_pBodies[BODYPART_LEFT_LOWER_LEG], localA, localB);
	hingeC->setLimit(btScalar(0), btScalar(M_PI_2));
	m_pJoints[JOINT_LEFT_KNEE] = hingeC;
	hingeC->setDbgDrawSize(CONSTRAINT_DEBUG_SIZE);

	m_pWorld->addConstraint(m_pJoints[JOINT_LEFT_KNEE], true);


	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0,0,M_PI_4); localA.setOrigin(btVector3(btScalar(0.18), btScalar(-0.10), btScalar(0.)));
	localB.getBasis().setEulerZYX(0,0,M_PI_4); localB.setOrigin(btVector3(btScalar(0.), btScalar(0.225), btScalar(0.)));
	coneC = new btConeTwistConstraint(*m_pBodies[BODYPART_PELVIS], *m_pBodies[BODYPART_RIGHT_UPPER_LEG], localA, localB);
	coneC->setLimit(M_PI_4, M_PI_4, 0);
	m_pJoints[JOINT_RIGHT_HIP] = coneC;
	coneC->setDbgDrawSize(CONSTRAINT_DEBUG_SIZE);

	m_pWorld->addConstraint(m_pJoints[JOINT_RIGHT_HIP], true);

	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0,M_PI_2,0); localA.setOrigin(btVector3(btScalar(0.), btScalar(-0.225), btScalar(0.)));
	localB.getBasis().setEulerZYX(0,M_PI_2,0); localB.setOrigin(btVector3(btScalar(0.), btScalar(0.185), btScalar(0.)));
	hingeC =  new btHingeConstraint(*m_pBodies[BODYPART_RIGHT_UPPER_LEG], *m_pBodies[BODYPART_RIGHT_LOWER_LEG], localA, localB);
	hingeC->setLimit(btScalar(0), btScalar(M_PI_2));
	m_pJoints[JOINT_RIGHT_KNEE] = hingeC;
	hingeC->setDbgDrawSize(CONSTRAINT_DEBUG_SIZE);

	m_pWorld->addConstraint(m_pJoints[JOINT_RIGHT_KNEE], true);


	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0,0,M_PI); localA.setOrigin(btVector3(btScalar(-0.2), btScalar(0.15), btScalar(0.)));
	localB.getBasis().setEulerZYX(0,0,M_PI_2); localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.18), btScalar(0.)));
	coneC = new btConeTwistConstraint(*m_pBodies[BODYPART_SPINE], *m_pBodies[BODYPART_LEFT_UPPER_ARM], localA, localB);
	coneC->setLimit(M_PI_2, M_PI_2, 0);
	coneC->setDbgDrawSize(CONSTRAINT_DEBUG_SIZE);

	m_pJoints[JOINT_LEFT_SHOULDER] = coneC;
	m_pWorld->addConstraint(m_pJoints[JOINT_LEFT_SHOULDER], true);

	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0,M_PI_2,0); localA.setOrigin(btVector3(btScalar(0.), btScalar(0.18), btScalar(0.)));
	localB.getBasis().setEulerZYX(0,M_PI_2,0); localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.14), btScalar(0.)));
	hingeC =  new btHingeConstraint(*m_pBodies[BODYPART_LEFT_UPPER_ARM], *m_pBodies[BODYPART_LEFT_LOWER_ARM], localA, localB);
	//		hingeC->setLimit(btScalar(-M_PI_2), btScalar(0));
	hingeC->setLimit(btScalar(0), btScalar(M_PI_2));
	m_pJoints[JOINT_LEFT_ELBOW] = hingeC;
	hingeC->setDbgDrawSize(CONSTRAINT_DEBUG_SIZE);

	m_pWorld->addConstraint(m_pJoints[JOINT_LEFT_ELBOW], true);



	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0,0,0); localA.setOrigin(btVector3(btScalar(0.2), btScalar(0.15), btScalar(0.)));
	localB.getBasis().setEulerZYX(0,0,M_PI_2); localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.18), btScalar(0.)));
	coneC = new btConeTwistConstraint(*m_pBodies[BODYPART_SPINE], *m_pBodies[BODYPART_RIGHT_UPPER_ARM], localA, localB);
	coneC->setLimit(M_PI_2, M_PI_2, 0);
	m_pJoints[JOINT_RIGHT_SHOULDER] = coneC;
	coneC->setDbgDrawSize(CONSTRAINT_DEBUG_SIZE);

	m_pWorld->addConstraint(m_pJoints[JOINT_RIGHT_SHOULDER], true);

	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0,M_PI_2,0); localA.setOrigin(btVector3(btScalar(0.), btScalar(0.18), btScalar(0.)));
	localB.getBasis().setEulerZYX(0,M_PI_2,0); localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.14), btScalar(0.)));
	hingeC =  new btHingeConstraint(*m_pBodies[BODYPART_RIGHT_UPPER_ARM], *m_pBodies[BODYPART_RIGHT_LOWER_ARM], localA, localB);
	//		hingeC->setLimit(btScalar(-M_PI_2), btScalar(0));
	hingeC->setLimit(btScalar(0), btScalar(M_PI_2));
	m_pJoints[JOINT_RIGHT_ELBOW] = hingeC;
	hingeC->setDbgDrawSize(CONSTRAINT_DEBUG_SIZE);

	m_pWorld->addConstraint(m_pJoints[JOINT_RIGHT_ELBOW], true);
}

void RagDoll::release()
{
	if(NULL != m_pWorld)
	{
		int i;
		// Remove all constraints
		for ( i = 0; i < JOINT_COUNT; ++i)
		{
			if(NULL != m_pJoints[i])
			{
				m_pWorld->removeConstraint(m_pJoints[i]);
				delete m_pJoints[i]; m_pJoints[i] = 0;
			}
		}

		// Remove all bodies and shapes
		for ( i = 0; i < BODYPART_COUNT; ++i)
		{
			if(NULL != m_pBodies[i])
			{
				m_pWorld->removeRigidBody(m_pBodies[i]);

				if(NULL != m_pBodies[i]->getMotionState())
					delete m_pBodies[i]->getMotionState();

				delete m_pBodies[i]; 
				m_pBodies[i] = 0;
			}
			if( NULL != m_pShapes[i])
			{
				delete m_pShapes[i]; 
				m_pShapes[i] = 0;
			}
		}
		m_pWorld = NULL;
	}

	if(NULL != m_pDebug)
	{
		m_pDebug->release();
		delete m_pDebug;
		m_pDebug = NULL;
	}
}

void RagDoll::setDebug(PhysicDebug *debug)
{
	m_pDebug = debug;
}


//{
//	btVector3 max, min;
//	m_pBodies[BODYPART_SPINE]->getAabb(min, max);	
//	btTransform world;
//	m_pBodies[BODYPART_SPINE]->getMotionState()->getWorldTransform(world);
//	btQuaternion q;
//	world.setRotation(q);
//}