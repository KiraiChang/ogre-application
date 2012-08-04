#include "vtfw_simulation.h"
#include <Ogre.h>
#include <SdkSample.h>
#include "vtfw_mesh.h"

VTFWSimulation::VTFWSimulation(Ogre::SceneManager *sceneMgr, Ogre::Camera* cam):m_pSceneMgr(sceneMgr),
 						m_pCamera(cam),
 						m_fTimeoutDelay(0.0f),
 						m_fHeadDepth(1.0f),
 						m_fMoveSpeed(1.0f)
 						
{
}

VTFWSimulation::~VTFWSimulation(void)
{
	release();
}

void VTFWSimulation::init(void)
{
}

void VTFWSimulation::release(void)
{
	if(m_pSceneMgr != NULL)
	{

	}
}

void VTFWSimulation::setupControls(OgreBites::SdkTrayManager* sdkTray)
{
}

void VTFWSimulation::sliderMoved(OgreBites::Slider* slider)
{
}

void VTFWSimulation::checkBoxToggled(OgreBites::CheckBox* checkBox)
{
}

void VTFWSimulation::itemSelected(OgreBites::SelectMenu* menu)
{
}

void VTFWSimulation::update(float timePass)
{
}
