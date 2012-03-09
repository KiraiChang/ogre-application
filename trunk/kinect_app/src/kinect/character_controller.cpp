
#include "character_controller.h"
#include <OgreSceneManager.h>
#include <OgreEntity.h>
#include <OgreAnimation.h>


CharacterController::CharacterController(Ogre::SceneManager* mgr):mSceneMgr(mgr), m_pBodyNode(NULL), m_pBodyEnt(NULL)
{
	init();
}

CharacterController::~CharacterController()
{
	release();
}

void CharacterController::init(void)
{
	if(m_pBodyNode == NULL)
	{
		m_pBodyEnt = mSceneMgr->createEntity("m_pBodyEnt", "sinbad.mesh");

		m_pBodyNode = mSceneMgr->getRootSceneNode()->
			createChildSceneNode("m_pBodyNode", Ogre::Vector3(0.0f, 0.0f, 25.0f));
		m_pBodyNode->attachObject(m_pBodyEnt);
		//m_pBodyNode->setScale(0.01, 0.01, 0.01);
		setupAnimations();
	}
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
				m_pCharNode[j]->setPosition(frame.SkeletonData[i].SkeletonPositions[j].x*10 + 100,
					frame.SkeletonData[i].SkeletonPositions[j].y*10 + 270,
					frame.SkeletonData[i].SkeletonPositions[j].z*10 + 100);
			}

			//NUI_SKELETON_POSITION_HIP_CENTER;
			//NUI_SKELETON_POSITION_SPINE;
			//NUI_SKELETON_POSITION_SHOULDER_CENTER;
			//NUI_SKELETON_POSITION_HEAD;
			//NUI_SKELETON_POSITION_SHOULDER_LEFT;
			//NUI_SKELETON_POSITION_ELBOW_LEFT;
			//NUI_SKELETON_POSITION_WRIST_LEFT;
			//NUI_SKELETON_POSITION_HAND_LEFT;
			//NUI_SKELETON_POSITION_SHOULDER_RIGHT;
			//NUI_SKELETON_POSITION_ELBOW_RIGHT;
			//NUI_SKELETON_POSITION_WRIST_RIGHT;
			//NUI_SKELETON_POSITION_HAND_RIGHT;
			//NUI_SKELETON_POSITION_HIP_LEFT;
			//NUI_SKELETON_POSITION_KNEE_LEFT;
			//NUI_SKELETON_POSITION_ANKLE_LEFT;
			//NUI_SKELETON_POSITION_FOOT_LEFT;
			//NUI_SKELETON_POSITION_HIP_RIGHT;
			//NUI_SKELETON_POSITION_KNEE_RIGHT;
			//NUI_SKELETON_POSITION_ANKLE_RIGHT;
			//NUI_SKELETON_POSITION_FOOT_RIGHT;
		}
	}
}

void CharacterController::release(void)
{
	if(mSceneMgr)
	{
		if(m_pBodyEnt)
		{
			if(m_pBodyNode)
				m_pBodyNode->detachObject(m_pBodyEnt);

			mSceneMgr->destroyEntity(m_pBodyEnt);
			m_pBodyEnt = NULL;
		}

		if(m_pBodyNode)
		{
			mSceneMgr->destroySceneNode(m_pBodyNode);
			m_pBodyNode = NULL;
		}
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

Ogre::Quaternion CharacterController::calcQuaternion(const Ogre::Vector3 &child, const Ogre::Vector3 &parent)
{
	return (Ogre::Vector3::NEGATIVE_UNIT_Z).getRotationTo((parent - child).normalisedCopy());
}

void CharacterController::setupBone(const std::string& boneName,const Ogre::Quaternion& q)
{
	Ogre::Bone* bone = m_pBodyEnt->getSkeleton()->getBone(boneName);
	bone->setManuallyControlled(true);
	bone->setInheritOrientation(false);

	bone->resetOrientation();
	bone->setOrientation(q);

	bone->setInitialState();
}

void CharacterController::setupBone(const std::string& name,const Ogre::Degree& yaw,const Ogre::Degree& pitch,const Ogre::Degree& roll)
{
	Ogre::Bone* bone = m_pBodyEnt->getSkeleton()->getBone(name);
	bone->setManuallyControlled(true);
	bone->setInheritOrientation(false);

	bone->resetOrientation();

	bone->yaw(yaw);
	bone->pitch(pitch);
	bone->roll(roll);

	//Matrix3 mat = bone->getLocalAxes();
	bone->setInitialState();

}

void CharacterController::setupAnimations()
{
	// this is very important due to the nature of the exported animations
	m_pBodyEnt->getSkeleton()->setBlendMode(Ogre::ANIMBLEND_CUMULATIVE);	

	std::string animNames[] =
	{"IdleBase", "IdleTop", "RunBase", "RunTop", "HandsClosed", "HandsRelaxed", "DrawSwords",
	"SliceVertical", "SliceHorizontal", "Dance", "JumpStart", "JumpLoop", "JumpEnd"};

	//set all to manualy controlled
	//Ogre::Bone* handleLeft = mBodyEnt->getSkeleton()->getBone("Hand.L");
	//handleLeft->setManuallyControlled(true);

	//Ogre::Matrix3 mat;
	Ogre::Quaternion q = Ogre::Quaternion::IDENTITY;
	Ogre::Quaternion q2,q3;
	Ogre::Vector3 xAxis,yAxis,zAxis;
	q.FromAngleAxis(Ogre::Degree(90), Ogre::Vector3(0,0,-1));
	q.ToAxes(xAxis,yAxis,zAxis);
	q2.FromAngleAxis(Ogre::Degree(90),xAxis);
	setupBone("Humerus.L",q*q2);
	q.FromAngleAxis(Ogre::Degree(90), Ogre::Vector3(0,0,1));
	q.ToAxes(xAxis,yAxis,zAxis);
	q2.FromAngleAxis(Ogre::Degree(90),xAxis);
	setupBone("Humerus.R",q*q2);

	q.FromAngleAxis(Ogre::Degree(90), Ogre::Vector3(0,0,-1));	 
	q2.FromAngleAxis(Ogre::Degree(45), Ogre::Vector3(0,-1,0));

	setupBone("Ulna.L",q*q2);

	q.FromAngleAxis(Ogre::Degree(90), Ogre::Vector3(0,0,1));	 	
	setupBone("Ulna.R",q*q2.Inverse());

	q.FromAngleAxis(Ogre::Degree(180), Ogre::Vector3(0,1,0));
	setupBone("Chest",q);
	setupBone("Stomach",q);
	q.FromAngleAxis(Ogre::Degree(180), Ogre::Vector3(1,0,0));	 	
	q2.FromAngleAxis(Ogre::Degree(180), Ogre::Vector3(0,1,0));
	setupBone("Thigh.L",q*q2);
	setupBone("Thigh.R",q*q2);
	setupBone("Calf.L",q*q2);
	setupBone("Calf.R",q*q2);
	setupBone("Root",Ogre::Degree(0), Ogre::Degree(0), Ogre::Degree(0));

	//rotate the body
	//mBodyNode->yaw(Degree(0),Node::TransformSpace::TS_WORLD);

	Ogre::Skeleton* skel = m_pBodyEnt->getSkeleton();

	Ogre::Skeleton::BoneIterator bi = skel->getBoneIterator();

	// populate our animation list
	for (int i = 0; i < NUM_ANIMS; i++)
	{
		m_pAnims[i] = m_pBodyEnt->getAnimationState(animNames[i]);
		m_pAnims[i]->setLoop(true);
		m_bFadingIn[i] = false;
		m_bFadingOut[i] = false;

		// disable animation updates
		Ogre::Animation* anim = m_pBodyEnt->getSkeleton()->getAnimation(animNames[i]);

		anim->destroyNodeTrack(m_pBodyEnt->getSkeleton()->getBone("Ulna.L")->getHandle());
		anim->destroyNodeTrack(m_pBodyEnt->getSkeleton()->getBone("Ulna.R")->getHandle());
		anim->destroyNodeTrack(m_pBodyEnt->getSkeleton()->getBone("Humerus.L")->getHandle());
		anim->destroyNodeTrack(m_pBodyEnt->getSkeleton()->getBone("Humerus.R")->getHandle());
		anim->destroyNodeTrack(m_pBodyEnt->getSkeleton()->getBone("Stomach")->getHandle());
		anim->destroyNodeTrack(m_pBodyEnt->getSkeleton()->getBone("Chest")->getHandle());
		anim->destroyNodeTrack(m_pBodyEnt->getSkeleton()->getBone("Thigh.L")->getHandle());
		anim->destroyNodeTrack(m_pBodyEnt->getSkeleton()->getBone("Thigh.R")->getHandle());
		anim->destroyNodeTrack(m_pBodyEnt->getSkeleton()->getBone("Calf.L")->getHandle());
		anim->destroyNodeTrack(m_pBodyEnt->getSkeleton()->getBone("Calf.R")->getHandle());
		anim->destroyNodeTrack(m_pBodyEnt->getSkeleton()->getBone("Root")->getHandle());
	}

	// start off in the idle state (top and bottom together)
	setBaseAnimation(ANIM_IDLE_BASE);
	setTopAnimation(ANIM_IDLE_TOP);

	// relax the hands since we're not holding anything
	m_pAnims[ANIM_HANDS_RELAXED]->setEnabled(true);

	m_bSwordsDrawn = false;
}

void CharacterController::setBaseAnimation(AnimID id, bool reset)
{
	if (m_eBaseAnimID >= 0 && m_eBaseAnimID < NUM_ANIMS)
	{
		// if we have an old animation, fade it out
		m_bFadingIn[m_eBaseAnimID] = false;
		m_bFadingOut[m_eBaseAnimID] = true;
	}

	m_eBaseAnimID = id;

	if (id != ANIM_NONE)
	{
		// if we have a new animation, enable it and fade it in
		m_pAnims[id]->setEnabled(true);
		m_pAnims[id]->setWeight(0);
		m_bFadingOut[id] = false;
		m_bFadingIn[id] = true;
		if (reset) m_pAnims[id]->setTimePosition(0);
	}
}

void CharacterController::setTopAnimation(AnimID id, bool reset)
{
	if (m_eTopAnimID >= 0 && m_eTopAnimID < NUM_ANIMS)
	{
		// if we have an old animation, fade it out
		m_bFadingIn[m_eTopAnimID] = false;
		m_bFadingOut[m_eTopAnimID] = true;
	}

	m_eTopAnimID = id;

	if (id != ANIM_NONE)
	{
		// if we have a new animation, enable it and fade it in
		m_pAnims[id]->setEnabled(true);
		m_pAnims[id]->setWeight(0);
		m_bFadingOut[id] = false;
		m_bFadingIn[id] = true;
		if (reset) m_pAnims[id]->setTimePosition(0);
	}
}