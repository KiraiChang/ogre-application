
#include "character_controller.h"
#include <OgreSceneManager.h>
#include <OgreEntity.h>
#include <OgreAnimation.h>


CharacterController::CharacterController(Ogre::SceneManager* mgr, const DWORD &kinectID):m_pSceneMgr(mgr), m_pBodyNode(NULL), m_pBodyEnt(NULL), m_iKinectID(kinectID)
{
	init();
}

CharacterController::~CharacterController()
{
	release();
}

DWORD CharacterController::getID()const
{
	return m_iKinectID;
}

void CharacterController::init(void)
{
	char name[64];
	if(m_pBodyNode == NULL)
	{
		sprintf(name, "m_pBodyEnt_%d", m_iKinectID);
		m_pBodyEnt = m_pSceneMgr->createEntity(name, "sinbad.mesh");

		sprintf(name, "m_pBodyNode%d", m_iKinectID);
		m_pBodyNode = m_pSceneMgr->getRootSceneNode()->
			createChildSceneNode(name, Ogre::Vector3(0.0f, 0.0f, 25.0f));
		m_pBodyNode->attachObject(m_pBodyEnt);
		m_pBodyNode->setScale(5, 5, 5);
		setupAnimations();
	}

	for(int i = 0 ; i < NUI_SKELETON_POSITION_COUNT; i++)
	{
		sprintf(name, "m_pCharEnt%d_%d", i, m_iKinectID);
		m_pCharEnt[i] = m_pSceneMgr->createEntity(name, "sphere.mesh");

		// Create the scene node
		sprintf(name, "CharNode%d_%d", i, m_iKinectID);
		m_pCharNode[i] = m_pSceneMgr->getRootSceneNode()->
			createChildSceneNode(name, Ogre::Vector3(0.0f, 0.0f, 25.0f));
		m_pCharNode[i]->attachObject(m_pCharEnt[i]);
		m_pCharNode[i]->setScale(0.01, 0.01, 0.01);
	}
}


void CharacterController::update(const NUI_SKELETON_DATA &data)
{
	if(data.eTrackingState == NUI_SKELETON_TRACKED)
	{
		for(int j = 0; j < NUI_SKELETON_POSITION_COUNT; j++)
		{
			m_pCharNode[j]->setPosition(data.SkeletonPositions[j].x*10 + 100,
				data.SkeletonPositions[j].y*10 + 270,
				data.SkeletonPositions[j].z*10 + 100);
		}
		if(m_pBodyNode != NULL)
		{
			transformBone("Stomach",NUI_SKELETON_POSITION_SPINE, data);
			transformBone("Waist", NUI_SKELETON_POSITION_HIP_CENTER, data);
			transformBone("Root", NUI_SKELETON_POSITION_HIP_CENTER, data);
			transformBone("Chest",NUI_SKELETON_POSITION_SPINE, data);
			transformBone("Humerus.L",NUI_SKELETON_POSITION_SHOULDER_LEFT, data);
			transformBone("Humerus.R",NUI_SKELETON_POSITION_SHOULDER_RIGHT, data);
			transformBone("Ulna.L",NUI_SKELETON_POSITION_ELBOW_LEFT, data);
			transformBone("Ulna.R",NUI_SKELETON_POSITION_ELBOW_RIGHT, data);
			transformBone("Thigh.L",NUI_SKELETON_POSITION_HIP_LEFT, data);
			transformBone("Thigh.R",NUI_SKELETON_POSITION_HIP_RIGHT, data);
			transformBone("Calf.L",NUI_SKELETON_POSITION_KNEE_LEFT, data);
			transformBone("Calf.R",NUI_SKELETON_POSITION_KNEE_RIGHT, data);
			m_pBodyNode->setPosition(150, 270, 150);
		}
	}
}

void CharacterController::release(void)
{
	if(m_pSceneMgr)
	{
		if(m_pBodyEnt != NULL)
		{
			if(m_pBodyNode)
				m_pBodyNode->detachObject(m_pBodyEnt);

			m_pSceneMgr->destroyEntity(m_pBodyEnt);
			m_pBodyEnt = NULL;
		}

		if(m_pBodyNode!= NULL)
		{
			m_pSceneMgr->destroySceneNode(m_pBodyNode);
			m_pBodyNode = NULL;
		}
		for(int i = 0; i < NUI_SKELETON_POSITION_COUNT; i++)
		{
			if(m_pCharEnt[i]!= NULL)
			{
				if(m_pCharNode[i])
					m_pCharNode[i]->detachObject(m_pCharEnt[i]);

				m_pSceneMgr->destroyEntity(m_pCharEnt[i]);
				m_pCharEnt[i] = NULL;
			}

			if(m_pCharNode[i]!= NULL)
			{
				m_pSceneMgr->destroySceneNode(m_pCharNode[i]);
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

void CharacterController::transformBone(const std::string& modelBoneName, const NUI_SKELETON_POSITION_INDEX &posIndex, const NUI_SKELETON_DATA &data)
{
	// Get the model skeleton bone info
	Ogre::Skeleton* skel = m_pBodyEnt->getSkeleton();
	Ogre::Bone* bone = skel->getBone(modelBoneName);
	Ogre::Quaternion qI = bone->getInitialOrientation();
	Ogre::Quaternion newQ = Ogre::Quaternion::IDENTITY;
	NUI_SKELETON_POSITION_INDEX parentIndex = NUI_SKELETON_POSITION_HIP_CENTER;
	Ogre::Vector3 child = Ogre::Vector3::ZERO;
	Ogre::Vector3 parent = Ogre::Vector3::ZERO;

	if(data.eSkeletonPositionTrackingState[posIndex] != NUI_SKELETON_NOT_TRACKED )
	{
		child.x = data.SkeletonPositions[posIndex].x;
		child.y = data.SkeletonPositions[posIndex].y;
		child.z = data.SkeletonPositions[posIndex].z;
		switch(posIndex)
		{
		case NUI_SKELETON_POSITION_HIP_CENTER:
			break;
		case NUI_SKELETON_POSITION_SPINE:
		case NUI_SKELETON_POSITION_HIP_LEFT:
		case NUI_SKELETON_POSITION_HIP_RIGHT:
				parentIndex = NUI_SKELETON_POSITION_HIP_CENTER;
			break;
		case NUI_SKELETON_POSITION_SHOULDER_CENTER:
				parentIndex = NUI_SKELETON_POSITION_SPINE;
			break;
		case NUI_SKELETON_POSITION_HEAD:
		case NUI_SKELETON_POSITION_SHOULDER_RIGHT:
		case NUI_SKELETON_POSITION_SHOULDER_LEFT:
				parentIndex = NUI_SKELETON_POSITION_SHOULDER_CENTER;
			break;
		case NUI_SKELETON_POSITION_ELBOW_LEFT:
			parentIndex = NUI_SKELETON_POSITION_SHOULDER_LEFT;
			break;
		case NUI_SKELETON_POSITION_WRIST_LEFT:
			parentIndex = NUI_SKELETON_POSITION_ELBOW_LEFT;
			break;
		case NUI_SKELETON_POSITION_HAND_LEFT:
			parentIndex = NUI_SKELETON_POSITION_WRIST_LEFT;
			break;
		case NUI_SKELETON_POSITION_ELBOW_RIGHT:
			parentIndex = NUI_SKELETON_POSITION_SHOULDER_RIGHT;
			break;
		case NUI_SKELETON_POSITION_WRIST_RIGHT:
			parentIndex = NUI_SKELETON_POSITION_ELBOW_RIGHT;
			break;
		case NUI_SKELETON_POSITION_HAND_RIGHT:
			parentIndex = NUI_SKELETON_POSITION_WRIST_RIGHT;
			break;
		case NUI_SKELETON_POSITION_KNEE_LEFT:
			parentIndex = NUI_SKELETON_POSITION_HIP_LEFT;
			break;
		case NUI_SKELETON_POSITION_ANKLE_LEFT:
			parentIndex = NUI_SKELETON_POSITION_KNEE_LEFT;
			break;
		case NUI_SKELETON_POSITION_FOOT_LEFT:
			parentIndex = NUI_SKELETON_POSITION_ANKLE_LEFT;
			break;
		case NUI_SKELETON_POSITION_KNEE_RIGHT:
			parentIndex = NUI_SKELETON_POSITION_HIP_RIGHT;
			break;
		case NUI_SKELETON_POSITION_ANKLE_RIGHT:
			parentIndex = NUI_SKELETON_POSITION_ANKLE_RIGHT;
			break;
		case NUI_SKELETON_POSITION_FOOT_RIGHT:
			parentIndex = NUI_SKELETON_POSITION_ANKLE_RIGHT;
			break;
		default:
			break;
		}

		parent.x = data.SkeletonPositions[parentIndex].x;
		parent.y = data.SkeletonPositions[parentIndex].y;
		parent.z = data.SkeletonPositions[parentIndex].z;

		newQ = calcQuaternion(child, parent);

		bone->resetOrientation(); //in order for the conversion from world to local to work.
		newQ = bone->convertWorldToLocalOrientation(newQ);

		bone->setOrientation(newQ*qI);			
	} 
}