#include "stable_fluids.h"
#include "stable_fluids_grid.h"


#define ANIMATIONS_PER_SECOND 100.0f
#define IX(i,j,N) ((i)+(N+2)*(j))

#define COMPLEXITY 64

StableFluids::StableFluids(Ogre::SceneManager *sceneMgr, Ogre::Camera *camera):
	m_pSceneMgr(sceneMgr), 
	m_pFish(NULL),
	m_pFishNode(NULL),
	m_pSwimState(NULL),
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

	m_pFishNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
	m_pFish = m_pSceneMgr->createEntity("FishEntity", "fish.mesh");
	m_pFishNode->attachObject(m_pFish);
	m_pFishNode->setPosition(32.0, 0.0, 32.0);
	m_pFishNode->yaw(Ogre::Degree(-90));
	m_pSwimState = m_pFish->getAnimationState("swim");
	m_pSwimState->setEnabled(true);
	m_pSwimState->setLoop(true);
}

void StableFluids::release()
{
	if(m_pWaterInterface != NULL)
	{
		((StableFluidsGrid *)m_pWaterInterface)->release();
		delete m_pWaterInterface;
	}

	if(m_pSceneMgr!= NULL)
	{
		if(m_pFishNode != NULL)
		{
			m_pFishNode->detachObject(m_pFish);
			m_pSceneMgr->destroySceneNode(m_pFishNode);
			m_pFishNode = NULL;
		}
		if(m_pFish != NULL)
		{
			m_pSceneMgr->destroyEntity( m_pFish );
			m_pFish = NULL;
		}
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
	m_pSwimState->addTime(timeSinceLastFrame);
	static int I = 0;
	I++;
	//if(I % 50 == 0)
	//	((StableFluidsGrid *)m_pWaterInterface)->push(32, 48, 1);

	if(I == 50)
		((StableFluidsGrid *)m_pWaterInterface)->push(32, 36, 1);
	//else if(I == 150)
	//	((StableFluidsGrid *)m_pWaterInterface)->push(16, 32, 1);
	//else if(I == 200)
	//	((StableFluidsGrid *)m_pWaterInterface)->push(32, 16, 1);
	//else if(I == 250)
	//	((StableFluidsGrid *)m_pWaterInterface)->push(32, 48, 1);
	//else if(I == 300)
	//	((StableFluidsGrid *)m_pWaterInterface)->push(48, 32, 1);

	((StableFluidsGrid *)m_pWaterInterface)->updateMeshData(m_pFishNode, m_pFish);
	m_pWaterInterface->updateMesh(timeSinceLastFrame);
}




