#include "physic_kinect.h"
#include "../score_system/score_system.h"
#include "../score_system/score_object.h"

const float DEF_POWER_RADIN = 10.0f;

PhysicKinect::PhysicKinect(Ogre::SceneManager *scene, btDynamicsWorld *world):
	m_pSceneMgr(scene),
	m_pWorld(world)
{
	for(int i = 0; i < BORDY_PART_MAX; i ++)
	{
		m_pBody[i] = NULL;
		m_iBodyID[i] = 0;
	}
}

PhysicKinect::~PhysicKinect(void)
{
	release();
}

void PhysicKinect::init(DWORD id)
{
	
	m_iKinectID = id;
	float scale[3] = {0.01, 0.01, 0.01};
	float pos[3] = {0.0, 0.0, 25.0};
	float quat[4] = {1.0, 0.0, 0.0, 0.0};

	
	m_pBody[eKinectLeftHand] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_HAND), 2);
	m_iBodyID[eKinectLeftHand] = NUI_SKELETON_POSITION_HAND_LEFT;

	m_pBody[eKinectRightHand] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_HAND), 2);
	m_iBodyID[eKinectRightHand] = NUI_SKELETON_POSITION_HAND_RIGHT;


	//m_pBody[NUI_SKELETON_POSITION_HIP_CENTER] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);
	//m_pBody[NUI_SKELETON_POSITION_SPINE] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);
	//m_pBody[NUI_SKELETON_POSITION_SHOULDER_CENTER] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);
	//m_pBody[NUI_SKELETON_POSITION_HEAD] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);
	//m_pBody[NUI_SKELETON_POSITION_SHOULDER_LEFT] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);
	//m_pBody[NUI_SKELETON_POSITION_ELBOW_LEFT] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);
	//m_pBody[NUI_SKELETON_POSITION_WRIST_LEFT] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);

	//m_pBody[NUI_SKELETON_POSITION_SHOULDER_RIGHT] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);
	//m_pBody[NUI_SKELETON_POSITION_ELBOW_RIGHT] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);
	//m_pBody[NUI_SKELETON_POSITION_WRIST_RIGHT] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);



	//m_pBody[NUI_SKELETON_POSITION_HIP_LEFT] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);
	//m_pBody[NUI_SKELETON_POSITION_KNEE_LEFT] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);
	//m_pBody[NUI_SKELETON_POSITION_ANKLE_LEFT] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);
	//m_pBody[NUI_SKELETON_POSITION_FOOT_LEFT] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);
	//m_pBody[NUI_SKELETON_POSITION_HIP_RIGHT] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);
	//m_pBody[NUI_SKELETON_POSITION_KNEE_RIGHT] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);
	//m_pBody[NUI_SKELETON_POSITION_ANKLE_RIGHT] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);
	//m_pBody[NUI_SKELETON_POSITION_FOOT_RIGHT] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);
	
}

void PhysicKinect::release(void)
{
	for(int i = 0; i < BORDY_PART_MAX; i ++)
	{
		if(NULL != m_pBody[i])
		{
			m_pBody[i]->release();
			delete m_pBody[i];
			m_pBody[i] = NULL;
		}
	}
}

PhysicRigidBody *PhysicKinect::createRidigBody(const char *modelName, float mass, float *scale, float *pos, float *quat, PhysicDebug *debug, void *userPoint, int flag)
{
	OgreShapeSphere *shape  = new OgreShapeSphere(m_pSceneMgr);
	shape->init(modelName, scale);
	PhysicRigidBody *body = new PhysicRigidBody(m_pWorld);
	body->init(shape, debug, mass, pos, quat, userPoint, flag);
	
	return body;
}

void PhysicKinect::update(const NUI_SKELETON_DATA &data)
{
	for(int i = 0; i < BORDY_PART_MAX; i++)
	{
		if(m_pBody[i] != NULL)
		{
			m_pBody[i]->setOrigin(data.SkeletonPositions[m_iBodyID[i]].x * DEF_POWER_RADIN, // + data.Position.x,
			data.SkeletonPositions[m_iBodyID[i]].y * DEF_POWER_RADIN, // + data.Position.y,
			data.SkeletonPositions[m_iBodyID[i]].z * DEF_POWER_RADIN // + data.Position.z
			);
			m_pBody[i]->update();
		}
	}
}

void PhysicKinect::updateDebug(float data[2][3])
{
	for(int i = 0; i < BORDY_PART_MAX; i++)
	{
		if(m_pBody[i] != NULL)
		{
			float x = data[i][0];
			float y = data[i][1];
			float z = data[i][2];
			m_pBody[i]->setOrigin(x, // + data.Position.x,
									y, // + data.Position.y,
									z // + data.Position.z
									);
			m_pBody[i]->update();
		}
	}
}

void PhysicKinect::getPartPos(unsigned int offset, float *pos)
{
	if(offset < BORDY_PART_MAX)
	{
		if(m_pBody[offset] != NULL)
			m_pBody[offset]->getPos(pos);
	}
}

DWORD PhysicKinect::getID(void)const
{
	return m_iKinectID;
}