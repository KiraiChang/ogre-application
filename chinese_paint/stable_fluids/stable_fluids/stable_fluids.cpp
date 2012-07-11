#include "stable_fluids.h"
#include "stable_fluids_grid.h"
#include <Ogre.h>



StableFluids::StableFluids(Ogre::SceneManager *sceneMgr, Ogre::Camera* cam):m_pSceneMgr(sceneMgr),
																m_pCamera(cam),
																m_pFluidGrid(NULL)
																
{


}

StableFluids::~StableFluids(void)
{
	release();
}

void StableFluids::init(void)
{
	// Set ambient light
	m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.75, 0.75, 0.75));

	// Create a light
	Ogre::Light* l = m_pSceneMgr->createLight("MainLight");

	l->setPosition(200,300,100);

	m_pFluidGrid = new StableFluidsGrid(64);
	m_pFluidGrid->init();
}

void StableFluids::release(void)
{
	if(m_pSceneMgr != NULL)
	{

	}

	if(m_pFluidGrid != NULL)
	{
		delete m_pFluidGrid;
		m_pFluidGrid = NULL;
	}
}

void StableFluids::update(float timePass)
{
	if(m_pFluidGrid != NULL)
	{
		m_pFluidGrid->update(timePass);
	}
}