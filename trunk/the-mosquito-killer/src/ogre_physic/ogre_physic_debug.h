#ifndef _OGRE_PHYSIC_DEBUG_H_
#define _OGRE_PHYSIC_DEBUG_H_

#include "../physic/physic_debug.h"
#include <vector>
#include <map>

namespace Ogre
{
	class SceneManager;
	class ManualObject;
	class SceneNode;
	class Vector3;
}

typedef std::vector< Ogre::Vector3 > SET_VECTOR3;
typedef std::map<int, SET_VECTOR3 > MAP_BOUNDING_BOX;

class OgrePhysicDebug : public PhysicDebug
{
private:
	Ogre::SceneManager *		m_pSceneMgr;
	Ogre::ManualObject *		m_pManualObject;
	Ogre::SceneNode *			m_pBodyNode;
	MAP_BOUNDING_BOX			m_mBoundingBox;
public:
	OgrePhysicDebug(void);
	virtual ~OgrePhysicDebug(void);

	virtual void init(Ogre::SceneManager *sceneMgr);
	virtual void beginDraw(void);
	virtual void draw(const int &part, btRigidBody *body);
	virtual void endDraw(void);
	virtual void release(void);
	virtual void setVisible(bool visible);
};

#endif //_OGRE_PHYSIC_DEBUG_H_