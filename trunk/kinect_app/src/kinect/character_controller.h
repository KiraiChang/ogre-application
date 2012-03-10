#ifndef _CHARACTER_CONTROLLER_H_
#define _CHARACTER_CONTROLLER_H_


#include "kinect_device.h"
#include <string>

namespace Ogre
{
class SceneManager;
class SceneNode;
class Entity;
class Quaternion;
class Vector3;
class AnimationState;
class Degree;
}

class CharacterController
{
private:
	enum AnimID
	{
		ANIM_IDLE_BASE,
		ANIM_IDLE_TOP,
		ANIM_RUN_BASE,
		ANIM_RUN_TOP,
		ANIM_HANDS_CLOSED,
		ANIM_HANDS_RELAXED,
		ANIM_DRAW_SWORDS,
		ANIM_SLICE_VERTICAL,
		ANIM_SLICE_HORIZONTAL,
		ANIM_DANCE,
		ANIM_JUMP_START,
		ANIM_JUMP_LOOP,
		ANIM_JUMP_END,
		ANIM_NONE,
		NUM_ANIMS = ANIM_NONE
	};
	DWORD						m_iKinectID;
	Ogre::SceneManager*			m_pSceneMgr;
	Ogre::SceneNode *			m_pCharNode[NUI_SKELETON_POSITION_COUNT];
	Ogre::Entity *				m_pCharEnt[NUI_SKELETON_POSITION_COUNT];
	Ogre::SceneNode *			m_pBodyNode;
	Ogre::Entity *				m_pBodyEnt;
	Ogre::AnimationState*		m_pAnims[NUM_ANIMS];
	bool						m_bSwordsDrawn;
	AnimID						m_eBaseAnimID;                   // current base (full- or lower-body) animation
	AnimID						m_eTopAnimID;                    // current top (upper-body) animation
	bool						m_bFadingIn[NUM_ANIMS];            // which animations are fading in
	bool						m_bFadingOut[NUM_ANIMS];           // which animations are fading out
	
public:
	CharacterController(Ogre::SceneManager* mgr, const DWORD &kinectID);
	~CharacterController();
	DWORD getID()const;
	void init(void);
	void update(const NUI_SKELETON_DATA &data);
	void release(void);
	Ogre::Quaternion calcQuaternion(const Ogre::Vector3 &child, const Ogre::Vector3 &parent);

	void setupBone(const std::string& name, const Ogre::Quaternion& q);
	void setupBone(const std::string& name,const Ogre::Degree& yaw,const Ogre::Degree& pitch,const Ogre::Degree& roll);

	virtual void setupAnimations();
	virtual void setBaseAnimation(AnimID id, bool reset = false);
	virtual void setTopAnimation(AnimID id, bool reset = false);
};

#endif // _CHARACTER_CONTROLLER_H_