#ifndef _PHYSIC_KINECT_H_
#define _PHYSIC_KINECT_H_

#include "kinect_device.h"
#include "../physic/physic_rigid_body.h"
#include "../ogre_physic/ogre_physic_shape.h"

class PhysicKinect
{
private:
	DWORD										m_iKinectID;
	Ogre::SceneManager*							m_pSceneMgr;
	btDynamicsWorld*							m_pWorld;
	PhysicRigidBody*							m_pBody[NUI_SKELETON_POSITION_COUNT];

public:
												PhysicKinect(Ogre::SceneManager *scene, btDynamicsWorld *world);
												~PhysicKinect(void);

	void										init(DWORD id);
	void										release(void);
	PhysicRigidBody *							createRidigBody(const char *modelName, float mass, float *scale, float *pos, float *quat, 
																	PhysicDebug *debug, void *userPoint, int flag);
	void										update(const NUI_SKELETON_DATA &data);
	DWORD										getID(void)const;
};

#endif //_PHYSIC_KINECT_H_
