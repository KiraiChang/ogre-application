#include "kinect_application.h"

KinectApplication::KinectApplication(void):m_pKinectDevice(NULL), m_pChar(NULL)
{
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
	initCharacter();
}

bool KinectApplication::frameEnded(const Ogre::FrameEvent& evt)
{
	bool bRet = OgreApplication::frameEnded(evt);
	NUI_SKELETON_FRAME frame = {0};
	m_pKinectDevice->getSkeletonFrame(frame);
	if(m_pChar != NULL)
		m_pChar->update(frame);

	return bRet;
}

void KinectApplication::initCharacter()
{
	if(m_pChar == NULL)
	{
		m_pChar = new CharacterController(mSceneMgr);
	}
}

void KinectApplication::releaseCharacter()
{
	if(m_pChar != NULL)
	{
		delete m_pChar;
		m_pChar = NULL;
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