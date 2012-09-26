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
	//m_pFish(NULL),
	//m_pFishNode(NULL),
	m_eMoveType(MOVE_AUTO),
	m_eCurrentMesh(MESH_FISH),
	m_pSwimState(NULL),
	m_pCameraNode(NULL),
	m_queryPlane(Ogre::Vector3::UNIT_Y, 0),
	m_bSelectMesh(false),
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

	Ogre::Entity *ent;
	Ogre::SceneNode *node;
	{
		node = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
		ent = m_pSceneMgr->createEntity("Fish", "fish.mesh");
		node->attachObject(ent);
		node->setPosition(COMPLEXITY/2, 0.0, COMPLEXITY/2);
		node->yaw(Ogre::Degree(-90));
		node->setScale(FISH_SCALE_SIZE, FISH_SCALE_SIZE, FISH_SCALE_SIZE);
		node->setVisible(false);

		m_pSwimState = ent->getAnimationState("swim");
		m_pSwimState->setEnabled(true);
		m_pSwimState->setLoop(true);
		m_vpEntity.push_back(ent);
		m_vpNode.push_back(node);
	}
	{
		node = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
		ent = m_pSceneMgr->createEntity("Sphere", "sphere.mesh");
		node->attachObject(ent);
		node->setPosition(COMPLEXITY/2, 0.0, COMPLEXITY/2);
		node->setScale(FISH_SCALE_SIZE, FISH_SCALE_SIZE, FISH_SCALE_SIZE);
		node->setVisible(false);
		m_vpEntity.push_back(ent);
		m_vpNode.push_back(node);
	}
	{
		node = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
		ent = m_pSceneMgr->createEntity("Cube", "cube.mesh");
		node->attachObject(ent);
		node->setPosition(COMPLEXITY/2, 0.0, COMPLEXITY/2);
		node->setScale(FISH_SCALE_SIZE, FISH_SCALE_SIZE, FISH_SCALE_SIZE);
		node->setVisible(false);
		m_vpEntity.push_back(ent);
		m_vpNode.push_back(node);
	}
	{
		node = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
		ent = m_pSceneMgr->createEntity("Triangle", "triangle.mesh");
		node->attachObject(ent);
		node->setPosition(COMPLEXITY/2, 0.0, COMPLEXITY/2);
		node->setScale(FISH_SCALE_SIZE, FISH_SCALE_SIZE, FISH_SCALE_SIZE);
		node->setVisible(false);
		m_vpEntity.push_back(ent);
		m_vpNode.push_back(node);
	}
	m_vpNode[m_eCurrentMesh]->setVisible(true);

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

	V_ENTITY::iterator eIte;
	V_NODE::iterator nIte;
	Ogre::Entity *ent;
	Ogre::SceneNode *node;
	if(m_pSceneMgr!= NULL)
	{
		for(eIte = m_vpEntity.begin(), nIte = m_vpNode.begin();eIte != m_vpEntity.end();eIte++, nIte++)
		{
			ent = *eIte;node = *nIte;
			if(node != NULL)
			{
				node->detachObject(ent);
				m_pSceneMgr->destroySceneNode(node);
				node = NULL;
			}
			if(ent != NULL)
			{
				m_pSceneMgr->destroyEntity( ent );
				ent = NULL;
			}
		}
		m_vpEntity.clear();
		m_vpNode.clear();
	}
}

void StableFluids::setupControls(OgreBites::SdkTrayManager* mTrayMgr)
{
	OgreBites::SelectMenu* menu;
	OgreBites::CheckBox* check;

	menu = mTrayMgr->createThickSelectMenu(OgreBites::TL_TOPLEFT, "MeshMenu", "Mesh Type", PANEL_WIDTH, 9);
	menu->addItem("MESH_FISH");
	menu->addItem("MESH_SPHERE");
	menu->addItem("MESH_CUBE");
	menu->addItem("MESH_TRIANGLE");

	menu = mTrayMgr->createThickSelectMenu(OgreBites::TL_TOPLEFT, "MoveMenu", "Move Type", PANEL_WIDTH, 9);
	menu->addItem("MOVE_AUTO");
	menu->addItem("MOVE_CUSTOM");

	menu = mTrayMgr->createThickSelectMenu(OgreBites::TL_TOPLEFT, "VelocityMenu", "Velocity Field Display", PANEL_WIDTH, 9);
	menu->addItem("DISPLAY_ORIGIN");
	menu->addItem("DISPLAY_VELOCITY_NONE");
	menu->addItem("DISPLAY_ADD_FORCE");
	menu->addItem("DISPLAY_BOUNDARY");

	menu = mTrayMgr->createThickSelectMenu(OgreBites::TL_TOPLEFT, "DensityMenu", "Density Field Display", PANEL_WIDTH, 9);
	menu->addItem("DISPLAY_MAP_NONE");
	menu->addItem("DISPLAY_DENSITY_MAP");
	menu->addItem("DISPLAY_BOOLEAN_GRID");
	menu->selectItem("DISPLAY_DENSITY_MAP");

	check = mTrayMgr->createCheckBox(OgreBites::TL_TOPLEFT, "ExternForceCB", "Extern Force", PANEL_WIDTH);
}

void StableFluids::sliderMoved(OgreBites::Slider* slider)
{

}

void StableFluids::checkBoxToggled(OgreBites::CheckBox* checkBox)
{
	StableFluidsGrid *fg = (StableFluidsGrid *)m_pWaterInterface;
	if(checkBox->getName() == "ExternForceCB")
	{
		fg->m_bExternlForce = checkBox->isChecked();
	}
}

void StableFluids::itemSelected(OgreBites::SelectMenu* menu)
{
	if(m_pWaterInterface == NULL)
		return;

	StableFluidsGrid *fg = (StableFluidsGrid *)m_pWaterInterface;
	if(menu->getName() == "MeshMenu")
	{
		const Ogre::String& materialName = menu->getSelectedItem();
		m_vpNode[m_eCurrentMesh]->setVisible(false);
		if(materialName == "MESH_FISH")
			m_eCurrentMesh = MESH_FISH;
		else if(materialName == "MESH_SPHERE")
			m_eCurrentMesh = MESH_SPHERE;
		else if(materialName == "MESH_CUBE")
			m_eCurrentMesh = MESH_CUBE;
		else if(materialName == "MESH_TRIANGLE")
			m_eCurrentMesh = MESH_TRIANGLE;
		m_vpNode[m_eCurrentMesh]->setVisible(true);
	}
	else if(menu->getName() == "MoveMenu")
	{
		const Ogre::String& materialName = menu->getSelectedItem();
		if(materialName == "MOVE_AUTO")
			m_eMoveType = MOVE_AUTO;
		else if(materialName == "MOVE_CUSTOM")
			m_eMoveType = MOVE_CUSTOM;
	}
	else if(menu->getName() == "VelocityMenu")
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

bool StableFluids::mouseMoved(const OIS::MouseEvent& evt)
{
	if(m_bSelectMesh)
	{
		Ogre::Real screenWidth = Ogre::Root::getSingleton().getAutoCreatedWindow()->getWidth();
		Ogre::Real screenHeight = Ogre::Root::getSingleton().getAutoCreatedWindow()->getHeight();

		// convert to 0-1 offset
		Ogre::Real offsetX = evt.state.X.abs / screenWidth;
		Ogre::Real offsetY = evt.state.Y.abs / screenHeight;

		// set up the ray
		Ogre::Ray mouseRay = m_pCamera->getCameraToViewportRay(offsetX, offsetY);

		// check if the ray intersects our plane
		// intersects() will return whether it intersects or not (the bool value) and
		// what distance (the Real value) along the ray the intersection is
		std::pair<bool, Ogre::Real> result = mouseRay.intersects(m_queryPlane);

		if(result.first) {
			// if the ray intersect the plane, we have a distance value
			// telling us how far from the ray origin the intersection occurred.
			// the last thing we need is the point of the intersection.
			// Ray provides us getPoint() function which returns a point
			// along the ray, supplying it with a distance value.

			// get the point where the intersection is
			Ogre::Vector3 point = mouseRay.getPoint(result.second);
			point.y = 0;

			// position our ninja to that point  
			m_vpNode[m_eCurrentMesh]->setPosition(point);
		}
	}
    return true;
}

bool StableFluids::mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
	switch(id) 
	{
	case OIS::MB_Left:
		if(m_eMoveType == MOVE_CUSTOM)
		{
			Ogre::Real screenWidth = Ogre::Root::getSingleton().getAutoCreatedWindow()->getWidth();
			Ogre::Real screenHeight = Ogre::Root::getSingleton().getAutoCreatedWindow()->getHeight();

			// convert to 0-1 offset
			Ogre::Real offsetX = evt.state.X.abs / screenWidth;
			Ogre::Real offsetY = evt.state.Y.abs / screenHeight;

			// set up the ray
			Ogre::Ray mouseRay = m_pCamera->getCameraToViewportRay(offsetX, offsetY);
			std::pair<bool, Ogre::Real> result = mouseRay.intersects(m_vpNode[m_eCurrentMesh]->_getWorldAABB());
			if(result.first)
				m_bSelectMesh = true;
		}
		break;
	default:
		break;
	}
	return true;
}

bool StableFluids::mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
	switch(id) 
	{
	case OIS::MB_Left:
		if(m_eMoveType == MOVE_CUSTOM)
			m_bSelectMesh = false;
		break;
	default:
		break;
	}
	return true;
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

	if(m_eMoveType == MOVE_AUTO)
	{
		m_fCurrentTime += timeSinceLastFrame;
		if(m_fCurrentTime >= ONE_CIRCLE_NEED_TIME)
			m_fCurrentTime = 0;
		Ogre::Vector3 pos, prev;
		float T = m_fCurrentTime / ONE_CIRCLE_NEED_TIME * 2 * PI;
		pos.x = cos(T) * (COMPLEXITY/3) + (COMPLEXITY/2);
		pos.y = 0.0;
		pos.z = sin(T) * (COMPLEXITY/3) + (COMPLEXITY/2);

		if(m_eCurrentMesh < m_vpNode.size())
		{
			Ogre::Vector3 dir = pos - m_vpNode[m_eCurrentMesh]->getPosition();
			m_vpNode[m_eCurrentMesh]->setPosition(pos);

			if(m_eCurrentMesh == MESH_FISH)
			{
				Ogre::Vector3 src = m_vpNode[m_eCurrentMesh]->getOrientation() * Ogre::Vector3::NEGATIVE_UNIT_X;/*Ogre::Vector3::UNIT_X;*/
				Ogre::Quaternion quat = src.getRotationTo(dir);
				m_vpNode[m_eCurrentMesh]->rotate(quat);
			}

			//m_pCameraNode->setPosition(pos.x, 32, pos.z);
		}
	}
	((StableFluidsGrid *)m_pWaterInterface)->updateMeshData(m_vpNode[m_eCurrentMesh], m_vpEntity[m_eCurrentMesh]);
	m_pWaterInterface->updateMesh(timeSinceLastFrame);
}




