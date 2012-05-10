#ifndef _GAME_SYSTEM_H_
#define _GAME_SYSTEM_H_

#include "dot_scene_loader.h"
#include "../game_object/mosquito.h"
#include "../game_object/weapon.h"
#include "../physic/physic_shape_base.h"
#include "../physic/physic_rigid_body.h"
#include "../kinect/physic_kinect.h"
#include "wave_system.h"
#include <list>

typedef std::list< PhysicRigidBody * > V_RIGID_BODY;
typedef std::list<  PhysicShapeBase* > V_SHAPE;
typedef std::list< MosquitoBase * > V_MOSQUITO;
typedef std::list< WeaponKnife * > V_WEAPON;
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
	enum DebugHandVState
	{
		eHandMoveVNothing,
		eHandMoveUp,
		eHandMoveDown,
	};

	enum DebugHandHState
	{
		eHandMoveHNothing,
		eHandMoveRight,
		eHandMoveLeft,
	};

	enum DebugHandAttackState
	{
		eHandAttackWait,
		eHandAttacking,
		eHandAttacked,
	};

	enum HandState
	{
		eOnHandOpen = 0,
		eOnHandClose,
		eOnHandWaitAttack,
		eOnHandAttacked,
		eOnHandWaitShoot,
	};
	DebugHandVState								m_eDebugHandVState;
	DebugHandHState								m_eDebugHandHState;
	DebugHandAttackState						m_eDebugHandAttackState;
	float										m_vfHandDebugPos[3];
	float										m_fTwoHandDistance;
	bool										m_bUIInit;
private:
												GameSystem(void);
private:
	enum GameState
	{
		eOnMenu = 0,//主選單畫面
		eOnPlaying,//遊戲中畫面
		eOnEnd,//結算畫面
	};
	Ogre::DotSceneLoader						m_dotSceneLoader;
	static GameSystem *							g_instance;
	WaveSystem									m_waveSystem;
	btDynamicsWorld*							m_pWorld;
	Ogre::SceneManager *						m_pSceneMgr;
	//V_RIGID_BODY								m_vRigidBody;
	V_MOSQUITO									m_vMosquito;
	//V_SHAPE										m_vShape;
	V_WEAPON									m_vWeapon;
	float										m_fTimePass;
	GameState									m_eState;
	//Hand control
	HandState									m_eHandState;
	float										m_fHandCloseTime;
	float										m_fShootTimePass;
	float										m_fRightHandZPos;
	//kinect
	PhysicKinect *								m_vpPlayer[NUI_SKELETON_COUNT];
	DWORD										m_iCurrentID;
	int											NumBomb;
	int											NumBook;
	

public:
												~GameSystem(void);
	static GameSystem *							getInstance(void);
	void										init(btDynamicsWorld* world, Ogre::SceneManager *sceneMgr);
	void										release(void);
	void										releaseMosquito(void);
	void										releaseWeapon(void);
	void										releaseCharacter(void);
	void										restart(void);
	float										getTimePass(void)const;
	HandState									getHandState(void)const;
	void										setHandState(HandState state);

	void										createShape(const char *modelName, float *scale, float *pos, float *quat);
	void										createWeapon(WEAPON_TYPE type, const char *modelName, float mass, float *scale, float *pos, float *quat, int score, int otherData);
	PhysicRigidBody *							createRidigBody(const char *modelName, float mass, float *scale, float *pos, float *quat, PhysicDebug *debug = NULL, void *userPoint = NULL, int flag = 0);
	void										createMosquito(MOSQUITO_TYPE type, unsigned int moveType, float speed, const char *modelName, float mass, float *scale, float *pos, float *quat, int score, int otherData = 0);
	void										randomShoot(MOSQUITO_TYPE type = eMosquitoBase);
	void										initScene(void);//產生場景
	void										initPlayer(void);//產生空的玩家類別
	void										initPlayer(unsigned int playerCount);//初始化人數
	void										update(float timePass);//根據遊戲狀態分配要進入哪個流程
	void										updateMosquito(float timePass);//更新蚊子的狀態及位置
	void										updateWeapon(float timePass);//更新蚊子的狀態及位置
	void										updateMenu(float timePass);//主要menu畫面
	void										updatePlaying(float timePass);//遊戲中主要的流程
	void										updateEnd(float timePass);//結算畫面
	void										updatePlayer(const NUI_SKELETON_FRAME &frame);
	void										updatePlayerDebug(float timePass);//若是沒有kinect採取的動作
	void										updateHandState(float timePass);//依據雙手位置判斷遊戲動作狀態
	void										testCollision();
	float										getFullTime(void)const;

	static bool									MaterialProcessedCallback(btManifoldPoint& cp,btCollisionObject* body0,btCollisionObject* body1);
	static bool									MaterialCombinerCallback(btManifoldPoint& cp,	const btCollisionObject* colObj0,int partId0,int index0,const btCollisionObject* colObj1,int partId1,int index1);
};

#endif //_GAME_SYSTEM_H_
