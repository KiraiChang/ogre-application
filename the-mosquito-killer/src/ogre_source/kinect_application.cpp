#include "kinect_application.h"
#include "../physic/physic.h"
#include "../physic/rag_doll.h"
#include "../ogre_physic/ogre_physic_debug.h"
#include "../ogre_physic/ogre_physic_shape.h"
#include "../physic/physic_rigid_body.h"
#include "../score_system/score_system.h"
#include "../score_system/score_object.h"
#include "../score_system/game_system.h"

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
		m_pKinectDevice->setCameraElevationAngle(15);
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

	CEGUI::Slider *slider = (CEGUI::Slider *)mpSheet->getChild("Root/Timepass");

	slider->setCurrentValue(DEF_MAX_PLAY_TIME);
	slider->setMaxValue(DEF_MAX_PLAY_TIME);
}

bool KinectApplication::frameEnded(const Ogre::FrameEvent& evt)
{
	bool bRet = OgreApplication::frameEnded(evt);
	
	if(NULL != m_pPhysicSimulation)
	{
		if(NULL != m_pKinectDevice)
		{
			NUI_SKELETON_FRAME frame = {0};
			if(m_pKinectDevice->getSkeletonFrame(frame))
				GameSystem::getInstance()->updatePlayer(frame);
		}
		else
			GameSystem::getInstance()->updatePlayerDebug();
		float timePass = m_pPhysicSimulation->update();

		if(m_pRagDoll != NULL)
			m_pRagDoll->update();

		GameSystem::getInstance()->update(timePass);

		CEGUI::Slider *slider = (CEGUI::Slider *)mpSheet->getChild("Root/Timepass");

		float current = DEF_MAX_PLAY_TIME - GameSystem::getInstance()->getTimePass();
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
	//	if (arg.key == OIS::KC_ESCAPE)
	//	{
	//		mShutDown = true;
	//	}
	//	else if (arg.key == OIS::KC_A)
	//	{
	//		GameSystem::getInstance()->m_vfHandDebugPos[0][0] -= 0.1;
	//	}
	//	else if(arg.key == OIS::KC_D)
	//	{
	//		GameSystem::getInstance()->m_vfHandDebugPos[0][0] += 0.1;
	//	}
	//	else if(arg.key == OIS::KC_W)
	//	{
	//		GameSystem::getInstance()->m_vfHandDebugPos[0][1] += 0.1;
	//	}
	//	else if(arg.key == OIS::KC_X)
	//	{
	//		GameSystem::getInstance()->m_vfHandDebugPos[0][1] -= 0.1;
	//	}
	//	GameSystem::getInstance()->updatePlayerDebug();
	//}
	//else
	//{
		OgreApplication::keyPressed(arg);
		CEGUI::System &sys = CEGUI::System::getSingleton();
		sys.injectKeyDown(arg.key);
		sys.injectChar(arg.text);
		mCameraMan->injectKeyDown(arg);
	}
	return true;
}

bool KinectApplication::keyReleased( const OIS::KeyEvent &arg )
{
	OgreApplication::keyReleased(arg);
    if(CEGUI::System::getSingleton().injectKeyUp(arg.key)) return true;
    mCameraMan->injectKeyUp(arg);
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