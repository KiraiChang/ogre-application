#include "kinect_application.h"

KinectApplication::KinectApplication(void):m_pKinectDevice(NULL)
{
	for(int i = 0; i < NUI_SKELETON_COUNT; i++)
	{
		m_vpPlayer[i] = NULL;
	}
}

KinectApplication::~KinectApplication(void)
{
	releaseKinect();
	releaseCharacter();
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
			if(frame.SkeletonData[i].dwTrackingID == 0)
			{
				if(m_vpPlayer[i] != NULL)
				{
					delete m_vpPlayer[i];
					m_vpPlayer[i] = NULL;
				}
			}
		}
	}
}