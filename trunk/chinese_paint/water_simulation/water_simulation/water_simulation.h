#ifndef _WATER_SIMULATION_H_
#define _WATER_SIMULATION_H_

namespace Ogre
{
	class SceneManager;
	class SceneNode;
	class AnimationState;
	class ParticleSystem;
	class ParticleEmitter;
	class Camera;
}

class WaterSimulation
{
private:
	Ogre::SceneManager*							m_pSceneMgr;
	Ogre::Camera*								m_pCamera;

	Ogre::SceneNode*							m_pHeadNode ;
	Ogre::AnimationState*						m_pAnimState;
	Ogre::ParticleSystem*						m_pParticleSystem ;
	Ogre::ParticleEmitter*						m_pParticleEmitter ;

	//WaterMesh*								m_pWaterMesh;
	//Ogre::Entity*								m_pWaterEntity ;

	float										m_fTimeoutDelay ;
	float										m_fHeadDepth ;
	//typedef vector<WaterCircle*>::type WaterCircles ;
	//WaterCircles circles ;
public:
										WaterSimulation(Ogre::SceneManager *sceneMgr, Ogre::Camera* cam);
										~WaterSimulation(void);
	void								init(void);
	void								release(void);
	void								update(float timePass);
public:
	/** Head animation */
	void animateHead(float timeSinceLastFrame);
	void prepareCircleMaterial();
	void processCircles(float timeSinceLastFrame);
	void processParticles();
};


#endif //_WATER_SIMULATION_H_