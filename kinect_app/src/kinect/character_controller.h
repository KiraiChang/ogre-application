#ifndef _CHARACTER_CONTROLLER_H_
#define _CHARACTER_CONTROLLER_H_


#include "kinect_device.h"

namespace Ogre
{
class SceneManager;
class SceneNode;
class Entity;
}

class CharacterController
{
private:
	Ogre::SceneManager* mSceneMgr;
	Ogre::SceneNode *	m_pCharNode[NUI_SKELETON_POSITION_COUNT];
	Ogre::Entity *		m_pCharEnt[NUI_SKELETON_POSITION_COUNT];
public:
	CharacterController(Ogre::SceneManager* mgr);
	~CharacterController();

	void init(void);
	void update(NUI_SKELETON_FRAME &frame);
	void release(void);
};

#endif // _CHARACTER_CONTROLLER_H_