#ifndef _KINECT_APPLICATION_H_
#define _KINECT_APPLICATION_H_

#include "../kinect/kinect_device.h"
#include "ogre_application.h"

class KinectApplication : public OgreApplication
{
private:
	KinectDevice *		m_pKinectDevice;

	Ogre::SceneNode *	m_pCharNode[NUI_SKELETON_POSITION_COUNT];
	Ogre::Entity *		m_pCharEnt[NUI_SKELETON_POSITION_COUNT];

public:
	KinectApplication(void);
	virtual ~KinectApplication(void);
	virtual const std::string getApplicationName(void)const;
	virtual void createScene(void);

	virtual bool frameEnded(const Ogre::FrameEvent& evt);

	//show Skeleton Part Position
	void initSkeletonMesh(void);
	void releaseSkeletonMesh(void);

	//kinect operator
	KinectDevice *getKinectDevice(void);
	void releaseKinect(void);
	void drawSkeleton(NUI_SKELETON_FRAME &frame);
};

#endif _KINECT_APPLICATION_H_