#ifndef _MOSQUITO_H_
#define _MOSQUITO_H_
#include "../kinect/kinect_device.h"
namespace Ogre
{
	class SceneManager;
}
class PhysicRigidBody;
class MoveBase;
class btDynamicsWorld;
class ScoreBase;
//*******************************************************
//***************  MOSQUITO_BASE  ***********************
//*******************************************************
enum MOSQUITO_ANI_TYPE
{
	eMosquitoAniMove = 0,
	eMosquitoAniHit,
	eMosquitoAniCut,
};

enum MOSQUITO_STATE
{
	eMosquitoMove = 0,
	eMosuqitoHit,
	eMosuqitoCut,
	eMosquitoBlood,
	eMosuqitoDead,
	eMosquitoAlert,
};

enum MOSQUITO_TYPE
{
	eMosquitoBase = 0,
	eMosquitoSplit,
	eMosquitoFat,
};
class MosquitoBase
{
private:
	Ogre::SceneManager *		m_pSceneMgr;
	btDynamicsWorld *			m_pWorld;
	unsigned int				m_uiMeshID;
protected:
	PhysicRigidBody*			m_pBody;
	MoveBase*					m_pMove;
	ScoreBase*					m_pScore;
	float						m_fMoveSpeed;
	float						m_fTimer;
	MOSQUITO_STATE				m_eState;
	bool						m_bDestory;
public:
								MosquitoBase(void);
	virtual						~MosquitoBase(void);
	virtual void				init(Ogre::SceneManager *scene, btDynamicsWorld *world);
	virtual void				create(const char *modelName, unsigned int moveType, float speed, float mass, float *scale, float *pos, float *size, float *quat, int score);
	virtual void				release(void);
	virtual void				update(float timepass);
	virtual MOSQUITO_TYPE		getType(void)				{return eMosquitoBase;}
	virtual bool				isDestory(void)const		{return m_bDestory;}
	virtual void				setDestory(void)			{m_bDestory = 1;}
	virtual void				setMeshID(unsigned int id)	{m_uiMeshID = id;}
	virtual unsigned int		getMeshID(void)				{return m_uiMeshID;}
	virtual void				setAnimation(const char *aniName, bool loop, bool blend);
	virtual void				setParticle(const char *fileName);
	virtual void				setBillboard(const char *fileName);
	virtual void				setVisible(bool visible);
	void						getPos(float *pos);
	void						setState(MOSQUITO_STATE state);
	MOSQUITO_STATE				getState(void)				{return m_eState;}
	ScoreBase*					getScoreBase(void);
};
//*******************************************************
//***************  MOSQUITO_SPLIT  **********************
//*******************************************************
class MosquitoSplit: public MosquitoBase
{
private:
	int							m_iSplitNumber;
public:
								MosquitoSplit(void);
	virtual						~MosquitoSplit(void);
	virtual void				init(Ogre::SceneManager *scene, btDynamicsWorld *world);
	virtual void				create(const char *modelName, unsigned int moveType, float speed, float mass, float *scale, float *pos, float *size, float *quat, int score);
	virtual void				release(void);
	virtual void				update(float timepass);
	void						setSplitNumber(int number)	{m_iSplitNumber = number;}
	int							getSplitNumber()			{return m_iSplitNumber;}
	virtual MOSQUITO_TYPE		getType(void)				{return eMosquitoSplit;}
};
//*******************************************************
//****************  MOSQUITO_FAT  ***********************
//*******************************************************
class MosquitoFat: public MosquitoBase
{
private:
	int							m_iBloodNumber;
public:
								MosquitoFat(void);
	virtual						~MosquitoFat(void);
	virtual void				init(Ogre::SceneManager *scene, btDynamicsWorld *world);
	virtual void				create(const char *modelName, unsigned int moveType, float speed, float mass, float *scale, float *pos, float *size, float *quat, int score);
	virtual void				release(void);
	virtual void				update(float timepass);
	virtual MOSQUITO_TYPE		getType(void)				{return eMosquitoFat;}
	void						setBloodNumber(int number)	{m_iBloodNumber = number;}
	void						decreaseBlood()				{m_iBloodNumber--;}
};
//*******************************************************
//********************  END  ****************************
//*******************************************************
#endif //_MOSQUITO_H_