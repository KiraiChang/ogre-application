#include "audio_system.h"

#include <irrKlang.h>

#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

AudioSystem *AudioSystem::g_pInstance = NULL;

AudioSystem::AudioSystem(void):
	m_pEngine(NULL)
{
}


AudioSystem *AudioSystem::getInstance(void)
{
	if(g_pInstance == NULL)
	{
		g_pInstance = new AudioSystem();
		g_pInstance->m_pEngine = irrklang::createIrrKlangDevice();
	}
	return g_pInstance;
}

AudioSystem::~AudioSystem(void)
{
	release();
}

void AudioSystem::release(void)
{
	irrklang::ISound *music = NULL;
	V_SOUND::const_iterator ite;
	for(ite = m_vSound.begin(); ite != m_vSound.end(); ++ite)
	{
		music = *ite;
		music->drop();
	}
	m_vSound.clear();

	if(m_pEngine != NULL)
		m_pEngine->drop(); // delete engine
}

void AudioSystem::play2D(const char *fileName)
{
	m_pEngine->play2D(fileName);
}

void AudioSystem::play3D(const char *fileName, float *vec, float minDist, bool isLoop, bool startPaused)
{
	irrklang::ISound* music = m_pEngine->play3D(fileName,
		irrklang::vec3df(vec[0], vec[1], vec[2]), isLoop, startPaused, true);

	if (music)
		music->setMinDistance(minDist);

	if(isLoop || !startPaused)
		m_vSound.push_back(music);
}

void AudioSystem::setListenerPosition(float *pos, float *dir)
{
	m_pEngine->setListenerPosition(irrklang::vec3df(pos[0], pos[1], pos[2]), irrklang::vec3df(dir[0], dir[1], dir[2]));
}

void AudioSystem::update(float timePass)
{
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