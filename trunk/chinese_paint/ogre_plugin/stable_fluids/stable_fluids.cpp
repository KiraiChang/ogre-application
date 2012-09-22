#include "stable_fluids.h"
#include "stable_fluids_grid.h"
#include <SdkSample.h>
#include <math.h>
#define PANEL_WIDTH 200

#define ANIMATIONS_PER_SECOND 100.0f
#define IX(i,j,N) ((i)+(N+2)*(j))

#define COMPLEXITY 128
#define PI 3.141596
#define ONE_CIRCLE_NEED_TIME 75

StableFluids::StableFluids(Ogre::SceneManager *sceneMgr, Ogre::Camera *camera):
	m_pSceneMgr(sceneMgr), 
	m_pFish(NULL),
	m_pFishNode(NULL),
	m_pSwimState(NULL),
	m_pCameraNode(NULL),
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
	l->setPosition(COMPLEXITY/2, COMPLEXITY/2, COMPLEXITY/2);

	m_pCameraNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
	m_pCameraNode->translate(COMPLEXITY/2, COMPLEXITY, COMPLEXITY/2);
	m_pCameraNode->pitch(Ogre::Degree(-90));
	m_pCameraNode->attachObject(m_pCamera);

	m_pFishNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
	m_pFish = m_pSceneMgr->createEntity("FishEntity", "fish.mesh");
	m_pFishNode->attachObject(m_pFish);
	m_pFishNode->setPosition(COMPLEXITY/2, 0.0, COMPLEXITY/2);
	m_pFishNode->yaw(Ogre::Degree(-90));
	m_pFishNode->setScale(FISH_SCALE_SIZE, FISH_SCALE_SIZE, FISH_SCALE_SIZE);
	
	m_pSwimState = m_pFish->getAnimationState("swim");
	m_pSwimState->setEnabled(true);
	m_pSwimState->setLoop(true);

	//m_uiCurrentTarget = 0;
	//m_vTarget.push_back(Ogre::Vector3(8.0, 0.0, 8.0));
	//m_vTarget.push_back(Ogre::Vector3(8.0, 0.0, 56.0));
	//m_vTarget.push_back(Ogre::Vector3(56.0, 0.0, 56.0));
	//m_vTarget.push_back(Ogre::Vector3(56.0, 0.0, 8.0));
	//m_vec3Pos = m_vTarget[m_uiCurrentTarget];
	m_vec3Pos.x = 0;
	m_vec3Pos.y = 0;
	m_vec3Pos.z = 0;

	m_fCurrentTime = 0.0;
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

void StableFluids::setupControls(OgreBites::SdkTrayManager* mTrayMgr)
{
	OgreBites::SelectMenu* waterMaterial = mTrayMgr->createThickSelectMenu(OgreBites::TL_TOPLEFT, "VelocityMenu", "Velocity Field Display", PANEL_WIDTH, 9);
	waterMaterial->addItem("DISPLAY_ORIGIN");
	waterMaterial->addItem("DISPLAY_VELOCITY_NONE");
	waterMaterial->addItem("DISPLAY_ADD_FORCE");
	waterMaterial->addItem("DISPLAY_BOUNDARY");

	waterMaterial = mTrayMgr->createThickSelectMenu(OgreBites::TL_TOPLEFT, "DensityMenu", "Density Field Display", PANEL_WIDTH, 9);
	waterMaterial->addItem("DISPLAY_MAP_NONE");
	waterMaterial->addItem("DISPLAY_DENSITY_MAP");
	waterMaterial->addItem("DISPLAY_BOOLEAN_GRID");
	waterMaterial->selectItem("DISPLAY_DENSITY_MAP");
}

void StableFluids::sliderMoved(OgreBites::Slider* slider)
{

}

void StableFluids::checkBoxToggled(OgreBites::CheckBox* checkBox)
{

}

void StableFluids::itemSelected(OgreBites::SelectMenu* menu)
{
	if(m_pWaterInterface == NULL)
		return;

	StableFluidsGrid *fg = (StableFluidsGrid *)m_pWaterInterface;
	if(menu->getName() == "VelocityMenu")
	{
		const Ogre::String& materialName = menu->getSelectedItem();
		if(materialName == "DISPLAY_VELOCITY_NONE")
			fg->m_eVelocityType = StableFluidsGrid::DISPLAY_VELOCITY_NONE;
		else if(materialName == "DISPLAY_ORIGIN")
			fg->m_eVelocityType = StableFluidsGrid::DISPLAY_ORIGIN;
		else if(materialName == "DISPLAY_ADD_FORCE")
			fg->m_eVelocityType = StableFluidsGrid::DISPLAY_ADD_FORCE;
		else if(materialName == "DISPLAY_BOUNDARY")
			fg->m_eVelocityType = StableFluidsGrid::DISPLAY_BOUNDARY;
	}
	else if(menu->getName() == "DensityMenu")
	{
		const Ogre::String& materialName = menu->getSelectedItem();
		if(materialName == "DISPLAY_MAP_NONE")
			fg->m_eMapDisplayType = StableFluidsGrid::DISPLAY_MAP_NONE;
		else if(materialName == "DISPLAY_BOOLEAN_GRID")
			fg->m_eMapDisplayType = StableFluidsGrid::DISPLAY_BOOLEAN_GRID;
		else if(materialName == "DISPLAY_DENSITY_MAP")
			fg->m_eMapDisplayType = StableFluidsGrid::DISPLAY_DENSITY_MAP;

	}
}

void StableFluids::update(float timeSinceLastFrame)
{
	//static int I = 0;
	//I++;
	//static int J = 0;
	//J++;
	//if(J < 40)
		m_pSwimState->addTime(timeSinceLastFrame);//mesh animation
	//if(I % 50 == 0)
		//((StableFluidsGrid *)m_pWaterInterface)->push(32, 25, 1);

	//if(I == 50)
	//	((StableFluidsGrid *)m_pWaterInterface)->push(32, 42, 1);//add a force up

	//if(m_pFishNode->getPosition().distance(m_vec3Pos) >= 5.0)//mesh move around
	//{
	//	Ogre::Vector3 dir = m_vec3Pos - m_pFishNode->getPosition();
	//	m_pFishNode->translate(dir * timeSinceLastFrame * 0.1);

	//	Ogre::Vector3 src = m_pFishNode->getOrientation() * Ogre::Vector3::NEGATIVE_UNIT_X;/*Ogre::Vector3::UNIT_X;*/
	//	Ogre::Quaternion quat = src.getRotationTo(dir);
	//	m_pFishNode->rotate(quat);
	//}
	//else
	//{
	//	m_uiCurrentTarget++;
	//	if(m_uiCurrentTarget >= m_vTarget.size())
	//		m_uiCurrentTarget = 0;
	//	m_vec3Pos = m_vTarget[m_uiCurrentTarget];
	//}

	m_fCurrentTime += timeSinceLastFrame;
	if(m_fCurrentTime >= ONE_CIRCLE_NEED_TIME)
		m_fCurrentTime = 0;
	Ogre::Vector3 pos, prev;
	float T = m_fCurrentTime / ONE_CIRCLE_NEED_TIME * 2 * PI;
	pos.x = cos(T) * (COMPLEXITY/3) + (COMPLEXITY/2);
	pos.y = 0.0;
	pos.z = sin(T) * (COMPLEXITY/3) + (COMPLEXITY/2);

	Ogre::Vector3 dir = pos - m_pFishNode->getPosition();
	m_pFishNode->setPosition(pos);

	Ogre::Vector3 src = m_pFishNode->getOrientation() * Ogre::Vector3::NEGATIVE_UNIT_X;/*Ogre::Vector3::UNIT_X;*/
	Ogre::Quaternion quat = src.getRotationTo(dir);
	m_pFishNode->rotate(quat);

	//m_pCameraNode->setPosition(pos.x, 32, pos.z);


	((StableFluidsGrid *)m_pWaterInterface)->updateMeshData(m_pFishNode, m_pFish);
	m_pWaterInterface->updateMesh(timeSinceLastFrame);
}




