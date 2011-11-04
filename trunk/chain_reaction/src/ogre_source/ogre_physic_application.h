#ifndef _OGRE_PHYSIC_APPLACTION_H_
#define _OGRE_PHYSIC_APPLACTION_H_

#include "ogre_application.h"

#include "OgreBulletDynamicsRigidBody.h"				 // for OgreBullet
#include "Shapes/OgreBulletCollisionsStaticPlaneShape.h" // for static planes
#include "Shapes/OgreBulletCollisionsBoxShape.h"		 // for Boxes
#include "Shapes/OgreBulletCollisionsTerrainShape.h"

//#########################################
//#######  OgrePhysicApplaction   #########
//#########################################

namespace CL
{
	class VehiclesAttrib;
}

class OgrePhysicApplication : public OgreApplication
{
private:
	//Physic
 	OgreBulletDynamics::DynamicsWorld *mWorld;	// OgreBullet World
 	OgreBulletCollisions::DebugDrawer *mDebugDrawer;
 	int mNumEntitiesInstanced;
	float mMoveSpeed;
 	std::deque<OgreBulletDynamics::RigidBody *>         mBodies;
 	std::deque<OgreBulletCollisions::CollisionShape *>  mShapes;
	float *mpTerrainHeightDataConvert;
	btRigidBody *mpBody;
	btHeightfieldTerrainShape* mpHeightShape;
	CL::VehiclesAttrib *pVehiclesAttrib;

public:
	OgrePhysicApplication();
	virtual ~OgrePhysicApplication();

	//release resource
	virtual void release(void);

	//create resource
	virtual void createScene(void);
	virtual void createUI(void);
	virtual void createPhysic(void);

	//operator fun
	virtual void createBody(void);
	virtual void createBody(const std::string &modelName);
	virtual void createVehicles(const std::string &vehicleName, const std::string &wheelsName);

	

	// Ogre::FrameListener
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

	virtual bool frameStarted(const Ogre::FrameEvent& evt);
	bool onVehiclesframeStarted(const Ogre::FrameEvent& evt);

	virtual bool frameEnded(const Ogre::FrameEvent& evt);

    // OIS::KeyListener
    virtual bool keyPressed( const OIS::KeyEvent &arg );
	bool onVehiclesKeyPressed( const OIS::KeyEvent &arg );

    virtual bool keyReleased( const OIS::KeyEvent &arg );
	bool onVehiclesKeyReleased( const OIS::KeyEvent &arg );


    // OIS::MouseListener
    virtual bool mouseMoved( const OIS::MouseEvent &arg );
    virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
    virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

	//callback event
	bool onQuit(const CEGUI::EventArgs &e);
};

//#########################################
//################  END   #################
//#########################################

#endif //_OGRE_PHYSIC_APPLACTION_H_