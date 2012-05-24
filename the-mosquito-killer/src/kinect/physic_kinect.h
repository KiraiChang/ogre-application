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
public:
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
	PhysicRigidBody*							m_pBody[eKinectBodyPart];//拍擊蚊子時用的ridig body
	OgreShapeBox*								m_pShape[eKinectBodyPart];//射擊時用的mesh
	unsigned int								m_iBodyID[eKinectBodyPart];
	float										m_vfScale[eScaleCount];
	float										m_vfSkeleton[NUI_SKELETON_POSITION_COUNT][eScaleCount];
	float										m_vfOverallPos[eScaleCount];

public:
												PhysicKinect(Ogre::SceneManager *scene, btDynamicsWorld *world);
												~PhysicKinect(void);

	void										init(DWORD id);
	void										release(void);
	PhysicRigidBody *							createRidigBody(const char *modelName, float mass, float *scale, float *pos, float *size, float *quat, 
																	PhysicDebug *debug, void *userPoint, int flag);
	void										update(const NUI_SKELETON_DATA &data);
	void										updateDebug(float data[3], float distance);
	void										getPartPos(unsigned int offset, float *pos);
	float										getScale(unsigned int offset);
	DWORD										getID(void)const;
	void										setVisible(bool visible);
};

#endif //_PHYSIC_KINECT_H_
