#ifndef _KINECT_APPLICATION_H_
#define _KINECT_APPLICATION_H_

#include "../kinect/character_controller.h"
#include "ogre_application.h"

class KinectApplication : public OgreApplication
{
private:
	KinectDevice *			m_pKinectDevice;
	CharacterController *	m_pChar;

public:
	KinectApplication(void);
	virtual ~KinectApplication(void);
	virtual const std::string getApplicationName(void)const;
	virtual void createScene(void);

	virtual bool frameEnded(const Ogre::FrameEvent& evt);

	void initCharacter();
	void releaseCharacter();

	//kinect operator
	KinectDevice *getKinectDevice(void);
	void releaseKinect(void);
};

#endif _KINECT_APPLICATION_H_