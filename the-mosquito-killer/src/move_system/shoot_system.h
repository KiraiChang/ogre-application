#ifndef _SHOOT_SYSTEM_H_
#define _SHOOT_SYSTEM_H_
#include <Ogre.h>
//*******************************************************
//*****************  MOVE_BASE  *************************
//*******************************************************
enum SHOOT_TYPE
{
	eShootBase = 0,
	eShootNormal,
};

class ShootBase
{
protected:
	Ogre::Node*				m_pNode;
public:
							ShootBase(Ogre::Node *node);
	virtual					~ShootBase();
	virtual void			release();

	virtual void			setRoute(void);
	virtual void			move(float moveDistance);

	Ogre::Vector3			getPosition(void)const;
	Ogre::Quaternion		getOrientation(void)const;
	void					update(bool &destory, float moveDistance);
};

class ShootNormal : public ShootBase
{
public:
private:
	Ogre::Vector3			P0;
	Ogre::Vector3			P2;
	Ogre::Vector3			C1;
	float					t;
	Ogre::Vector3			step_move;

public:
							ShootNormal(Ogre::Node *node,float *tar);
	virtual					~ShootNormal();
private:
	virtual void			setRoute(void);
	virtual void			move(float moveDistance);
};
//*******************************************************
//********************  END  ****************************
//*******************************************************
#endif //_MOVE_SYSTEM_H_