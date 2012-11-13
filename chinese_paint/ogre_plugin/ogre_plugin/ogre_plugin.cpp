#include "ogre_plugin.h"
#include "OgreBillboardParticleRenderer.h"
#include "../water_simulation/water_simulation.h"
#include "../vtfw_simulation/vtfw_simulation.h"
#include "../stable_fluids/stable_fluids.h"

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
	mTrayMgr->hideLogo();
	//m_pSim = new VTFWSimulation(mSceneMgr, mCamera, mWindow, mViewport);
	Ogre::SceneManager *mgr = mRoot->createSceneManager(Ogre::ST_GENERIC, mInfo["Title"]);
	Ogre::Camera* cam = mgr->createCamera(mInfo["Title"] + "Camera");
	mWindow->removeAllViewports();
	mWindow->addViewport(cam);
	m_pSim = new StableFluids(mgr, cam);
	//m_pSim = new WaterSimulation(mSceneMgr, mCamera);
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

	Ogre::SceneManager *mgr = mRoot->getSceneManager(mInfo["Title"]);
	if(mgr != NULL)
	{
		mgr->destroyCamera(mInfo["Title"] + "Camera");
		mRoot->destroySceneManager(mgr);
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

bool OgreSample::mouseMoved(const OIS::MouseEvent& evt)
{
	bool ret = true;//OgreBites::SdkSample::mouseMoved(evt);
	if (mTrayMgr->injectMouseMove(evt)) return true;

	mCameraMan->injectMouseMove(evt);
	if(m_pSim != NULL)
	{
		StableFluids *sf = (StableFluids*)m_pSim;
		sf->mouseMoved(evt);
	}
	return ret;
}

bool OgreSample::mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
	bool ret = true;//OgreBites::SdkSample::mousePressed(evt, id);
	if (mTrayMgr->injectMouseDown(evt, id)) return true;

	//if (mDragLook && id == OIS::MB_Left)
	//{
	//	mCameraMan->setStyle(OgreBites::CS_FREELOOK);
	//	mTrayMgr->hideCursor();
	//}

	mCameraMan->injectMouseDown(evt, id);
	if(m_pSim != NULL)
	{
		StableFluids *sf = (StableFluids*)m_pSim;
		sf->mousePressed(evt, id);
	}
	return ret;
}

bool OgreSample::mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
	bool ret = true;//OgreBites::SdkSample::mouseReleased(evt, id);
	if (mTrayMgr->injectMouseUp(evt, id)) return true;

	//if (mDragLook && id == OIS::MB_Left)
	//{
	//	mCameraMan->setStyle(OgreBites::CS_MANUAL);
	//	mTrayMgr->showCursor();
	//}

	mCameraMan->injectMouseUp(evt, id);
	if(m_pSim != NULL)
	{
		StableFluids *sf = (StableFluids*)m_pSim;
		sf->mouseReleased(evt, id);
	}
	return ret;
}
