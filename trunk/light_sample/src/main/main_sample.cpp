
#include "main_sample.h"
#include "light_sample.h"

//-------------------------------------------------------------------------------------
MainSample::MainSample(void)
{
}
//-------------------------------------------------------------------------------------
MainSample::~MainSample(void)
{
}

//-------------------------------------------------------------------------------------
void MainSample::createScene(void)
{
	/*****************************************************************
							User input begin
	*****************************************************************/
	Ogre::TexturePtr tex = Ogre::TextureManager::getSingleton().createManual("MirrorReflection",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, 512, 512, 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET);

	mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE);
	Ogre::Entity* ogreHead = mSceneMgr->createEntity("Head", "ogrehead.mesh");

	Ogre::SceneNode* headNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	headNode->attachObject(ogreHead);
	headNode->setScale(0.1, 0.1, 0.1);
	headNode->setPosition(0, 5, 0);

	// Set ambient light
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.1));

	// Create a light
	l1 = mSceneMgr->createLight("MainLight");
	l1->setPosition(2,10,2);
	l1->setDirection(0, -1, 0);
	l1->setType(Ogre::Light::LightTypes::LT_DIRECTIONAL);
	l1->setSpecularColour(0.9, 0.9, 0.9);

	l2 = mSceneMgr->createLight("PointL");
	l2->setPosition(-3,10,0);
	l2->setType(Ogre::Light::LightTypes::LT_POINT);
	l2->setVisible(false);
	l1->setSpecularColour(0.9, 0.9, 0.9);

	l3 = mSceneMgr->createLight("PointSP");
	l3->setPosition(5,10,0);
	l3->setDirection(0, -1, 0);
	l3->setSpotlightOuterAngle(Ogre::Radian(10));
	l3->setType(Ogre::Light::LightTypes::LT_SPOTLIGHT);
	l3->setVisible(false);

	MirrorPlane = Ogre::Plane(Ogre::Vector3::UNIT_Y, 0);
	Ogre::MeshManager::getSingleton().createPlane("floor", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		MirrorPlane, 100, 100, 10, 10, true, 1, 10, 10, Ogre::Vector3::UNIT_Z);
	mMirrorEnt = mSceneMgr->createEntity("Floor", "floor");
	mMirrorEnt->setMaterialName("MirrorPlane");
	mMirrorEnt->setCastShadows(false);
	mSceneMgr->getRootSceneNode()->attachObject(mMirrorEnt);


	// 	RenderTarget* rtt = tex->getBuffer()->getRenderTarget();
	// 	rtt->addViewport(mCamera)->setOverlaysEnabled(false);
	// 	rtt->addListener(this);
	// 
	// 	mMirrorTarget = rtt;

	Ogre::CompositorManager::getSingleton().addCompositor(mCamera->getViewport(), "Glow");
	Ogre::CompositorManager::getSingleton().setCompositorEnabled(mCamera->getViewport(), "Glow", true);
	GlowMaterialListener *gml = new GlowMaterialListener();
	Ogre::MaterialManager::getSingleton().addListener(gml);

	mGlowObjEnt = mSceneMgr->createEntity("GlowPG", "penguin.mesh");
	mGlowObjNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	mGlowObjEnt->setMaterialName("GlowMt");
	mGlowObjNode->attachObject(mGlowObjEnt);
	mGlowObjNode->setPosition(10,10,0);
	mGlowObjNode->setScale(0.2, 0.2, 0.2);
	Ogre::MaterialPtr mpt = Ogre::MaterialManager::getSingleton().getByName("GlowMt");
	fpPP = mpt->getTechnique(1)->getPass(0)->getFragmentProgramParameters();
	dtime = 0;
	/*****************************************************************
							User input end
	*****************************************************************/
}

bool MainSample::keyPressed( const OIS::KeyEvent &arg )
{
	if (arg.key == OIS::KC_1)
		mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));
	if (arg.key == OIS::KC_2)
		mSceneMgr->setAmbientLight(Ogre::ColourValue(0.0, 0.0, 0.0));

	if (arg.key == OIS::KC_NUMPAD0)
		mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE);
	if (arg.key == OIS::KC_NUMPAD1)
		mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
	if (arg.key == OIS::KC_NUMPAD2)
		mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);
	if (arg.key == OIS::KC_NUMPAD3)
		mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_MODULATIVE);

	/*****************************************************************
							User input begin
	*****************************************************************/
	if (arg.key == OIS::KC_3)
		l1->setVisible(!l1->getVisible());
	if (arg.key == OIS::KC_4)
		l2->setVisible(!l2->getVisible());
	if (arg.key == OIS::KC_5)
		l3->setVisible(!l3->getVisible());
	/*****************************************************************
							User input end
	*****************************************************************/

	return BaseApplication::keyPressed(arg);
}

void MainSample::preRenderTargetUpdate( const Ogre::RenderTargetEvent& evt )
{
	/*****************************************************************
							User input begin
	*****************************************************************/
	// 	if (evt.source == mMirrorTarget)  // for reflection, turn on camera reflection and hide submerged entities
	// 	{
	// 		mMirrorEnt->setVisible(false);
	// 		mCamera->enableReflection(MirrorPlane);
	// 	}
	/*****************************************************************
							User input end
	*****************************************************************/
}

void MainSample::postRenderTargetUpdate( const Ogre::RenderTargetEvent& evt )
{
	/*****************************************************************
							User input begin
	*****************************************************************/
	// 	if (evt.source == mMirrorTarget)  // for reflection, turn on camera reflection and hide submerged entities
	// 	{
	// 		mMirrorEnt->setVisible(true);
	// 		mCamera->disableReflection();
	// 	}
	/*****************************************************************
							User input end
	*****************************************************************/
}

bool MainSample::frameRenderingQueued( const Ogre::FrameEvent& evt )
{
	/*****************************************************************
							User input begin
	*****************************************************************/
	dtime+=evt.timeSinceLastFrame*0.5;
	if (dtime>1.0f)
		dtime-=1.0f;
	fpPP->setNamedConstant("time", &dtime, 1);
	/*****************************************************************
							User input end
	*****************************************************************/
	return BaseApplication::frameRenderingQueued(evt);
}