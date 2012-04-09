#ifndef _KINECT_APPLICATION_H_
#define _KINECT_APPLICATION_H_

#include "../kinect/character_controller.h"
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
	RagDoll*								m_pRagDoll1;
	//PhysicRigidBody*						m_pRigidBody;
	//PhysicRigidBody*						m_pRigidBody1;
	//OgreShapeBox*							m_pOgreShape;
	//OgreShapeBox*							m_pOgreShape1;
	//OgreShapeBox*							m_pOgreShape2;
	//OgreShapeBox*							m_pOgreShape3;

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