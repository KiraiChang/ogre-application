#ifndef _PHYSIC_KINECT_H_
#define _PHYSIC_KINECT_H_

#include "kinect_device.h"
#include "../physic/physic_rigid_body.h"
#include "../ogre_physic/ogre_physic_shape.h"

//const int BORDY_PART_MAX = 2;//

enum
{
	eKinectLeftHand = 0,
	eKinectRightHand,
	eKinectBodyPart,
};

class PhysicKinect
{
private:
	enum SCALE_POSITION
	{
		eScaleX = 0,
		eScaleY,
		eScaleZ,
		eScaleCount,
	};
private:
	DWORD										m_iKinectID;
	Ogre::SceneManager*							m_pSceneMgr;
	btDynamicsWorld*							m_pWorld;
	PhysicRigidBody*							m_pBody[eKinectBodyPart];//�����A�l�ɥΪ�ridig body
	OgreShapeBox*								m_pShape[eKinectBodyPart];//�g���ɥΪ�mesh
	unsigned int								m_iBodyID[eKinectBodyPart];
	float										m_vfScale[eScaleCount];
	float										m_vfSkeleton[NUI_SKELETON_POSITION_COUNT][eScaleCount];
	float										m_vfOverallPos[eScaleCount];

public:
												PhysicKinect(Ogre::SceneManager *scene, btDynamicsWorld *world);
												~PhysicKinect(void);

	void										init(DWORD id);
	void										release(void);
	PhysicRigidBody *							createRidigBody(const char *modelName, float mass, float *scale, float *pos, float *quat, 
																	PhysicDebug *debug, void *userPoint, int flag);
	void										update(const NUI_SKELETON_DATA &data);
	void										updateDebug(float data[2][3]);
	void										getPartPos(unsigned int offset, float *pos);
	DWORD										getID(void)const;
};

#endif //_PHYSIC_KINECT_H_
