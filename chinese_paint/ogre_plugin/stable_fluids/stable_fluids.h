#ifndef STABLE_FLUIDS_MESH_H_
#define STABLE_FLUIDS_MESH_H_

#include "../simulation_interface/simulation_interface.h"
#include <iostream>
#include <ogre.h>

class WaterInterface;

class StableFluids : public SimulationInterface
{
private:
	Ogre::SceneManager *					m_pSceneMgr;
	Ogre::Camera *							m_pCamera;
	float									m_fLastTimeStamp ;
	float									m_fLastAnimationTimeStamp;
	float									m_fLastFrameTime ;
	WaterInterface*							m_pWaterInterface;

public:
											StableFluids(Ogre::SceneManager *sceneMgr, Ogre::Camera *camera) ;
    
    virtual									~StableFluids ();
	void									init();
	virtual void							release();
	virtual void							setupControls(OgreBites::SdkTrayManager* sdkTray);
	virtual void							sliderMoved(OgreBites::Slider* slider);
	virtual void							checkBoxToggled(OgreBites::CheckBox* checkBox);
	virtual void							itemSelected(OgreBites::SelectMenu* menu);
	virtual void							update(float timePass);
};

#endif //STABLE_FLUIDS_MESH_H_
