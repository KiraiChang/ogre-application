#ifndef _GAME_SYSTEM_H_
#define _GAME_SYSTEM_H_

#include "../physic/physic_shape_base.h"
#include "../physic/physic_rigid_body.h"
#include <list>

typedef std::list< PhysicRigidBody * > V_RIGID_BODY;
typedef std::list<  PhysicShapeBase* > V_SHAPE;
class PhysicDebug;
class btDynamicsWorld;

const float DEF_MAX_PLAY_TIME = 120.0;

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
	float										m_fFullTime;
	float										m_fTimePass;
	bool										m_bShoot;

public:
												~GameSystem(void);
	static GameSystem *							getInstance(void);
	void										init(btDynamicsWorld* world, Ogre::SceneManager *sceneMgr);
	void										release(void);
	void										restart(void);
	float										getTimePass(void)const;

	void										createShape(const char *modelName, float *scale, float *pos, float *quat);
	PhysicRigidBody *							createRidigBody(const char *modelName, float mass, float *scale, float *pos, float *quat, PhysicDebug *debug = NULL, void *userPoint = NULL, int flag = 0);
	void										randomShoot(void);
	void										initScene(void);
	void										update(float timePass);
};

#endif //_GAME_SYSTEM_H_
