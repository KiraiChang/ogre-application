#include "ogre_physic_application.h"
#include "ogre_object.h"

//#########################################
//#######  OgrePhysicApplication   ########
//#########################################

OgrePhysicApplication::OgrePhysicApplication():OgreApplication(), 
	mWorld(NULL), 
	mDebugDrawer(NULL), 
	mpTerrainHeightDataConvert(NULL),
	mpBody(NULL),
	mpHeightShape(NULL),
	pVehiclesAttrib(NULL)
{
}

OgrePhysicApplication::~OgrePhysicApplication()
{
	release();
}

//release resource
void OgrePhysicApplication::release(void)
{
	if(mWorld != NULL)
	{
 		std::deque<OgreBulletDynamics::RigidBody *>::iterator itBody = mBodies.begin();
 		while (mBodies.end() != itBody)
 		{   
 			delete *itBody; 
 			++itBody;
 		}	
 		// OgreBullet physic delete - Shapes
 		std::deque<OgreBulletCollisions::CollisionShape *>::iterator itShape = mShapes.begin();
 		while (mShapes.end() != itShape)
 		{   
 			delete *itShape; 
 			++itShape;
 		}
 		mBodies.clear();
 		mShapes.clear();
 		delete mWorld->getDebugDrawer();
 		mWorld->setDebugDrawer(0);
 		delete mWorld;
		mDebugDrawer = NULL;
		mWorld = NULL;
	}

	if(mpTerrainHeightDataConvert != NULL)
	{
		delete []mpTerrainHeightDataConvert;
		mpTerrainHeightDataConvert = NULL;
	}

	if(mpBody != NULL)
	{
		if(mpBody->getMotionState() != NULL)
		{
			delete mpBody->getMotionState();
			mpBody->setMotionState(NULL);
		}
		delete mpBody;
		mpBody = NULL;
	}

	if(mpHeightShape != NULL)
	{
		delete mpHeightShape;
		mpHeightShape = NULL;
	}
	CL::ObjectManger::fnRelease();
	pVehiclesAttrib = NULL;
}


//create resource
void OgrePhysicApplication::createScene(void)
{
	OgreApplication::createScene();

	createPhysic();

	createVehicles("chassis.mesh", "wheel.mesh");
}

void OgrePhysicApplication::createUI(void)
{
	//Load layout and set Event
	CEGUI::WindowManager &windowMgr = CEGUI::WindowManager::getSingleton();

	mpSheet = windowMgr.loadWindowLayout("TrainProject.layout");
 
    CEGUI::System::getSingleton().setGUISheet(mpSheet);

	CEGUI::Window *root = mpSheet->getChild("MainWindow");

	CEGUI::PushButton *pButton = (CEGUI::PushButton *)root->getChild("MainWindow/QuitButton");
	pButton->subscribeEvent(CEGUI::PushButton::EventClicked,
    CEGUI::Event::Subscriber(&OgrePhysicApplication::onQuit, this));
}

void OgrePhysicApplication::createPhysic(void)
{
 		mMoveSpeed = 50;	// defined in ExampleFrameListener
 		mNumEntitiesInstanced = 0; // how many shapes are created

 		// Start Bullet
		Ogre::AxisAlignedBox bounds(Ogre::Vector3 (-10000, -10000, -10000), Ogre::Vector3(10000,  10000,  10000)); //aligned box for Bullet
		Ogre::Vector3 gravityVector(0,-9.81,0);
 		mWorld = new OgreBulletDynamics::DynamicsWorld(mSceneMgr, bounds, gravityVector);
 
 	    // add Debug info display tool
 		mDebugDrawer = new OgreBulletCollisions::DebugDrawer();
 		mDebugDrawer->setDrawWireframe(true);	// we want to see the Bullet containers
 
 		mWorld->setDebugDrawer(mDebugDrawer);
 		mWorld->setShowDebugShapes(true);		// enable it if you want to see the Bullet containers
 		Ogre::SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode("DebugDrawer", Ogre::Vector3::ZERO);
 		node->attachObject(static_cast <Ogre::SimpleRenderable *> (mDebugDrawer));
 
 		// add collision detection to it
		if(mTerrainInfo != NULL)
		{
			int terrainPageSize = mTerrainInfo->getSize(); // Number of vertices along x/z axe

			// >>> We need to mirror the ogre-height-data along the z axis first!
			// This is related to how Ogre and Bullet differ in heighmap storing
			float *pTerrainHeightData = mTerrainInfo->getHeightData();
			mpTerrainHeightDataConvert = new float[terrainPageSize * terrainPageSize];
			for(int i = 0; i < terrainPageSize; ++i)
			{
				memcpy(mpTerrainHeightDataConvert + terrainPageSize * i,
					pTerrainHeightData + terrainPageSize * (terrainPageSize - i - 1),
					sizeof(float)*(terrainPageSize));
			}
			// <<< End of conversion

			
				mpHeightShape = new btHeightfieldTerrainShape(terrainPageSize,
				terrainPageSize,
				mpTerrainHeightDataConvert,
				1, /* Terrains getHeightData() is already scaled perfectly */
				mTerrainInfo->getMinHeight(),
				mTerrainInfo->getMaxHeight(),
				1, /* upVector is Y positive in ogre-, bullet- and our world */
				PHY_FLOAT,
				true);

			// Scale the mesh along x/z
			float unitsBetweenVertices = mTerrainInfo->getWorldSize() / (terrainPageSize - 1);
			btVector3 scaling(unitsBetweenVertices, 1, unitsBetweenVertices);
			mpHeightShape->setLocalScaling(scaling);

			// Ogre uses DiamonSubdivision for Terrain-mesh, so bullet should use it too
			mpHeightShape->setUseDiamondSubdivision(true);

			// Now we create a btRigidBody
			mpBody = new btRigidBody(0.0 /* mass 0.0 means static */,
				new btDefaultMotionState(),
				mpHeightShape);
			
			// 
			Ogre::Vector3 terrainPosition = mTerrainInfo->getPosition();
			mpBody->getWorldTransform().setOrigin(btVector3(terrainPosition.x,
				terrainPosition.y 
				+ (mTerrainInfo->getMaxHeight() - mTerrainInfo->getMinHeight()) / 2,  // Bullet's position differs from Ogre's. Ogre's y is at the bottom, bullet needs the middle if the height to be positioned right
				terrainPosition.z));

			mpBody->getWorldTransform().setRotation(btQuaternion(Ogre::Quaternion::IDENTITY.x,
				Ogre::Quaternion::IDENTITY.y,
				Ogre::Quaternion::IDENTITY.z,
				Ogre::Quaternion::IDENTITY.w));

			mWorld->getBulletDynamicsWorld()->addRigidBody(mpBody);
		}
}

//operator fun
void OgrePhysicApplication::createBody(void)
{
	Ogre::Vector3 size = Ogre::Vector3::ZERO;	// size of the box
	// starting position of the box
	Ogre::Vector3 position = (mCamera->getDerivedPosition() + mCamera->getDerivedDirection().normalisedCopy() * 10);

	// create an ordinary, Ogre mesh with texture
	Ogre::Entity *pEntity = mSceneMgr->createEntity(
		"Box" + Ogre::StringConverter::toString(mNumEntitiesInstanced),
		"cube.mesh");			    
	pEntity->setCastShadows(true);
	// we need the bounding box of the box to be able to set the size of the Bullet-box
	Ogre::AxisAlignedBox boundingB = pEntity->getBoundingBox();
	size = boundingB.getSize(); size /= 2.0f; // only the half needed
	size *= 0.95f;	// Bullet margin is a bit bigger so we need a smaller size
	// (Bullet 2.76 Physics SDK Manual page 18)
	pEntity->setMaterialName("Examples/BumpyMetal");
	Ogre::SceneNode *pNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	pNode->attachObject(pEntity);
	pNode->scale(0.05f, 0.05f, 0.05f);	// the cube is too big for us
	size *= 0.05f;						// don't forget to scale down the Bullet-box too

	// after that create the Bullet shape with the calculated size
	OgreBulletCollisions::BoxCollisionShape *pSceneBoxShape = new OgreBulletCollisions::BoxCollisionShape(size);
	// and the Bullet rigid body
	OgreBulletDynamics::RigidBody *pDefaultBody = new OgreBulletDynamics::RigidBody(
		"defaultBoxRigid" + Ogre::StringConverter::toString(mNumEntitiesInstanced), 
		mWorld);
	pDefaultBody->setShape(	pNode,
		pSceneBoxShape,
		0.6f,			// dynamic body restitution
		0.6f,			// dynamic body friction
		1.0f, 			// dynamic bodymass
		position,		// starting position of the box
		Ogre::Quaternion(0,0,0,1));// orientation of the box
	mNumEntitiesInstanced++;				

	pDefaultBody->setLinearVelocity(
		mCamera->getDerivedDirection().normalisedCopy() * 7.0f ); // shooting speed
	// push the created objects to the dequese
	mShapes.push_back(pSceneBoxShape);
	mBodies.push_back(pDefaultBody);				
}

void OgrePhysicApplication::createBody(const std::string &modelName)
{
	CL::BaseObject &base = CL::ObjectManger::fnAdd(new CL::OgrePhysicAttrib());

	Ogre::Vector3 pos = (mCamera->getDerivedPosition() + mCamera->getDerivedDirection().normalisedCopy() * 10);
	base.setPos((float *)&pos);
	base.setScale((float *)&Ogre::Vector3(0.05f, 0.05f, 0.05f));
	CL::OgrePhysicAttrib *attrib = (CL::OgrePhysicAttrib *)base.getAttrib();
	attrib->create(mSceneMgr, modelName);
	attrib->createPhysic(mWorld);
	attrib->getBody()->setLinearVelocity(mCamera->getDerivedDirection().normalisedCopy() * 7.0f );
}

void OgrePhysicApplication::createVehicles(const std::string &vehicleName, const std::string &wheelsName)
{
	CL::BaseObject &base = CL::ObjectManger::fnAdd(new CL::VehiclesAttrib());
	Ogre::Vector3 pos = (mCamera->getDerivedPosition() + mCamera->getDerivedDirection().normalisedCopy() * 10);
	base.setPos((float *)&pos);

	pVehiclesAttrib = (CL::VehiclesAttrib *)base.getAttrib();
	pVehiclesAttrib->create(mSceneMgr, vehicleName, wheelsName);
	pVehiclesAttrib->createPhysic(mWorld);
}

// Ogre::FrameListener
bool OgrePhysicApplication::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	return OgreApplication::frameRenderingQueued(evt);
}

bool OgrePhysicApplication::frameStarted(const Ogre::FrameEvent& evt)
{
	bool bRet = OgreApplication::frameStarted(evt);
	onVehiclesframeStarted(evt);
	mWorld->stepSimulation(evt.timeSinceLastFrame);	// update Bullet Physics animation
	return bRet;
}

bool OgrePhysicApplication::onVehiclesframeStarted(const Ogre::FrameEvent& evt)
{
	if(pVehiclesAttrib != NULL)
	{
		if (pVehiclesAttrib->mSteeringLeft)
		{
			pVehiclesAttrib->mSteering += CL::gSteeringIncrement;
			if (pVehiclesAttrib->mSteering > CL::gSteeringClamp)
				pVehiclesAttrib->mSteering = CL::gSteeringClamp;
		}
		else if (pVehiclesAttrib->mSteeringRight)
		{
			pVehiclesAttrib->mSteering -= CL::gSteeringIncrement;
			if (pVehiclesAttrib->mSteering < -CL::gSteeringClamp)
				pVehiclesAttrib->mSteering = -CL::gSteeringClamp;
		}
		pVehiclesAttrib->update();
		return true;
	}
	return false;
}

bool OgrePhysicApplication::frameEnded(const Ogre::FrameEvent& evt)
{
	bool bRet = OgreApplication::frameEnded(evt);
	mWorld->stepSimulation(evt.timeSinceLastFrame);	// update Bullet Physics animation
	return bRet;
}

// OIS::KeyListener
bool OgrePhysicApplication::keyPressed( const OIS::KeyEvent &arg )
{
	OgreApplication::keyPressed(arg);
	onVehiclesKeyPressed( arg );
	return true;
}

bool OgrePhysicApplication::onVehiclesKeyPressed( const OIS::KeyEvent &arg )
{
	if(pVehiclesAttrib != NULL)
	{
		switch(arg.key)
		{
		case OIS::KC_J: 
			pVehiclesAttrib->mSteeringLeft = true;
			break;
		case OIS::KC_L: 
			pVehiclesAttrib->mSteeringRight = true;
			break;
		case OIS::KC_K: 
			pVehiclesAttrib->mEngineForce = -CL::gMaxEngineForce;
			break;
		case OIS::KC_I: 
			pVehiclesAttrib->mEngineForce = CL::gMaxEngineForce;
			break;
		default:
			break;
		}
		return true;
	}
	return false;
}

bool OgrePhysicApplication::keyReleased( const OIS::KeyEvent &arg )
{
	OgreApplication::keyReleased(arg);
	onVehiclesKeyReleased(arg);
	return true;
}

bool OgrePhysicApplication::onVehiclesKeyReleased( const OIS::KeyEvent &arg )
{
	if(pVehiclesAttrib != NULL)
	{
		switch(arg.key)
		{

		case OIS::KC_J: 
			pVehiclesAttrib->mSteeringLeft = false;
			break;
		case OIS::KC_L: 
			pVehiclesAttrib->mSteeringRight = false;
			break;
		case OIS::KC_K: 
			pVehiclesAttrib->mEngineForce = 0;
			break;
		case OIS::KC_I: 
			pVehiclesAttrib->mEngineForce = 0;
			break;
		default:
			break;
		}
		return true;
	}
	return false;
}

// OIS::MouseListener
bool OgrePhysicApplication::mouseMoved( const OIS::MouseEvent &arg )
{
	OgreApplication::mouseMoved(arg);
	return true;
}

bool OgrePhysicApplication::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	OgreApplication::mousePressed(arg, id);
	switch(id)
	{
	case OIS::MB_Right:
		mMousePress = MOUSE_PRESS_RIGHT;
		break;
	case OIS::MB_Left:
		//createBody();
		//createBody("knot.mesh");
		
		mMousePress = MOUSE_PRESS_LEFT;
		break;
	default:
		break;
	}
	return true;
}

bool OgrePhysicApplication::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	OgreApplication::mouseReleased(arg, id);
	mMousePress = MOUSE_PRESS_NONE;
	return true;
}

//callback event
bool OgrePhysicApplication::onQuit(const CEGUI::EventArgs &e)
{
	mShutDown = true;
    return true;
}

//#########################################
//################  END   #################
//#########################################