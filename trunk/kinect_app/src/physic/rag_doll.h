#ifndef _RAG_DOLL_H_
#define _RAG_DOLL_H_

class btDynamicsWorld;
class btCollisionShape;
class btRigidBody;
class btTypedConstraint;
class btTransform;

class RagDoll
{
private:
	enum
	{
		BODYPART_PELVIS = 0,
		BODYPART_SPINE,
		BODYPART_HEAD,

		BODYPART_LEFT_UPPER_LEG,
		BODYPART_LEFT_LOWER_LEG,

		BODYPART_RIGHT_UPPER_LEG,
		BODYPART_RIGHT_LOWER_LEG,

		BODYPART_LEFT_UPPER_ARM,
		BODYPART_LEFT_LOWER_ARM,

		BODYPART_RIGHT_UPPER_ARM,
		BODYPART_RIGHT_LOWER_ARM,

		BODYPART_COUNT
	};

	enum
	{
		JOINT_PELVIS_SPINE = 0,
		JOINT_SPINE_HEAD,

		JOINT_LEFT_HIP,
		JOINT_LEFT_KNEE,

		JOINT_RIGHT_HIP,
		JOINT_RIGHT_KNEE,

		JOINT_LEFT_SHOULDER,
		JOINT_LEFT_ELBOW,

		JOINT_RIGHT_SHOULDER,
		JOINT_RIGHT_ELBOW,

		JOINT_COUNT
	};

	btDynamicsWorld*							m_pWorld;
	btCollisionShape*							m_pShapes[BODYPART_COUNT];
	btRigidBody*								m_pBodies[BODYPART_COUNT];
	btTypedConstraint*							m_pJoints[JOINT_COUNT];
	btRigidBody* localCreateRigidBody (float mass, const btTransform& startTransform, btCollisionShape* shape);
public:
	RagDoll(btDynamicsWorld* pWorld);
	virtual ~RagDoll();
	virtual void init(const int &x = 0, const int &y, const int &z);
	virtual void release();
};

#endif _RAG_DOLL_H_