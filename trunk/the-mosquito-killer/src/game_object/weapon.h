#ifndef _WEAPON_H_
#define _WEAPON_H_

namespace Ogre
{
	class SceneManager;
}
class PhysicRigidBody;
class ShootBase;
class btDynamicsWorld;
class ScoreBase;
class ShootBase;


enum WEAPON_TYPE
{
	eWeaponKnife = 0,
	eWeaponBook,
	eWeaponBomb,
};

class WeaponKnife
{
private:
	Ogre::SceneManager *		m_pSceneMgr;
	btDynamicsWorld *			m_pWorld;
	PhysicRigidBody*			m_pBody;
	ShootBase*					m_pMove;
	ScoreBase*					m_pScore;
protected:
	bool						m_bDestory;
public:
								WeaponKnife(void);
								~WeaponKnife(void);
	virtual void				init(Ogre::SceneManager *scene, btDynamicsWorld *world);
	virtual void				create(const char *modelName, float mass, float *scale, float *pos, float *quat, int score);
	virtual void				release(void);
	virtual void				update(float timePass);
	virtual WEAPON_TYPE			getType(void)				{return eWeaponKnife;}
	virtual bool				isDestory(void)const		{return m_bDestory;}
	virtual void				setDestory(void)			{m_bDestory = 1;}
};

class WeaponBook : public WeaponKnife
{
public:
	WeaponBook(void);
	~WeaponBook(void);
	virtual WEAPON_TYPE			getType(void)				{return eWeaponBook;}

};


class WeaponBomb : public WeaponKnife
{
public:
	WeaponBomb(void);
	~WeaponBomb(void);
	virtual WEAPON_TYPE			getType(void)				{return eWeaponBomb;}
};

#endif //_WEAPON_H_