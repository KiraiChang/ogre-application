#include "ogre_object.h"

#include <OgreSceneManager.h>
#include <OgreEntity.h>

#include "OgreBulletCollisions.h"
#include "Shapes/OgreBulletCollisionsCompoundShape.h"

#include "OgreBulletDynamicsRigidBody.h"				 // for OgreBullet
#include "Shapes/OgreBulletCollisionsStaticPlaneShape.h" // for static planes
#include "Shapes/OgreBulletCollisionsBoxShape.h"		 // for Boxes
#include "Shapes/OgreBulletCollisionsSphereShape.h"		 // for Sphere
#include "Shapes/OgreBulletCollisionsCylinderShape.h"    // for Cylinder
#include "Shapes/OgreBulletCollisionsConeShape.h"        // for Cone
#include "Shapes/OgreBulletCollisionsTrimeshShape.h"     // for None above
#include "Utils/OgreBulletCollisionsMeshToShapeConverter.h"


#include "btBulletDynamicsCommon.h"

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
			//if(mpBodyNode)
			//	mpBodyNode->detachObject(mBody);
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

			//if(mpBodyNode)
			//{
			//	mpSceneMgr->destroySceneNode(mpBodyNode);
			//	mpBodyNode = NULL;
			//}
		}
		else
		{
			assert(mpEntity);
			assert(mpNode);
			//assert(mpBodyNode);
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
		const float &restitution, const float &friction, const float &mass,
		const ShapeType &type)
	{
		Ogre::Vector3 size = Ogre::Vector3::ZERO;	// size of the box
		// we need the bounding box of the box to be able to set the size of the Bullet-box
		Ogre::AxisAlignedBox boundingB = mpEntity->getBoundingBox();
		size = boundingB.getSize(); size /= 2.0f; // only the half needed
		size *= 0.95f;	// Bullet margin is a bit bigger so we need a smaller size
		// (Bullet 2.76 Physics SDK Manual page 18)
		size *=  mpObj->getScale().y;// don't forget to scale down the Bullet-box too

		// after that create the Bullet shape with the calculated size
		switch(type)
		{
		case SHAPE_TYPE_CUBE:
			mShape = new OgreBulletCollisions::BoxCollisionShape(size);
			break;
		case SHAPE_TYPE_SPHERE:
			mShape = new OgreBulletCollisions::SphereCollisionShape(size.x);
			break;
		case SHAPE_TYPE_CYLINDER:
			mShape = new OgreBulletCollisions::CylinderCollisionShape(size, Ogre::Vector3::UNIT_X);
			break;
		case SHAPE_TYPE_CONE:
			mShape = new OgreBulletCollisions::ConeCollisionShape(size.x * 1.5, size.y * 1.5, Ogre::Vector3::UNIT_Y);
			break;
		default:
			OgreBulletCollisions::StaticMeshToShapeConverter *trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(mpEntity);
			mShape = trimeshConverter->createTrimesh();
			delete trimeshConverter;
			break;
		}
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
		//vec3 vec = mpObj->getPos();
		//mpNode->setPosition(Ogre::Vector3((float *)&vec));
		//vec = mpObj->getScale();
		//mpNode->setScale(Ogre::Vector3((float *)&vec));
		//vec = mpObj->getDir();
		//mpNode->setDirection(Ogre::Vector3((float *)&vec));
		Ogre::Vector3 vec = mpNode->getPosition();
		mpObj->setPos((float *)&vec);

		vec = mpNode->getScale();
		mpObj->setScale((float *)&vec);

		Ogre::Quaternion q = mpNode->getOrientation();
		vec.x = q.x;
		vec.y = q.y;
		vec.z = q.z;
		mpObj->setDir((float *)&vec);
	}

	void OgrePhysicAttrib::setPos()
	{
		if(mBody)
		{
			vec3 vec = mpObj->getPos();
			mpNode->setPosition(Ogre::Vector3((float *)&vec));
			Ogre::Quaternion q = mpNode->getOrientation();
			mBody->getBulletRigidBody()->getWorldTransform().setOrigin(btVector3(vec.x, vec.y, vec.z));
		}
	}

	void OgrePhysicAttrib::setScale()
	{
		if(mBody)
		{
			vec3 vec = mpObj->getScale();
			mpNode->setScale(Ogre::Vector3((float *)&vec));
		}
	}

	void OgrePhysicAttrib::setDir()
	{
		//if(mBody)
		//{
		//	vec3 vec = mpObj->getDir();
		//	mpNode->setDirection(Ogre::Vector3((float *)&vec));
		//}
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
//###########  RagDollAttrib   ############
//#########################################

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2     1.57079632679489661923
#endif

#ifndef M_PI_4
#define M_PI_4     0.785398163397448309616
#endif

#define CONSTRAINT_DEBUG_SIZE 0.2f

	btRigidBody* RagDoll::localCreateRigidBody (btScalar mass, 
		const btTransform& startTransform, 
		btCollisionShape* shape,
		const int &part)
	{
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0,0,0);
		if (isDynamic)
			shape->calculateLocalInertia(mass,localInertia);

		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

		char name[64];
#ifdef _WIN32
		sprintf_s(name, "%s_RagDoll_%d",mpObj->mName.c_str(), part);
#else
		sprintf(name, "%s_RagDoll_%d",mpObj->mName.c_str(), part);
#endif
		mpBodyEntity[part] = mpSceneMgr->createEntity(name, "cube.mesh");
		mpBodyEntity[part]->setCastShadows(true);
		vec3 vec;
		mpBodyNodes[part] = mpSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3((float *)&startTransform));
		mpBodyNodes[part]->attachObject((Ogre::MovableObject *)mpBodyEntity[part]);
		vec = mpObj->getScale();
		mpBodyNodes[part]->setScale(Ogre::Vector3((float *)&vec));
		vec = mpObj->getDir();
		mpBodyNodes[part]->setDirection(Ogre::Vector3((float *)&vec));
		//put self to 
		mpBodyNodes[part]->setUserAny(Ogre::Any(static_cast<BaseObject*>(mpObj)));

		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,shape,localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		mWorld->addRigidBody(body);

		return body;
	}

	RagDoll::RagDoll()
	{
		m_bodies.resize(BODYPART_COUNT);
		m_shapes.resize(BODYPART_COUNT);
		m_joints.resize(JOINT_COUNT);
	}

	RagDoll::~RagDoll()
	{
		release();
	}

	void RagDoll::release(void)
	{
		// Remove all constraints
		for(std::vector<btTypedConstraint* >::iterator i=m_joints.begin();
			i!=m_joints.end();
			++i) 
		{
			mWorld->removeConstraint(*i);
			delete *i;
		}

		// Remove all bodies and shapes
		for(std::vector<btRigidBody* >::iterator i=m_bodies.begin();
			i!=m_bodies.end();
			++i) 
		{
			mWorld->removeRigidBody(*i);

			delete (*i)->getMotionState();
			delete *i;
		}
		for(std::vector<btCollisionShape* >::iterator i=m_shapes.begin();
			i!=m_shapes.end();
			++i) 
		{
			delete *i;
		}

				if(mpSceneMgr)
		{
			for(int i = 0; i < BODYPART_COUNT; i++)
			{
				if(mpBodyEntity[i])
				{
					if(mpBodyNodes[i])
						mpBodyNodes[i]->detachObject(mpBodyEntity[i]);

					mpSceneMgr->destroyEntity(mpBodyEntity[i]);
					mpBodyEntity[i] = NULL;
				}

				if(mpBodyNodes[i])
				{
					mpSceneMgr->destroySceneNode(mpBodyNodes[i]);
					mpBodyNodes[i] = NULL;
				}
			}
		}
	}

	AttribType RagDoll::getType()const
	{
		return ATTRIB_TYPE_RAGDOLL;
	}

	//Mesh update and create
	void RagDoll::create(Ogre::SceneManager *pSceneMgr)
	{
		mpSceneMgr = pSceneMgr;
	}

	void RagDoll::createPhysic(btDynamicsWorld *pWorld)
	{
		mWorld = pWorld;
		// Setup all the rigid bodies
		//btTransform offset; offset.setIdentity();
		//offset.setOrigin(*(btVector3 *)&pos);


		//btTransform transform;

		//for(int i = 0; i < 2; i++)
		//{
		//	transform.setIdentity();
		//	transform.setOrigin(btVector3(btScalar(0.), btScalar(1.), btScalar(0.)));
		//	m_bodies.push_back(
		//		localCreateRigidBody(btScalar(1.), 
		//								offset*transform, 
		//								new btCapsuleShape(btScalar(0.15), btScalar(0.20))
		//								)
		//							);
		//}

		//// Setup some damping on the m_bodies
		//for(std::vector<btRigidBody* >::iterator i=m_bodies.begin();
		//	i!=m_bodies.end();
		//	++i) 
		//{
		//	(*i)->setDamping(0.05, 0.85);
		//	(*i)->setDeactivationTime(0.8);
		//	(*i)->setSleepingThresholds(1.6, 2.5);
		//}

		//// Now setup the constraints
		//btHingeConstraint* hingeC;
		////btConeTwistConstraint* coneC;
		//btTransform localA, localB;

		//// for
		//{
		//	localA.setIdentity(); localB.setIdentity();
		//	localA.getBasis().setEulerZYX(0, Ogre::Math::TWO_PI,0); 
		//	localA.setOrigin(btVector3(btScalar(0.), btScalar(0.15), btScalar(0.)));
		//	localB.getBasis().setEulerZYX(0,Ogre::Math::TWO_PI,0); 
		//	localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.15), btScalar(0.)));
		//	hingeC =  new btHingeConstraint(*m_bodies[0], *m_bodies[1], 
		//		localA, localB);
		//	hingeC->setLimit(btScalar(-Ogre::Math::TWO_PI*2), btScalar(Ogre::Math::TWO_PI));
		//	m_joints.push_back(hingeC);
		//	mWorld->addConstraint(hingeC, true);
		//}
		// Setup the geometry
		m_shapes[BODYPART_PELVIS] = new btCapsuleShape(btScalar(0.15), btScalar(0.20));
		m_shapes[BODYPART_SPINE] = new btCapsuleShape(btScalar(0.15), btScalar(0.28));
		m_shapes[BODYPART_HEAD] = new btCapsuleShape(btScalar(0.10), btScalar(0.05));
		m_shapes[BODYPART_LEFT_UPPER_LEG] = new btCapsuleShape(btScalar(0.07), btScalar(0.45));
		m_shapes[BODYPART_LEFT_LOWER_LEG] = new btCapsuleShape(btScalar(0.05), btScalar(0.37));
		m_shapes[BODYPART_RIGHT_UPPER_LEG] = new btCapsuleShape(btScalar(0.07), btScalar(0.45));
		m_shapes[BODYPART_RIGHT_LOWER_LEG] = new btCapsuleShape(btScalar(0.05), btScalar(0.37));
		m_shapes[BODYPART_LEFT_UPPER_ARM] = new btCapsuleShape(btScalar(0.05), btScalar(0.33));
		m_shapes[BODYPART_LEFT_LOWER_ARM] = new btCapsuleShape(btScalar(0.04), btScalar(0.25));
		m_shapes[BODYPART_RIGHT_UPPER_ARM] = new btCapsuleShape(btScalar(0.05), btScalar(0.33));
		m_shapes[BODYPART_RIGHT_LOWER_ARM] = new btCapsuleShape(btScalar(0.04), btScalar(0.25));

		// Setup all the rigid bodies
		btTransform offset; offset.setIdentity();
		vec3 pos = mpObj->getPos();
		offset.setOrigin(*(btVector3 *)&pos);

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(btVector3(btScalar(0.), btScalar(1.), btScalar(0.)));
		m_bodies[BODYPART_PELVIS] = localCreateRigidBody(btScalar(1.), offset*transform, m_shapes[BODYPART_PELVIS], BODYPART_PELVIS);

		transform.setIdentity();
		transform.setOrigin(btVector3(btScalar(0.), btScalar(1.2), btScalar(0.)));
		m_bodies[BODYPART_SPINE] = localCreateRigidBody(btScalar(1.), offset*transform, m_shapes[BODYPART_SPINE], BODYPART_SPINE);

		transform.setIdentity();
		transform.setOrigin(btVector3(btScalar(0.), btScalar(1.6), btScalar(0.)));
		m_bodies[BODYPART_HEAD] = localCreateRigidBody(btScalar(1.), offset*transform, m_shapes[BODYPART_HEAD], BODYPART_HEAD);

		transform.setIdentity();
		transform.setOrigin(btVector3(btScalar(-0.18), btScalar(0.65), btScalar(0.)));
		m_bodies[BODYPART_LEFT_UPPER_LEG] = localCreateRigidBody(btScalar(1.), offset*transform, m_shapes[BODYPART_LEFT_UPPER_LEG], BODYPART_LEFT_UPPER_LEG);

		transform.setIdentity();
		transform.setOrigin(btVector3(btScalar(-0.18), btScalar(0.2), btScalar(0.)));
		m_bodies[BODYPART_LEFT_LOWER_LEG] = localCreateRigidBody(btScalar(1.), offset*transform, m_shapes[BODYPART_LEFT_LOWER_LEG], BODYPART_LEFT_LOWER_LEG);

		transform.setIdentity();
		transform.setOrigin(btVector3(btScalar(0.18), btScalar(0.65), btScalar(0.)));
		m_bodies[BODYPART_RIGHT_UPPER_LEG] = localCreateRigidBody(btScalar(1.), offset*transform, m_shapes[BODYPART_RIGHT_UPPER_LEG], BODYPART_RIGHT_UPPER_LEG);

		transform.setIdentity();
		transform.setOrigin(btVector3(btScalar(0.18), btScalar(0.2), btScalar(0.)));
		m_bodies[BODYPART_RIGHT_LOWER_LEG] = localCreateRigidBody(btScalar(1.), offset*transform, m_shapes[BODYPART_RIGHT_LOWER_LEG], BODYPART_RIGHT_LOWER_LEG);

		transform.setIdentity();
		transform.setOrigin(btVector3(btScalar(-0.35), btScalar(1.45), btScalar(0.)));
		transform.getBasis().setEulerZYX(0,0,M_PI_2);
		m_bodies[BODYPART_LEFT_UPPER_ARM] = localCreateRigidBody(btScalar(1.), offset*transform, m_shapes[BODYPART_LEFT_UPPER_ARM], BODYPART_LEFT_UPPER_ARM);

		transform.setIdentity();
		transform.setOrigin(btVector3(btScalar(-0.7), btScalar(1.45), btScalar(0.)));
		transform.getBasis().setEulerZYX(0,0,M_PI_2);
		m_bodies[BODYPART_LEFT_LOWER_ARM] = localCreateRigidBody(btScalar(1.), offset*transform, m_shapes[BODYPART_LEFT_LOWER_ARM], BODYPART_LEFT_LOWER_ARM);

		transform.setIdentity();
		transform.setOrigin(btVector3(btScalar(0.35), btScalar(1.45), btScalar(0.)));
		transform.getBasis().setEulerZYX(0,0,-M_PI_2);
		m_bodies[BODYPART_RIGHT_UPPER_ARM] = localCreateRigidBody(btScalar(1.), offset*transform, m_shapes[BODYPART_RIGHT_UPPER_ARM], BODYPART_RIGHT_UPPER_ARM);

		transform.setIdentity();
		transform.setOrigin(btVector3(btScalar(0.7), btScalar(1.45), btScalar(0.)));
		transform.getBasis().setEulerZYX(0,0,-M_PI_2);
		m_bodies[BODYPART_RIGHT_LOWER_ARM] = localCreateRigidBody(btScalar(1.), offset*transform, m_shapes[BODYPART_RIGHT_LOWER_ARM], BODYPART_RIGHT_LOWER_ARM);

		// Setup some damping on the m_bodies
		for (int i = 0; i < BODYPART_COUNT; ++i)
		{
			m_bodies[i]->setDamping(0.05, 0.85);
			m_bodies[i]->setDeactivationTime(0.8);
			m_bodies[i]->setSleepingThresholds(1.6, 2.5);
		}

		// Now setup the constraints
		btHingeConstraint* hingeC;
		btConeTwistConstraint* coneC;

		btTransform localA, localB;

		localA.setIdentity(); localB.setIdentity();
		localA.getBasis().setEulerZYX(0,M_PI_2,0); localA.setOrigin(btVector3(btScalar(0.), btScalar(0.15), btScalar(0.)));
		localB.getBasis().setEulerZYX(0,M_PI_2,0); localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.15), btScalar(0.)));
		hingeC =  new btHingeConstraint(*m_bodies[BODYPART_PELVIS], *m_bodies[BODYPART_SPINE], localA, localB);
		hingeC->setLimit(btScalar(-M_PI_4), btScalar(M_PI_2));
		m_joints[JOINT_PELVIS_SPINE] = hingeC;
		hingeC->setDbgDrawSize(CONSTRAINT_DEBUG_SIZE);

		mWorld->addConstraint(m_joints[JOINT_PELVIS_SPINE], true);


		localA.setIdentity(); localB.setIdentity();
		localA.getBasis().setEulerZYX(0,0,M_PI_2); localA.setOrigin(btVector3(btScalar(0.), btScalar(0.30), btScalar(0.)));
		localB.getBasis().setEulerZYX(0,0,M_PI_2); localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.14), btScalar(0.)));
		coneC = new btConeTwistConstraint(*m_bodies[BODYPART_SPINE], *m_bodies[BODYPART_HEAD], localA, localB);
		coneC->setLimit(M_PI_4, M_PI_4, M_PI_2);
		m_joints[JOINT_SPINE_HEAD] = coneC;
		coneC->setDbgDrawSize(CONSTRAINT_DEBUG_SIZE);

		mWorld->addConstraint(m_joints[JOINT_SPINE_HEAD], true);


		localA.setIdentity(); localB.setIdentity();
		localA.getBasis().setEulerZYX(0,0,-M_PI_4*5); localA.setOrigin(btVector3(btScalar(-0.18), btScalar(-0.10), btScalar(0.)));
		localB.getBasis().setEulerZYX(0,0,-M_PI_4*5); localB.setOrigin(btVector3(btScalar(0.), btScalar(0.225), btScalar(0.)));
		coneC = new btConeTwistConstraint(*m_bodies[BODYPART_PELVIS], *m_bodies[BODYPART_LEFT_UPPER_LEG], localA, localB);
		coneC->setLimit(M_PI_4, M_PI_4, 0);
		m_joints[JOINT_LEFT_HIP] = coneC;
		coneC->setDbgDrawSize(CONSTRAINT_DEBUG_SIZE);

		mWorld->addConstraint(m_joints[JOINT_LEFT_HIP], true);

		localA.setIdentity(); localB.setIdentity();
		localA.getBasis().setEulerZYX(0,M_PI_2,0); localA.setOrigin(btVector3(btScalar(0.), btScalar(-0.225), btScalar(0.)));
		localB.getBasis().setEulerZYX(0,M_PI_2,0); localB.setOrigin(btVector3(btScalar(0.), btScalar(0.185), btScalar(0.)));
		hingeC =  new btHingeConstraint(*m_bodies[BODYPART_LEFT_UPPER_LEG], *m_bodies[BODYPART_LEFT_LOWER_LEG], localA, localB);
		hingeC->setLimit(btScalar(0), btScalar(M_PI_2));
		m_joints[JOINT_LEFT_KNEE] = hingeC;
		hingeC->setDbgDrawSize(CONSTRAINT_DEBUG_SIZE);

		mWorld->addConstraint(m_joints[JOINT_LEFT_KNEE], true);


		localA.setIdentity(); localB.setIdentity();
		localA.getBasis().setEulerZYX(0,0,M_PI_4); localA.setOrigin(btVector3(btScalar(0.18), btScalar(-0.10), btScalar(0.)));
		localB.getBasis().setEulerZYX(0,0,M_PI_4); localB.setOrigin(btVector3(btScalar(0.), btScalar(0.225), btScalar(0.)));
		coneC = new btConeTwistConstraint(*m_bodies[BODYPART_PELVIS], *m_bodies[BODYPART_RIGHT_UPPER_LEG], localA, localB);
		coneC->setLimit(M_PI_4, M_PI_4, 0);
		m_joints[JOINT_RIGHT_HIP] = coneC;
		coneC->setDbgDrawSize(CONSTRAINT_DEBUG_SIZE);

		mWorld->addConstraint(m_joints[JOINT_RIGHT_HIP], true);

		localA.setIdentity(); localB.setIdentity();
		localA.getBasis().setEulerZYX(0,M_PI_2,0); localA.setOrigin(btVector3(btScalar(0.), btScalar(-0.225), btScalar(0.)));
		localB.getBasis().setEulerZYX(0,M_PI_2,0); localB.setOrigin(btVector3(btScalar(0.), btScalar(0.185), btScalar(0.)));
		hingeC =  new btHingeConstraint(*m_bodies[BODYPART_RIGHT_UPPER_LEG], *m_bodies[BODYPART_RIGHT_LOWER_LEG], localA, localB);
		hingeC->setLimit(btScalar(0), btScalar(M_PI_2));
		m_joints[JOINT_RIGHT_KNEE] = hingeC;
		hingeC->setDbgDrawSize(CONSTRAINT_DEBUG_SIZE);

		mWorld->addConstraint(m_joints[JOINT_RIGHT_KNEE], true);


		localA.setIdentity(); localB.setIdentity();
		localA.getBasis().setEulerZYX(0,0,M_PI); localA.setOrigin(btVector3(btScalar(-0.2), btScalar(0.15), btScalar(0.)));
		localB.getBasis().setEulerZYX(0,0,M_PI_2); localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.18), btScalar(0.)));
		coneC = new btConeTwistConstraint(*m_bodies[BODYPART_SPINE], *m_bodies[BODYPART_LEFT_UPPER_ARM], localA, localB);
		coneC->setLimit(M_PI_2, M_PI_2, 0);
		coneC->setDbgDrawSize(CONSTRAINT_DEBUG_SIZE);

		m_joints[JOINT_LEFT_SHOULDER] = coneC;
		mWorld->addConstraint(m_joints[JOINT_LEFT_SHOULDER], true);

		localA.setIdentity(); localB.setIdentity();
		localA.getBasis().setEulerZYX(0,M_PI_2,0); localA.setOrigin(btVector3(btScalar(0.), btScalar(0.18), btScalar(0.)));
		localB.getBasis().setEulerZYX(0,M_PI_2,0); localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.14), btScalar(0.)));
		hingeC =  new btHingeConstraint(*m_bodies[BODYPART_LEFT_UPPER_ARM], *m_bodies[BODYPART_LEFT_LOWER_ARM], localA, localB);
//		hingeC->setLimit(btScalar(-M_PI_2), btScalar(0));
		hingeC->setLimit(btScalar(0), btScalar(M_PI_2));
		m_joints[JOINT_LEFT_ELBOW] = hingeC;
		hingeC->setDbgDrawSize(CONSTRAINT_DEBUG_SIZE);

		mWorld->addConstraint(m_joints[JOINT_LEFT_ELBOW], true);



		localA.setIdentity(); localB.setIdentity();
		localA.getBasis().setEulerZYX(0,0,0); localA.setOrigin(btVector3(btScalar(0.2), btScalar(0.15), btScalar(0.)));
		localB.getBasis().setEulerZYX(0,0,M_PI_2); localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.18), btScalar(0.)));
		coneC = new btConeTwistConstraint(*m_bodies[BODYPART_SPINE], *m_bodies[BODYPART_RIGHT_UPPER_ARM], localA, localB);
		coneC->setLimit(M_PI_2, M_PI_2, 0);
		m_joints[JOINT_RIGHT_SHOULDER] = coneC;
		coneC->setDbgDrawSize(CONSTRAINT_DEBUG_SIZE);

		mWorld->addConstraint(m_joints[JOINT_RIGHT_SHOULDER], true);

		localA.setIdentity(); localB.setIdentity();
		localA.getBasis().setEulerZYX(0,M_PI_2,0); localA.setOrigin(btVector3(btScalar(0.), btScalar(0.18), btScalar(0.)));
		localB.getBasis().setEulerZYX(0,M_PI_2,0); localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.14), btScalar(0.)));
		hingeC =  new btHingeConstraint(*m_bodies[BODYPART_RIGHT_UPPER_ARM], *m_bodies[BODYPART_RIGHT_LOWER_ARM], localA, localB);
//		hingeC->setLimit(btScalar(-M_PI_2), btScalar(0));
		hingeC->setLimit(btScalar(0), btScalar(M_PI_2));
		m_joints[JOINT_RIGHT_ELBOW] = hingeC;
		hingeC->setDbgDrawSize(CONSTRAINT_DEBUG_SIZE);

		mWorld->addConstraint(m_joints[JOINT_RIGHT_ELBOW], true);
	}

	void RagDoll::update()
	{
	}

	//// Make ragdoll  active.
	//void RagDoll::ragdollEnable(bool doEnable)
	//{
	//}

	//// is ragdoll active.
	//bool RagDoll::isRagdollEnabled()
	//{
	//}


	//// Make constraint between body part active.
	//void RagDoll::constraintEnable(bool value)
	//{
	//}

	//// are constraint between body part active ?
	//bool RagDoll::isConstraintEnabled()
	//{
	//}


	//// Make it collidable or not.
	//void RagDoll::collisionEnable(bool value)
	//{
	//}

	//// if shapes can collide
	//bool RagDoll::isCollisionEnabled()
	//{
	//}


	//// Make it fixed in a pose or not.
	//void RagDoll::rigidityEnable(bool value)
	//{
	//}

	//// is it fixed on a pose?
	//bool RagDoll::isRigidityEnable()
	//{
	//}

//#########################################
//###############  END   ##################
//#########################################
}