#ifndef _VTFW_SIMULATION_H_
#define _VTFW_SIMULATION_H_
#include <iostream>
#include "../simulation_interface/simulation_interface.h"
namespace Ogre
{
	class SceneManager;
	class SceneNode;
	class AnimationState;
	class ParticleSystem;
	class ParticleEmitter;
	class Camera;
	class Entity;
	class RenderWindow;
	class Viewport;
}

class WaterInterface;
class VTFWRenderListener;

class VTFWSimulation : public SimulationInterface
{
private:
	VTFWRenderListener*							m_pRenderListener;
	Ogre::SceneManager*							m_pSceneMgr;
	Ogre::Camera*								m_pCamera;
	Ogre::RenderWindow*							m_pWindow;
	Ogre::Viewport *							m_pViewport;

	Ogre::SceneNode*							m_pHeadNode ;

	WaterInterface*								m_pWaterInterface;
	Ogre::Entity*								m_pWaterEntity ;

	float										m_fTimeoutDelay ;
	float										m_fHeadDepth ;
	float										m_fMoveSpeed;
public:
												VTFWSimulation(Ogre::SceneManager *sceneMgr, Ogre::Camera* cam, Ogre::RenderWindow *win, Ogre::Viewport *v);
												~VTFWSimulation(void);
	void										init(void);
	void										release(void);
	void										setupControls(OgreBites::SdkTrayManager* sdkTray);
	void										sliderMoved(OgreBites::Slider* slider);
	void										checkBoxToggled(OgreBites::CheckBox* checkBox);
	void										itemSelected(OgreBites::SelectMenu* menu);
	void										update(float timePass);
};
#endif //_VTFW_SIMULATION_H_