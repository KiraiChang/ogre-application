#ifndef _OGRE_OBJECT_H_
#define _OGRE_OBJECT_H_

#include "../manger_source/object_manger.h"
#include "BulletSoftBody/btSoftBody.h"
//#include "BulletDynamics\Dynamics\btDynamicsWorld.h"
#include <vector>

namespace Ogre
{
	class Entity;
	class SceneManager;
	class SceneNode;
}

namespace OgreBulletDynamics
{
	class DynamicsWorld;
	class RigidBody;
	class WheeledRigidBody;
    class VehicleTuning;
    class VehicleRayCaster;
    class RaycastVehicle;
}

namespace OgreBulletCollisions
{
	class CollisionShape;
}

class btDynamicsWorld;

namespace CL
{
//#########################################
//############  OgreAttrib   ##############
//#########################################
class OgrePhysicAttrib : public BaseAttrib
{
public:
	enum ShapeType
	{
		SHAPE_TYPE_NONE,
		SHAPE_TYPE_CUBE,
		SHAPE_TYPE_SPHERE,
		SHAPE_TYPE_CYLINDER,
		SHAPE_TYPE_CONE,
	};
protected:
	//mesh
	Ogre::Entity *mpEntity;
	Ogre::SceneManager *mpSceneMgr;
	Ogre::SceneNode *mpNode;

	//phsicy
	//Ogre::SceneNode *mpBodyNode;
	OgreBulletDynamics::RigidBody *mBody;
 	OgreBulletCollisions::CollisionShape *mShape;

public:
	OgrePhysicAttrib(void);
	~OgrePhysicAttrib(void);
	virtual void release(void);
	virtual AttribType getType()const;

	//Mesh update and create
	virtual void create(Ogre::SceneManager *pSceneMgr, const std::string &modelName);
	virtual void createPhysic(OgreBulletDynamics::DynamicsWorld *mWorld,
		const float &restitution, const float &friction, const float &mass, 
		const ShapeType &type);
	virtual void update();

	OgreBulletDynamics::RigidBody *getBody()const;
};
//#########################################
//##########  VehiclesAttrib   ############
//#########################################

static const float		gMaxEngineForce = 1500.0f;
static float			gIncrementEngineForce = 1000.0f;
static const float		gMinEngineForce = 1000.0f;

//static const float		gMinSteeringClamp = 0.004f;
static const float		gSteeringIncrement = 0.004f;
static const float		gSteeringClamp = 0.8f;

class VehiclesAttrib : public BaseAttrib
{
	enum MESH_ID
	{
		MESH_CHASSIS,
		MESH_WHEEL_FRONT_RIGHT,
		MESH_WHEEL_FRONT_LEFT,
		MESH_WHEEL_REAR_RIGHT,
		MESH_WHEEL_REAR_LEFT,
		MESH_COUNT,
	};
protected:
	//mesh
	Ogre::SceneManager *mpSceneMgr;
	Ogre::SceneNode *mpMeshNodes[MESH_COUNT];
	Ogre::Entity *mpMeshEntity[MESH_COUNT];

	//phsicy
    OgreBulletDynamics::WheeledRigidBody        *mpCarChassis;
    OgreBulletDynamics::VehicleTuning	        *mpTuning;
    OgreBulletDynamics::VehicleRayCaster	    *mpVehicleRayCaster;
    OgreBulletDynamics::RaycastVehicle	        *mpVehicle;

	Ogre::SceneNode *mpPhsicyNode;
	//OgreBulletDynamics::RigidBody *mBody;
 //	OgreBulletCollisions::CollisionShape *mShape;

	int mWheelsEngine[4];
    int mWheelsEngineCount;
    int mWheelsSteerable[4];
    int mWheelsSteerableCount;

public:
    bool mSteeringLeft;
    bool mSteeringRight;
	float mSteering;
	float mEngineForce;

public:
	VehiclesAttrib(void);
	~VehiclesAttrib(void);
	virtual void release(void);
	virtual AttribType getType()const;

	//Mesh update and create
	virtual void create(Ogre::SceneManager *pSceneMgr, const std::string &vehiclesName, const std::string &wheelName);
	virtual void createPhysic(OgreBulletDynamics::DynamicsWorld *mWorld);
	virtual void update();

	//OgreBulletDynamics::RigidBody *getBody()const;
};

//#########################################
//##########  SoftBodyAttrib   ############
//#########################################

class SoftBodyAttrib : public BaseAttrib
{
protected:
	//mesh
	Ogre::SceneManager *mpSceneMgr;
	Ogre::SceneNode *mpMeshNodes;
	Ogre::Entity *mpMeshEntity;

	//phsicy
	btSoftBody *mpSoftBody;
public:
	SoftBodyAttrib(void);
	~SoftBodyAttrib(void);
	virtual void release(void);
	virtual AttribType getType()const;

	//Mesh update and create
	virtual void create(Ogre::SceneManager *pSceneMgr, const std::string &vehiclesName, const std::string &wheelName);
	virtual void createPhysic(OgreBulletDynamics::DynamicsWorld *mWorld);
	virtual void update();
};

//#########################################
//###########  RagDollAttrib   ############
//#########################################

class RagDoll : public BaseAttrib
{
protected:
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

	//mesh
	Ogre::SceneManager *mpSceneMgr;
	Ogre::SceneNode *mpBodyNodes[BODYPART_COUNT];
	Ogre::Entity *mpBodyEntity[BODYPART_COUNT];
	//Ogre::SceneNode *mpJointNodes[BODYPART_COUNT];
	//Ogre::Entity *mpJointEntity[BODYPART_COUNT];

	//phisc
	btDynamicsWorld* mWorld;

	std::vector<btCollisionShape* >  m_shapes;
	std::vector<btRigidBody* >		 m_bodies;
	std::vector<btTypedConstraint* > m_joints;

	btRigidBody* localCreateRigidBody (btScalar mass, 
		const btTransform& startTransform, 
		btCollisionShape* shape, 
		const int &part);
public:
	RagDoll();
	~RagDoll();
	virtual void release(void);
	virtual AttribType getType()const;

	//Mesh update and create
	virtual void create(Ogre::SceneManager *pSceneMgr);
	virtual void createPhysic(btDynamicsWorld *mWorld);
	virtual void update();

	//// Make ragdoll  active.
	//void ragdollEnable(bool doEnable);
	//// is ragdoll active.
	//bool isRagdollEnabled();

	//// Make constraint between body part active.
	//void constraintEnable(bool value);
	//// are constraint between body part active ?
	//bool isConstraintEnabled();

	//// Make it collidable or not.
	//void collisionEnable(bool value);
	//// if shapes can collide
	//bool isCollisionEnabled();

	//// Make it fixed in a pose or not.
	//void rigidityEnable(bool value);
	//// is it fixed on a pose?
	//bool isRigidityEnable();
};

//#########################################
//###############  END   ##################
//#########################################
}

#endif //_OGRE_OBJECT_H_