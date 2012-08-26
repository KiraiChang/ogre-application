#include "stable_fluids.h"
#include "stable_fluids_grid.h"

#define ANIMATIONS_PER_SECOND 100.0f
#define IX(i,j,N) ((i)+(N+2)*(j))

#define COMPLEXITY 64

StableFluids::StableFluids(Ogre::SceneManager *sceneMgr, Ogre::Camera *camera):
	m_pSceneMgr(sceneMgr), 
	m_pCamera(camera)
{
}
    
StableFluids::~StableFluids ()
{
	release();
}

void StableFluids::init()
{
	m_pWaterInterface = new StableFluidsGrid(COMPLEXITY);
	((StableFluidsGrid *)m_pWaterInterface)->init(m_pSceneMgr);

	m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.75, 0.75, 0.75));

	Ogre::Light* l = m_pSceneMgr->createLight("MainLight");
	l->setPosition(200,300,100);

	Ogre::SceneNode* camNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
	camNode->translate(32, 64, 32);
	camNode->pitch(Ogre::Degree(-90));
	camNode->attachObject(m_pCamera);
}

void StableFluids::release()
{
	if(m_pWaterInterface != NULL)
	{
		((StableFluidsGrid *)m_pWaterInterface)->release();
		delete m_pWaterInterface;
	}
}

void StableFluids::setupControls(OgreBites::SdkTrayManager* sdkTray)
{

}

void StableFluids::sliderMoved(OgreBites::Slider* slider)
{

}

void StableFluids::checkBoxToggled(OgreBites::CheckBox* checkBox)
{

}

void StableFluids::itemSelected(OgreBites::SelectMenu* menu)
{

}

void StableFluids::update(float timeSinceLastFrame)
{
	m_fLastFrameTime = timeSinceLastFrame ;
	m_fLastTimeStamp += timeSinceLastFrame ;
	static int I = 0;
	I++;
	if(I == 100)
		((StableFluidsGrid *)m_pWaterInterface)->push(32, 32, 5);
	m_pWaterInterface->updateMesh(timeSinceLastFrame);
}


