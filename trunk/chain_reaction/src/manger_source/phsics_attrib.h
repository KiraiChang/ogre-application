#include "object_manger.h"
#include <LinearMath\btTransform.h>

class btRigidBody;
class btDynamicsWorld;
class btCollisionShape;
struct btDefaultMotionState;

namespace CL
{
//#########################################
//############  BaseAttrib   ##############
//#########################################

class PhsicsAttrib : public BaseAttrib
{
public:
	enum SHAPE_TYPE
	{
		SHAPE_TYPE_BOX,

	};
protected:
	btRigidBody *mpBody;
	btDynamicsWorld *mpOwnerWorld;
	btDefaultMotionState *mpMotionState;
	btCollisionShape *mpShape;

public:
	PhsicsAttrib(void);
	virtual ~PhsicsAttrib(void);
	virtual void init(BaseObject *obj);
	virtual void release(void);
	virtual AttribType getType()const;

	//phsics
	virtual void init(btDynamicsWorld *world, float mass, const btTransform& startTransform, SHAPE_TYPE type);
	btRigidBody* localCreateRigidBody (float mass, const btTransform& startTransform, btCollisionShape* shape);
};

}