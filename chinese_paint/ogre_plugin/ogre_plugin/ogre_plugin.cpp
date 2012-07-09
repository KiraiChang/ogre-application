#include "ogre_plugin.h"
#include "OgreBillboardParticleRenderer.h"
#include "../../water_simulation/water_simulation/water_simulation.h"

#define MATERIAL_PREFIX "Examples/Water"
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
	m_pSim = new WaterSimulation(mSceneMgr, mCamera);
	m_pSim->init();

	setupControls();

	setDragLook(true);
}


void OgreSample::setupControls()
{
	mTrayMgr->createLabel(OgreBites::TL_TOPLEFT, "GeneralLabel", "General", PANEL_WIDTH);
	mTrayMgr->createCheckBox(OgreBites::TL_TOPLEFT, "FakeNormalsCB", "Fake normals", PANEL_WIDTH);
	mTrayMgr->createCheckBox(OgreBites::TL_TOPLEFT, "SkyboxCB", "Skybox", PANEL_WIDTH);
	mTrayMgr->createThickSlider(OgreBites::TL_TOPLEFT, "HeadDepthSlider", "Head Depth", PANEL_WIDTH, 80, 1, 3, 50)->setValue(2.0f);
	OgreBites::SelectMenu* waterMaterial = mTrayMgr->createThickSelectMenu(OgreBites::TL_TOPLEFT, "WaterMaterialMenu", "Water material", PANEL_WIDTH, 9);
	for (size_t i = 0; i < 9; i++)
	{
		waterMaterial->addItem(MATERIAL_PREFIX + Ogre::StringConverter::toString(i));
	}
	waterMaterial->selectItem(8);
	mTrayMgr->createLabel(OgreBites::TL_TOPLEFT, "RainLabel", "Rain : [Space]", PANEL_WIDTH);

	mTrayMgr->createLabel(OgreBites::TL_TOPRIGHT, "AdvancedLabel", "Advanced", PANEL_WIDTH);
	mTrayMgr->createThickSlider(OgreBites::TL_TOPRIGHT, "RippleSpeedSlider", "Ripple Speed", PANEL_WIDTH, 80, 0, 2, 50)->setValue(0.3, false);
	mTrayMgr->createThickSlider(OgreBites::TL_TOPRIGHT, "DistanceSlider", "Distance", PANEL_WIDTH, 80, 0.1, 5.0, 50)->setValue(0.4, false);
	mTrayMgr->createThickSlider(OgreBites::TL_TOPRIGHT, "ViscositySlider", "Viscosity", PANEL_WIDTH, 80, 0, 1, 50)->setValue(0.05, false);
	mTrayMgr->createThickSlider(OgreBites::TL_TOPRIGHT, "FrameTimeSlider", "FrameTime", PANEL_WIDTH, 80, 0, 1, 61)->setValue(0.13, false);

	mTrayMgr->showCursor();
}

void OgreSample::cleanupContent()
{
	if(m_pSim != NULL)
	{
		delete m_pSim;
		m_pSim = NULL;
	}
}

void OgreSample::sliderMoved(OgreBites::Slider* slider)
{
	if (slider->getName() == "HeadDepthSlider")
	{
		//headDepth = slider->getValue();
	}
	else if (slider->getName() == "RippleSpeedSlider")
	{
		//waterMesh->PARAM_C = slider->getValue();
	}
	else if (slider->getName() == "DistanceSlider")
	{
		//waterMesh->PARAM_D = slider->getValue();
	}
	else if (slider->getName() == "ViscositySlider")
	{
		//waterMesh->PARAM_U = slider->getValue();
	}
	else if (slider->getName() == "FrameTimeSlider")
	{
		//waterMesh->PARAM_T = slider->getValue();
	}
}

void OgreSample::checkBoxToggled(OgreBites::CheckBox* checkBox)
{
	if (checkBox->getName() == "FakeNormalsCB")
	{
		//waterMesh->useFakeNormals = checkBox->isChecked();
	}
	else if (checkBox->getName() == "SkyboxCB")
	{
		//sceneMgr->setSkyBox(checkBox->isChecked(), "Examples/SceneSkyBox2");
	}
}

void OgreSample::itemSelected(OgreBites::SelectMenu* menu)
{
	//Only one menu in this demo
	const Ogre::String& materialName = menu->getSelectedItem();
	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName(materialName);
	if (material.isNull())
	{
		OGRE_EXCEPT(Ogre::Exception::ERR_INTERNAL_ERROR,
			"Material "+materialName+"doesn't exist!",
			"WaterListener::updateMaterial");
	}
	//waterEntity->setMaterialName(materialName);
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
