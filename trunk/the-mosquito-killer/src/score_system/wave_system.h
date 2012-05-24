#ifndef _WAVE_SYSTEM_H_
#define _WAVE_SYSTEM_H_
#include "../game_object/mosquito.h"
#include <string>
#include <vector>

class MosquitoData
{
public:
	MOSQUITO_TYPE					m_eType;
	unsigned int					m_uiMeshID;
	float							m_vfScale[3];
	float							m_vfPos[3];
	float							m_vfQuat[4];
	float							m_fMoveSpeed;
	unsigned int					m_uiMoveType;
	int								m_iScore;
	int								m_iParaData;

	float							m_fTimepass;
	int								m_iCount;
};

typedef std::vector< MosquitoData > V_MOSQUITO_DATA;

class WaveSystem
{
private:
	V_MOSQUITO_DATA					m_vMosquitoData;
	float							m_fTotalTime;
	float							m_fTimepass;
	unsigned int					m_iCurrentIndex;
public:
									WaveSystem(void);
									~WaveSystem(void);
	void							init(unsigned int stage, std::string &scene, std::string &sceneGroup, std::string &audio);
	void							release(void);
	bool							work(float timepass);//回傳值是是否結束
	float							getFullTime(void)const							{return m_fTotalTime;}
};

#endif //_WAVE_SYSTEM_H_