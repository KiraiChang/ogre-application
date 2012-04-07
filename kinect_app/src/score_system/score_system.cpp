#include "score_system.h"
#include "score_object.h"

ScoreSystem *ScoreSystem::g_pSystem = 0;

ScoreSystem::ScoreSystem():m_iScore(0)
{
}


ScoreSystem *ScoreSystem::getSystem()
{
	if(g_pSystem == 0)
		g_pSystem = new ScoreSystem();
	return g_pSystem;
}

ScoreSystem::~ScoreSystem()
{
}

ScoreBase *ScoreSystem::createScoreObject(unsigned int type)
{
	ScoreBase *object = 0;
	switch(type)
	{
	case SCORE_TYPE_BASE:
	case SCORE_TYPE_HAND:
	case SCORE_TYPE_BODY:
		object = new ScoreBase(type);
		break;
	case SCORE_TYPE_COIN:
		break;
	case SCORE_TYPE_ARROW:
		break;
	case SCORE_TYPE_BOME:
		break;
	default:
		break;
	}
	return object;
}

void ScoreSystem::resetScore(void)
{
	getSystem()->m_iScore = 0;
}

int ScoreSystem::calcScore(ScoreBase *bodyPart, ScoreBase *attachObj)
{
	int effectScore = attachObj->getEffect(bodyPart->getType());
	getSystem()->m_iScore += effectScore;
	return effectScore;
}

int ScoreSystem::getScore(void)
{
	return getSystem()->m_iScore;
}