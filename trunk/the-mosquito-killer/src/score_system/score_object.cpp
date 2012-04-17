#include "score_object.h"

//*******************************************************
//********************  BASE  ***************************
//*******************************************************

ScoreBase::ScoreBase(unsigned int type):
	m_iType(type),
	m_bDestory(false),
	m_pParent(0)
{
}

ScoreBase::~ScoreBase(void)
{
	m_pParent = 0;
}

unsigned int ScoreBase::getType(void)const
{
	return m_iType;
}

int ScoreBase::getEffect(unsigned int bodyPart)const
{
	return 0;
}

void ScoreBase::regParent(void *parent)
{
	m_pParent = parent;
}

void *ScoreBase::getParent(void)
{
	return m_pParent;
}

//*******************************************************
//*******************  WEAPON  **************************
//*******************************************************
ScoreWeapon::ScoreWeapon(unsigned int type):
		ScoreBase(type)
{

}

ScoreWeapon::~ScoreWeapon(void)
{
}

int ScoreWeapon::getEffect(unsigned int bodyPart)const
{
	return 0;
}

//*******************************************************
//********************  ENEMY  **************************
//*******************************************************
ScoreEnemy::ScoreEnemy(unsigned int type, int score):
		ScoreBase(type),
		m_iScore(score)
{

}

ScoreEnemy::~ScoreEnemy(void)
{

}

int ScoreEnemy::getEffect(unsigned int bodyPart)const
{
		int effect = 0;
		switch(bodyPart)
		{
		case SCORE_TYPE_HAND:
			effect = m_iScore;
			break;
		case SCORE_TYPE_WEAPON:
			effect = m_iScore;
			break;
		}
		return effect;
}
////*******************************************************
////********************  COIN  ***************************
////*******************************************************
//
//ScoreCoin::ScoreCoin(unsigned int type):ScoreBase(type)
//{
//}
//
//ScoreCoin::~ScoreCoin(void)
//{
//}
//
//int ScoreCoin::getEffect(unsigned int bodyPart)const
//{
//	int effect = 0;
//	switch(bodyPart)
//	{
//	case SCORE_TYPE_HAND:
//		effect = 20;
//		break;
//	case SCORE_TYPE_BODY:
//		effect = 10;
//		break;
//	}
//	return effect;
//}
//
////*******************************************************
////********************  ARROW  **************************
////*******************************************************
//
//ScoreArrow::ScoreArrow(unsigned int type):ScoreBase(type)
//{
//
//}
//
//ScoreArrow::~ScoreArrow(void)
//{
//}
//
//int ScoreArrow::getEffect(unsigned int bodyPart)const
//{
//	int effect = 0;
//	switch(bodyPart)
//	{
//	case SCORE_TYPE_HAND:
//		effect = 10;
//		break;
//	case SCORE_TYPE_BODY:
//		effect = -10;
//		break;
//	}
//	return effect;
//}
//
////*******************************************************
////********************  BOMB  ***************************
////*******************************************************
//
//ScoreBomb::ScoreBomb(unsigned int type):ScoreBase(type)
//{
//
//}
//ScoreBomb::~ScoreBomb(void)
//{
//}
//
//int ScoreBomb::getEffect(unsigned int bodyPart)const
//{
//	int effect = 0;
//	switch(bodyPart)
//	{
//	case SCORE_TYPE_HAND:
//		effect = -10;
//		break;
//	case SCORE_TYPE_BODY:
//		effect = -20;
//		break;
//	}
//	return effect;
//}
//
//*******************************************************
//*********************  END  ***************************
//*******************************************************