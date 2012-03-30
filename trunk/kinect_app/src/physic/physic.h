#ifndef _PHYSIC_H_
#define _PHYSIC_H_

class btDynamicsWorld;
class btBroadphaseInterface;
class btCollisionDispatcher;
class btConstraintSolver;
class btDefaultCollisionConfiguration;
class btClock;

class btRigidBody;
class btCollisionShape;

class PhysicSimulation
{
private:
	btDynamicsWorld*						m_pDynamicsWorld;
	btBroadphaseInterface*					m_pBroadphase;
	btCollisionDispatcher*					m_pDispatcher;
	btConstraintSolver*						m_pSolver;
	btDefaultCollisionConfiguration*		m_pCollisionConfiguration;
	btClock*								m_pClock;

	btRigidBody *							m_pTerrainBody;//Terrain
	btCollisionShape *						m_pTerrainShape;
	float getDeltaTimeMicroseconds();

public:
	PhysicSimulation(void);
	virtual ~PhysicSimulation(void);

	virtual void init(const int &x = 10000, const int &y = 10000, const int &z = 10000);
	virtual void release(void);
	virtual void update(const float &timePass = 0);

	btDynamicsWorld* getDynamicsWorld(void);

};

#endif //_PHYSIC_H_