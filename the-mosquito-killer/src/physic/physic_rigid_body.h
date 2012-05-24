#ifndef _PHYSIC_BODY_H_
#define _PHYSIC_BODY_H_

class btDynamicsWorld;
class btRigidBody;
class btCollisionShape;
class btTransform;
class PhysicDebug;
class PhysicShapeBase;

//*******************************************************
//****************  RIGID_BODY  *************************
//*******************************************************
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

	void init(PhysicShapeBase *shapeData, PhysicDebug *debugShape, float mass, float *pos, float *quat, void *userPoint, int collisionflag);
	void setOrigin(float x, float y, float z);
	void translate(float x, float y, float z);
	void force(float x, float y, float z, float ax, float ay, float az, float force);
	void release(void);
	void update(float timePass);
	void update(float timePass, float *pos, float *quat);
	void setEulerZYX (const float &yaw, const float &pitch, const float &roll);
	void getPos(float *pos)const;
	void *getUserPointer()const;
	void setAnimation(const char * aniName);
	void setParticle(const char *fileName);
	void setVisible(bool visible);
};
//*******************************************************
//********************  END  ****************************
//*******************************************************
#endif //_PHYSIC_BODY_H_