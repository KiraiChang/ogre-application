#ifndef _MOVE_SYSTEM_H_
#define _MOVE_SYSTEM_H_
#include <Ogre.h>
//*******************************************************
//*****************  MOVE_BASE  *************************
//*******************************************************
class MoveBase
{
public:
private:
	Ogre::Vector3			P4;
	Ogre::Vector3			C1;
	Ogre::Vector3			C2;
	Ogre::Vector3			C3;
	Ogre::Node*				m_pNode;
	float					t;
public:
							MoveBase(Ogre::Node *node);
	virtual					~MoveBase();
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