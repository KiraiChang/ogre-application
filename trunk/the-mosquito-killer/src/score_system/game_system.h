#ifndef _GAME_SYSTEM_H_
#define _GAME_SYSTEM_H_

#include "../physic/physic_shape_base.h"
#include "../physic/physic_rigid_body.h"
#include "../kinect/physic_kinect.h"
#include <list>

typedef std::list< PhysicRigidBody * > V_RIGID_BODY;
typedef std::list<  PhysicShapeBase* > V_SHAPE;
class PhysicDebug;
class btDynamicsWorld;
class btManifoldPoint;
class btCollisionObject;

const float DEF_MAX_PLAY_TIME = 120.0;

namespace Ogre
{
	class SceneManager;
}

class GameSystem
{
public:
	enum HandState
	{
		eOnHandOpen = 0,
		eOnHandClose,
		eOnHandWaitAttack,
		eOnHandAttacked,
		eOnHandWaitShoot,
	};
	float										m_vfHandDebugPos[2][3];
private:
												GameSystem(void);
private:
	enum GameState
	{
		eOnBegin = 0,
		eOnPlaying,
		eOnEnd,
	};

	static GameSystem *							g_instance;
	btDynamicsWorld*							m_pWorld;
	Ogre::SceneManager *						m_pSceneMgr;
	V_RIGID_BODY								m_vRigidBody;
	V_SHAPE										m_vShape;
	float										m_fFullTime;
	float										m_fTimePass;
	bool										m_bShoot;
	GameState									m_eState;
	//Hand control
	HandState									m_eHandState;
	float										m_fHandClose;
	//kinect
	PhysicKinect *								m_vpPlayer[NUI_SKELETON_COUNT];
	DWORD										m_iCurrentID;
	

public:
												~GameSystem(void);
	static GameSystem *							getInstance(void);
	void										init(btDynamicsWorld* world, Ogre::SceneManager *sceneMgr);
	void										release(void);
	void										releaseCharacter(void);
	void										restart(void);
	float										getTimePass(void)const;
	HandState									getHandState(void)const;
	void										setHandState(HandState state);

	void										createShape(const char *modelName, float *scale, float *pos, float *quat);
	PhysicRigidBody *							createRidigBody(const char *modelName, float mass, float *scale, float *pos, float *quat, PhysicDebug *debug = NULL, void *userPoint = NULL, int flag = 0);
	void										randomShoot(void);
	void										initScene(void);//產生場景
	void										initPlayer(void);//產生空的玩家類別
	void										initPlayer(unsigned int playerCount);//初始化人數
	void										update(float timePass);//根據遊戲狀態分配要進入哪個流程
	void										updatePlaying(float timePass);//遊戲中主要的流程
	void										updatePlayer(const NUI_SKELETON_FRAME &frame);
	void										updatePlayerDebug(void);//若是沒有kinect採取的動作
	void										updateHandState(float timePass);//依據雙手位置判斷遊戲動作狀態
	void										testCollision();

	static bool									MaterialProcessedCallback(btManifoldPoint& cp,btCollisionObject* body0,btCollisionObject* body1);
	static bool									MaterialCombinerCallback(btManifoldPoint& cp,	const btCollisionObject* colObj0,int partId0,int index0,const btCollisionObject* colObj1,int partId1,int index1);
};

#endif //_GAME_SYSTEM_H_
