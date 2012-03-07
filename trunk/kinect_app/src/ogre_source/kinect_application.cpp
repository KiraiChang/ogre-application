#include "kinect_application.h"

KinectApplication::KinectApplication(void):m_pKinectDevice(NULL)
{
}

KinectApplication::~KinectApplication(void)
{
	releaseKinect();
	releaseSkeletonMesh();
}

const std::string KinectApplication::getApplicationName(void)const
{
	return "KinectOgreApplication";
}

void KinectApplication::createScene(void)
{
	OgreApplication::createScene();
	initSkeletonMesh();
}

bool KinectApplication::frameEnded(const Ogre::FrameEvent& evt)
{
	bool bRet = OgreApplication::frameEnded(evt);
	NUI_SKELETON_FRAME frame = {0};
	m_pKinectDevice->getSkeletonFrame(frame);
	drawSkeleton(frame);
	return bRet;
}

void KinectApplication::initSkeletonMesh(void)
{
	char entName[64];
	char nodeName[64];
	for(int i = 0 ; i < NUI_SKELETON_POSITION_COUNT; i++)
	{
		sprintf(entName, "CharEnt%d", i);
		m_pCharEnt[i] = mSceneMgr->createEntity(entName, "sphere.mesh");
 
        // Create the scene node
		sprintf(nodeName, "CharNode%d", i);
        m_pCharNode[i] = mSceneMgr->getRootSceneNode()->
            createChildSceneNode(nodeName, Ogre::Vector3(0.0f, 0.0f, 25.0f));
		m_pCharNode[i]->attachObject(m_pCharEnt[i]);
		m_pCharNode[i]->setScale(0.01, 0.01, 0.01);
	}
}

void KinectApplication::releaseSkeletonMesh(void)
{
	if(mSceneMgr)
	{
		for(int i = 0; i < NUI_SKELETON_POSITION_COUNT; i++)
		{
			if(m_pCharEnt[i])
			{
				if(m_pCharNode[i])
					m_pCharNode[i]->detachObject(m_pCharEnt[i]);

				mSceneMgr->destroyEntity(m_pCharEnt[i]);
				m_pCharEnt[i] = NULL;
			}

			if(m_pCharNode[i])
			{
				mSceneMgr->destroySceneNode(m_pCharNode[i]);
				m_pCharNode[i] = NULL;
			}
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

void KinectApplication::drawSkeleton(NUI_SKELETON_FRAME &frame)
{
	for(int i = 0; i < NUI_SKELETON_COUNT; i++)
	{
		if(frame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED)
		{
			for(int j = 0; j < NUI_SKELETON_POSITION_COUNT; j++)
			{
				//frame->SkeletonData[i].SkeletonPositions[j].x;
				//frame->SkeletonData[i].SkeletonPositions[j].y;
				//frame->SkeletonData[i].SkeletonPositions[j].z;
				m_pCharNode[j]->setPosition(frame.SkeletonData[i].SkeletonPositions[j].x*10 + 100,
					frame.SkeletonData[i].SkeletonPositions[j].y*10 + 270,
					frame.SkeletonData[i].SkeletonPositions[j].z*10 + 100);
			}
		}
	}
}