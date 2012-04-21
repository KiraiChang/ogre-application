#ifndef _GAME_SYSTEM_H_
#define _GAME_SYSTEM_H_

#include "../game_object/mosquito.h"
#include "../physic/physic_shape_base.h"
#include "../physic/physic_rigid_body.h"
#include "../kinect/physic_kinect.h"
#include <list>

typedef std::list< PhysicRigidBody * > V_RIGID_BODY;
typedef std::list<  PhysicShapeBase* > V_SHAPE;
typedef std::list< MosquitoBase * > V_MOSQUITO;
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
		eOnMenu = 0,//�D���e��
		eOnPlaying,//�C�����e��
		eOnEnd,//����e��
	};

	static GameSystem *							g_instance;
	btDynamicsWorld*							m_pWorld;
	Ogre::SceneManager *						m_pSceneMgr;
	V_RIGID_BODY								m_vRigidBody;
	V_MOSQUITO									m_vMosquito;
	V_SHAPE										m_vShape;
	float										m_fFullTime;
	float										m_fTimePass;
	bool										m_bShoot;
	GameState									m_eState;
	//Hand control
	HandState									m_eHandState;
	float										m_fHandCloseTime;
	float										m_fShootTimePass;
	float										m_fRightHandZPos;
	//kinect
	PhysicKinect *								m_vpPlayer[NUI_SKELETON_COUNT];
	DWORD										m_iCurrentID;
	

public:
												~GameSystem(void);
	static GameSystem *							getInstance(void);
	void										init(btDynamicsWorld* world, Ogre::SceneManager *sceneMgr);
	void										release(void);
	void										releaseMosquito(void);
	void										releaseCharacter(void);
	void										restart(void);
	float										getTimePass(void)const;
	HandState									getHandState(void)const;
	void										setHandState(HandState state);

	void										createShape(const char *modelName, float *scale, float *pos, float *quat);
	PhysicRigidBody *							createRidigBody(const char *modelName, float mass, float *scale, float *pos, float *quat, PhysicDebug *debug = NULL, void *userPoint = NULL, int flag = 0);
	void										createMosquito(MOSQUITO_TYPE type, const char *modelName, float mass, float *scale, float *pos, float *quat, int score, int otherData = 0);
	void										randomShoot(void);
	void										initScene(void);//���ͳ���
	void										initPlayer(void);//���ͪŪ����a���O
	void										initPlayer(unsigned int playerCount);//��l�ƤH��
	void										update(float timePass);//�ھڹC�����A���t�n�i�J���Ӭy�{
	void										updateMosquito(float timePass);//��s�A�l�����A�Φ�m
	void										updateMenu(float timePass);//�D�nmenu�e��
	void										updatePlaying(float timePass);//�C�����D�n���y�{
	void										updateEnd(float timePass);//����e��
	void										updatePlayer(const NUI_SKELETON_FRAME &frame);
	void										updatePlayerDebug(void);//�Y�O�S��kinect�Ĩ����ʧ@
	void										updateHandState(float timePass);//�̾������m�P�_�C���ʧ@���A
	void										testCollision();

	static bool									MaterialProcessedCallback(btManifoldPoint& cp,btCollisionObject* body0,btCollisionObject* body1);
	static bool									MaterialCombinerCallback(btManifoldPoint& cp,	const btCollisionObject* colObj0,int partId0,int index0,const btCollisionObject* colObj1,int partId1,int index1);
};

#endif //_GAME_SYSTEM_H_
