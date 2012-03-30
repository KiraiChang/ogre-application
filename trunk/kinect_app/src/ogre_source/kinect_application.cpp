#include "kinect_application.h"
#include "../physic/physic.h"
#include "../physic/rag_doll.h"
#include "../ogre_physic/ogre_physic_debug.h"

#include <Ogre.h>


KinectApplication::KinectApplication(void):
		m_pKinectDevice(NULL), 
		m_pPhysicSimulation(NULL),
		m_bHasDevice(TRUE),
		m_pRagDoll(NULL)
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
	if(NULL != m_pPhysicSimulation)
	{
		delete m_pRagDoll;
		m_pRagDoll = NULL;

		delete m_pPhysicSimulation;
		m_pPhysicSimulation = NULL;
	}
}

const std::string KinectApplication::getApplicationName(void)const
{
	return "KinectOgreApplication";
}

void KinectApplication::createScene(void)
{
	OgreApplication::createScene();

	m_pRagDoll = new RagDoll(m_pPhysicSimulation->getDynamicsWorld());
	m_pRagDoll->init(0, 270, 0);
	OgrePhysicDebug *debug = new OgrePhysicDebug();
	debug->init(mSceneMgr);
	m_pRagDoll->setDebug(debug);



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
		//static int count = 100;
		//if(count > 0)
		//{
		m_pPhysicSimulation->update();
		//count--;
		//}
		m_pRagDoll->update();
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
			if(m_vpPlayer[i] != NULL)
			{
				if(m_vpPlayer[i]->getID() != frame.SkeletonData[i].dwTrackingID)
				{
					delete m_vpPlayer[i];
					m_vpPlayer[i] = new CharacterController(mSceneMgr, frame.SkeletonData[i].dwTrackingID);
				}
			}
			else
				m_vpPlayer[i] = new CharacterController(mSceneMgr, frame.SkeletonData[i].dwTrackingID);

			m_vpPlayer[i]->update(frame.SkeletonData[i]);
		}
		else
		{
			if(m_vpPlayer[i] != NULL)
			{
				delete m_vpPlayer[i];
				m_vpPlayer[i] = NULL;
			}
		}
	}
}