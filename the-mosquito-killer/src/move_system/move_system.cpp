#include "move_system.h"
//*******************************************************
//*****************  MOVE_BASE  *************************
//*******************************************************
const float DEPTH_MOSQUITO_MOVE = 100;
MoveBase::MoveBase(Ogre::Node *node):
	m_pNode(node)
{
}

MoveBase::~MoveBase()
{
	release();
}

void MoveBase::release()
{
	m_pNode = NULL;
}


Ogre::Vector3 MoveBase::getPosition(void)const
{
	return m_pNode->getPosition();
}

Ogre::Quaternion MoveBase::getOrientation(void)const
{
	return m_pNode->getOrientation();
}


void MoveBase::update(bool &destory, float moveDistance)
{
	if(m_pNode == NULL)
		return;

	if(m_pNode->getPosition().z > DEPTH_MOSQUITO_MOVE)
		destory = true;

	if(isChangeRoute())
	{
		setRoute();
	}
	else		//keep flying
		move(moveDistance);
}

//*******************************************************
//****************  MOVE_RANDOM  ************************
//*******************************************************
MoveRandom::MoveRandom(Ogre::Node *node):
	MoveBase(node),
	t(0)
{
	P0 = m_pNode->getPosition();
	P4 = m_pNode->getPosition();
}

MoveRandom::~MoveRandom()
{
	release();
}

void MoveRandom::release()
{
	m_pNode = NULL;
}

void MoveRandom::move(float moveDistance)
{
	Ogre::Vector3 nextPos;

	if(t <= 1)
	{
		nextPos = P0 * Ogre::Math::Pow(1-t,3)
					+ 3 * C1 * Ogre::Math::Pow(1-t,2) *  t
					+ 3 * C2 * (1-t) * Ogre::Math::Pow(t,2)
					+ P4 * Ogre::Math::Pow(t,3);

		nextPos.z = m_pNode->getPosition().z;

		m_pNode->setPosition(nextPos);

		//if(m_pNode->getPosition().z > -20)
			//m_pNode->translate(Ogre::Vector3(0,0,0.2));
		m_pNode->translate(Ogre::Vector3(0,0, moveDistance * 10));

		//t += 0.016;
		t+=moveDistance;

		if(t > 1) t =1;
	}
}

void MoveRandom::setRoute(void)
{
	t = 0;

	P0 = m_pNode->getPosition();
	P0.z = 0;
	float rxMIN = P0.x - 25.0;
	float ryMIN = P0.y - 25.0;
	float rxMAX = P0.x + 25.0;
	float ryMAX = P0.y + 25.0;

	if(m_pNode->getPosition().z > -200)
	{
		if(rxMIN < -50)
			rxMIN = -50;

		if(rxMAX > 50)
			rxMAX = 50;

		if(ryMAX > 35)
			ryMIN = 35;
	}
	if(ryMIN < 0)
		ryMIN = 10;

	float rx = Ogre::Math::RangeRandom(rxMIN,rxMAX);
	float ry = Ogre::Math::RangeRandom(ryMIN,ryMAX);
	C1 = Ogre::Vector3(rx,ry,0);
	rx = Ogre::Math::RangeRandom(rxMIN,rxMAX);
	ry = Ogre::Math::RangeRandom(ryMIN,ryMAX);
	C2 = Ogre::Vector3(rx,ry,0);
	rx = Ogre::Math::RangeRandom(rxMIN,rxMAX);
	ry = Ogre::Math::RangeRandom(ryMIN,ryMAX);
	P4 = Ogre::Vector3(rx,ry,0);
}

bool MoveRandom::isChangeRoute(void)
{
	Ogre::Vector3 d = m_pNode->getPosition() - P4;
	if(d.x < 1.5 && d.x > -1.5 && d.y < 1.5 && d.y > -1.5)
		return true;
	else
		return false;
}

//*******************************************************
//***************  MOVE_STRAIGHT  ***********************
//*******************************************************

MoveStraight::MoveStraight(Ogre::Node *node):
	MoveBase(node)
{
}

MoveStraight::~MoveStraight()
{
	release();
}

void MoveStraight::release()
{
	MoveBase::release();
}

void MoveStraight::setRoute(void)
{
}

bool MoveStraight::isChangeRoute(void)
{
	return false;
}

void MoveStraight::move(float moveDistance)
{
	m_pNode->translate(Ogre::Vector3(0,0, moveDistance * 10));
}
//*******************************************************
//*****************  MOVE_BASE  *************************
//*******************************************************