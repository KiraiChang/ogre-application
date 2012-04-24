#ifndef _KINECT_APPLICATION_H_
#define _KINECT_APPLICATION_H_

#include "../kinect/character_controller.h"
#include "../kinect/physic_kinect.h"
#include "ogre_application.h"

class PhysicSimulation;
class PhysicRigidBody;
class RagDoll;
class OgreShapeBox;

class KinectApplication : public OgreApplication
{
private:
	//Physic
	PhysicSimulation*						m_pPhysicSimulation;
	RagDoll*								m_pRagDoll;

	//Kinect
	KinectDevice *							m_pKinectDevice;
	bool									m_bHasDevice;

public:
	KinectApplication(void);
	virtual ~KinectApplication(void);
	virtual const std::string getApplicationName(void)const;
	virtual void createCamera(void);
	virtual void initCamera(void);
	virtual void createScene(void);
	virtual void createUI(void);
	virtual void createLight(void);

	virtual bool frameEnded(const Ogre::FrameEvent& evt);
	// OIS::KeyListener
    virtual bool keyPressed( const OIS::KeyEvent &arg );
    virtual bool keyReleased( const OIS::KeyEvent &arg );
	//kinect operator
	KinectDevice *getKinectDevice(void);
	void releaseKinect(void);
	void updatePlayer(const NUI_SKELETON_FRAME &frame);
};

#endif _KINECT_APPLICATION_H_