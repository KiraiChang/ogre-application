#ifndef _SCORE_OBJECT_H_
#define _SCORE_OBJECT_H_

enum ScoreType
{
	SCORE_TYPE_BASE,
	SCORE_TYPE_HAND,
	SCORE_TYPE_BODY,
	SCORE_TYPE_COIN,
	SCORE_TYPE_ARROW,
	SCORE_TYPE_BOMB,
};

//*******************************************************
//********************  BASE  ***************************
//*******************************************************

class ScoreBase
{
public:
	ScoreBase(unsigned int type);
	virtual ~ScoreBase(void);
	unsigned int getType(void)const;
	virtual int getEffect(unsigned int bodyPart);
	bool		m_bDestory;
private:
	unsigned int m_iType;
};

//*******************************************************
//********************  COIN  ***************************
//*******************************************************

class ScoreCoin : public ScoreBase
{
public:
	ScoreCoin(unsigned int type);
	virtual ~ScoreCoin(void);
	virtual int getEffect(unsigned int bodyPart);
private:
};

//*******************************************************
//********************  ARROW  **************************
//*******************************************************
class ScoreArrow : public ScoreBase
{
public:
	ScoreArrow(unsigned int type);
	virtual ~ScoreArrow(void);
	virtual int getEffect(unsigned int bodyPart);
private:
};

//*******************************************************
//********************  BOMB  ***************************
//*******************************************************

class ScoreBomb : public ScoreBase
{
public:
	ScoreBomb(unsigned int type);
	virtual ~ScoreBomb(void);
	virtual int getEffect(unsigned int bodyPart);
private:
};

//*******************************************************
//*********************  END  ***************************
//*******************************************************
#endif //_SCORE_OBJECT_H_