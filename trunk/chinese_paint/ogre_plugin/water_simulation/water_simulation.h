#ifndef _WATER_SIMULATION_H_
#define _WATER_SIMULATION_H_
#include <iostream>
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

class WaterInterface;

class WaterSimulation
{
private:
	Ogre::SceneManager*							m_pSceneMgr;
	Ogre::Camera*								m_pCamera;

	Ogre::SceneNode*							m_pHeadNode ;
	//Ogre::AnimationState*						m_pAnimState;
	//Ogre::ParticleSystem*						m_pParticleSystem ;
	//Ogre::ParticleEmitter*						m_pParticleEmitter ;

	WaterInterface*								m_pWaterInterface;
	Ogre::Entity*								m_pWaterEntity ;

	float										m_fTimeoutDelay ;
	float										m_fHeadDepth ;
	float										m_fMoveSpeed;
	//typedef vector<WaterCircle*>::type		WaterCircles ;
	//WaterCircles								circles ;
public:
												WaterSimulation(Ogre::SceneManager *sceneMgr, Ogre::Camera* cam);
												~WaterSimulation(void);
	void										init(void);
	void										release(void);
	void										update(float timePass);
	void										setHeadDepth(const float &depth) {m_fHeadDepth = depth;}
public:
	/** Head animation */
	void										animateHead(float timeSinceLastFrame);
	void										prepareCircleMaterial();
	void										processCircles(float timeSinceLastFrame);
	void										processParticles();
	void										setWaterMaterialName(const std::string &name);
	void										setMoveSpeed(const float &speed);
	WaterInterface*								getWater()	{return m_pWaterInterface;}
};


#endif //_WATER_SIMULATION_H_