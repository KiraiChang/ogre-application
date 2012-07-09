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

protected:
	// Just override the mandatory create scene method
    virtual void setupContent(void);
	virtual void setupControls();
	virtual void cleanupContent();
	virtual void sliderMoved(OgreBites::Slider* slider);
	virtual void checkBoxToggled(OgreBites::CheckBox* checkBox);
	virtual void itemSelected(OgreBites::SelectMenu* menu);
};
#endif