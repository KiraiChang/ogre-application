#include "ogre_physic_application.h"

#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>
#include "btBulletDynamicsCommon.h"

#include "ogre_object.h"

//#########################################
//#######  OgrePhysicApplication   ########
//#########################################
float g_restitution = 0.6f;
float g_friction = 0.6f;
float g_mass = 1.0f;
const float MAX_MASS = 1.0f;
std::string g_meshName = "cube.mesh";

OgrePhysicApplication::OgrePhysicApplication():OgreApplication(), 
	mView(VIEW_PERSPACTIVE),
	mWorld(NULL), 
	mDebugDrawer(NULL), 
	mpTerrainHeightDataConvert(NULL),
	mpBody(NULL),
	mpHeightShape(NULL),
	pVehiclesAttrib(NULL),
	mpSoftWorld(NULL),
	mpDynamicsWorld(NULL),
	mpCollisionConfiguration(NULL),
	mpDispatcher(NULL),
	mpBroadphase(NULL),
	mpSolver(NULL)
{
	mTimer.reset();
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

	if( mpSoftWorld != NULL)
	{
		delete mpSoftWorld;
		mpSoftWorld = NULL;
	}

	if( mpDynamicsWorld != NULL)
	{
		delete mpDynamicsWorld;
		mpDynamicsWorld = NULL;
	}
		
	if( mpSolver != NULL)
	{
		delete mpSolver;
		mpSolver = NULL;
	}

	if( mpBroadphase != NULL)
	{
		delete mpBroadphase;
		mpBroadphase = NULL;
	}

	if( mpDispatcher != NULL)
	{
		delete mpDispatcher;
		mpDispatcher = NULL;
	}

	if( mpCollisionConfiguration != NULL)
	{
		delete mpCollisionConfiguration;
		mpCollisionConfiguration = NULL;
	}
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
	char text[48];
	CEGUI::WindowManager &windowMgr = CEGUI::WindowManager::getSingleton();

	 //const CEGUI::Image* selImg = &CEGUI::ImagesetManager::getSingleton().create("OgreTray.imageset", "Imagesets").getImage("ListboxItem");

	mpSheet = windowMgr.loadWindowLayout("ChainReaction.layout");
 
    CEGUI::System::getSingleton().setGUISheet(mpSheet);

	CEGUI::Window *root = mpSheet->getChild("MainWindow");

	CEGUI::PushButton *pButton = (CEGUI::PushButton *)root->getChild("MainWindow/QuitButton");
	pButton->subscribeEvent(CEGUI::PushButton::EventMouseButtonDown,
    CEGUI::Event::Subscriber(&OgrePhysicApplication::onQuit, this));

	pButton = (CEGUI::PushButton *)root->getChild("MainWindow/WorldButton");
	pButton->subscribeEvent(CEGUI::PushButton::EventMouseButtonDown,
    CEGUI::Event::Subscriber(&OgrePhysicApplication::onViewWorld, this));

	pButton = (CEGUI::PushButton *)root->getChild("MainWindow/CarButton");
	pButton->subscribeEvent(CEGUI::PushButton::EventMouseButtonDown,
    CEGUI::Event::Subscriber(&OgrePhysicApplication::onViewCar, this));

	

	CEGUI::Slider *slider = (CEGUI::Slider *)root->getChild("MainWindow/ForceSlider");

	slider->subscribeEvent(CEGUI::Slider::EventValueChanged,
    CEGUI::Event::Subscriber(&OgrePhysicApplication::onChangeForceSliderValue, this));
	slider->setCurrentValue(CL::gIncrementEngineForce);
	slider->setMaxValue(CL::gMaxEngineForce);

	
	slider->subscribeEvent(CEGUI::PushButton::EventMouseEnters,
		CEGUI::Event::Subscriber(&OgrePhysicApplication::onMouseEnterUIEvent, this));

	slider->subscribeEvent(CEGUI::PushButton::EventMouseLeaves,
		CEGUI::Event::Subscriber(&OgrePhysicApplication::onMouseLeaveUIEvent, this));

	CEGUI::Editbox *edit = (CEGUI::Editbox *)root->getChild("MainWindow/ForceEdit");

	sprintf(text, "%.2f", CL::gIncrementEngineForce);
	edit->setText(CEGUI::String (text));

	edit->subscribeEvent(CEGUI::Editbox::EventTextChanged,
    CEGUI::Event::Subscriber(&OgrePhysicApplication::onChangeForceEditValue, this));

	edit->subscribeEvent(CEGUI::PushButton::EventMouseEnters,
		CEGUI::Event::Subscriber(&OgrePhysicApplication::onMouseEnterUIEvent, this));
	edit->subscribeEvent(CEGUI::PushButton::EventMouseLeaves,
		CEGUI::Event::Subscriber(&OgrePhysicApplication::onMouseLeaveUIEvent, this));

	//slider = (CEGUI::Slider *)root->getChild("MainWindow/TurnSlider");

	//slider->subscribeEvent(CEGUI::Slider::EventValueChanged,
 //   CEGUI::Event::Subscriber(&OgrePhysicApplication::onChangeTurnSliderValue, this));
	//slider->setCurrentValue(CL::gSteeringIncrement);
	//slider->setMaxValue(CL::gSteeringClamp);

	//slider->subscribeEvent(CEGUI::PushButton::EventMouseEnters,
	//	CEGUI::Event::Subscriber(&OgrePhysicApplication::onMouseEnterUIEvent, this));
	//slider->subscribeEvent(CEGUI::PushButton::EventMouseLeaves,
	//	CEGUI::Event::Subscriber(&OgrePhysicApplication::onMouseLeaveUIEvent, this));

	//edit = (CEGUI::Editbox *)root->getChild("MainWindow/TurnEdit");

	//sprintf(text, "%.2f", CL::gSteeringIncrement);
	//edit->setText(CEGUI::String (text));

	//edit->subscribeEvent(CEGUI::Editbox::EventTextChanged,
 //   CEGUI::Event::Subscriber(&OgrePhysicApplication::onChangeTurnEditValue, this));

	//edit->subscribeEvent(CEGUI::PushButton::EventMouseEnters,
	//	CEGUI::Event::Subscriber(&OgrePhysicApplication::onMouseEnterUIEvent, this));
	//edit->subscribeEvent(CEGUI::PushButton::EventMouseLeaves,
	//	CEGUI::Event::Subscriber(&OgrePhysicApplication::onMouseLeaveUIEvent, this));

	slider = (CEGUI::Slider *)root->getChild("MainWindow/MassSlider");

	slider->subscribeEvent(CEGUI::Slider::EventValueChanged,
    CEGUI::Event::Subscriber(&OgrePhysicApplication::onChangeMassSliderValue, this));
	slider->setCurrentValue(g_mass);
	slider->setMaxValue(MAX_MASS);

	slider->subscribeEvent(CEGUI::PushButton::EventMouseEnters,
		CEGUI::Event::Subscriber(&OgrePhysicApplication::onMouseEnterUIEvent, this));
	slider->subscribeEvent(CEGUI::PushButton::EventMouseLeaves,
		CEGUI::Event::Subscriber(&OgrePhysicApplication::onMouseLeaveUIEvent, this));

	edit = (CEGUI::Editbox *)root->getChild("MainWindow/MassEdit");

	sprintf(text, "%.2f", g_mass);
	edit->setText(CEGUI::String (text));

	edit->subscribeEvent(CEGUI::Editbox::EventTextChanged,
    CEGUI::Event::Subscriber(&OgrePhysicApplication::onChangeMassEditValue, this));

	edit->subscribeEvent(CEGUI::PushButton::EventMouseEnters,
		CEGUI::Event::Subscriber(&OgrePhysicApplication::onMouseEnterUIEvent, this));
	edit->subscribeEvent(CEGUI::PushButton::EventMouseLeaves,
		CEGUI::Event::Subscriber(&OgrePhysicApplication::onMouseLeaveUIEvent, this));

	CEGUI::Listbox *pList = (CEGUI::Listbox *)root->getChild("MainWindow/MeshList");

	pList->subscribeEvent(CEGUI::PushButton::EventMouseEnters,
		CEGUI::Event::Subscriber(&OgrePhysicApplication::onMouseEnterUIEvent, this));
	pList->subscribeEvent(CEGUI::PushButton::EventMouseLeaves,
		CEGUI::Event::Subscriber(&OgrePhysicApplication::onMouseLeaveUIEvent, this));

	CEGUI::ListboxItem *pItem = new CEGUI::ListboxTextItem("cube.mesh");
	pItem->setSelectionBrushImage("TaharezLook", "ListboxSelectionBrush");
	pList->addItem(pItem);
	pList->setItemSelectState(pItem, true);
	pItem = new CEGUI::ListboxTextItem("knot.mesh");
	pItem->setSelectionBrushImage("TaharezLook", "ListboxSelectionBrush");
	pList->addItem(pItem);
	pItem = new CEGUI::ListboxTextItem(	"Crate.mesh");
	pItem->setSelectionBrushImage("TaharezLook", "ListboxSelectionBrush");
	pList->addItem(pItem);
	pItem = new CEGUI::ListboxTextItem("ball.mesh");
	pItem->setSelectionBrushImage("TaharezLook", "ListboxSelectionBrush");
	pList->addItem(pItem);
	pItem = new CEGUI::ListboxTextItem("cylinder.mesh");
	pItem->setSelectionBrushImage("TaharezLook", "ListboxSelectionBrush");
	pList->addItem(pItem);
	//pItem = new CEGUI::ListboxTextItem("Cone.mesh");
	//pItem->setSelectionBrushImage("TaharezLook", "ListboxSelectionBrush");
	//pList->addItem(pItem);
	pItem = new CEGUI::ListboxTextItem("Cone1.mesh");
	pItem->setSelectionBrushImage("TaharezLook", "ListboxSelectionBrush");
	pList->addItem(pItem);
	pItem = new CEGUI::ListboxTextItem("Cone2.mesh");
	pItem->setSelectionBrushImage("TaharezLook", "ListboxSelectionBrush");
	pList->addItem(pItem);
	
	pList->subscribeEvent(CEGUI::Listbox::EventMouseButtonDown,
    CEGUI::Event::Subscriber(&OgrePhysicApplication::onChangeMeshListSelect, this));
}

void OgrePhysicApplication::createPhysic(void)
{
	//mpCollisionConfiguration = new btDefaultCollisionConfiguration();

	//mpDispatcher = new btCollisionDispatcher(mpCollisionConfiguration);

	//btVector3 worldAabbMin(-10000,-10000,-10000);
	//btVector3 worldAabbMax(10000,10000,10000);
	//mpBroadphase = new btAxisSweep3 (worldAabbMin, worldAabbMax);

	//mpSolver = new btSequentialImpulseConstraintSolver;

	//mpDynamicsWorld = new btDiscreteDynamicsWorld(mpDispatcher,mpBroadphase,mpSolver,mpCollisionConfiguration);

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

void OgrePhysicApplication::createBody(const std::string &modelName, 
	const float &restitution, const float &friction, const float &mass)
{
	CL::BaseObject &base = CL::ObjectManger::fnAdd(new CL::OgrePhysicAttrib());

	Ogre::Vector3 pos = (mCamera->getDerivedPosition() + mCamera->getDerivedDirection().normalisedCopy() * 10);
	base.setPos((float *)&pos);
	CL::OgrePhysicAttrib *attrib = (CL::OgrePhysicAttrib *)base.getAttrib();
	if(modelName == "cube.mesh")
	{
		base.setScale((float *)&Ogre::Vector3(0.05f, 0.05f, 0.05f));
		attrib->create(mSceneMgr, modelName);
		attrib->createPhysic(mWorld, restitution, friction, mass, CL::OgrePhysicAttrib::SHAPE_TYPE_CUBE);
	}
	else if(modelName == "Crate.mesh")
	{
		base.setScale((float *)&Ogre::Vector3(1.5f, 1.5f, 1.5f));
		attrib->create(mSceneMgr, modelName);
		attrib->createPhysic(mWorld, restitution, friction, mass, CL::OgrePhysicAttrib::SHAPE_TYPE_CUBE);
	}
	else if(modelName == "knot.mesh")
	{
		base.setScale((float *)&Ogre::Vector3(0.05f, 0.05f, 0.05f));
		attrib->create(mSceneMgr, modelName);
		attrib->createPhysic(mWorld, restitution, friction, mass, CL::OgrePhysicAttrib::SHAPE_TYPE_SPHERE);
	}
	else if(modelName == "ball.mesh")
	{
		base.setScale((float *)&Ogre::Vector3(0.5f, 0.5f, 0.5f));
		attrib->create(mSceneMgr, modelName);
		attrib->createPhysic(mWorld, restitution, friction, mass, CL::OgrePhysicAttrib::SHAPE_TYPE_SPHERE);
	}
	else if(modelName == "cylinder.mesh")
	{
		base.setScale((float *)&Ogre::Vector3(1.0f, 1.0f, 1.05f));
		attrib->create(mSceneMgr, modelName);
		attrib->createPhysic(mWorld, restitution, friction, mass, CL::OgrePhysicAttrib::SHAPE_TYPE_CYLINDER);
	}
	else if(modelName == "Crate.mesh")
	{
		base.setScale((float *)&Ogre::Vector3(1.0f, 1.0f, 1.0f));
		attrib->create(mSceneMgr, modelName);
		attrib->createPhysic(mWorld, restitution, friction, mass, CL::OgrePhysicAttrib::SHAPE_TYPE_CUBE);
	}
	//else if(modelName == "Cone.mesh")
	//{
	//	base.setScale((float *)&Ogre::Vector3(10.0f, 10.0f, 10.0f));
	//	attrib->create(mSceneMgr, modelName);
	//	attrib->createPhysic(mWorld, restitution, friction, mass, CL::OgrePhysicAttrib::SHAPE_TYPE_CONE);
	//}
	else if(modelName == "Cone1.mesh" || modelName == "Cone2.mesh")
	{
		base.setScale((float *)&Ogre::Vector3(3.0f, 3.0f, 3.0f));
		attrib->create(mSceneMgr, modelName);
		attrib->createPhysic(mWorld, restitution, friction, mass, CL::OgrePhysicAttrib::SHAPE_TYPE_CONE);
	}
	else
	{
		base.setScale((float *)&Ogre::Vector3(0.05f, 0.05f, 0.05f));
		attrib->create(mSceneMgr, modelName);
		attrib->createPhysic(mWorld, restitution, friction, mass, CL::OgrePhysicAttrib::SHAPE_TYPE_CUBE);
	}
	attrib->getBody()->setLinearVelocity(mCamera->getDerivedDirection().normalisedCopy() * 7.0f );

	//CL::BaseObject &base2 = CL::ObjectManger::fnAdd(new CL::RagDoll());

	////Ogre::Vector3 pos = (mCamera->getDerivedPosition() + mCamera->getDerivedDirection().normalisedCopy() * 10);
	//base2.setPos((float *)&pos);
	//base2.setScale((float *)&Ogre::Vector3(0.05f, 0.05f, 0.05f));
	//CL::RagDoll *attribRag = (CL::RagDoll *)base2.getAttrib();
	//attribRag->create(mSceneMgr);
	//attribRag->createPhysic(mWorld->getBulletDynamicsWorld());
	////attribRag->getBody()->setLinearVelocity(mCamera->getDerivedDirection().normalisedCopy() * 7.0f );
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
	if(mView == VIEW_CAR)
		onViewCar();

	if (mpDynamicsWorld)
	{
		mpDynamicsWorld->stepSimulation(evt.timeSinceLastFrame);
		//optional but useful: debug drawing
		mpDynamicsWorld->debugDrawWorld();
	}

	mWorld->stepSimulation(evt.timeSinceLastFrame);	// update Bullet Physics animation
	
	return bRet;
}

bool OgrePhysicApplication::onVehiclesframeStarted(const Ogre::FrameEvent& evt)
{
	if(pVehiclesAttrib != NULL)
	{
		if(mTimer.getMicroseconds() >= 5000)
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
			mTimer.reset();
			return true;
		}
	}
	return false;
}

void OgrePhysicApplication::onViewCar()
{
	Ogre::Vector3 pos((float *)&pVehiclesAttrib->getObj()->getPos());
	mCameraNode->setPosition(pos.x, pos.y, pos.z);
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
	onVehiclesKeyPressed( arg );
	OgreApplication::keyPressed(arg);
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
			pVehiclesAttrib->mEngineForce = -CL::gIncrementEngineForce;
			break;
		case OIS::KC_I: 
			pVehiclesAttrib->mEngineForce = CL::gIncrementEngineForce;
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
	onVehiclesKeyReleased(arg);
	OgreApplication::keyReleased(arg);
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
	if(mMousePress == MOUSE_INSIDE_UI)
		return true;

	switch(id)
	{
	case OIS::MB_Right:
		mMousePress = MOUSE_PRESS_RIGHT;
		break;
	case OIS::MB_Left:
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
	switch(mMousePress)
	{
	case MOUSE_PRESS_LEFT:
		{
			//createBody();
			CEGUI::Window *root = mpSheet->getChild("MainWindow");
			CEGUI::Listbox *pList = (CEGUI::Listbox *)root->getChild("MainWindow/MeshList");
			g_meshName = pList->getFirstSelectedItem()->getText().c_str();
			//pList->getFirstSelectedItem()->setSelected(true);
			if(g_meshName != "")
			{
				createBody(g_meshName, 
					g_restitution, 
					g_friction,
					g_mass);
			}
		}
		break;
	default:
		break;
	}
	mMousePress = MOUSE_PRESS_NONE;
	return true;
}

//callback event
bool OgrePhysicApplication::onQuit(const CEGUI::EventArgs &e)
{
	mShutDown = true;
	mMousePress = MOUSE_PRESS_LEFT;
    return true;
}

bool OgrePhysicApplication::onViewCar(const CEGUI::EventArgs &e)
{
	mView = VIEW_CAR;
	mMousePress = MOUSE_PRESS_LEFT;
	return true;
}

bool OgrePhysicApplication::onViewWorld(const CEGUI::EventArgs &e)
{
	mView = VIEW_PERSPACTIVE;
	 //       mCamera->setFarClipDistance(0);   // enable infinite far clip distance if we can
		//mCamera->setPosition(Ogre::Vector3(0, 270, 80));
		//mCamera->lookAt(Ogre::Vector3(0, 180, 0));
	float hight = mTerrainInfo->getHeightAtWorldPosition(Ogre::Vector3(0, 0, 0));
	//mCamera->setPosition(Ogre::Vector3(0, hight + 100, 160));
	//mCamera->lookAt(Ogre::Vector3(0, hight, 0));
	mCameraNode->setPosition(0, hight + 50, 160);
	mCamera->setNearClipDistance(0.1);
    mCamera->setFarClipDistance(500);
 
    if (mRoot->getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_INFINITE_FAR_PLANE))
    {
        mCamera->setFarClipDistance(0);   // enable infinite far clip distance if we can
    }
	mMousePress = MOUSE_PRESS_LEFT;
	return true;
}

bool OgrePhysicApplication::onMouseEnterUIEvent(const CEGUI::EventArgs &e)
{
	mMousePress = MOUSE_INSIDE_UI;
	return true;
}

bool OgrePhysicApplication::onMouseLeaveUIEvent(const CEGUI::EventArgs &e)
{
	mMousePress = MOUSE_PRESS_NONE;
	return true;
}

bool OgrePhysicApplication::onChangeForceSliderValue(const CEGUI::EventArgs &e)
{
	CEGUI::Window *root = mpSheet->getChild("MainWindow");
	CEGUI::Slider *slider = (CEGUI::Slider *)root->getChild("MainWindow/ForceSlider");
	CL::gIncrementEngineForce = slider->getCurrentValue();
	if(CL::gIncrementEngineForce < CL::gMinEngineForce)
	{
		CL::gIncrementEngineForce = CL::gMinEngineForce;
		slider->setCurrentValue(CL::gIncrementEngineForce);
	}

	CEGUI::Editbox *edit = (CEGUI::Editbox *)root->getChild("MainWindow/ForceEdit");
	char text[48];
	sprintf(text, "%.0f", CL::gIncrementEngineForce);
	edit->setText(CEGUI::String (text));

	return true;
}

//bool OgrePhysicApplication::onChangeTurnSliderValue(const CEGUI::EventArgs &e)
//{
//	CEGUI::Window *root = mpSheet->getChild("MainWindow");
//	CEGUI::Slider *slider = (CEGUI::Slider *)root->getChild("MainWindow/TurnSlider");
//	CL::gSteeringIncrement = slider->getCurrentValue();
//	if(CL::gSteeringIncrement < CL::gMinSteeringClamp)
//	{
//		CL::gSteeringIncrement = CL::gMinSteeringClamp;
//		slider->setCurrentValue(CL::gSteeringIncrement);
//	}
//
//	CEGUI::Editbox *edit = (CEGUI::Editbox *)root->getChild("MainWindow/TurnEdit");
//	char text[48];
//	sprintf(text, "%.5f", CL::gSteeringIncrement);
//	edit->setText(CEGUI::String (text));
//	return true;
//}

bool OgrePhysicApplication::onChangeMassSliderValue(const CEGUI::EventArgs &e)
{
	CEGUI::Window *root = mpSheet->getChild("MainWindow");
	CEGUI::Slider *slider = (CEGUI::Slider *)root->getChild("MainWindow/MassSlider");
	g_mass = slider->getCurrentValue();

	CEGUI::Editbox *edit = (CEGUI::Editbox *)root->getChild("MainWindow/MassEdit");
	char text[48];
	sprintf(text, "%.2f", g_mass);
	edit->setText(CEGUI::String (text));
	return true;

}

bool OgrePhysicApplication::onChangeForceEditValue(const CEGUI::EventArgs &e)
{
	CEGUI::Window *root = mpSheet->getChild("MainWindow");
	CEGUI::Editbox *edit = (CEGUI::Editbox *)root->getChild("MainWindow/ForceEdit");
	CL::gIncrementEngineForce = atof(edit->getText().c_str());
	if(CL::gIncrementEngineForce > CL::gMaxEngineForce)
	{
		CL::gIncrementEngineForce = CL::gMaxEngineForce;
		char text[48];
		sprintf(text, "%.0f", CL::gIncrementEngineForce);
		edit->setText(CEGUI::String (text));
	}
	else if(CL::gIncrementEngineForce < CL::gMinEngineForce)
	{
		CL::gIncrementEngineForce = CL::gMinEngineForce;
		char text[48];
		sprintf(text, "%.0f", CL::gIncrementEngineForce);
		edit->setText(CEGUI::String (text));
	}
	CEGUI::Slider *slider = (CEGUI::Slider *)root->getChild("MainWindow/ForceSlider");
	slider->setCurrentValue(CL::gIncrementEngineForce);
	return true;
}

//bool OgrePhysicApplication::onChangeTurnEditValue(const CEGUI::EventArgs &e)
//{
//	CEGUI::Window *root = mpSheet->getChild("MainWindow");
//	CEGUI::Editbox *edit = (CEGUI::Editbox *)root->getChild("MainWindow/TurnEdit");
//	CL::gSteeringIncrement = atof(edit->getText().c_str());
//	if(CL::gSteeringIncrement > CL::gSteeringClamp)
//	{
//		CL::gSteeringIncrement = CL::gSteeringClamp;
//		char text[48];
//		sprintf(text, "%.5f", CL::gSteeringIncrement);
//		edit->setText(CEGUI::String (text));
//	}
//	else if(CL::gSteeringIncrement < CL::gMinSteeringClamp)
//	{
//		CL::gSteeringIncrement = CL::gMinSteeringClamp;
//		char text[48];
//		sprintf(text, "%.5f", CL::gIncrementEngineForce);
//		edit->setText(CEGUI::String (text));
//	}
//	CEGUI::Slider *slider = (CEGUI::Slider *)root->getChild("MainWindow/TurnSlider");
//	slider->setCurrentValue(CL::gSteeringIncrement);
//	return true;
//}

bool OgrePhysicApplication::onChangeMassEditValue(const CEGUI::EventArgs &e)
{
	CEGUI::Window *root = mpSheet->getChild("MainWindow");
	CEGUI::Editbox *edit = (CEGUI::Editbox *)root->getChild("MainWindow/MassEdit");
	g_mass = atof(edit->getText().c_str());
	if(g_mass > MAX_MASS)
	{
		g_mass = MAX_MASS;
		char text[48];
		sprintf(text, "%.4f", g_mass);
		edit->setText(CEGUI::String (text));
	}
	CEGUI::Slider *slider = (CEGUI::Slider *)root->getChild("MainWindow/MassSlider");
	slider->setCurrentValue(g_mass);
	return true;
}

bool OgrePhysicApplication::onChangeMeshListSelect(const CEGUI::EventArgs &e)
{
	mMousePress = MOUSE_PRESS_LEFT;
	return true;
}

//#########################################
//################  END   #################
//#########################################