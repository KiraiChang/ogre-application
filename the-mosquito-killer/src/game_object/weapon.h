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


enum WEAPON_TYPE
{
	eWeaponKnife = 0,
	eWeaponBook,
	eWeaponBomb,
	eChooseKnife, // add 快捷武器類別
	eChooseBook,
	eChooseBomb
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
	virtual void				create(const char *modelName, float mass, float *scale, float *pos, float *size, float *quat, int score,float *tar);
	virtual void				release(void);
	virtual void				update(float timePass);
	virtual WEAPON_TYPE			getType(void)				{return eWeaponKnife;}
	virtual bool				isDestory(void)const		{return m_bDestory;}
	virtual void				setDestory(void)			{m_bDestory = 1;}
	virtual void				setVisible(bool v);
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

class ChooseKnife : public WeaponKnife  //下面都是新增的add
{
public:
	ChooseKnife(void);
	~ChooseKnife(void);
	virtual WEAPON_TYPE			getType(void)				{return eChooseKnife;}
};


class ChooseBook : public WeaponKnife
{
public:
	ChooseBook(void);
	~ChooseBook(void);
	virtual WEAPON_TYPE			getType(void)				{return eChooseBook;}
};


class ChooseBomb : public WeaponKnife
{
public:
	ChooseBomb(void);
	~ChooseBomb(void);
	virtual WEAPON_TYPE			getType(void)				{return eChooseBomb;}
};

#endif //_WEAPON_H_