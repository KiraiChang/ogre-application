#ifndef _MOVE_SYSTEM_H_
#define _MOVE_SYSTEM_H_
#include <Ogre.h>
//*******************************************************
//*****************  MOVE_BASE  *************************
//*******************************************************
enum MOVE_TYPE
{
	eMoveRandom,
	eMoveStraight,
};

class MoveBase
{
protected:
	Ogre::Node*				m_pNode;
public:
							MoveBase(Ogre::Node *node);
	virtual					~MoveBase();
	virtual void			release();

	virtual void			setRoute(void) = 0;
	virtual void			move(float moveDistance) = 0;
	virtual bool			isChangeRoute(void) = 0;

	Ogre::Vector3			getPosition(void)const;
	Ogre::Quaternion		getOrientation(void)const;
	void					update(bool &destory, float moveDistance);
};

//*******************************************************
//****************  MOVE_RANDOM  ************************
//*******************************************************
class MoveRandom : public MoveBase
{
public:
private:
	Ogre::Vector3			P0;
	Ogre::Vector3			P4;
	Ogre::Vector3			C1;
	Ogre::Vector3			C2;
	Ogre::Vector3			C3;
	Ogre::Node*				m_pNode;
	float					t;
public:
							MoveRandom(Ogre::Node *node);
	virtual					~MoveRandom();
	virtual void			release();

	virtual void			setRoute(void);
	virtual void			move(float moveDistance);
	virtual bool			isChangeRoute(void);
};
//*******************************************************
//***************  MOVE_STRAIGHT  ***********************
//*******************************************************
class MoveStraight : public MoveBase
{
public:
							MoveStraight(Ogre::Node *node);
	virtual					~MoveStraight();
	virtual void			release();

	virtual void			setRoute(void);
	virtual bool			isChangeRoute(void);
	virtual void			move(float moveDistance);
};
//*******************************************************
//********************  END  ****************************
//*******************************************************
#endif //_MOVE_SYSTEM_H_