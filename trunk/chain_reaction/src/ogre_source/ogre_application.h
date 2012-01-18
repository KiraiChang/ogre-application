#ifndef _OGRE_APPLACTION_H_
#define _OGRE_APPLACTION_H_

//
//This is use for base UI and Camera Control
//

#include "base_application.h"
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include <Caelum.h>



//#########################################
//#######  OgrePhysicApplaction   #########
//#########################################

class OgreApplication : public BaseApplication
{
protected:
enum MOUSE_PRESS_TYPE
{
	MOUSE_PRESS_NONE,
	MOUSE_PRESS_RIGHT,
	MOUSE_PRESS_LEFT,
	MOUSE_INSIDE_UI,
	MOUSE_TYPE_NODE,
};
	//controll
	MOUSE_PRESS_TYPE mMousePress;
	//Terrain
    Ogre::TerrainGlobalOptions* mTerrainGlobals;
    Ogre::TerrainGroup* mTerrainGroup;
	Ogre::Terrain *mTerrainInfo;
    bool mTerrainsImported;
	OgreBites::Label* mInfoLabel;
	Caelum::CaelumSystem *mCaelumSystem;//Date and Night
 
	//CEGUI
	CEGUI::OgreRenderer* mRenderer;
	CEGUI::Window *mpSheet;

    void defineTerrain(long x, long y);
    void initBlendMaps(Ogre::Terrain* terrain);
    void configureTerrainDefaults(Ogre::Light* light);

public:
	OgreApplication(void);
	virtual ~OgreApplication(void);

	//release resource
	virtual void release(void);
	void destroyScene(void);

	//create resource
	virtual void createScene(void);
	void initCamera(void);
	void initCEGUI(void);
	void createCompositor(void);
	void createLight(void);
	void createShadow(void);
	void createSkyDome(void);
	void createTerrain(void);
	void createCaelum(void);
	void createFrameListener(void);
	virtual void createUI(void) = 0;

	//process resource
	void processTerrain(void);

	// Ogre::FrameListener
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	virtual bool frameStarted(const Ogre::FrameEvent& evt);
	virtual bool frameEnded(const Ogre::FrameEvent& evt);

    // OIS::KeyListener
    virtual bool keyPressed( const OIS::KeyEvent &arg );
    virtual bool keyReleased( const OIS::KeyEvent &arg );
    // OIS::MouseListener
    virtual bool mouseMoved( const OIS::MouseEvent &arg );
    virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
    virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
};

//#########################################
//################  END   #################
//#########################################

#endif //_OGRE_APPLACTION_H_