#include "physic_rigid_body.h"
#include "btBulletDynamicsCommon.h"

#include "physic_debug.h"
#include "physic_shape_base.h"

btVector3 BT_UNIT_Y(0.0, 1.0, 0.0);
btVector3 BT_UNIT_X(1.0, 0.0, 0.0);
btVector3 BT_UNIT_Z(0.0, 0.0, 1.0);

btRigidBody* PhysicRigidBody::localCreateRigidBody (float mass, const btTransform& startTransform, btCollisionShape* shape, void *userPoint, int collisionflag)
{
	if(shape == NULL)
		return;

	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);

	if (isDynamic)
		shape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	//set collision flag to rigidbody.
	body->setCollisionFlags(body->getCollisionFlags() | collisionflag);
	//把指標放到rigidbody內
	body->setUserPointer(userPoint);

	m_pWorld->addRigidBody(body);

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

void PhysicRigidBody::init(PhysicShapeBase *shapeData, PhysicDebug *debugShape, float mass, const btTransform& startTransform, void *userPoint, int collisionflag)
{
	if(m_pWorld != NULL)
	{
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
		m_pBodies = localCreateRigidBody(mass, startTransform, m_pShapes, userPoint, collisionflag);
		m_pWorld->addRigidBody(m_pBodies);
	}
}

void PhysicRigidBody::release(void)
{
	if(NULL != m_pWorld)
	{
		if(m_pShapeData != NULL)
		{
			delete m_pShapeData;
			m_pShapeData = NULL;
		}
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
				delete m_pBodies->getMotionState();

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

	if(NULL != m_pDebug)
	{
		m_pDebug->release();
		delete m_pDebug;
		m_pDebug = NULL;
	}
}