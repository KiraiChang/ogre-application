#ifndef _SCORE_SYSTEM_H_
#define _SCORE_SYSTEM_H_

class ScoreBase;
class ScoreSystem
{
private:
	ScoreSystem();
	static ScoreSystem *g_pSystem;
	int m_iScore;
public:
	static ScoreSystem *getSystem(void);
	virtual ~ScoreSystem(void);
	static ScoreBase *createScoreObject(unsigned int type, int score = 0);
	static void resetScore(void);
	static int calcScore(ScoreBase *bodyPart, ScoreBase *attachObj);
	static int getScore(void);
};

#endif _SCORE_SYSTEM_H_