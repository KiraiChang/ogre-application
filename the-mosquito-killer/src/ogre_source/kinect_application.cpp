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
	for(int i = 0; i < NUI_SKELETON_COUNT; i++)
	{
		m_vpPlayer[i] = NULL;
	}
	if(NULL == m_pPhysicSimulation)
	{
		m_pPhysicSimulation = new PhysicSimulation();
		m_pPhysicSimulation->init();
	}
}

KinectApplication::~KinectApplication(void)
{
	releaseKinect();
	releaseCharacter();
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
	GameSystem::getInstance()->initScene();

	for(int i = 0; i < NUI_SKELETON_COUNT; i++)
	{
		m_vpPlayer[i] = new PhysicKinect(mSceneMgr, m_pPhysicSimulation->getDynamicsWorld());
	}

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

	if(NULL != m_pKinectDevice)
	{
		NUI_SKELETON_FRAME frame = {0};
		if(m_pKinectDevice->getSkeletonFrame(frame))
			updatePlayer(frame);
	}
	
	if(NULL != m_pPhysicSimulation)
	{
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

void KinectApplication::releaseCharacter()
{
	for(int i = 0; i < NUI_SKELETON_COUNT; i++)
	{
		if(m_vpPlayer[i] != NULL)
		{
			delete m_vpPlayer[i];
			m_vpPlayer[i] = NULL;
		}
	}
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

void KinectApplication::updatePlayer(const NUI_SKELETON_FRAME &frame)
{
	for(int i = 0; i < NUI_SKELETON_COUNT; i++ )
	{
		if(frame.SkeletonData[i].eTrackingState != NUI_SKELETON_NOT_TRACKED)
		{
			if(m_vpPlayer[i]->getID() != frame.SkeletonData[i].dwTrackingID)
			{
				m_vpPlayer[i]->release();
				m_vpPlayer[i]->init(frame.SkeletonData[i].dwTrackingID);
			}
			m_vpPlayer[i]->update(frame.SkeletonData[i]);
		}
		else
		{
			m_vpPlayer[i]->release();
		}
	}
}