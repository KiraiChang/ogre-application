#include "ogre_plugin.h"
#include "OgreBillboardParticleRenderer.h"
#include "../water_simulation/water_simulation.h"
#include "../vtfw_simulation/vtfw_simulation.h"

#define PANEL_WIDTH 200


OgreSample::OgreSample() : m_pSim(0)
{
	mInfo["Title"] = "ChinesePaint";
	mInfo["Description"] = "A demo of a simple water effect and transfer to chinese paint.";
	mInfo["Thumbnail"] = "thumb_water.png";
	mInfo["Category"] = "Environment";
}
    

// Just override the mandatory create scene method
void OgreSample::setupContent(void)
{
	m_pSim = new VTFWSimulation(mSceneMgr, mCamera, mWindow, mViewport);
	m_pSim->init();

	setupControls();

	setDragLook(true);
}


void OgreSample::setupControls()
{
	if(m_pSim != NULL)
	{
		m_pSim->setupControls(mTrayMgr);
	}
}

void OgreSample::cleanupContent()
{
	if(m_pSim != NULL)
	{
		m_pSim->release();
		delete m_pSim;
		m_pSim = NULL;
	}
}

void OgreSample::sliderMoved(OgreBites::Slider* slider)
{
	if(m_pSim != NULL)
	{
		m_pSim->sliderMoved(slider);
	}
}

void OgreSample::checkBoxToggled(OgreBites::CheckBox* checkBox)
{
	if(m_pSim != NULL)
	{
		m_pSim->checkBoxToggled(checkBox);
	}
}

void OgreSample::itemSelected(OgreBites::SelectMenu* menu)
{
	if(m_pSim != NULL)
	{
		m_pSim->itemSelected(menu);
	}
}

bool OgreSample::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if( SdkSample::frameRenderingQueued(evt) == false )
	{
		// check if we are exiting, if so, clear static HardwareBuffers to avoid segfault
		//WaterCircle::clearStaticBuffers();
		return false;
	}

#if OGRE_PLATFORM != OGRE_PLATFORM_IPHONE
	if (mKeyboard->isKeyDown(OIS::KC_SPACE)) 
	{
		//particleEmitter->setEmissionRate(20.0f);
	} 
	else 
	{
		//particleEmitter->setEmissionRate(0.0f);
	}
#endif

	if(m_pSim != NULL)
		m_pSim->update(evt.timeSinceLastFrame);

	return true;
}
