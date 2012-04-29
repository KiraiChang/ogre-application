#include "shoot_system.h"
//*******************************************************
//*****************  MOVE_BASE  *************************
//*******************************************************
const float DEPTH_MOSQUITO_MOVE = 100;
ShootBase::ShootBase(Ogre::Node *node):
	t(0),
	m_pNode(node)
{
	P0 = m_pNode->getPosition();
	P2 = m_pNode->getPosition();
}

ShootBase::~ShootBase()
{
	m_pNode = NULL;
}

void ShootBase::move(void)
{
	Ogre::Vector3 nextPos;

	if(t <= 1)
	{
		nextPos = P0 * Ogre::Math::Pow(1-t,2)
					+ 2 * C1 * (1-t) *  t
					+ P2 * Ogre::Math::Pow(t,2);

		nextPos.y = m_pNode->getPosition().y;

		m_pNode->setPosition(nextPos);

	t += 0.001;

	if(t > 1) t =1;
	}
}

void ShootBase::setRoute(void)
{
	t = 0;

	P0.y = 0;

	P2 = P0 + Ogre::Vector3(0,0,-200);
	C1 = (P0 + P2)/2 + Ogre::Vector3(50,0,0);
}

Ogre::Vector3 ShootBase::getPosition(void)const
{
	return m_pNode->getPosition();
}

Ogre::Quaternion ShootBase::getOrientation(void)const
{
	return m_pNode->getOrientation();
}


void ShootBase::update(bool &destory)
{
	if(m_pNode == NULL)
		return;

	if(!destory)
		move();
}
//*******************************************************
//*****************  MOVE_BASE  *************************
//*******************************************************