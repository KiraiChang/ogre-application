#include "kinect_application.h"
#include "../physic/physic.h"
#include <Ogre.h>


KinectApplication::KinectApplication(void):
		m_pKinectDevice(NULL), 
		m_pPhysicSimulation(NULL),
		m_pTimer(NULL)
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
	if(NULL == m_pTimer)
	{
		m_pTimer= new Ogre::Timer();
		m_pTimer->reset();
	}
}

KinectApplication::~KinectApplication(void)
{
	releaseKinect();
	releaseCharacter();
	if(NULL != m_pPhysicSimulation)
	{
		delete m_pPhysicSimulation;
		m_pPhysicSimulation = NULL;
	}

	if(NULL != m_pTimer)
	{
		delete m_pTimer;
		m_pTimer = NULL;
	}
}

const std::string KinectApplication::getApplicationName(void)const
{
	return "KinectOgreApplication";
}

void KinectApplication::createScene(void)
{
	OgreApplication::createScene();
}

bool KinectApplication::frameEnded(const Ogre::FrameEvent& evt)
{
	bool bRet = OgreApplication::frameEnded(evt);
	NUI_SKELETON_FRAME frame = {0};
	if(m_pKinectDevice->getSkeletonFrame(frame))
		updatePlayer(frame);

	if(NULL != m_pPhysicSimulation)
	{
		float ms =  m_pTimer->getMicroseconds();

		float minFPS = 1000000.f/60.f;
		if (ms > minFPS)
			ms = minFPS;
		m_pPhysicSimulation->update(ms / 1000000.f);
		m_pTimer->reset();
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
	if(NULL == m_pKinectDevice)
	{
		m_pKinectDevice = new KinectDevice();
		m_pKinectDevice->Nui_Init();
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