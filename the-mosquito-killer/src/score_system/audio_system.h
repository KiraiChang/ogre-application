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
	V_SOUND														m_vSound;
																AudioSystem(void);
public:
	static AudioSystem *										getInstance(void);
	virtual														~AudioSystem(void);
	virtual void												release(void);
	virtual void												play2D(const char *fileName);
	virtual void												play3D(const char *fileName, float *vec, float minDist = 5.0f, bool isLoop = false, bool startPaused = false);
	virtual void												setListenerPosition(float *pos, float *dir);
	virtual void												update(float timePass);
};

#endif //_AUDIO_SYSTEM_H_