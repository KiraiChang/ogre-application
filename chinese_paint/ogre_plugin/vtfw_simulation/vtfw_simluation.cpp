#include "vtfw_simulation.h"
#include <Ogre.h>
#include <SdkSample.h>
#include "vtfw_mesh.h"
#include "vtfw_render_listener.h"

#define MESH_NAME "WaterMesh"
#define ENTITY_NAME "WaterEntity"
#define MATERIAL_NAME "ChinesePaint/Water"
#define COMPLEXITY 128 		// watch out - number of polys is 2*ACCURACY*ACCURACY !
#define PLANE_SIZE 256.0f

//#define PANEL_WIDTH 200
//#define RAIN_HEIGHT_RANDOM 5
//#define RAIN_HEIGHT_CONSTANT 5

VTFWSimulation::VTFWSimulation(Ogre::SceneManager *sceneMgr, Ogre::Camera* cam, Ogre::RenderWindow *win, Ogre::Viewport *v):m_pSceneMgr(sceneMgr),
 						m_pCamera(cam),
						m_pWindow(win),
						m_pViewport(v),
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
	m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.75, 0.75, 0.75));

	Ogre::Light* l = m_pSceneMgr->createLight("MainLight");
	l->setPosition(200,300,100);

	//m_pRenderListener = new VTFWRenderListener("heightSampler", m_pCamera, COMPLEXITY);
	//m_pSceneMgr->addRenderQueueListener(m_pRenderListener);
	//m_pSceneMgr->addSpecialCaseRenderQueue(m_pRenderListener->getQueueID());

	m_pWaterInterface = new VTFWMesh(MESH_NAME, PLANE_SIZE, COMPLEXITY, m_pWindow);
	m_pWaterEntity = m_pSceneMgr->createEntity(ENTITY_NAME, MESH_NAME);
	m_pWaterEntity->setMaterialName("ChinesePaint/Texture");
	Ogre::SceneNode *waterNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
	waterNode->attachObject(m_pWaterEntity);

	//m_pHeadNode = waterNode->createChildSceneNode();
	//Ogre::Entity *ent = m_pSceneMgr->createEntity("head", "ogrehead.mesh");
	//m_pHeadNode->attachObject(ent);

	Ogre::SceneNode* camNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
	camNode->translate(128, 256, 128);
	camNode->pitch(Ogre::Degree(-90));
	camNode->attachObject(m_pCamera);

	Ogre::SceneNode* lightNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
	lightNode->attachObject(l);
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
	if(m_fMoveSpeed > 0)
	{
		float t = timePass * m_fMoveSpeed;
		static int i = 0;
		if(i == 100)
		{
			((VTFWMesh *)m_pWaterInterface)->push(64, 64, -0.01);
		}
		i++;
		m_fTimeoutDelay-=t ;
		if (m_fTimeoutDelay<=0)
			m_fTimeoutDelay = 0;

		m_pWaterInterface->updateMesh(t);
	}
}
