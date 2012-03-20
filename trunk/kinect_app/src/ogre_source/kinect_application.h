#ifndef _KINECT_APPLICATION_H_
#define _KINECT_APPLICATION_H_

#include "../kinect/character_controller.h"
#include "ogre_application.h"

class PhysicSimulation;
namespace Ogre
{
	class Timer;
}

class KinectApplication : public OgreApplication
{
private:
	//Physic
	PhysicSimulation*						m_pPhysicSimulation;

	//Kinect
	KinectDevice *							m_pKinectDevice;
	bool									m_bHasDevice;

	//Ogre
	CharacterController *					m_vpPlayer[NUI_SKELETON_COUNT];

public:
	KinectApplication(void);
	virtual ~KinectApplication(void);
	virtual const std::string getApplicationName(void)const;
	virtual void createScene(void);

	virtual bool frameEnded(const Ogre::FrameEvent& evt);

	void releaseCharacter();

	//kinect operator
	KinectDevice *getKinectDevice(void);
	void releaseKinect(void);
	void updatePlayer(const NUI_SKELETON_FRAME &frame);
};

#endif _KINECT_APPLICATION_H_