#include "phsics_attrib.h"
#include <BulletDynamics/btBulletDynamicsCommon.h>

namespace CL
{
//#########################################
//############  BaseAttrib   ##############
//#########################################


PhsicsAttrib::PhsicsAttrib(void):BaseAttrib(),mpBody(NULL), mpShape(NULL), mpMotionState(NULL)
{
}

PhsicsAttrib::~PhsicsAttrib(void)
{
	release();
}

void PhsicsAttrib::init(BaseObject *obj)
{
	BaseAttrib::init(obj);
}

void PhsicsAttrib::release(void)
{
	if(mpBody != NULL)
	{
		mpOwnerWorld->removeRigidBody(mpBody);
		if(mpMotionState != NULL)
		{
			delete mpMotionState;
			mpBody->setMotionState(NULL);
		}
		delete mpBody;
		mpBody = NULL;
	}

	if(mpShape != NULL)
	{
		delete mpShape;
		mpShape = NULL;
	}

	BaseAttrib::release();
}

AttribType PhsicsAttrib::getType()const
{
	return ATTRIB_TYPE_PHSICS_BASE;
}

//phsics
void PhsicsAttrib::init(btDynamicsWorld *world, float mass, const btTransform& startTransform, SHAPE_TYPE type)
{
	switch(type)
	{
	case SHAPE_TYPE_BOX:
		mpShape = new btBoxShape(btVector3(50,3,50));
		break;
	default:
		return;
	}
	mpOwnerWorld = world;
	localCreateRigidBody(mass, startTransform, mpShape);
}

btRigidBody* PhsicsAttrib::localCreateRigidBody (float mass, const btTransform& startTransform, btCollisionShape* shape)
{
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0,0,0);
	if (isDynamic)
		shape->calculateLocalInertia(mass,localInertia);

	mpMotionState = new btDefaultMotionState(startTransform);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, mpMotionState, shape, localInertia);
	mpBody = new btRigidBody(rbInfo);

	mpOwnerWorld->addRigidBody(mpBody);

	return mpBody;
}

}
