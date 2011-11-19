#ifndef _OGRE_OBJECT_H_
#define _OGRE_OBJECT_H_

#include "../manger_source/object_manger.h"

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

namespace CL
{
//#########################################
//############  OgreAttrib   ##############
//#########################################
class OgrePhysicAttrib : public BaseAttrib
{
protected:
	//mesh
	Ogre::Entity *mpEntity;
	Ogre::SceneManager *mpSceneMgr;
	Ogre::SceneNode *mpNode;

	//phsicy
	Ogre::SceneNode *mpBodyNode;
	OgreBulletDynamics::RigidBody *mBody;
 	OgreBulletCollisions::CollisionShape *mShape;

public:
	OgrePhysicAttrib(void);
	~OgrePhysicAttrib(void);
	virtual void release(void);
	virtual AttribType getType()const;

	//Mesh update and create
	virtual void create(Ogre::SceneManager *pSceneMgr, const std::string &modelName);
	virtual void createPhysic(OgreBulletDynamics::DynamicsWorld *mWorld);
	virtual void update();

	OgreBulletDynamics::RigidBody *getBody()const;
};
//#########################################
//##########  VehiclesAttrib   ############
//#########################################

static float	gMaxEngineForce = 1000.f;
static float	gIncrementEngineForce = 1000.f;

static float	gSteeringIncrement = 0.004f;
static float	gSteeringClamp = 0.8f;

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
//###############  END   ##################
//#########################################
}

#endif //_OGRE_OBJECT_H_