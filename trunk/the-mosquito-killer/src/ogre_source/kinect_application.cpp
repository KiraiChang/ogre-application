#include "kinect_application.h"
#include "../physic/physic.h"
#include "../physic/rag_doll.h"
#include "../ogre_physic/ogre_physic_debug.h"
#include "../ogre_physic/ogre_physic_shape.h"
#include "../physic/physic_rigid_body.h"
#include "../score_system/score_system.h"
#include "../score_system/score_object.h"
#include "../score_system/game_system.h"

#include <iostream>
#include <fstream>
#include <json_spirit.h>

#include <Ogre.h>


KinectApplication::KinectApplication(void):
		m_pKinectDevice(NULL), 
		m_pPhysicSimulation(NULL),
		m_pRagDoll(NULL),
		m_bHasDevice(TRUE)
		
{
	if(NULL == m_pPhysicSimulation)
	{
		m_pPhysicSimulation = new PhysicSimulation();
		m_pPhysicSimulation->init();
	}
}

KinectApplication::~KinectApplication(void)
{
	releaseKinect();
	GameSystem::getInstance()->release();
	if(NULL != m_pPhysicSimulation)
	{
		if(m_pRagDoll != NULL)
		{
			delete m_pRagDoll;
			m_pRagDoll = NULL;
		}

		delete m_pPhysicSimulation;
		m_pPhysicSimulation = NULL;
	}
}

const std::string KinectApplication::getApplicationName(void)const
{
	return "The Mosquito Killer";
}

void KinectApplication::createCamera(void)
{
	std::ifstream is("../configure/camera.cfg");
	if(is.is_open())
	{
		json_spirit::mValue value;
		json_spirit::read(is, value);
		json_spirit::mObject obj;
		obj = value.get_obj()["camera"].get_obj();

		// Create the camera
		mCamera = mSceneMgr->createCamera(obj["name"].get_str());

		mCameraNode = mSceneMgr->createSceneNode(obj["node_name"].get_str());
		mCameraNode->attachObject((Ogre::MovableObject *)mCamera);
		mCameraNode->setPosition(obj["position"].get_array()[0].get_real(),
								obj["position"].get_array()[1].get_real(),
								obj["position"].get_array()[2].get_real());
		// Look back along -Z
		mCamera->lookAt(Ogre::Vector3(obj["look_at"].get_array()[0].get_real(),
								obj["look_at"].get_array()[1].get_real(),
								obj["look_at"].get_array()[2].get_real()));
		//mCamera->setNearClipDistance(5);
		mCamera->setNearClipDistance(obj["near_clip"].get_real());
		mCamera->setFarClipDistance(obj["far_clip"].get_real()); 
	}
	else
	{
		// Create the camera
		mCamera = mSceneMgr->createCamera("PlayerCam");

		mCameraNode = mSceneMgr->createSceneNode("CameraNode");
		mCameraNode->attachObject((Ogre::MovableObject *)mCamera);
		// Position it at 500 in Z direction
		//mCamera->setPosition(Ogre::Vector3(0,270,0));
		mCameraNode->setPosition(0, 0, 0);
		// Look back along -Z
		mCamera->lookAt(Ogre::Vector3(0,0,0));
		//mCamera->setNearClipDistance(5);
		mCamera->setNearClipDistance(0.1);
		mCamera->setFarClipDistance(500); 
	}

    mCameraMan = new OgreBites::SdkCameraMan(mCamera);   // create a default camera controller
}

void KinectApplication::initCamera(void)
{
}

void KinectApplication::createScene(void)
{
	OgreApplication::createScene();
	createUI();

	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);

	Ogre::MeshManager::getSingleton().createPlane("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane, 1500, 1500, 20, 20, true, 1, 50, 50, Ogre::Vector3::UNIT_Z);

	Ogre::Entity* entGround = mSceneMgr->createEntity("GroundEntity", "ground");
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(entGround);

	entGround->setMaterialName("Examples/Rockwall");
	entGround->setCastShadows(true);

	GameSystem::getInstance()->init(m_pPhysicSimulation->getDynamicsWorld(), mSceneMgr);
	GameSystem::getInstance()->initPlayer();
	GameSystem::getInstance()->initScene();
	if(m_pKinectDevice == NULL)
	{
		GameSystem::getInstance()->initPlayer(1);
	}
	else
		m_pKinectDevice->setCameraElevationAngle(10);
	//m_pRagDoll = new RagDoll(m_pPhysicSimulation->getDynamicsWorld());
	//m_pRagDoll->init(0, 0.0, 0);
	//OgrePhysicDebug *debug = new OgrePhysicDebug();
	//debug->init(mSceneMgr);
	//m_pRagDoll->setDebug(debug);
}

void KinectApplication::createUI(void)
{
	CEGUI::WindowManager &windowMgr = CEGUI::WindowManager::getSingleton();

	mpSheet = windowMgr.loadWindowLayout("kinect_game.layout");
 
    CEGUI::System::getSingleton().setGUISheet(mpSheet);

	//CEGUI::Slider *slider = (CEGUI::Slider *)mpSheet->getChild("Root/Timepass");

	//slider->setCurrentValue(DEF_MAX_PLAY_TIME);
	//slider->setMaxValue(DEF_MAX_PLAY_TIME);
}

void KinectApplication::createLight(void)
{
	Ogre::Light* pLight = NULL;

	std::ifstream is("../configure/scene_light.cfg");
	if(is.is_open())
	{
		json_spirit::mValue value;
		json_spirit::read(is, value);
		json_spirit::mArray arr;
		json_spirit::mObject obj;
		arr = value.get_obj()["scene_light"].get_array();
		for(int i = 0; i <arr.size();i++)
		{
			obj = arr[i].get_obj();
			switch(obj["type"].get_int())
			{
			case Ogre::Light::LT_POINT:
				{
					pLight = mSceneMgr->createLight(obj["lightName"].get_str());
					pLight->setType(Ogre::Light::LT_POINT);
					pLight->setDiffuseColour(obj["diffuse"].get_array()[0].get_real(),
							obj["diffuse"].get_array()[1].get_real(),
							obj["diffuse"].get_array()[2].get_real());
					pLight->setSpecularColour(obj["specular"].get_array()[0].get_real(),
							obj["specular"].get_array()[1].get_real(),
							obj["specular"].get_array()[2].get_real());
					pLight->setPosition(obj["position"].get_array()[0].get_real(),
							obj["position"].get_array()[1].get_real(),
							obj["position"].get_array()[2].get_real());
					pLight->setAttenuation(obj["attenuation"].get_array()[0].get_real(),
							obj["attenuation"].get_array()[1].get_real(),
							obj["attenuation"].get_array()[2].get_real(),
							obj["attenuation"].get_array()[3].get_real());
				}
				break;
			case Ogre::Light::LT_DIRECTIONAL:
				{
					pLight = mSceneMgr->createLight(obj["lightName"].get_str());
					pLight->setType(Ogre::Light::LT_DIRECTIONAL);
					pLight->setDiffuseColour(obj["diffuse"].get_array()[0].get_real(),
							obj["diffuse"].get_array()[1].get_real(),
							obj["diffuse"].get_array()[2].get_real());
					pLight->setSpecularColour(obj["specular"].get_array()[0].get_real(),
							obj["specular"].get_array()[1].get_real(),
							obj["specular"].get_array()[2].get_real());
					pLight->setDirection(obj["direction"].get_array()[0].get_real(),
							obj["direction"].get_array()[1].get_real(),
							obj["direction"].get_array()[2].get_real());
				}
				break;
			case Ogre::Light::LT_SPOTLIGHT:
				{
					pLight = mSceneMgr->createLight(obj["lightName"].get_str());
					pLight->setType(Ogre::Light::LT_SPOTLIGHT);
					pLight->setDiffuseColour(obj["diffuse"].get_array()[0].get_real(),
							obj["diffuse"].get_array()[1].get_real(),
							obj["diffuse"].get_array()[2].get_real());
					pLight->setSpecularColour(obj["specular"].get_array()[0].get_real(),
							obj["specular"].get_array()[1].get_real(),
							obj["specular"].get_array()[2].get_real());
					pLight->setPosition(obj["position"].get_array()[0].get_real(),
							obj["position"].get_array()[1].get_real(),
							obj["position"].get_array()[2].get_real());
					pLight->setDirection(obj["direction"].get_array()[0].get_real(),
						obj["direction"].get_array()[1].get_real(),
						obj["direction"].get_array()[2].get_real());
					pLight->setSpotlightRange(Ogre::Degree(obj["range"].get_array()[0].get_real()),
						Ogre::Degree(obj["range"].get_array()[1].get_real()),
						obj["range"].get_array()[2].get_real());
				}
				break;
			}
		}
	}
	else
	{
		//create Light
		pLight = mSceneMgr->createLight("directLight");
		pLight->setType(Ogre::Light::LT_DIRECTIONAL);
		pLight->setDirection(0.0, -1.0, 0.0);
		pLight->setDiffuseColour(1.0, 1.0, 1.0);
		pLight->setSpecularColour(0.4, 0.4, 0.0);
		mSceneMgr->setAmbientLight(Ogre::ColourValue(0.2, 0.2, 0.0));

		pLight = mSceneMgr->createLight("pointLight");
		pLight->setType(Ogre::Light::LT_POINT);// make this light a point light
		pLight->setDiffuseColour(1.0, .5, 0.0);      //color the light orange 
		pLight->setSpecularColour(1.0, 1.0, 0.0);    //yellow highlights
		pLight->setAttenuation(100, 1.0, 0.045, 0.0075);
		pLight->setPosition(0.0, 10.0, 50.0);

		pLight = mSceneMgr->createLight("spotLight");
		pLight->setType(Ogre::Light::LT_SPOTLIGHT);
		pLight->setDiffuseColour(1.0, 1.0, 1.0);
		pLight->setSpecularColour(1.0, 1.0, 1.0);

		pLight->setDirection(-1, -1, -1);
		pLight->setPosition(0.0, 20.0, 0.0);
		pLight->setSpotlightRange(Ogre::Degree(35), Ogre::Degree(150), 0.1);
	}
}

bool KinectApplication::frameEnded(const Ogre::FrameEvent& evt)
{
	bool bRet = OgreApplication::frameEnded(evt);
	
	if(NULL != m_pPhysicSimulation)
	{
		float timePass = m_pPhysicSimulation->update();
		if(NULL != m_pKinectDevice)
		{
			NUI_SKELETON_FRAME frame = {0};
			if(m_pKinectDevice->getSkeletonFrame(frame))
				GameSystem::getInstance()->updatePlayer(frame);
		}
		else
			GameSystem::getInstance()->updatePlayerDebug(timePass);
		

		if(m_pRagDoll != NULL)
			m_pRagDoll->update();

		GameSystem::getInstance()->update(timePass);
		CEGUI::Slider *slider = (CEGUI::Slider *)mpSheet->getChild("Root/Timepass");
		if(GameSystem::getInstance()->m_bUIInit)
		{
			GameSystem::getInstance()->m_bUIInit = FALSE;

			//slider->setCurrentValue(GameSystem::getInstance()->getFullTime());
			slider->setMaxValue(GameSystem::getInstance()->getFullTime());
		}

		float current = GameSystem::getInstance()->getFullTime() - GameSystem::getInstance()->getTimePass();
		slider->setCurrentValue(current);
	}

	{
		int score = ScoreSystem::getScore();
		char text[128];
		sprintf_s(text, "SCORE:%016d", score);
		CEGUI::Editbox *edit = (CEGUI::Editbox *)mpSheet->getChild("Root/ScoreText");
		edit->setText(CEGUI::String (text));
	}
	return bRet;
}

    // OIS::KeyListener
bool KinectApplication::keyPressed( const OIS::KeyEvent &arg )
{
	if(m_pKinectDevice == NULL)
	{
		if (arg.key == OIS::KC_ESCAPE)
		{
			mShutDown = true;
		}
		else if (arg.key == OIS::KC_A)
		{
			GameSystem::getInstance()->m_eDebugHandHState = GameSystem::eHandMoveLeft;
		}
		else if(arg.key == OIS::KC_D)
		{
			GameSystem::getInstance()->m_eDebugHandHState = GameSystem::eHandMoveRight;
		}
		else if(arg.key == OIS::KC_W)
		{
			GameSystem::getInstance()->m_eDebugHandVState = GameSystem::eHandMoveUp;
		}
		else if(arg.key == OIS::KC_X)
		{
			GameSystem::getInstance()->m_eDebugHandVState = GameSystem::eHandMoveDown;
		}
		else if(arg.key == OIS::KC_SPACE)
		{
			if(GameSystem::getInstance()->m_eDebugHandAttackState == GameSystem::eHandAttackWait)
			{
				GameSystem::getInstance()->m_eDebugHandAttackState = GameSystem::eHandAttacking;
			}
		}
	}
	else
	{
		OgreApplication::keyPressed(arg);
		CEGUI::System &sys = CEGUI::System::getSingleton();
		sys.injectKeyDown(arg.key);
		sys.injectChar(arg.text);
		if (arg.key == OIS::KC_ESCAPE)
		{
			mShutDown = true;
		}
		mCameraMan->injectKeyDown(arg);
	}
	return true;
}

bool KinectApplication::keyReleased( const OIS::KeyEvent &arg )
{
	if(m_pKinectDevice == NULL)
	{
		if (arg.key == OIS::KC_A)
		{
			GameSystem::getInstance()->m_eDebugHandHState = GameSystem::eHandMoveHNothing;
		}
		else if(arg.key == OIS::KC_D)
		{
			GameSystem::getInstance()->m_eDebugHandHState = GameSystem::eHandMoveHNothing;
		}
		else if(arg.key == OIS::KC_W)
		{
			GameSystem::getInstance()->m_eDebugHandVState = GameSystem::eHandMoveVNothing;
		}
		else if(arg.key == OIS::KC_X)
		{
			GameSystem::getInstance()->m_eDebugHandVState = GameSystem::eHandMoveVNothing;
		}
	}
	else
	{
		OgreApplication::keyReleased(arg);
		if(CEGUI::System::getSingleton().injectKeyUp(arg.key)) return true;
		mCameraMan->injectKeyUp(arg);
	}
    return true;
}

KinectDevice *KinectApplication::getKinectDevice()
{
	if(NULL == m_pKinectDevice && m_bHasDevice)
	{
		m_pKinectDevice = new KinectDevice();
		if(FAILED(m_pKinectDevice->Nui_Init()))
		{
			m_bHasDevice = FALSE;
			releaseKinect();
		}
	}

	return m_pKinectDevice;
}

void KinectApplication::releaseKinect()
{
	if( NULL != m_pKinectDevice)
	{
		m_pKinectDevice->Nui_UnInit();
		m_pKinectDevice->Nui_Zero();
		delete m_pKinectDevice;
		m_pKinectDevice = NULL;
	}
}