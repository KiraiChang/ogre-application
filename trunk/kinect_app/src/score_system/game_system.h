#ifndef _GAME_SYSTEM_H_
#define _GAME_SYSTEM_H_

#include "../physic/physic_shape_base.h"
#include "../physic/physic_rigid_body.h"
#include <list>

typedef std::list< PhysicRigidBody * > V_RIGID_BODY;
typedef std::list<  PhysicShapeBase* > V_SHAPE;
class PhysicDebug;
class btDynamicsWorld;
namespace Ogre
{
	class SceneManager;
}

class GameSystem
{
private:
												GameSystem(void);
	static GameSystem *							g_instance;
private:
	btDynamicsWorld*							m_pWorld;
	Ogre::SceneManager *						m_pSceneMgr;
	V_RIGID_BODY								m_vRigidBody;
	V_SHAPE										m_vShape;

public:
												~GameSystem(void);
	static GameSystem *							getInstance(void);
	void										init(btDynamicsWorld* world, Ogre::SceneManager *sceneMgr);
	void										release(void);
	void										restart(void);

	void										createShape(const char *modelName, float *scale, float *pos, float *quat);
	PhysicRigidBody *							createRidigBody(const char *modelName, float mass, float *scale, float *pos, PhysicDebug *debug = NULL, void *userPoint = NULL, int flag = 0);
	void										randomShoot(void);
	void										initScene(void);
	void										update(void);
};

#endif //_GAME_SYSTEM_H_
