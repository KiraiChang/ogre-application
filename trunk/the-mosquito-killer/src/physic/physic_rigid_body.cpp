#include "physic_rigid_body.h"
#include "btBulletDynamicsCommon.h"

#include "physic_debug.h"
#include "physic_shape_base.h"

btVector3 BT_UNIT_Y(0.0, 1.0, 0.0);
btVector3 BT_UNIT_X(1.0, 0.0, 0.0);
btVector3 BT_UNIT_Z(0.0, 0.0, 1.0);

//*******************************************************
//****************  RIGID_BODY  *************************
//*******************************************************

btRigidBody* PhysicRigidBody::localCreateRigidBody (float mass, const btTransform& startTransform, btCollisionShape* shape, void *userPoint, int collisionflag)
{
	if(shape == NULL)
		return NULL;

	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);

	if (isDynamic)
		shape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	if (isDynamic)
	{
		body->setRestitution(10);
		body->setFriction(0.6);
	}
	//body->setCcdMotionThreshold(1.);

	m_pWorld->addRigidBody(body);

	//set collision flag to rigidbody.
	body->setCollisionFlags(body->getCollisionFlags() | collisionflag);
	//把指標放到rigidbody內
	body->setUserPointer(userPoint);

	return body;
}

PhysicRigidBody::PhysicRigidBody(btDynamicsWorld *world):
		m_pWorld(world), 
		m_pShapes(NULL),
		m_pBodies(NULL),
		m_pDebug(NULL),
		m_pShapeData(NULL)
{
}

//PhysicRigidBody::PhysicRigidBody(void)
//{
//	PhysicRigidBody(NULL);
//}

PhysicRigidBody::~PhysicRigidBody(void)
{
}

void PhysicRigidBody::init(PhysicShapeBase *shapeData, PhysicDebug *debugShape, float mass, float *pos, float *quat, void *userPoint, int collisionflag)
{
	if(m_pWorld != NULL)
	{
		btTransform trans;
		trans.setIdentity();
		trans.setRotation(*((btQuaternion *)quat));
		trans.setOrigin(*((btVector3 *)pos));
		m_pDebug = debugShape;
		m_pShapeData = shapeData;
		switch(m_pShapeData->getType())
		{
		case ePhysicShapeBox:
			m_pShapes = new btBoxShape(btVector3(
				m_pShapeData->getData(PhysicShapeBox::SIZE_X),
				m_pShapeData->getData(PhysicShapeBox::SIZE_Y),
				m_pShapeData->getData(PhysicShapeBox::SIZE_Z)
				));
			break;
		case ePhysicShapeCone:
			{
				btVector3 axe(m_pShapeData->getData(PhysicShapeCone::AXE_X),
					m_pShapeData->getData(PhysicShapeCone::AXE_Y),
					m_pShapeData->getData(PhysicShapeCone::AXE_Z));
				if (axe == BT_UNIT_Y)
					m_pShapes = new btConeShape (btScalar (m_pShapeData->getData(PhysicShapeCone::RADIUS)), 
													btScalar (m_pShapeData->getData(PhysicShapeCone::HEIGHT))
													);
				else if (axe == BT_UNIT_X)
					m_pShapes = new btConeShapeX (btScalar (m_pShapeData->getData(PhysicShapeCone::RADIUS)), 
													btScalar (m_pShapeData->getData(PhysicShapeCone::HEIGHT)));
				else
					m_pShapes = new btConeShapeZ (btScalar (m_pShapeData->getData(PhysicShapeCone::RADIUS)), 
													btScalar (m_pShapeData->getData(PhysicShapeCone::HEIGHT)));
			}
			break;
		case ePhysicShapeCylinder:
			{
				btVector3 axe(m_pShapeData->getData(PhysicShapeCylinder::AXE_X),
					m_pShapeData->getData(PhysicShapeCylinder::AXE_Y),
					m_pShapeData->getData(PhysicShapeCylinder::AXE_Z));
				btVector3 halfExtents(m_pShapeData->getData(PhysicShapeCylinder::HALF_X),
					m_pShapeData->getData(PhysicShapeCylinder::HALF_Y),
					m_pShapeData->getData(PhysicShapeCylinder::HALF_Z));
				if (axe == BT_UNIT_X)
					m_pShapes = new btCylinderShapeX(halfExtents);
				else if (axe == BT_UNIT_Y)
					m_pShapes = new btCylinderShape (halfExtents);
				else if (axe == BT_UNIT_Z)
					m_pShapes = new btCylinderShapeZ(halfExtents);
			}
			break;
		case ePhysicShapeSphere:
			m_pShapes = new btSphereShape(btScalar (m_pShapeData->getData(PhysicShapeSphere::RADIUS)));
			break;
		default:
			break;
		}
		m_pBodies = localCreateRigidBody(mass, trans, m_pShapes, userPoint, collisionflag);
	}
}

void PhysicRigidBody::release(void)
{
	if(NULL != m_pWorld)
	{
		// Remove bodies and shapes
		if(NULL != m_pBodies)
		{
			m_pWorld->removeRigidBody(m_pBodies);
			if(NULL != m_pBodies->getUserPointer())
			{
				delete m_pBodies->getUserPointer();
				m_pBodies->setUserPointer(NULL);
			}
			if(NULL != m_pBodies->getMotionState())
			{
				delete m_pBodies->getMotionState();
				m_pBodies->setMotionState(NULL);
			}

			delete m_pBodies; 
			m_pBodies = NULL;
		}
		if( NULL != m_pShapes)
		{
			delete m_pShapes; 
			m_pShapes = NULL;
		}
	}
	m_pWorld = NULL;

	if(m_pShapeData != NULL)
	{
		delete m_pShapeData;
		m_pShapeData = NULL;
	}
	if(NULL != m_pDebug)
	{
		m_pDebug->release();
		delete m_pDebug;
		m_pDebug = NULL;
	}
}

void PhysicRigidBody::setOrigin(float x, float y, float z)
{
	if(NULL != m_pBodies)
	{
		btTransform trans;
		m_pBodies->getMotionState()->getWorldTransform(trans);
		trans.setOrigin(btVector3(x, y, z));
		m_pBodies->getMotionState()->setWorldTransform(trans);
		m_pBodies->proceedToTransform(trans);
		//m_pBodies->setCenterOfMassTransform(trans);
	}
}

void PhysicRigidBody::translate(float x, float y, float z)
{
	if(NULL != m_pBodies)
	{
		m_pBodies->translate(btVector3(x, y, z));
	}
}

void PhysicRigidBody::force(float x, float y, float z, float ax, float ay, float az, float force)
{
	btVector3 linVel(x, y, z);
	linVel.normalize();
	linVel*=force;

	m_pBodies->setLinearVelocity(linVel);
	m_pBodies->setAngularVelocity(btVector3(ax, ay, az));
}

void PhysicRigidBody::update(float timePass)
{
	if(m_pWorld == NULL)
		return;

	if(m_pBodies == NULL)
		return;

	if(m_pShapes == NULL)
		return;

	if(NULL != m_pDebug)
	{
		m_pDebug->beginDraw();
		m_pDebug->draw(0, m_pBodies);
		m_pDebug->endDraw();
	}

	if(m_pShapeData != NULL)
	{
		btQuaternion q = m_pBodies->getOrientation();
		btTransform trans;
		m_pBodies->getMotionState()->getWorldTransform(trans);
		btVector3 pos = trans.getOrigin();
		m_pShapeData->update(timePass,
								(float *)&pos,
								(float *)&q);
	}
}

void PhysicRigidBody::update(float timePass, float *pos, float *quat)
{
	if(m_pWorld == NULL)
		return;

	if(m_pBodies == NULL)
		return;

	if(m_pShapes == NULL)
		return;

	if(NULL != m_pBodies)
	{
		btTransform trans;
		m_pBodies->getMotionState()->getWorldTransform(trans);
		trans.setOrigin(*(btVector3 *)pos);
		trans.setRotation(*(btQuaternion *)quat);
		m_pBodies->getMotionState()->setWorldTransform(trans);
		m_pBodies->proceedToTransform(trans);
	}
}

void PhysicRigidBody::getPos(float *pos)const 
{
	if(m_pBodies != NULL)
	{
		btTransform trans;
		m_pBodies->getMotionState()->getWorldTransform(trans);
		btVector3 vec = trans.getOrigin();
		pos[0] = vec.getX();
		pos[1] = vec.getY();
		pos[2] = vec.getZ();
	}
}

void *PhysicRigidBody::getUserPointer()const
{
	return m_pBodies->getUserPointer();
}

void PhysicRigidBody::setAnimation(const char * aniName)
{
	if(m_pShapeData != NULL)
		m_pShapeData->setAnimation(aniName);
}
//*******************************************************
//********************  END  ****************************
//*******************************************************