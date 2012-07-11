#ifndef _STABLE_FLUIDS_H_
#define _STABLE_FLUIDS_H_

namespace Ogre
{
	class SceneManager;
	class SceneNode;
	class AnimationState;
	class ParticleSystem;
	class ParticleEmitter;
	class Camera;
	class Entity;
}
class StableFluidsGrid;
class StableFluids
{
private:
	Ogre::SceneManager*							m_pSceneMgr;
	Ogre::Camera*								m_pCamera;
	StableFluidsGrid*							m_pFluidGrid;
public:
												StableFluids(Ogre::SceneManager *sceneMgr, Ogre::Camera* cam);
												~StableFluids(void);
	void										init(void);
	void										release(void);
	void										update(float timePass);
};


#endif //_STABLE_FLUIDS_H_