#include "audio_system.h"

#include <irrKlang.h>
#include <iostream>
#include <fstream>
#include <json_spirit.h>
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

AudioSystem *AudioSystem::g_pInstance = NULL;

AudioSystem::AudioSystem(void):
	m_pEngine(NULL),
	m_pBGM(NULL),
	m_fMasterVolume(0.5f),
	m_fBGMVolume(0.2f),
	m_fSoundVolume(0.75f)
{
}


AudioSystem *AudioSystem::getInstance(void)
{
	if(g_pInstance == NULL)
	{
		g_pInstance = new AudioSystem();
		g_pInstance->m_pEngine = irrklang::createIrrKlangDevice();

		std::ifstream is("../configure/sound_device.cfg");
		if(is.is_open())
		{
			json_spirit::mValue value;
			json_spirit::read(is, value);
			json_spirit::mObject obj;
			obj = value.get_obj();
			g_pInstance->setMasterVolune(obj["MasterVolume"].get_real());
			g_pInstance->setBGMVolume(obj["BGMVolume"].get_real());
			g_pInstance->setSoundVolume(obj["SoundVolume"].get_real());
			float pos[3];
			float dir[3];
			pos[0] = obj["ListenerPos"].get_array()[0].get_real();
			pos[1] = obj["ListenerPos"].get_array()[1].get_real();
			pos[2] = obj["ListenerPos"].get_array()[2].get_real();
			dir[0] = obj["ListenerDir"].get_array()[0].get_real();
			dir[1] = obj["ListenerDir"].get_array()[1].get_real();
			dir[2] = obj["ListenerDir"].get_array()[2].get_real();
			g_pInstance->setListenerPosition(pos, dir);
		}
	}
	return g_pInstance;
}

AudioSystem::~AudioSystem(void)
{
	release();
}

void AudioSystem::restart(void)
{
	irrklang::ISound *music = NULL;
	V_SOUND::const_iterator ite;
	for(ite = m_vSound.begin(); ite != m_vSound.end(); ++ite)
	{
		music = *ite;
		music->stop();
		music->drop();
	}
	m_vSound.clear();

	if(m_pBGM != NULL)
	{
		m_pBGM->stop();
		m_pBGM->drop();
		m_pBGM = NULL;
	}
}
void AudioSystem::release(void)
{
	restart();

	if(m_pEngine != NULL)
		m_pEngine->drop(); // delete engine
}

void AudioSystem::play2D(const char *fileName, bool isLoop, bool startPaused)
{
	m_pBGM = m_pEngine->play2D(fileName, isLoop, startPaused);
	if(m_pBGM)
		m_pBGM->setVolume((irrklang::ik_f32)m_fBGMVolume);
}

void AudioSystem::play3D(const char *fileName, float *vec, float minDist, bool isLoop, bool startPaused)
{
	irrklang::ISound* music = m_pEngine->play3D(fileName,
		irrklang::vec3df(vec[0], vec[1], vec[2]), isLoop, startPaused, true);

	if (music)
	{
		music->setMinDistance(minDist);
		music->setVolume((irrklang::ik_f32)m_fSoundVolume);
	}

	if(isLoop || !startPaused)
		m_vSound.push_back(music);
}

void AudioSystem::play3DBGM(const char *fileName, float *vec, float minDist, bool isLoop, bool startPaused)
{
	m_pBGM = m_pEngine->play3D(fileName, irrklang::vec3df(vec[0], vec[1], vec[2]), isLoop, startPaused, true);

	if (m_pBGM)
	{
		m_pBGM->setMinDistance(minDist);
		m_pBGM->setVolume((irrklang::ik_f32)m_fBGMVolume);
	}
}

void AudioSystem::setListenerPosition(float *pos, float *dir)
{
	m_pEngine->setListenerPosition(irrklang::vec3df(pos[0], pos[1], pos[2]), irrklang::vec3df(dir[0], dir[1], dir[2]));
}

void AudioSystem::setMasterVolune(float volume)
{
	m_fMasterVolume = volume;
	if(m_pEngine != NULL)
		m_pEngine->setSoundVolume((irrklang::ik_f32)m_fMasterVolume);
}

void AudioSystem::setBGMVolume(float volume)
{
	m_fBGMVolume = volume;
	if(m_pBGM != NULL)
	{
		m_pBGM->setVolume((irrklang::ik_f32)volume);
	}
}

void AudioSystem::setSoundVolume(float volume)
{
	m_fSoundVolume = volume;
	m_bIsSetVolume = true;
	irrklang::ISound *music = NULL;
	V_SOUND::const_iterator ite;
	for(ite = m_vSound.begin(); ite != m_vSound.end(); ++ite)
	{
		music = *ite;
		music->setVolume((irrklang::ik_f32)volume);
	}
	m_bIsSetVolume = false;
}

void AudioSystem::update(float timePass)
{
	if(m_bIsSetVolume)
		return;
	m_pEngine->update();
	irrklang::ISound *music = NULL;
	V_SOUND::const_iterator ite;
	V_SOUND::const_iterator delIte;
	for(ite = m_vSound.begin(); ite != m_vSound.end();)
	{
		delIte = ite;
		 ++ite;
		music = *delIte;
		if(music->isFinished())
		{
			music->drop();
			m_vSound.erase(delIte);
		}
	}
}