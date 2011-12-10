#include "ogre_object.h"

#include <OgreSceneManager.h>
#include <OgreEntity.h>

#include "OgreBulletCollisions.h"
#include "Shapes/OgreBulletCollisionsCompoundShape.h"

#include "OgreBulletDynamicsRigidBody.h"				 // for OgreBullet
#include "Shapes/OgreBulletCollisionsStaticPlaneShape.h" // for static planes
#include "Shapes/OgreBulletCollisionsBoxShape.h"		 // for Boxes

namespace CL
{
//#########################################
//############  OgreAttrib   ##############
//#########################################
	OgrePhysicAttrib::OgrePhysicAttrib(void):
		BaseAttrib(), 
		mpSceneMgr(NULL), 
		mpEntity(NULL), 
		mpNode(NULL),
		mBody(NULL),
		mShape(NULL)
	{
	}

	OgrePhysicAttrib::~OgrePhysicAttrib(void)
	{
		release();
	}

	void OgrePhysicAttrib::release(void)
	{
		if(mBody)
		{
			if(mpBodyNode)
				mpBodyNode->detachObject(mBody);
			delete mBody;
			mBody = NULL;
		}

		if(mShape)
		{
			delete mShape;
			mShape = NULL;
		}

		if(mpSceneMgr)
		{
			if(mpEntity)
			{
				if(mpNode)
					mpNode->detachObject(mpEntity);

				mpSceneMgr->destroyEntity(mpEntity);
				mpEntity = NULL;
			}

			if(mpNode)
			{
				mpSceneMgr->destroySceneNode(mpNode);
				mpNode = NULL;
			}

			if(mpBodyNode)
			{
				mpSceneMgr->destroySceneNode(mpBodyNode);
				mpBodyNode = NULL;
			}
		}
		else
		{
			assert(mpEntity);
			assert(mpNode);
			assert(mpBodyNode);
		}
	}

	AttribType OgrePhysicAttrib::getType()const
	{
		return ATTRIB_TYPE_OGRE_PHYSIC;
	}

	void OgrePhysicAttrib::create(Ogre::SceneManager *pSceneMgr, const std::string &modelName)
	{
		mpSceneMgr = pSceneMgr;
		mpEntity = mpSceneMgr->createEntity(mpObj->mName, modelName);
		mpEntity->setCastShadows(true);
		vec3 vec = mpObj->getPos();
		mpNode = mpSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3((float *)&vec));
		mpNode->attachObject((Ogre::MovableObject *)mpEntity);
		vec = mpObj->getScale();
		mpNode->setScale(Ogre::Vector3((float *)&vec));
		vec = mpObj->getDir();
		mpNode->setDirection(Ogre::Vector3((float *)&vec));
		//put self to 
		mpNode->setUserAny(Ogre::Any(static_cast<BaseObject*>(mpObj)));
	}

	void OgrePhysicAttrib::createPhysic(OgreBulletDynamics::DynamicsWorld *mWorld, 
		const float &restitution, const float &friction, const float &mass)
	{
		Ogre::Vector3 size = Ogre::Vector3::ZERO;	// size of the box
		// we need the bounding box of the box to be able to set the size of the Bullet-box
		Ogre::AxisAlignedBox boundingB = mpEntity->getBoundingBox();
		size = boundingB.getSize(); size /= 2.0f; // only the half needed
		size *= 0.95f;	// Bullet margin is a bit bigger so we need a smaller size
		// (Bullet 2.76 Physics SDK Manual page 18)
		size *=  mpObj->getScale().y;// don't forget to scale down the Bullet-box too

		// after that create the Bullet shape with the calculated size
		mShape = new OgreBulletCollisions::BoxCollisionShape(size);
		// and the Bullet rigid body
		mBody = new OgreBulletDynamics::RigidBody(
			mpObj->mName, mWorld);
		mBody->setShape(	
			mpNode,
			mShape,
			restitution,			// dynamic body restitution 0.6
			friction,			// dynamic body friction 0.6
			mass, 			// dynamic body mass 1.0
			Ogre::Vector3((float *)&mpObj->getPos()),		// starting position of the box
			Ogre::Quaternion(0,0,0,1));// orientation of the box
	}

	void OgrePhysicAttrib::update()
	{
		vec3 vec = mpObj->getPos();
		mpNode->setPosition(Ogre::Vector3((float *)&vec));
		vec = mpObj->getScale();
		mpNode->setScale(Ogre::Vector3((float *)&vec));
		vec = mpObj->getDir();
		mpNode->setDirection(Ogre::Vector3((float *)&vec));
	}

	OgreBulletDynamics::RigidBody *OgrePhysicAttrib::getBody()const
	{
		assert(mBody);
		return mBody;
	}
//#########################################
//##########  VehiclesAttrib   ############
//#########################################

	static float	gWheelRadius = 0.5f;
	static float	gWheelWidth = 0.4f;

	static float	gWheelFriction = 1e30f;//1000;//1e30f;
	static float	gSuspensionStiffness = 20.f;
	static float	gSuspensionDamping = 2.3f;
	static float	gSuspensionCompression = 4.4f;

	static float	gRollInfluence = 0.1f;//1.0f;
	static float   gSuspensionRestLength = 0.6;
	static float   gMaxSuspensionTravelCm = 500.0;
	static float   gFrictionSlip = 10.5;

	#define CUBE_HALF_EXTENTS 1

	VehiclesAttrib::VehiclesAttrib(void):
		BaseAttrib(), 
		mpSceneMgr(NULL), 
		mpCarChassis(NULL),
		mpTuning(NULL),
		mpVehicleRayCaster(NULL),
		mpVehicle(NULL),
		mSteeringLeft(FALSE),
		mSteeringRight(FALSE)
	{
			for(int i = 0; i < MESH_COUNT; i++)
			{
				mpMeshEntity[i] = NULL;
				mpMeshNodes[i] = NULL;
			}

			for (int i = 0; i < 4; i++)
			{
				mWheelsEngine[i] = 0;
				mWheelsSteerable[i] = 0;
			}
			mWheelsEngineCount = 2;
			mWheelsEngine[0] = 0;
			mWheelsEngine[1] = 1;
			mWheelsEngine[2] = 2;
			mWheelsEngine[3] = 3;

			mWheelsSteerableCount = 2;
			mWheelsSteerable[0] = 0;
			mWheelsSteerable[1] = 1;

			mEngineForce = 0;
			mSteering = 0;
	}
	VehiclesAttrib::~VehiclesAttrib(void)
	{
		release();
	}

	void VehiclesAttrib::release(void)
	{

		if(mpCarChassis)
		{
			if(mpPhsicyNode)
				mpPhsicyNode->detachObject(mpCarChassis);
			delete mpCarChassis;
			mpCarChassis = NULL;
		}

		if(mpTuning)
		{
			delete mpTuning;
			mpTuning = NULL;
		}

		if(mpVehicleRayCaster)
		{
			delete mpVehicleRayCaster;
			mpVehicleRayCaster = NULL;
		}

		if(mpVehicle)
		{
			delete mpVehicle;
			mpVehicle = NULL;
		}

		if(mpSceneMgr)
		{
			for(int i = 0; i < MESH_COUNT; i++)
			{
				if(mpMeshEntity[i])
				{
					if(mpMeshNodes[i])
						mpMeshNodes[i]->detachObject(mpMeshEntity[i]);

					mpSceneMgr->destroyEntity(mpMeshEntity[i]);
					mpMeshEntity[i] = NULL;
				}

				if(mpMeshNodes[i])
				{
					mpSceneMgr->destroySceneNode(mpMeshNodes[i]);
					mpMeshNodes[i] = NULL;
				}
			}

			if(mpPhsicyNode)
			{
				mpSceneMgr->destroySceneNode(mpPhsicyNode);
				mpPhsicyNode = NULL;
			}
		}
	}

	AttribType VehiclesAttrib::getType()const
	{
		return ATTRIB_TYPE_VEHICELES;
	}

	//Mesh update and create
	void VehiclesAttrib::create(Ogre::SceneManager *pSceneMgr, const std::string &vehiclesName, const std::string &wheelName)
	{
		assert(pSceneMgr != NULL);

		const Ogre::Vector3 chassisShift(0, 1.0, 0);;
		float connectionHeight = 0.7f;

		char name[128];
#ifdef _WIN32
		sprintf_s(name, "%s_Vehicles",mpObj->mName.c_str());
#else
		sprintf(name, "%s_Vehicles",mpObj->mName.c_str());
#endif
		mpSceneMgr = pSceneMgr;
		//Create Chassis Model;
		mpMeshEntity[MESH_CHASSIS] = mpSceneMgr->createEntity(name, vehiclesName);

		mpPhsicyNode = mpSceneMgr->getRootSceneNode ()->createChildSceneNode ();

		mpMeshNodes[MESH_CHASSIS] = mpPhsicyNode->createChildSceneNode();

		mpMeshNodes[MESH_CHASSIS]->attachObject (mpMeshEntity[MESH_CHASSIS]);
		mpMeshNodes[MESH_CHASSIS]->setPosition (chassisShift);
		mpMeshEntity[MESH_CHASSIS]->setCastShadows(true);

		//Create Wheels Model;
		Ogre::Vector3 wheelDirectionCS0(0,-1,0);
		Ogre::Vector3 wheelAxleCS(-1,0,0);

		for (size_t i = 0; i < 4; i++)
		{
#ifdef _WIN32
			sprintf_s(name, "%s_Wheel_%d",mpObj->mName.c_str(), i);
#else
			sprintf(name, "%s_Wheel_%d",mpObj->mName.c_str(), i);
#endif
			mpMeshEntity[i+1] = mpSceneMgr->createEntity(name, wheelName);

			mpMeshEntity[i+1]->setCastShadows(true);

			mpMeshNodes[i+1] = mpSceneMgr->getRootSceneNode ()->createChildSceneNode ();
			mpMeshNodes[i+1]->attachObject (mpMeshEntity[i+1]);

		}
	}

	void VehiclesAttrib::createPhysic(OgreBulletDynamics::DynamicsWorld *mWorld)
	{
		const Ogre::Vector3 chassisShift(0, 1.0, 0);
		float connectionHeight = 0.7f;

		Ogre::Vector3 CarPosition = Ogre::Vector3((float *)&mpObj->getPos());

		OgreBulletCollisions::BoxCollisionShape* chassisShape = new OgreBulletCollisions::BoxCollisionShape(Ogre::Vector3(1.f,0.75f,2.1f));
        OgreBulletCollisions::CompoundCollisionShape* compound = new OgreBulletCollisions::CompoundCollisionShape();
        compound->addChildShape(chassisShape, chassisShift); 

		char name[128];
#ifdef _WIN32
		sprintf_s(name, "%s_CarChassis",mpObj->mName.c_str());
#else
		sprintf(name, "%s_CarChassis",mpObj->mName.c_str());
#endif

        mpCarChassis = new OgreBulletDynamics::WheeledRigidBody(name, mWorld);

        mpCarChassis->setShape (mpPhsicyNode, compound, 0.6, 0.6, 800, CarPosition, Ogre::Quaternion::IDENTITY);
        mpCarChassis->setDamping(0.2, 0.2);

        mpCarChassis->disableDeactivation ();
        mpTuning = new OgreBulletDynamics::VehicleTuning(
            gSuspensionStiffness,
            gSuspensionCompression,
            gSuspensionDamping,
            gMaxSuspensionTravelCm,
            gFrictionSlip);

        mpVehicleRayCaster = new OgreBulletDynamics::VehicleRayCaster(mWorld);
        mpVehicle = new OgreBulletDynamics::RaycastVehicle(mpCarChassis, mpTuning, mpVehicleRayCaster);

        {
            int rightIndex = 0;
            int upIndex = 1;
            int forwardIndex = 2;

            mpVehicle->setCoordinateSystem(rightIndex, upIndex, forwardIndex);
		}

		{
                bool isFrontWheel = true;

				Ogre::Vector3 wheelDirectionCS0(0,-1,0);
				Ogre::Vector3 wheelAxleCS(-1,0,0);

                Ogre::Vector3 connectionPointCS0 (
                    CUBE_HALF_EXTENTS-(0.3*gWheelWidth),
                    connectionHeight,
                    2*CUBE_HALF_EXTENTS-gWheelRadius);


                mpVehicle->addWheel(
                    mpMeshNodes[MESH_WHEEL_FRONT_RIGHT],
                    connectionPointCS0,
                    wheelDirectionCS0,
                    wheelAxleCS,
                    gSuspensionRestLength,
                    gWheelRadius,
                    isFrontWheel, gWheelFriction, gRollInfluence);

                connectionPointCS0 = Ogre::Vector3(
                    -CUBE_HALF_EXTENTS+(0.3*gWheelWidth),
                    connectionHeight,
                    2*CUBE_HALF_EXTENTS-gWheelRadius);


                mpVehicle->addWheel(
                    mpMeshNodes[MESH_WHEEL_FRONT_LEFT],
                    connectionPointCS0,
                    wheelDirectionCS0,
                    wheelAxleCS,
                    gSuspensionRestLength,
                    gWheelRadius,
                    isFrontWheel, gWheelFriction, gRollInfluence);


                connectionPointCS0 = Ogre::Vector3(
                    -CUBE_HALF_EXTENTS+(0.3*gWheelWidth),
                    connectionHeight,
                    -2*CUBE_HALF_EXTENTS+gWheelRadius);

                isFrontWheel = false;
                mpVehicle->addWheel(
                    mpMeshNodes[MESH_WHEEL_REAR_RIGHT],
                    connectionPointCS0,
                    wheelDirectionCS0,
                    wheelAxleCS,
                    gSuspensionRestLength,
                    gWheelRadius,
                    isFrontWheel, gWheelFriction, gRollInfluence);

                connectionPointCS0 = Ogre::Vector3(
                    CUBE_HALF_EXTENTS-(0.3*gWheelWidth),
                    connectionHeight,
                    -2*CUBE_HALF_EXTENTS+gWheelRadius);

                mpVehicle->addWheel(
                    mpMeshNodes[MESH_WHEEL_REAR_LEFT],
                    connectionPointCS0,
                    wheelDirectionCS0,
                    wheelAxleCS,
                    gSuspensionRestLength,
                    gWheelRadius,
                    isFrontWheel, gWheelFriction, gRollInfluence);

                //mVehicle->setWheelsAttached();

            }
	}

	void VehiclesAttrib::update()
	{
		for (int i = mWheelsEngine[0]; i < mWheelsEngineCount; i++)
		{
			mpVehicle->applyEngineForce (mEngineForce, mWheelsEngine[i]);
		}

		for (int i = mWheelsSteerable[0]; i < mWheelsSteerableCount; i++)
		{
			if (i < 2)
				mpVehicle->setSteeringValue (mSteering, mWheelsSteerable[i]);
			else
				mpVehicle->setSteeringValue (-mSteering, mWheelsSteerable[i]);
		}
		
		mpObj->setPos((float *)&mpCarChassis->getRootNode()->getPosition());
	}



	//OgreBulletDynamics::RigidBody *VehiclesAttrib::getBody()const
	//{
	//	assert(mBody);
	//	return mBody;
	//}
//#########################################
//##########  SoftBodyAttrib   ############
//#########################################


	SoftBodyAttrib::SoftBodyAttrib(void)
	{
	}

	SoftBodyAttrib::~SoftBodyAttrib(void)
	{
	}

	void SoftBodyAttrib::release(void)
	{
	}

	AttribType SoftBodyAttrib::getType()const
	{
		return ATTRIB_TYPE_SOFTBODY;
	}


	//Mesh update and create
	void SoftBodyAttrib::create(Ogre::SceneManager *pSceneMgr, const std::string &vehiclesName, const std::string &wheelName)
	{
	}

	void SoftBodyAttrib::createPhysic(OgreBulletDynamics::DynamicsWorld *mWorld)
	{
		/*mpSoftBody = new btSoftBody();*/
	}

	void SoftBodyAttrib::update()
	{
	}



//#########################################
//###############  END   ##################
//#########################################
}