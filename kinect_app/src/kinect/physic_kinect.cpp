#include "physic_kinect.h"
#include "../score_system/score_system.h"
#include "../score_system/score_object.h"

const float DEF_POWER_RADIN = 10.0f;

PhysicKinect::PhysicKinect(Ogre::SceneManager *scene, btDynamicsWorld *world):
	m_pSceneMgr(scene),
	m_pWorld(world)
{
	for(int i = 0; i < NUI_SKELETON_POSITION_COUNT; i ++)
	{
		m_pBody[i] = NULL;
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

	m_pBody[NUI_SKELETON_POSITION_HIP_CENTER] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);
	//m_pBody[NUI_SKELETON_POSITION_SPINE] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);
	m_pBody[NUI_SKELETON_POSITION_SHOULDER_CENTER] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);
	m_pBody[NUI_SKELETON_POSITION_HEAD] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);
	//m_pBody[NUI_SKELETON_POSITION_SHOULDER_LEFT] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);
	//m_pBody[NUI_SKELETON_POSITION_ELBOW_LEFT] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);
	//m_pBody[NUI_SKELETON_POSITION_WRIST_LEFT] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);

	m_pBody[NUI_SKELETON_POSITION_HAND_LEFT] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_HAND), 2);

	//m_pBody[NUI_SKELETON_POSITION_SHOULDER_RIGHT] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);
	//m_pBody[NUI_SKELETON_POSITION_ELBOW_RIGHT] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);
	//m_pBody[NUI_SKELETON_POSITION_WRIST_RIGHT] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);

	m_pBody[NUI_SKELETON_POSITION_HAND_RIGHT] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_HAND), 2);

	//m_pBody[NUI_SKELETON_POSITION_HIP_LEFT] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);
	m_pBody[NUI_SKELETON_POSITION_KNEE_LEFT] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);
	//m_pBody[NUI_SKELETON_POSITION_ANKLE_LEFT] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);
	//m_pBody[NUI_SKELETON_POSITION_FOOT_LEFT] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);
	//m_pBody[NUI_SKELETON_POSITION_HIP_RIGHT] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);
	m_pBody[NUI_SKELETON_POSITION_KNEE_RIGHT] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);
	//m_pBody[NUI_SKELETON_POSITION_ANKLE_RIGHT] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);
	//m_pBody[NUI_SKELETON_POSITION_FOOT_RIGHT] = createRidigBody("sphere.mesh", 0.0, scale, pos, quat, NULL, ScoreSystem::createScoreObject(SCORE_TYPE_BODY), 2);
	
}

void PhysicKinect::release(void)
{
	for(int i = 0; i < NUI_SKELETON_POSITION_COUNT; i ++)
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
	for(int i = 0; i < NUI_SKELETON_POSITION_COUNT; i++)
	{
		if(m_pBody[i] != NULL)
		{
			m_pBody[i]->setOrigin(data.SkeletonPositions[i].x * DEF_POWER_RADIN, // + data.Position.x,
			data.SkeletonPositions[i].y * DEF_POWER_RADIN, // + data.Position.y,
			//data.SkeletonPositions[i].z * DEF_POWER_RADIN, // + data.Position.z
			25.0);
			m_pBody[i]->update();
		}
	}
}

DWORD PhysicKinect::getID(void)const
{
	return m_iKinectID;
}