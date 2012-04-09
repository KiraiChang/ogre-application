#include "score_object.h"

//*******************************************************
//********************  BASE  ***************************
//*******************************************************

ScoreBase::ScoreBase(unsigned int type):
	m_iType(type),
	m_bDestory(false)
{
}

ScoreBase::~ScoreBase(void)
{

}

unsigned int ScoreBase::getType(void)const
{
	return m_iType;
}

int ScoreBase::getEffect(unsigned int bodyPart)
{
	return 0;
}

//*******************************************************
//********************  COIN  ***************************
//*******************************************************

ScoreCoin::ScoreCoin(unsigned int type):ScoreBase(type)
{
}

ScoreCoin::~ScoreCoin(void)
{
}

int ScoreCoin::getEffect(unsigned int bodyPart)
{
	int effect = 0;
	switch(bodyPart)
	{
	case SCORE_TYPE_HAND:
		effect = 20;
		break;
	case SCORE_TYPE_BODY:
		effect = 10;
		break;
	}
	return effect;
}

//*******************************************************
//********************  ARROW  **************************
//*******************************************************

ScoreArrow::ScoreArrow(unsigned int type):ScoreBase(type)
{

}

ScoreArrow::~ScoreArrow(void)
{
}

int ScoreArrow::getEffect(unsigned int bodyPart)
{
	int effect = 0;
	switch(bodyPart)
	{
	case SCORE_TYPE_HAND:
		effect = 10;
		break;
	case SCORE_TYPE_BODY:
		effect = -10;
		break;
	}
	return effect;
}

//*******************************************************
//********************  BOMB  ***************************
//*******************************************************

ScoreBomb::ScoreBomb(unsigned int type):ScoreBase(type)
{

}
ScoreBomb::~ScoreBomb(void)
{
}

int ScoreBomb::getEffect(unsigned int bodyPart)
{
	int effect = 0;
	switch(bodyPart)
	{
	case SCORE_TYPE_HAND:
		effect = -10;
		break;
	case SCORE_TYPE_BODY:
		effect = -20;
		break;
	}
	return effect;
}


//*******************************************************
//*********************  END  ***************************
//*******************************************************