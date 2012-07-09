#ifndef _OGRE_H_
#define _OGRE_H_

#include "SdkSample.h"
#include "SamplePlugin.h"


class WaterSimulation;

class _OgreSampleClassExport OgreSample : public OgreBites::SdkSample
{
public:
    OgreSample();
    bool frameRenderingQueued(const Ogre::FrameEvent& evt);
    
protected:
	WaterSimulation*							m_pSim;
	//Ogre::SceneNode*							m_pHeadNode ;
	//Ogre::AnimationState*						m_pAnimState;
	//Ogre::ParticleSystem*						m_pParticleSystem ;
	//Ogre::ParticleEmitter*						m_pParticleEmitter ;

	//WaterMesh*								m_pWaterMesh;
	//Ogre::Entity*								m_pWaterEntity ;

	//Ogre::Real timeoutDelay ;
	//Ogre::Real headDepth ;
	//typedef vector<WaterCircle*>::type WaterCircles ;
	//WaterCircles circles ;

protected:
	// Just override the mandatory create scene method
    virtual void setupContent(void);
	virtual void setupControls();
	virtual void cleanupContent();
	virtual void sliderMoved(OgreBites::Slider* slider);
	virtual void checkBoxToggled(OgreBites::CheckBox* checkBox);
	virtual void itemSelected(OgreBites::SelectMenu* menu);

	///** Head animation */
	//void animateHead(Ogre::Real timeSinceLastFrame);
	//void prepareCircleMaterial();
	//void processCircles(Ogre::Real timeSinceLastFrame);
	//void processParticles();
};
#endif