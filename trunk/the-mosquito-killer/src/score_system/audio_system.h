#ifndef _AUDIO_SYSTEM_H_
#define _AUDIO_SYSTEM_H_
#include <list>



namespace irrklang
{
	class ISoundEngine;
	class ISound;
}

typedef std::list< irrklang::ISound *> V_SOUND;

class AudioSystem
{
private:
	static AudioSystem *										g_pInstance;
	irrklang::ISoundEngine*										m_pEngine;
	float														m_fMasterVolume;
	float														m_fBGMVolume;
	float														m_fSoundVolume;
	bool														m_bIsSetVolume;
	V_SOUND														m_vSound;
	irrklang::ISound *											m_pBGM;
																AudioSystem(void);
public:
	static AudioSystem *										getInstance(void);
	virtual														~AudioSystem(void);
	virtual void												restart(void);
	virtual void												release(void);
	virtual void												play2D(const char *fileName, bool isLoop = true, bool startPaused = false);
	virtual void												play3D(const char *fileName, float *vec, float minDist = 5.0f, bool isLoop = false, bool startPaused = false);
	virtual void												play3DBGM(const char *fileName, float *vec, float minDist = 5.0f, bool isLoop = true, bool startPaused = false);
	virtual void												setListenerPosition(float *pos, float *dir);
	virtual void												setMasterVolune(float volume);
	virtual void												setBGMVolume(float volume);
	virtual void												setSoundVolume(float volume);
	virtual void												update(float timePass);
};

#endif //_AUDIO_SYSTEM_H_