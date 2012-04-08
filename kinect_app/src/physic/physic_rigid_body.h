#ifndef _PHYSIC_BODY_H_
#define _PHYSIC_BODY_H_

class btDynamicsWorld;
class btRigidBody;
class btCollisionShape;
class btTransform;
class PhysicDebug;
class PhysicShapeBase;

class PhysicRigidBody
{
private:
	btRigidBody* localCreateRigidBody (float mass, const btTransform& startTransform, btCollisionShape* shape, void *userPoint, int collisionflag);

private:
	btDynamicsWorld*							m_pWorld;
	btCollisionShape*							m_pShapes;
	btRigidBody*								m_pBodies;
	PhysicDebug*								m_pDebug;
	PhysicShapeBase*							m_pShapeData;

public:
	PhysicRigidBody(btDynamicsWorld *world);
	//PhysicRigidBody(void);
	~PhysicRigidBody(void);

	void init(PhysicShapeBase *shapeData, PhysicDebug *debugShape, float mass, const btTransform& startTransform, void *userPoint, int collisionflag);
	void release(void);
};


#endif //_PHYSIC_BODY_H_