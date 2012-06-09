#include "shoot_system.h"


//*******************************************************
//****************  SHOOT_BASE  *************************
//*******************************************************
ShootBase::ShootBase(Ogre::Node *node):
	m_pNode(node)
{
}

ShootBase::~ShootBase()
{
	release();
}
void ShootBase::release()
{
	m_pNode = NULL;
}

Ogre::Vector3 ShootBase::getPosition(void)const
{
	return m_pNode->getPosition();
}

Ogre::Quaternion ShootBase::getOrientation(void)const
{
	return m_pNode->getOrientation();
}

void ShootBase::move(float moveDistance)
{
}

void ShootBase::setRoute(void)
{
}

void ShootBase::update(bool &destory, float moveDistance)
{
	if(m_pNode == NULL)
		return;

	if(!destory)
		move(moveDistance);
}
//*******************************************************
//***************  SHOOT_NORMAL  ************************
//*******************************************************
const float DEPTH_MOSQUITO_MOVE = 100;
ShootNormal::ShootNormal(Ogre::Node *node,float *tar):
	ShootBase(node)
{
	P0 = m_pNode->getPosition();
	P2.x = tar[0]*150;
	P2.y = tar[1]*150;
	P2.z = tar[2]*150;
	setRoute();
}

ShootNormal::~ShootNormal()
{
	m_pNode = NULL;
}

void ShootNormal::move(float moveDistance)
{
	//Ogre::Vector3 nextPos;

	/*if(t <= 1 && m_pNode->getPosition() != P2)
	{
		nextPos = P0 * Ogre::Math::Pow(1-t,2)
					+ 2 * C1 * (1-t) *  t
					+ P2 * Ogre::Math::Pow(t,2);

		m_pNode->setPosition(nextPos);

		t += 0.02;

		if(t > 1)
		{
			t =1;
			step_move = (P2 - P0)*0.02;
		}
	}
	else
	{
		m_pNode->translate(step_move);
	}
	*/
	//Ogre::Vector3 P1 = m_pNode->getPosition();
	//Ogre::Vector3 P3 = P2 - P0;
	//step_move = P3.normalise();
	//P1 += (step_move * moveDistance);
	//m_pNode->setPosition(P1);
	m_pNode->translate(step_move * moveDistance);
}

void ShootNormal::setRoute(void)
{
	t = 0;

	//P0.y = 0;

	//P2 = P0 + Ogre::Vector3(0,0,-200);
	//C1 = (P0 + P2)/2 ;
	step_move = (P2 - P0)*0.05;
}
//*******************************************************
//*****************  MOVE_BASE  *************************
//*******************************************************