
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
			createChildSceneNode(name, Ogre::Vector3(0.0f, 0.0f, 0.0f));
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
			createChildSceneNode(name, Ogre::Vector3(0.0f, 0.0f, 0.0f));
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
			m_pCharNode[j]->setPosition(data.SkeletonPositions[j].x*10 + 0,
				data.SkeletonPositions[j].y*10 + 0.0,
				data.SkeletonPositions[j].z*10 + 0);
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
			m_pBodyNode->setPosition(0, 0.0, 0);
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

//Ogre::Quaternion CharacterController::calcQuaternion(const Ogre::Vector3 &child, const Ogre::Vector3 &parent)
//{
//	return (Ogre::Vector3::NEGATIVE_UNIT_Z).getRotationTo((parent - child).normalisedCopy());
//}
//

//(B-A) and (C-B)
Ogre::Quaternion CharacterController::calcQuaternion(const Ogre::Vector3 &upper, const Ogre::Vector3 &center, const Ogre::Vector3 &downer)
{
	Ogre::Vector3 upperCenterDir = center - upper;
	Ogre::Vector3 centerDownerDir = downer - center;
	return upperCenterDir.getRotationTo(centerDownerDir);
}

//Ogre::Quaternion CharacterController::calcQuaternion(const NUI_SKELETON_POSITION_INDEX &childIndex, 
//														const NUI_SKELETON_POSITION_INDEX &parentIndex, 
//														const NUI_SKELETON_DATA &data)
//{
//	Ogre::Vector3 child = Ogre::Vector3::ZERO;
//	Ogre::Vector3 parent = Ogre::Vector3::ZERO;
//	child.x = data.SkeletonPositions[childIndex].x;
//	child.y = data.SkeletonPositions[childIndex].y;
//	child.z = data.SkeletonPositions[childIndex].z;
//	parent.x = data.SkeletonPositions[parentIndex].x;
//	parent.y = data.SkeletonPositions[parentIndex].y;
//	parent.z = data.SkeletonPositions[parentIndex].z;
//	return calcQuaternion(child, parent);
//}

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

//There are 20 joints provided, and they can be organized into a transformation hierarchy as follows:
//
//Skeleton origin: JointID.HipCenter
//Left leg hierarchy: JointID.HipCenter, JointID.HipLeft, JointID.KneeLeft, JointID.AnkleLeft, JointID.FootLeft
//Right leg hierarchy: JointID.HipCenter, JointID.HipRight, JointID.KneeRight, JointID.AnkleRight, JointID.FootRight
//Spine-to-neck hierarchy: JointID.HipCenter, JointID.Spine, JointID.ShoulderCenter
//Head hierarchy: JointID.ShoulderCenter, JointID.Head
//Left arm hierarchy: JointID.ShoulderCenter, JointID.ShoulderLeft, JointID.ElbowLeft, JointID.WristLeft, JointID.HandLeft
//Right arm hierarchy: JointID.ShoulderCenter, JointID.ShoulderRight, JointID.ElbowRight, JointID.WristRight, JointID.HandRight
//
//From each pair of adjacent joints A and B in hierarchy you can define a vector (B-A) between the two joints, and from each 3-tuple of adjacent joints A, B, C, you can define a 3D rotation from the 2 vectors (B-A) and (C-B).


Ogre::Vector3 GetPositionBetweenIndices(const NUI_SKELETON_DATA &data, NUI_SKELETON_POSITION_INDEX p1, NUI_SKELETON_POSITION_INDEX p2) 
{
	Ogre::Vector3 pVec1 = Ogre::Vector3(data.SkeletonPositions[p1].x,
										data.SkeletonPositions[p1].y,
										data.SkeletonPositions[p1].z);

	Ogre::Vector3 pVec2 = Ogre::Vector3(data.SkeletonPositions[p2].x,
										data.SkeletonPositions[p2].y,
										data.SkeletonPositions[p2].z);

	return pVec2 - pVec1;
}

//populate matrix using the columns
void PopulateMatrix(Ogre::Matrix3 &jointOrientation, const Ogre::Vector3& xCol,const Ogre::Vector3& yCol, const Ogre::Vector3& zCol) 
{
	jointOrientation.SetColumn(0, xCol);
	jointOrientation.SetColumn(1, yCol);
	jointOrientation.SetColumn(2, zCol);
}

//constructs an orientation from a vector that specifies the x axis
void MakeMatrixFromX(const Ogre::Vector3& v1, Ogre::Matrix3 &jointOrientation) {

	//matrix columns
	Ogre::Vector3 xCol;
	Ogre::Vector3 yCol;
	Ogre::Vector3 zCol;

	//get normalized version of vector between joints
	Ogre::Vector3 v1normalized = v1.normalisedCopy();

	//set first column to the vector between the previous joint and the current one, this sets the two degrees of freedom
	xCol = v1normalized;

	//set second column to an arbitrary vector perpendicular to the first column
	//the angle can be arbitrary since it corresponds to the rotation about relative position vector, which doesn't matter
	yCol.x = 0.0f;
	yCol.y = xCol.z;
	yCol.z = -xCol.y;

	yCol.normalise();

	//third column is fully determined by the first two, and it must be their cross product
	zCol = xCol.crossProduct(yCol);

	//copy values into matrix
	PopulateMatrix(jointOrientation, xCol, yCol, zCol);
}

//constructs an orientation from a vector that specifies the y axis
void MakeMatrixFromY(const Ogre::Vector3& v1, Ogre::Matrix3 &jointOrientation) 
{

	//matrix columns
	Ogre::Vector3 xCol;
	Ogre::Vector3 yCol;
	Ogre::Vector3 zCol;

	//get normalized version of vector between joints
	Ogre::Vector3 v1normalized = v1.normalisedCopy();

	//set first column to the vector between the previous joint and the current one, this sets the two degrees of freedom
	yCol = v1normalized;

	//set second column to an arbitrary vector perpendicular to the first column
	//the angle can be arbitrary since it corresponds to the rotation about relative position vector, which doesn't matter
	xCol.x = yCol.y;
	xCol.y = -yCol.x;
	xCol.z = 0.0f;

	xCol.normalise();

	//third column is fully determined by the first two, and it must be their cross product
	zCol = xCol.crossProduct(yCol);

	//copy values into matrix
	PopulateMatrix(jointOrientation, xCol, yCol, zCol);
}

//constructs an orientation from a vector that specifies the x axis
void MakeMatrixFromZ(const Ogre::Vector3& v1, Ogre::Matrix3 &jointOrientation) 
{

	//matrix columns
	Ogre::Vector3 xCol;
	Ogre::Vector3 yCol;
	Ogre::Vector3 zCol;

	//get normalized version of vector between joints
	Ogre::Vector3 v1normalized = v1.normalisedCopy();

	//set first column to the vector between the previous joint and the current one, this sets the two degrees of freedom
	zCol = v1normalized;

	//set second column to an arbitrary vector perpendicular to the first column
	//the angle can be arbitrary since it corresponds to the rotation about relative position vector, which doesn't matter
	xCol.x = zCol.y;
	xCol.y = -zCol.x;
	xCol.z = 0.0f;

	xCol.normalise();

	//third column is fully determined by the first two, and it must be their cross product
	yCol = zCol.crossProduct(xCol);

	//copy values into matrix
	PopulateMatrix(jointOrientation, xCol, yCol, zCol);
}

//constructs an orientation from 2 vectors: the first specifies the x axis, and the next specifies the y axis
//uses the first vector as x axis, then constructs the other axes using cross products
void MakeMatrixFromXY(const Ogre::Vector3& xUnnormalized, const Ogre::Vector3& yUnnormalized, Ogre::Matrix3 &jointOrientation) 
{

	//matrix columns
	Ogre::Vector3 xCol;
	Ogre::Vector3 yCol;
	Ogre::Vector3 zCol;

	Ogre::Vector3 xn = xUnnormalized.normalisedCopy();
	Ogre::Vector3 yn = yUnnormalized.normalisedCopy();

	//set up the three different columns to be rearranged and flipped
	xCol = xn;
	zCol = xCol.crossProduct(yn).normalise();
	yCol = zCol.crossProduct(xCol); //shouldn't need normalization

	//copy values into matrix
	PopulateMatrix(jointOrientation, xCol, yCol, zCol);
}

//constructs an orientation from 2 vectors: the first specifies the x axis, and the next specifies the y axis
//uses the second vector as y axis, then constructs the other axes using cross products
void MakeMatrixFromYX(const Ogre::Vector3& xUnnormalized, const Ogre::Vector3& yUnnormalized, Ogre::Matrix3 &jointOrientation) 
{

	//matrix columns
	Ogre::Vector3 xCol;
	Ogre::Vector3 yCol;
	Ogre::Vector3 zCol;

	Ogre::Vector3 xn = xUnnormalized.normalisedCopy();
	Ogre::Vector3 yn = yUnnormalized.normalisedCopy();

	//set up the three different columns to be rearranged and flipped
	yCol = yn;
	zCol = xn.crossProduct(yCol).normalisedCopy();
	xCol = yCol.crossProduct(zCol);

	//copy values into matrix
	PopulateMatrix(jointOrientation, xCol, yCol, zCol);
}

//constructs an orientation from 2 vectors: the first specifies the x axis, and the next specifies the y axis
//uses the second vector as y axis, then constructs the other axes using cross products
void MakeMatrixFromYZ(const Ogre::Vector3& yUnnormalized, const Ogre::Vector3& zUnnormalized, Ogre::Matrix3 &jointOrientation) 
{

	//matrix columns
	Ogre::Vector3 xCol;
	Ogre::Vector3 yCol;
	Ogre::Vector3 zCol;

	Ogre::Vector3 yn = yUnnormalized.normalisedCopy();
	Ogre::Vector3 zn = zUnnormalized.normalisedCopy();

	//set up the three different columns to be rearranged and flipped
	yCol = yn;
	xCol = yCol.crossProduct(zn).normalisedCopy();
	zCol = xCol.crossProduct(yCol);

	//copy values into matrix
	PopulateMatrix(jointOrientation, xCol, yCol, zCol);
}

bool GetSkeletonJointOrientation(const NUI_SKELETON_DATA &user, NUI_SKELETON_POSITION_INDEX index, Ogre::Matrix3& jointOrientation)
{
	Ogre::Vector3 vx;
	Ogre::Vector3 vy;
	Ogre::Vector3 vz;

	switch (index) {

	case NUI_SKELETON_POSITION_HIP_CENTER://use hipcenter-spine for y, hipleft-hipright for x
		vy = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_HIP_CENTER,NUI_SKELETON_POSITION_SPINE);
		vx = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_HIP_LEFT,NUI_SKELETON_POSITION_HIP_RIGHT);
		MakeMatrixFromYX(vx,vy,jointOrientation);
		break;

	case NUI_SKELETON_POSITION_SPINE://use shoulderleft-shoulderright for x
		vy = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_SPINE,NUI_SKELETON_POSITION_SHOULDER_CENTER);
		vx = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_SHOULDER_LEFT,NUI_SKELETON_POSITION_SHOULDER_RIGHT);
		MakeMatrixFromYX(vx,vy,jointOrientation);
		break;

	case NUI_SKELETON_POSITION_SHOULDER_CENTER://use shoulderleft-shoulder-right for x
		vy = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_SHOULDER_CENTER,NUI_SKELETON_POSITION_HEAD);
		vx = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_SHOULDER_LEFT,NUI_SKELETON_POSITION_SHOULDER_RIGHT);
		MakeMatrixFromYX(vx,vy,jointOrientation);
		break;

	case NUI_SKELETON_POSITION_HEAD:
		vy = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_SHOULDER_CENTER,NUI_SKELETON_POSITION_HEAD);
		MakeMatrixFromY(vy,jointOrientation);
		break;

	case NUI_SKELETON_POSITION_SHOULDER_LEFT:
		vy = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_ELBOW_LEFT,NUI_SKELETON_POSITION_WRIST_LEFT);
		vx = -GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_SHOULDER_LEFT,NUI_SKELETON_POSITION_ELBOW_LEFT);
		MakeMatrixFromXY(vx,vy,jointOrientation);
		break;

	case NUI_SKELETON_POSITION_ELBOW_LEFT:
		vx = -GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_ELBOW_LEFT,NUI_SKELETON_POSITION_WRIST_LEFT);
		vy = -GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_SHOULDER_LEFT,NUI_SKELETON_POSITION_ELBOW_LEFT);
		MakeMatrixFromXY(vx,vy,jointOrientation);
		break;

	case NUI_SKELETON_POSITION_WRIST_LEFT:
		vx = -GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_WRIST_LEFT,NUI_SKELETON_POSITION_HAND_LEFT);
		MakeMatrixFromX(vx,jointOrientation);
		break;

	case NUI_SKELETON_POSITION_HAND_LEFT:
		vx = -GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_WRIST_LEFT,NUI_SKELETON_POSITION_HAND_LEFT);
		MakeMatrixFromX(vx,jointOrientation);
		break;

	case NUI_SKELETON_POSITION_HIP_LEFT:
		vy = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_KNEE_LEFT,NUI_SKELETON_POSITION_HIP_LEFT);
		//vz = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_KNEE_LEFT,NUI_SKELETON_POSITION_ANKLE_LEFT);//this one could be off
		//if(areNearCollinear(vy,vz)) {
		vx = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_HIP_LEFT,NUI_SKELETON_POSITION_HIP_RIGHT);
		MakeMatrixFromYX(vx,vy,jointOrientation);
		//}else{
		//      MakeMatrixFromYZ(vy,vz,jointOrientation);
		//}
		break;

	case NUI_SKELETON_POSITION_KNEE_LEFT:
		{
			vy = -GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_KNEE_LEFT,NUI_SKELETON_POSITION_ANKLE_LEFT);
			vz = -GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_ANKLE_LEFT,NUI_SKELETON_POSITION_FOOT_LEFT);//this one could be off
			//XnSkeletonJointPosition footPos;
			//XnSkeletonJoint footJoint;
			//footJoint = MSRKinectJointMap::GetXnJointByNuiIndex(NUI_SKELETON_POSITION_FOOT_LEFT);
			//m_pSkeleton->GetSkeletonJointPosition(user,footJoint,footPos);
			//XnFloat minConfidence = 0.9f;
			//if (footPos.fConfidence < minConfidence) {
			//	printf("Knee position confidence was: %f\r\n",footPos.fConfidence);
			//	MakeMatrixFromY(vy,jointOrientation);
			//}else {
				MakeMatrixFromYZ(vy,vz,jointOrientation);
			//}
		}
		break;

	case NUI_SKELETON_POSITION_ANKLE_LEFT:
		vz = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_FOOT_LEFT,NUI_SKELETON_POSITION_ANKLE_LEFT);
		MakeMatrixFromZ(vz,jointOrientation);
		break;

	case NUI_SKELETON_POSITION_FOOT_LEFT:
		vz = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_FOOT_LEFT,NUI_SKELETON_POSITION_ANKLE_LEFT);
		MakeMatrixFromZ(vz,jointOrientation);
		break;


	case NUI_SKELETON_POSITION_SHOULDER_RIGHT:
		vy = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_ELBOW_RIGHT,NUI_SKELETON_POSITION_WRIST_RIGHT);
		vx = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_SHOULDER_RIGHT,NUI_SKELETON_POSITION_ELBOW_RIGHT);
		MakeMatrixFromXY(vx,vy,jointOrientation);
		break;

	case NUI_SKELETON_POSITION_ELBOW_RIGHT:
		vx = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_ELBOW_RIGHT,NUI_SKELETON_POSITION_WRIST_RIGHT);
		vy = -GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_SHOULDER_RIGHT,NUI_SKELETON_POSITION_ELBOW_RIGHT);
		MakeMatrixFromXY(vx,vy,jointOrientation);
		break;

	case NUI_SKELETON_POSITION_WRIST_RIGHT:
		vx = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_WRIST_RIGHT,NUI_SKELETON_POSITION_HAND_RIGHT);
		MakeMatrixFromX(vx,jointOrientation);
		break;

	case NUI_SKELETON_POSITION_HAND_RIGHT:
		vx = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_WRIST_RIGHT,NUI_SKELETON_POSITION_HAND_RIGHT);
		MakeMatrixFromX(vx,jointOrientation);
		break;

	case NUI_SKELETON_POSITION_HIP_RIGHT:
		vy = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_KNEE_RIGHT,NUI_SKELETON_POSITION_HIP_RIGHT);
		//vz = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_KNEE_RIGHT,NUI_SKELETON_POSITION_ANKLE_RIGHT);//this one could be off
		//if(areNearCollinear(vy,vz)) {
		vx = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_HIP_LEFT,NUI_SKELETON_POSITION_HIP_RIGHT);
		MakeMatrixFromYX(vx,vy,jointOrientation);
		//}else{
		//      MakeMatrixFromYZ(vy,vz,jointOrientation);
		//}
		break;

	case NUI_SKELETON_POSITION_KNEE_RIGHT:
		vy = -GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_KNEE_RIGHT,NUI_SKELETON_POSITION_ANKLE_RIGHT);
		vz = -GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_ANKLE_RIGHT,NUI_SKELETON_POSITION_FOOT_RIGHT);//this one could be off
		MakeMatrixFromYZ(vy,vz,jointOrientation);
		break;

	case NUI_SKELETON_POSITION_ANKLE_RIGHT:
		vz = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_FOOT_RIGHT,NUI_SKELETON_POSITION_ANKLE_RIGHT);
		MakeMatrixFromZ(vz,jointOrientation);
		break;

	case NUI_SKELETON_POSITION_FOOT_RIGHT:
		vz = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_FOOT_RIGHT,NUI_SKELETON_POSITION_ANKLE_RIGHT);
		MakeMatrixFromZ(vz,jointOrientation);
		break;

	}
	return true;
}

void CharacterController::transformBone(const std::string& modelBoneName, const NUI_SKELETON_POSITION_INDEX &posIndex, const NUI_SKELETON_DATA &data)
{
	// Get the model skeleton bone info
	Ogre::Bone* bone = m_pBodyEnt->getSkeleton()->getBone(modelBoneName);
	Ogre::Quaternion qI = bone->getInitialOrientation();
	Ogre::Quaternion newQ = Ogre::Quaternion::IDENTITY;


	if(data.eSkeletonPositionTrackingState[posIndex] != NUI_SKELETON_NOT_TRACKED  && data.SkeletonPositions[posIndex].w > 0)
	{
		Ogre::Matrix3 jointOri;
		GetSkeletonJointOrientation(data, posIndex, jointOri);

		Ogre::Matrix3 matOri(jointOri.GetColumn(0).x,-jointOri.GetColumn(1).x,jointOri.GetColumn(2).x,
							-jointOri.GetColumn(0).y,jointOri.GetColumn(1).y,-jointOri.GetColumn(2).y,
							jointOri.GetColumn(0).z,-jointOri.GetColumn(1).z,jointOri.GetColumn(2).z);

		newQ.FromRotationMatrix(matOri);
		bone->resetOrientation(); //in order for the conversion from world to local to work.
		newQ = bone->convertWorldToLocalOrientation(newQ);

		bone->setOrientation(newQ*qI);			
	} 
}