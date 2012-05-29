#ifndef _SCORE_OBJECT_H_
#define _SCORE_OBJECT_H_

enum ScoreType
{
	SCORE_TYPE_BASE,
	SCORE_TYPE_HAND,
	SCORE_TYPE_BODY,
	SCORE_TYPE_WEAPON,
	SCORE_TYPE_ENEMY,
	SCORE_TYPE_ICON,
	//SCORE_TYPE_COIN,
	//SCORE_TYPE_ARROW,
	//SCORE_TYPE_BOMB,
};

//*******************************************************
//********************  BASE  ***************************
//*******************************************************

class ScoreBase
{
public:
							ScoreBase(unsigned int type);
	virtual					~ScoreBase(void);
	unsigned int			getType(void)const;
	virtual int				getEffect(unsigned int bodyPart)const;
	void					regParent(void *parent);
	void *					getParent(void);
	//bool					m_bDestory;
private:
	unsigned int			m_iType;
	void *					m_pParent;
};

//*******************************************************
//*******************  WEAPON  **************************
//*******************************************************
class ScoreWeapon : public ScoreBase
{
public:
							ScoreWeapon(unsigned int type);
	virtual					~ScoreWeapon(void);
	virtual int				getEffect(unsigned int bodyPart)const;
private:
};
//*******************************************************
//********************  ENEMY  **************************
//*******************************************************
class ScoreEnemy : public ScoreBase
{
public:
							ScoreEnemy(unsigned int type, int score);
	virtual					~ScoreEnemy(void);
	virtual int				getEffect(unsigned int bodyPart)const;
private:
	int						m_iScore;
};
////*******************************************************
////********************  COIN  ***************************
////*******************************************************
//
//class ScoreCoin : public ScoreBase
//{
//public:
//	ScoreCoin(unsigned int type);
//	virtual ~ScoreCoin(void);
//	virtual int getEffect(unsigned int bodyPart)const;
//private:
//};
//
////*******************************************************
////********************  ARROW  **************************
////*******************************************************
//class ScoreArrow : public ScoreBase
//{
//public:
//	ScoreArrow(unsigned int type);
//	virtual ~ScoreArrow(void);
//	virtual int getEffect(unsigned int bodyPart)const;
//private:
//};
//
////*******************************************************
////********************  BOMB  ***************************
////*******************************************************
//
//class ScoreBomb : public ScoreBase
//{
//public:
//	ScoreBomb(unsigned int type);
//	virtual ~ScoreBomb(void);
//	virtual int getEffect(unsigned int bodyPart)const;
//private:
//};

//*******************************************************
//*********************  END  ***************************
//*******************************************************
#endif //_SCORE_OBJECT_H_