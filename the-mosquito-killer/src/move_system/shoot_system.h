#ifndef _SHOOT_SYSTEM_H_
#define _SHOOT_SYSTEM_H_
#include <Ogre.h>
//*******************************************************
//*****************  MOVE_BASE  *************************
//*******************************************************
class ShootBase
{
public:
private:
	Ogre::Vector3			P0;
	Ogre::Vector3			P2;
	Ogre::Vector3			C1;
	Ogre::Node*				m_pNode;
	float					t;
public:
							ShootBase(Ogre::Node *node);
	virtual					~ShootBase();
private:
	virtual void			setRoute(void);
	virtual void			move(void);
public:
	Ogre::Vector3			getPosition(void)const;
	Ogre::Quaternion		getOrientation(void)const;
	virtual void			update(bool &destory);
};
//*******************************************************
//********************  END  ****************************
//*******************************************************
#endif //_MOVE_SYSTEM_H_