#ifndef _PHYSIC_DEBUG_H_
#define _PHYSIC_DEBUG_H_

class btRigidBody;

class PhysicDebug
{
protected:
	static int g_iDebugID;
public:
	virtual void beginDraw(void) = 0;
	virtual void draw(const int &part, btRigidBody *body) = 0;
	virtual void endDraw(void) = 0;
	virtual void release(void) = 0;
};

#endif //_PHYSIC_DEBUG_H_