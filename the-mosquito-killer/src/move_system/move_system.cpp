#include "move_system.h"
//*******************************************************
//*****************  MOVE_BASE  *************************
//*******************************************************
MoveBase::MoveBase(Ogre::Node *node):
	t(0),
	m_pNode(node)
{
}

MoveBase::~MoveBase()
{
	m_pNode = NULL;
}

void MoveBase::move(void)
{
	Ogre::Vector3 nextPos;
	Ogre::Vector3 copyPos = m_pNode->getPosition();

	copyPos.z = 0;

	if(t <= 1)
	{
		nextPos = copyPos * Ogre::Math::Pow(1-t,3) 
					+ 3 * C1 * Ogre::Math::Pow(1-t,2) *  t
					+ 3 * C2 * (1-t) * Ogre::Math::Pow(t,2)
					+ P4 * Ogre::Math::Pow(t,3);

		nextPos.z = m_pNode->getPosition().z;

		m_pNode->setPosition(nextPos);

		if(m_pNode->getPosition().z > -20)
			m_pNode->translate(Ogre::Vector3(0,0,-0.2));

	t += 0.004;

	if(t > 1) t =1;
	}
}

void MoveBase::setRoute(void)
{
	t = 0;

	float rxMIN = m_pNode->getPosition().x - 100;
	float ryMIN = m_pNode->getPosition().y - 100;


	if(m_pNode->getPosition().z < 150)
	{
		if(m_pNode->getPosition().x < -180)
			rxMIN = -180;

		if(m_pNode->getPosition().x > 180)
			rxMIN = 180-200;

		if(m_pNode->getPosition().y < 100)
			ryMIN = 100;

		if(m_pNode->getPosition().y > 300)
			ryMIN = 180-200;
	}

	float rx = Ogre::Math::RangeRandom(rxMIN,rxMIN+200);
	float ry = Ogre::Math::RangeRandom(ryMIN,ryMIN+200);
	C1 = Ogre::Vector3(rx,ry,0);
	rx = Ogre::Math::RangeRandom(rxMIN,rxMIN+200);
	ry = Ogre::Math::RangeRandom(ryMIN,ryMIN+200);
	C2 = Ogre::Vector3(rx,ry,0);
	rx = Ogre::Math::RangeRandom(rxMIN,rxMIN+200);
	ry = Ogre::Math::RangeRandom(ryMIN,ryMIN+200);
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


void MoveBase::update(void)
{
	if(m_pNode == NULL)
		return;

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