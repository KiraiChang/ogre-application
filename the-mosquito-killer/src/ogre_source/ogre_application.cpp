#include "ogre_application.h"

//#########################################
//##########  OgreApplaction   ############
//#########################################

//Terrain
void getTerrainImage(bool flipX, bool flipY, Ogre::Image& img)
{
    img.load("terrain.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    if (flipX)
        img.flipAroundY();
    if (flipY)
        img.flipAroundX();
 
}

void OgreApplication::defineTerrain(long x, long y)
{
    Ogre::String filename = mTerrainGroup->generateFilename(x, y);
    if (Ogre::ResourceGroupManager::getSingleton().resourceExists(mTerrainGroup->getResourceGroup(), filename))
    {
        mTerrainGroup->defineTerrain(x, y);
    }
    else
    {
        Ogre::Image img;
        getTerrainImage(x % 2 != 0, y % 2 != 0, img);
        mTerrainGroup->defineTerrain(x, y, &img);
        mTerrainsImported = true;
    }
}

void OgreApplication::initBlendMaps(Ogre::Terrain* terrain)
{
    Ogre::TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
    Ogre::TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
    Ogre::Real minHeight0 = 70;
    Ogre::Real fadeDist0 = 40;
    Ogre::Real minHeight1 = 70;
    Ogre::Real fadeDist1 = 15;
    float* pBlend1 = blendMap1->getBlendPointer();
    for (Ogre::uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
    {
        for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
        {
            Ogre::Real tx, ty;
 
            blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
            Ogre::Real height = terrain->getHeightAtTerrainPosition(tx, ty);
            Ogre::Real val = (height - minHeight0) / fadeDist0;
            val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
 
            val = (height - minHeight1) / fadeDist1;
            val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
            *pBlend1++ = val;
        }
    }
    blendMap0->dirty();
    blendMap1->dirty();
    blendMap0->update();
    blendMap1->update();
}

void OgreApplication::configureTerrainDefaults(Ogre::Light* light)
{
	// Configure global
    mTerrainGlobals->setMaxPixelError(8);
    // testing composite map
    mTerrainGlobals->setCompositeMapDistance(30000);
 
    // Important to set these so that the terrain knows what to use for derived (non-realtime) data
    //mTerrainGlobals->setLightMapDirection(light->getDerivedDirection());
    //mTerrainGlobals->setCompositeMapAmbient(mSceneMgr->getAmbientLight());
    //mTerrainGlobals->setCompositeMapDiffuse(light->getDiffuseColour());
 
    // Configure default import settings for if we use imported image
    Ogre::Terrain::ImportData& defaultimp = mTerrainGroup->getDefaultImportSettings();
    defaultimp.terrainSize = 513;
    defaultimp.worldSize = 1200.0f;
    defaultimp.inputScale = 600;
    defaultimp.minBatchSize = 33;
    defaultimp.maxBatchSize = 65;
    // textures
    defaultimp.layerList.resize(3);
    defaultimp.layerList[0].worldSize = 100;
    defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
    defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");
    defaultimp.layerList[1].worldSize = 30;
    defaultimp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
    defaultimp.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
    defaultimp.layerList[2].worldSize = 200;
    defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
    defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");
}

OgreApplication::OgreApplication():
	mRenderer(NULL),
	mTerrainGlobals(NULL),
	mTerrainGroup(NULL),
	mInfoLabel(NULL),
	mTerrainInfo(NULL),
	mpSheet(NULL),
	mMousePress(MOUSE_PRESS_NONE)
{
}

OgreApplication::~OgreApplication()
{
	release();
}

const std::string OgreApplication::getApplicationName(void)const
{
	return "DefaultOgreApp";
}

void OgreApplication::release(void)
{
	if(mRenderer != NULL)
	{
		CEGUI::OgreRenderer::destroySystem();
		mRenderer = NULL;
		mpSheet = NULL;
	}
	destroyScene();
}

void OgreApplication::destroyScene(void)
{
	if(!mTerrainGroup)
	{
		OGRE_DELETE mTerrainGroup;
		mTerrainGroup = NULL;
	}

	if(!mTerrainGlobals)
	{
		OGRE_DELETE mTerrainGlobals;
		mTerrainGlobals = NULL;
	}

	mTerrainInfo = NULL;
}

void OgreApplication::createScene(void)
{
	initCamera();
	initCEGUI();
	//createCompositor();
	createLight();
	createShadow();
	createSkyDome();
	//createTerrain();
	createCaelum();
	//mSceneMgr->setFog(Ogre::FOG_LINEAR);
}

void OgreApplication::initCamera(void)
{
	//init Camera
 
  //  if (mRoot->getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_INFINITE_FAR_PLANE))
  //  {
  //      mCamera->setFarClipDistance(0);   // enable infinite far clip distance if we can
		////mCamera->setPosition(Ogre::Vector3(0, 270, 80));
		//
		////mCamera->lookAt(Ogre::Vector3(0, 180, 0));
  //  }
	//mCamera->setPosition(Ogre::Vector3(0, 270, 80));
	mCameraNode->setPosition(0, 10, 50);
}

void OgreApplication::initCEGUI(void)
{
	//init CEGUI
	mRenderer = &CEGUI::OgreRenderer::bootstrapSystem();
 
    CEGUI::Imageset::setDefaultResourceGroup("Imagesets");
    CEGUI::Font::setDefaultResourceGroup("Fonts");
    CEGUI::Scheme::setDefaultResourceGroup("Schemes");
    CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
    CEGUI::WindowManager::setDefaultResourceGroup("Layouts");

	//CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>(CEGUI::System::getSingleton().getResourceProvider());
	//rp->setResourceGroupDirectory("imagesets", "./mygame/datafiles/gui/imagesets/");

    CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
	CEGUI::SchemeManager::getSingleton().create("WindowsLook.scheme");
	CEGUI::SchemeManager::getSingleton().create("VanillaSkin.scheme");
	CEGUI::SchemeManager::getSingleton().create("OgreTray.scheme");

	//CEGUI::ImagesetManager::getSingleton().create("TaharezLook.imageset");
	//CEGUI::ImagesetManager::getSingleton().create("WindowsLook.imageset");
	//CEGUI::ImagesetManager::getSingleton().create("Vanilla.imageset");
	//CEGUI::ImagesetManager::getSingleton().create("OgreTray.imageset", "Imagesets");
 
    CEGUI::System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");

	//createUI();
}

void OgreApplication::createCompositor(void)
{
	Ogre::Viewport *pViewport = mWindow->getViewport(0);
	Ogre::CompositorManager::getSingleton().addCompositor(pViewport,"Bloom");
	Ogre::CompositorManager::getSingleton().setCompositorEnabled(pViewport,"Bloom",true);
}

void OgreApplication::createLight(void)
{
	//create Light
    Ogre::Light* pLight = mSceneMgr->createLight("MainLight");
    pLight->setType(Ogre::Light::LT_DIRECTIONAL);
    pLight->setDirection(0.0, -1.0, 0.0);
    pLight->setDiffuseColour(1.0, 1.0, 1.0);
    pLight->setSpecularColour(0.4, 0.4, 0.0);
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.2, 0.2, 0.0));

	pLight = mSceneMgr->createLight("pLight");
	pLight->setType(Ogre::Light::LT_POINT);// make this light a point light
	pLight->setDiffuseColour(1.0, .5, 0.0);      //color the light orange 
	pLight->setSpecularColour(1.0, 1.0, 0.0);    //yellow highlights
	pLight->setAttenuation(100, 1.0, 0.045, 0.0075);
	pLight->setPosition(0.0, 10.0, 50.0);

	pLight = mSceneMgr->createLight("spotLight");
    pLight->setType(Ogre::Light::LT_SPOTLIGHT);
    pLight->setDiffuseColour(1.0, 1.0, 1.0);
    pLight->setSpecularColour(1.0, 1.0, 1.0);
 
    pLight->setDirection(-1, -1, -1);
    pLight->setPosition(0.0, 20.0, 0.0);
 
    pLight->setSpotlightRange(Ogre::Degree(35), Ogre::Degree(150), 0.1);
}

void OgreApplication::createShadow(void)
{
	//mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_MODULATIVE);
	mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
	//mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED);
	
	mSceneMgr->setShadowColour(Ogre::ColourValue(0.6,0.6,0.6));
	mSceneMgr->setShadowTextureSize(1024);
}

void OgreApplication::createSkyDome(void)
{
	mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);
}

void OgreApplication::createTerrain(void)
{
	//create Terrain
	mTerrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();
 
    mTerrainGroup = OGRE_NEW Ogre::TerrainGroup(mSceneMgr, Ogre::Terrain::ALIGN_X_Z, 513, 12000.0f);
    mTerrainGroup->setFilenameConvention(Ogre::String("BasicTutorial3Terrain"), Ogre::String("dat"));
    mTerrainGroup->setOrigin(Ogre::Vector3::ZERO);
 
	Ogre::Light* pLight = NULL;
	if(pLight == NULL)
	{
		pLight = mSceneMgr->createLight("MainLight");
	}

    configureTerrainDefaults(pLight);
 
    for (long x = 0; x <= 0; ++x)
        for (long y = 0; y <= 0; ++y)
            defineTerrain(x, y);
 
    // sync load since we want everything in place when we start
    mTerrainGroup->loadAllTerrains(true);
 
    if (mTerrainsImported)
    {
        Ogre::TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
        while(ti.hasMoreElements())
        {
            mTerrainInfo = ti.getNext()->instance;
            initBlendMaps(mTerrainInfo);
        }
    }
 
    mTerrainGroup->freeTemporaryResources();
}

void OgreApplication::createCaelum(void)
{
 //   // Initialise Caelum
	//mCaelumSystem = new Caelum::CaelumSystem (mRoot, mSceneMgr, Caelum::CaelumSystem::CAELUM_COMPONENTS_DEFAULT);

 //   // Create subcomponents (and allow individual subcomponents to fail).
 //   try {
	//	mCaelumSystem->setSkyDome (new Caelum::SkyDome (mSceneMgr, mCaelumSystem->getCaelumCameraNode ()));
 //   } catch (Caelum::UnsupportedException& ex) {
 //       //logUnsupportedException (ex);
 //   }
 //   try {
 //       mCaelumSystem->setSun (new Caelum::SphereSun(mSceneMgr, mCaelumSystem->getCaelumCameraNode ()));
 //   } catch (Caelum::UnsupportedException& ex) {
 //       //logUnsupportedException (ex);
 //   }
 //   try {
 //       mCaelumSystem->setMoon (new Caelum::Moon(mSceneMgr, mCaelumSystem->getCaelumCameraNode ()));
 //   } catch (Caelum::UnsupportedException& ex) {
 //       //logUnsupportedException (ex);
 //   }
 //   try {
 //       mCaelumSystem->setCloudSystem (new Caelum::CloudSystem (mSceneMgr, mCaelumSystem->getCaelumGroundNode ()));
 //   } catch (Caelum::UnsupportedException& ex) {
 //       //logUnsupportedException (ex);
 //   }
 //   try {
	//	mCaelumSystem->setPointStarfield (new Caelum::PointStarfield (mSceneMgr, mCaelumSystem->getCaelumCameraNode ()));
 //   } catch (Caelum::UnsupportedException& ex) {
 //       //logUnsupportedException (ex);
 //   }

 //   // Register caelum.
 //   // Don't make it a frame listener; update it by hand.
 //   //Root::getSingletonPtr ()->addFrameListener (mCaelumSystem.get ());
 //   mCaelumSystem->attachViewport (mCamera->getViewport ());

 //   try {
	//	mCaelumSystem->setPrecipitationController (new Caelum::PrecipitationController (mSceneMgr));
 //   } catch (Caelum::UnsupportedException& ex) {
 //       //logUnsupportedException (ex);
 //   }

 //   mCaelumSystem->setSceneFogDensityMultiplier (0.0015);
 //   mCaelumSystem->setManageAmbientLight (true);
 //   mCaelumSystem->setMinimumAmbientLight (Ogre::ColourValue (0.1, 0.1, 0.1));

 //   // Test spinning the caelum root node. Looks wrong in the demo;
 //   // but at least the sky components are aligned with each other.
 //   if (false) {
 //       mCaelumSystem->getCaelumCameraNode ()->setOrientation(
 //               Ogre::Quaternion (Ogre::Radian (Ogre::Math::PI), Ogre::Vector3::UNIT_Z) *
 //               Ogre::Quaternion (Ogre::Radian (Ogre::Math::PI / 2), Ogre::Vector3::UNIT_X));
 //       mCaelumSystem->getCaelumCameraNode ()->_update (true, true);
 //   }

 //   // Setup sun options
 //   if (mCaelumSystem->getSun ()) {
 //       // Make the sun very obvious:
 //       //mCaelumSystem->getSun ()->setDiffuseMultiplier (Ogre::ColourValue (1, 10, 1));

 //       mCaelumSystem->getSun ()->setAutoDisableThreshold (0.05);
 //       mCaelumSystem->getSun ()->setAutoDisable (false);
 //   }

 //   if (mCaelumSystem->getMoon ()) {
 //       // Make the moon very obvious:
 //       //mCaelumSystem->getMoon ()->setDiffuseMultiplier (Ogre::ColourValue (1, 1, 10));

 //       mCaelumSystem->getMoon ()->setAutoDisableThreshold (0.05);
 //       mCaelumSystem->getMoon ()->setAutoDisable (false);
 //   }

 //   if (mCaelumSystem->getCloudSystem ()) {
 //       try {
 //           mCaelumSystem->getCloudSystem ()->createLayerAtHeight(2000);
 //           mCaelumSystem->getCloudSystem ()->createLayerAtHeight(5000);
 //           mCaelumSystem->getCloudSystem ()->getLayer(0)->setCloudSpeed(Ogre::Vector2(0.000005, -0.000009));
 //           mCaelumSystem->getCloudSystem ()->getLayer(1)->setCloudSpeed(Ogre::Vector2(0.0000045, -0.0000085));
 //       } catch (Caelum::UnsupportedException& ex) {
 //           //logUnsupportedException (ex);
 //       }
 //   }

 //   if (mCaelumSystem->getPrecipitationController ()) {
 //       mCaelumSystem->getPrecipitationController ()->setIntensity (0);
 //   }

 //   // Set time acceleration.
 //   mCaelumSystem->getUniversalClock ()->setTimeScale (100);

 //   // Sunrise with visible moon.
 //   mCaelumSystem->getUniversalClock ()->setGregorianDateTime (2007, 4, 9, 9, 33, 0);

	//mRoot->addFrameListener (mCaelumSystem);

	//mCaelumSystem->setDepthComposer (new Caelum::DepthComposer (mSceneMgr));

 //   Caelum::DepthComposerInstance* inst = mCaelumSystem->getDepthComposer ()->getViewportInstance (mCamera->getViewport ());
 //   //inst->getDepthRenderer()->setRenderGroupRangeFilter (20, 80);
 //   inst->getDepthRenderer()->setViewportVisibilityMask (~0x00001000);
 //   mCaelumSystem->forceSubcomponentVisibilityFlags (0x00001000);

 //   mCaelumSystem->setGroundFogDensityMultiplier (0.03);
 //   mCaelumSystem->getDepthComposer ()->setGroundFogVerticalDecay (0.06);
 //   mCaelumSystem->getDepthComposer ()->setGroundFogBaseLevel (0);
}

void OgreApplication::createFrameListener(void)
{
	//Call by BaseApplication::setup so not call again at create scene
	BaseApplication::createFrameListener();

	mInfoLabel = mTrayMgr->createLabel(OgreBites::TL_TOP, "TInfo", "", 350);
}

void OgreApplication::processTerrain(void)
{
	if(NULL != mTerrainGroup)
	{
		if (mTerrainGroup->isDerivedDataUpdateInProgress())
		{
			mTrayMgr->moveWidgetToTray(mInfoLabel, OgreBites::TL_TOP, 0);
			mInfoLabel->show();
			if (mTerrainsImported)
			{
				mInfoLabel->setCaption("Building terrain, please wait...");
			}
			else
			{
				mInfoLabel->setCaption("Updating textures, patience...");
			}
		}
		else
		{
			mTrayMgr->removeWidgetFromTray(mInfoLabel);
			mInfoLabel->hide();
			if (mTerrainsImported)
			{
				//mTerrainGroup->saveAllTerrains(true);
				mTerrainsImported = false;
			}
		}
	}
	else
	{
		mTrayMgr->removeWidgetFromTray(mInfoLabel);
		mInfoLabel->hide();
		if (mTerrainsImported)
		{
			//mTerrainGroup->saveAllTerrains(true);
			mTerrainsImported = false;
		}
	}
}

// Ogre::FrameListener
bool OgreApplication::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	processTerrain();

    if(mWindow->isClosed())
        return false;
 
    if(mShutDown)
        return false;
 
    //Need to capture/update each device
    mKeyboard->capture();
    mMouse->capture();
 
    return BaseApplication::frameRenderingQueued(evt);
}

bool OgreApplication::frameStarted(const Ogre::FrameEvent& evt)
{
	bool bRet = BaseApplication::frameStarted(evt);
	//mCaelumSystem->notifyCameraChanged (mCamera);
 //   mCaelumSystem->updateSubcomponents (evt.timeSinceLastFrame);
	return bRet;
}

bool OgreApplication::frameEnded(const Ogre::FrameEvent& evt)
{
	bool bRet = BaseApplication::frameEnded(evt);
	return bRet;
}

// OIS::KeyListener
bool OgreApplication::keyPressed( const OIS::KeyEvent &arg )
{
	BaseApplication::keyPressed(arg);
    CEGUI::System &sys = CEGUI::System::getSingleton();
    sys.injectKeyDown(arg.key);
    sys.injectChar(arg.text);
 
    if (arg.key == OIS::KC_ESCAPE)
    {
        mShutDown = true;
    }
 
    mCameraMan->injectKeyDown(arg);
	return true;
}

bool OgreApplication::keyReleased( const OIS::KeyEvent &arg )
{
	BaseApplication::keyReleased(arg);
    if(CEGUI::System::getSingleton().injectKeyUp(arg.key)) return true;
    mCameraMan->injectKeyUp(arg);
    return true;
}

// OIS::MouseListener
CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID)
{
    switch (buttonID)
    {
    case OIS::MB_Left:
        return CEGUI::LeftButton;
        break;
 
    case OIS::MB_Right:
        return CEGUI::RightButton;
        break;
 
    case OIS::MB_Middle:
        return CEGUI::MiddleButton;
        break;
 
    default:
        return CEGUI::LeftButton;
        break;
    }
}

bool OgreApplication::mouseMoved( const OIS::MouseEvent &arg )
{
	CEGUI::System::getSingleton().injectMouseMove(arg.state.X.rel, arg.state.Y.rel);
	if(mMousePress == MOUSE_PRESS_RIGHT)
		mCameraMan->injectMouseMove(arg);
	return true;
}

bool OgreApplication::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	return CEGUI::System::getSingleton().injectMouseButtonDown(convertButton(id));
}

bool OgreApplication::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	CEGUI::System::getSingleton().injectMouseButtonUp(convertButton(id));
	return true;
}

//#########################################
//################  END   #################
//#########################################