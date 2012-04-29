#include "move_system.h"
//*******************************************************
//*****************  MOVE_BASE  *************************
//*******************************************************
const float DEPTH_MOSQUITO_MOVE = 100;
MoveBase::MoveBase(Ogre::Node *node):
	t(0),
	m_pNode(node)
{
	P0 = m_pNode->getPosition();
	P4 = m_pNode->getPosition();
}

MoveBase::~MoveBase()
{
	m_pNode = NULL;
}

void MoveBase::move(void)
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
			m_pNode->translate(Ogre::Vector3(0,0,0.2));

	t += 0.016;

	if(t > 1) t =1;
	}
}

void MoveBase::setRoute(void)
{
	t = 0;

	P0 = m_pNode->getPosition();
	P0.z = 0;
	float rxMIN = P0.x - 50.0;
	float ryMIN = P0.y - 50.0;
	float rxMAX = P0.x + 50.0;
	float ryMAX = P0.y + 50.0;

	if(m_pNode->getPosition().z > -200)
	{
		if(rxMIN < -100)
			rxMIN = -100;

		if(rxMAX > 100)
			rxMAX = 100;

		if(ryMAX > 100)
			ryMIN = 100;
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

Ogre::Vector3 MoveBase::getPosition(void)const
{
	return m_pNode->getPosition();
}

Ogre::Quaternion MoveBase::getOrientation(void)const
{
	return m_pNode->getOrientation();
}


void MoveBase::update(bool &destory)
{
	if(m_pNode == NULL)
		return;

	if(m_pNode->getPosition().z > DEPTH_MOSQUITO_MOVE)
		destory = true;

	Ogre::Vector3 d = m_pNode->getPosition() - P4;

	if(d.x < 1.5 && d.x > -1.5 && d.y < 1.5 && d.y > -1.5)	//if end of fly , so create new route
	{
		setRoute();
		//std::cout<<"------- the P4" << Mosqu->P4<<"and the pos :"<<Mosqu->getPosition()<<std::endl;
	}
	else		//keep flying
		move();
}
//*******************************************************
//*****************  MOVE_BASE  *************************
//*******************************************************