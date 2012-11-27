#ifndef _MAIN_SAMPLE_H_
#define _MAIN_SAMPLE_H_

#include "base_application.h"
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include "resource.h"
#endif

class MainSample : public BaseApplication, public Ogre::RenderTargetListener
{
private:
	/*****************************************************************
							User input begin
	*****************************************************************/
	Ogre::Light *l1, *l2, *l3;

	Ogre::RenderTarget* mMirrorTarget;
	Ogre::Plane MirrorPlane;
	Ogre::Entity* mMirrorEnt;

	Ogre::Entity* mGlowObjEnt;
	Ogre::SceneNode* mGlowObjNode;
	Ogre::GpuProgramParametersSharedPtr fpPP;
	float dtime;
	/*****************************************************************
							User input end
	*****************************************************************/
public:
	MainSample(void);
	virtual ~MainSample(void);

	void preRenderTargetUpdate(const Ogre::RenderTargetEvent& evt);
	void postRenderTargetUpdate(const Ogre::RenderTargetEvent& evt);
protected:
	virtual void createScene(void);
	bool keyPressed( const OIS::KeyEvent &arg );
	bool frameRenderingQueued(const Ogre::FrameEvent& evt);
};

#endif //_MAIN_SAMPLE_H_