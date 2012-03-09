
#include "character_controller.h"
#include <OgreSceneManager.h>
#include <OgreEntity.h>


CharacterController::CharacterController(Ogre::SceneManager* mgr):mSceneMgr(mgr)
{
	init();
}

CharacterController::~CharacterController()
{
	release();
}

void CharacterController::init(void)
{
	char entName[64];
	char nodeName[64];
	for(int i = 0 ; i < NUI_SKELETON_POSITION_COUNT; i++)
	{
		sprintf(entName, "CharEnt%d", i);
		m_pCharEnt[i] = mSceneMgr->createEntity(entName, "sphere.mesh");

		// Create the scene node
		sprintf(nodeName, "CharNode%d", i);
		m_pCharNode[i] = mSceneMgr->getRootSceneNode()->
			createChildSceneNode(nodeName, Ogre::Vector3(0.0f, 0.0f, 25.0f));
		m_pCharNode[i]->attachObject(m_pCharEnt[i]);
		m_pCharNode[i]->setScale(0.01, 0.01, 0.01);
	}
}


void CharacterController::update(NUI_SKELETON_FRAME &frame)
{
	for(int i = 0; i < NUI_SKELETON_COUNT; i++)
	{
		if(frame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED)
		{
			for(int j = 0; j < NUI_SKELETON_POSITION_COUNT; j++)
			{
				//frame->SkeletonData[i].SkeletonPositions[j].x;
				//frame->SkeletonData[i].SkeletonPositions[j].y;
				//frame->SkeletonData[i].SkeletonPositions[j].z;
				m_pCharNode[j]->setPosition(frame.SkeletonData[i].SkeletonPositions[j].x*10 + 100,
					frame.SkeletonData[i].SkeletonPositions[j].y*10 + 270,
					frame.SkeletonData[i].SkeletonPositions[j].z*10 + 100);
			}
		}
	}
}

void CharacterController::release(void)
{
	if(mSceneMgr)
	{
		for(int i = 0; i < NUI_SKELETON_POSITION_COUNT; i++)
		{
			if(m_pCharEnt[i])
			{
				if(m_pCharNode[i])
					m_pCharNode[i]->detachObject(m_pCharEnt[i]);

				mSceneMgr->destroyEntity(m_pCharEnt[i]);
				m_pCharEnt[i] = NULL;
			}

			if(m_pCharNode[i])
			{
				mSceneMgr->destroySceneNode(m_pCharNode[i]);
				m_pCharNode[i] = NULL;
			}
		}
	}
}

