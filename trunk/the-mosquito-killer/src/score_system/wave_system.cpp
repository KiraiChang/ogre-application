#include "wave_system.h"
#include "game_system.h"
#include "../move_system/move_system.h"

#include <iostream>
#include <fstream>
#include <json_spirit.h>

WaveSystem::WaveSystem(void):
	m_fTotalTime(0),
	m_fTimepass(0),
	m_iCurrentIndex(0)
{
}

WaveSystem::~WaveSystem(void)
{
}

void WaveSystem::init(unsigned int stage)
{
	char fileName[128];
	sprintf(fileName, "../stage/stage%.3d.dat", stage);
	std::fstream file(fileName);
	if(file.is_open())
	{
		m_fTotalTime = 0;
		m_fTimepass = 0;
		m_iCurrentIndex = 0;
		m_vMosquitoData.clear();

		json_spirit::mValue value;
		json_spirit::read(file, value);
		json_spirit::mArray arr;
		json_spirit::mObject obj;
		arr = value.get_obj()["mosquito"].get_array();

		for(int i = 0; i <arr.size();i++)
		{
			obj = arr[i].get_obj();
			MosquitoData data;
			if(obj["m_eType"].get_str() == "eMosquitoBase")
				data.m_eType = eMosquitoBase;
			else if(obj["m_eType"].get_str() == "eMosquitoSplit")
				data.m_eType = eMosquitoSplit;
			else
				data.m_eType = eMosquitoFat;

			data.m_sModelName = obj["m_sModelName"].get_str();

			for(int i = 0; i < 3; i++)
			{
				data.m_vfScale[i] = obj["m_vfScale[3]"].get_array()[i].get_real();
				data.m_vfPos[i] = obj["m_vfPos[3]"].get_array()[i].get_real();
				data.m_vfQuat[i] = obj["m_vfQuat[4]"].get_array()[i].get_real();
			}
			data.m_vfQuat[3] = obj["m_vfQuat[4]"].get_array()[3].get_real();
			data.m_fMoveSpeed = obj["m_fMoveSpeed"].get_real();

			data.m_iScore = obj["m_iScore"].get_int();
			data.m_iParaData = obj["m_iParaData"].get_int();
			data.m_fTimepass = obj["m_fTimepass"].get_real();
			data.m_iCount = obj["m_iCount"].get_int();
			if(obj.count("m_uiMoveType") <= 0)
			{
				data.m_uiMoveType = 0;
			}
			else
			{
				if(obj["m_uiMoveType"].get_str() == "eMoveRandom")
					data.m_uiMoveType = eMoveRandom;
				else if(obj["m_uiMoveType"].get_str() == "eMoveStraight")
					data.m_uiMoveType = eMoveStraight;
				else
					data.m_uiMoveType = eMoveRandom;
			}
			m_vMosquitoData.push_back(data);
			m_fTotalTime += data.m_fTimepass;
		}
	}
	else
	{

	}
}

void WaveSystem::release(void)
{
	m_vMosquitoData.clear();
}

bool WaveSystem::work(float timepass)
{
	m_fTimepass += timepass;
	if(m_iCurrentIndex < m_vMosquitoData.size())
	{
		if(m_fTimepass > m_vMosquitoData[m_iCurrentIndex].m_fTimepass)
		{
			for(int i = 0; i < m_vMosquitoData[m_iCurrentIndex].m_iCount; i++)
			{
				GameSystem::getInstance()->createMosquito(m_vMosquitoData[m_iCurrentIndex].m_eType,
					m_vMosquitoData[m_iCurrentIndex].m_uiMoveType,
					m_vMosquitoData[m_iCurrentIndex].m_fMoveSpeed,
					m_vMosquitoData[m_iCurrentIndex].m_sModelName.c_str(),
					0.0,
					m_vMosquitoData[m_iCurrentIndex].m_vfScale,
					m_vMosquitoData[m_iCurrentIndex].m_vfPos,
					m_vMosquitoData[m_iCurrentIndex].m_vfQuat,
					m_vMosquitoData[m_iCurrentIndex].m_iScore,
					m_vMosquitoData[m_iCurrentIndex].m_iParaData);
			}
			m_iCurrentIndex++;
			m_fTimepass = 0.0;
		}
		return false;
	}
	return true;
}