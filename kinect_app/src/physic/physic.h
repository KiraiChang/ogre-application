#ifndef _PHYSIC_H_
#define _PHYSIC_H_

class btDynamicsWorld;
class btBroadphaseInterface;
class btCollisionDispatcher;
class btConstraintSolver;
class btDefaultCollisionConfiguration;
class btClock;

class PhysicSimulation
{
private:
	btDynamicsWorld*						m_pDynamicsWorld;
	btBroadphaseInterface*					m_pBroadphase;
	btCollisionDispatcher*					m_pDispatcher;
	btConstraintSolver*						m_pSolver;
	btDefaultCollisionConfiguration*		m_pCollisionConfiguration;
	btClock*								m_pClock;

	float getDeltaTimeMicroseconds();

public:
	PhysicSimulation(void);
	virtual ~PhysicSimulation(void);

	virtual void init(void);
	virtual void release(void);
	virtual void update(const float &timePass = 0);

};

#endif //_PHYSIC_H_