#ifndef _MOSQUITO_H_
#define _MOSQUITO_H_

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
class MosquitoBase
{
private:
	Ogre::SceneManager *		m_pSceneMgr;
	btDynamicsWorld *			m_pWorld;
	PhysicRigidBody*			m_pBody;
	MoveBase*					m_pMove;
	ScoreBase*					m_pScore;
	bool						m_bDestory;
public:
								MosquitoBase(void);
	virtual						~MosquitoBase(void);
	virtual void				init(Ogre::SceneManager *scene, btDynamicsWorld *world);
	virtual void				create(const char *modelName, float mass, float *scale, float *pos, float *quat, int flag, int score);
	virtual void				release(void);
	virtual void				update(void);
	virtual bool				isDestory(void)const		{return m_bDestory;}
	virtual void				setDestory(void)			{m_bDestory = 1;}
};
//*******************************************************
//********************  END  ****************************
//*******************************************************
#endif //_MOSQUITO_H_