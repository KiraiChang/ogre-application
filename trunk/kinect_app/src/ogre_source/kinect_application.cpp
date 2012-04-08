#include "kinect_application.h"
#include "../physic/physic.h"
#include "../physic/rag_doll.h"
#include "../ogre_physic/ogre_physic_debug.h"
#include "../ogre_physic/ogre_physic_shape.h"
#include "../physic/physic_rigid_body.h"
#include "../score_system/score_system.h"
#include "../score_system/score_object.h"

#include <Ogre.h>


KinectApplication::KinectApplication(void):
		m_pKinectDevice(NULL), 
		m_pPhysicSimulation(NULL),
		m_bHasDevice(TRUE),
		m_pRagDoll(NULL),
		m_pRagDoll1(NULL),
		m_pRigidBody(NULL),
		m_pRigidBody1(NULL)
{
	for(int i = 0; i < NUI_SKELETON_COUNT; i++)
	{
		m_vpPlayer[i] = NULL;
	}
	if(NULL == m_pPhysicSimulation)
	{
		m_pPhysicSimulation = new PhysicSimulation();
		m_pPhysicSimulation->init();
	}
}

KinectApplication::~KinectApplication(void)
{
	releaseKinect();
	releaseCharacter();
	if(NULL != m_pPhysicSimulation)
	{
		if(m_pRagDoll != NULL)
		{
			delete m_pRagDoll;
			m_pRagDoll = NULL;
		}

		if(m_pRagDoll1 != NULL)
		{
			delete m_pRagDoll1;
			m_pRagDoll1 = NULL;
		}

		if(NULL != m_pRigidBody)
		{
			delete m_pRigidBody;
			m_pRigidBody = NULL;
		}

		if(NULL != m_pRigidBody1)
		{
			delete m_pRigidBody1;
			m_pRigidBody1 = NULL;
		}

		delete m_pPhysicSimulation;
		m_pPhysicSimulation = NULL;
	}
}

const std::string KinectApplication::getApplicationName(void)const
{
	return "KinectOgreApplication";
}

void KinectApplication::createScene(void)
{
	OgreApplication::createScene();

	m_pRagDoll = new RagDoll(m_pPhysicSimulation->getDynamicsWorld());
	m_pRagDoll->init(0, 270, 0);
	OgrePhysicDebug *debug = new OgrePhysicDebug();
	debug->init(mSceneMgr);
	m_pRagDoll->setDebug(debug);

	m_pRagDoll1 = new RagDoll(m_pPhysicSimulation->getDynamicsWorld());
	m_pRagDoll1->init(0, 275, 0);
	OgrePhysicDebug *debug1 = new OgrePhysicDebug();
	debug1->init(mSceneMgr);
	m_pRagDoll1->setDebug(debug1);

	OgreShapeBox *shape = new OgreShapeBox(mSceneMgr);
	shape->init(100, "cube.mesh", (float *)&Ogre::Vector3(0.01, 0.01, 0.01));
	m_pRigidBody = new PhysicRigidBody(m_pPhysicSimulation->getDynamicsWorld());
	m_pRigidBody->init(shape, NULL, 100, (float *)&Ogre::Vector3(0.0, 280, 0.0), ScoreSystem::createScoreObject(SCORE_TYPE_BOMB), 8);
	m_pRigidBody->force(-0.5, 0, 1, 0, 0, 0, 10);

	OgreShapeBox *shape1 = new OgreShapeBox(mSceneMgr);
	shape1->init(101, "cube.mesh", (float *)&Ogre::Vector3(0.01, 0.01, 0.01));
	m_pRigidBody1 = new PhysicRigidBody(m_pPhysicSimulation->getDynamicsWorld());
	m_pRigidBody1->init(shape1, NULL, 1.0, (float *)&Ogre::Vector3(0.0, 285, 0.0), ScoreSystem::createScoreObject(SCORE_TYPE_BOMB), 8);
}

bool KinectApplication::frameEnded(const Ogre::FrameEvent& evt)
{
	bool bRet = OgreApplication::frameEnded(evt);
	
	if(NULL != m_pKinectDevice)
	{
		NUI_SKELETON_FRAME frame = {0};
		if(m_pKinectDevice->getSkeletonFrame(frame))
			updatePlayer(frame);
	}

	if(NULL != m_pPhysicSimulation)
	{
		m_pPhysicSimulation->update();

		if(m_pRagDoll != NULL)
			m_pRagDoll->update();

		if(m_pRagDoll1 != NULL)
			m_pRagDoll1->update();

		if(m_pRigidBody != NULL)
			m_pRigidBody->update();

		if(m_pRigidBody1 != NULL)
			m_pRigidBody1->update();
	}

	return bRet;
}

void KinectApplication::releaseCharacter()
{
	for(int i = 0; i < NUI_SKELETON_COUNT; i++)
	{
		if(m_vpPlayer[i] != NULL)
		{
			delete m_vpPlayer[i];
			m_vpPlayer[i] = NULL;
		}
	}
}

KinectDevice *KinectApplication::getKinectDevice()
{
	if(NULL == m_pKinectDevice && m_bHasDevice)
	{
		m_pKinectDevice = new KinectDevice();
		if(FAILED(m_pKinectDevice->Nui_Init()))
		{
			m_bHasDevice = FALSE;
			releaseKinect();
		}
	}

	return m_pKinectDevice;
}

void KinectApplication::releaseKinect()
{
	if( NULL != m_pKinectDevice)
	{
		m_pKinectDevice->Nui_UnInit();
		m_pKinectDevice->Nui_Zero();
		delete m_pKinectDevice;
		m_pKinectDevice = NULL;
	}
}

void KinectApplication::updatePlayer(const NUI_SKELETON_FRAME &frame)
{
	for(int i = 0; i < NUI_SKELETON_COUNT; i++ )
	{
		if(frame.SkeletonData[i].eTrackingState != NUI_SKELETON_NOT_TRACKED)
		{
			if(m_vpPlayer[i] != NULL)
			{
				if(m_vpPlayer[i]->getID() != frame.SkeletonData[i].dwTrackingID)
				{
					delete m_vpPlayer[i];
					m_vpPlayer[i] = new CharacterController(mSceneMgr, frame.SkeletonData[i].dwTrackingID);
				}
			}
			else
				m_vpPlayer[i] = new CharacterController(mSceneMgr, frame.SkeletonData[i].dwTrackingID);

			m_vpPlayer[i]->update(frame.SkeletonData[i]);
		}
		else
		{
			if(m_vpPlayer[i] != NULL)
			{
				delete m_vpPlayer[i];
				m_vpPlayer[i] = NULL;
			}
		}
	}
}