#include "mosquito.h"
//*******************************************************
//***************  MOSQUITO_BASE  ***********************
//*******************************************************
#include "../physic/physic_rigid_body.h"
#include "../move_system/move_system.h"
#include "../ogre_physic/ogre_physic_shape.h"
#include "../ogre_physic/ogre_physic_debug.h"
#include "../score_system/score_system.h"
#include "../score_system/score_object.h"
#include "../score_system/game_system.h"

#include "../score_system/audio_system.h"

const float MAX_WAIT_BLOOD_TIME = 1.0f;
const float MAX_WAIT_DEAD_TIME = 5.0f;
const float MAX_ALERT_DIST = 40.0f;
const float MAX_CUT_HIT_TIME = 0.719;
extern bool MOSQUITO_DEBUG_MODE;
MosquitoBase::MosquitoBase(void):
		m_eState(eMosquitoMove),
		m_pScore(NULL),
		m_pBody(NULL),
		m_pMove(NULL),
		m_pSceneMgr(NULL),
		m_pWorld(NULL),
		m_fTimer(0),
		m_bDestory(FALSE)
{
}

MosquitoBase::~MosquitoBase(void)
{
	release();
}

void MosquitoBase::init(Ogre::SceneManager *scene, btDynamicsWorld *world)
{
	m_pSceneMgr = scene;
	m_pWorld = world;
}

void MosquitoBase::create(const char *modelName, unsigned int moveType, float speed, float mass, float *scale, float *pos, float *size, float *quat, int score)
{
	m_fMoveSpeed = speed;
	float size_tmp[3];
	//create shape
	OgreShapeBox *shape  = new OgreShapeBox(m_pSceneMgr);
	shape->init(modelName, scale);
	shape->update(0.0, pos, quat);
	shape->setQueryMask(MOSQUITO_MASK);
	shape->setUserData(this);

	//create score object
	m_pScore = ScoreSystem::createScoreObject(SCORE_TYPE_ENEMY, score);
	m_pScore->regParent(this);

	size_tmp[0] = size[0] * scale[0];
	size_tmp[1] = size[1] * scale[1];
	size_tmp[2] = size[2] * scale[2];
	//create physic body
	m_pBody = new PhysicRigidBody(m_pWorld);
	PhysicDebug *debug = NULL;
	if(MOSQUITO_DEBUG_MODE)
	{
		debug = new OgrePhysicDebug();
		((OgrePhysicDebug *)debug)->init(m_pSceneMgr);
	}
	m_pBody->init(shape, debug, mass, pos, size_tmp, quat, m_pScore, 8);

	//create move type
	switch(moveType)
	{
	case eMoveStraight:
		m_pMove = new MoveStraight(shape->getNode());
		break;
	default:
		m_pMove = new MoveRandom(shape->getNode());
		break;
	}
}

void MosquitoBase::release(void)
{
	if(m_pMove != NULL)
	{
		delete m_pMove;
		m_pMove = NULL;
	}

	if(m_pBody != NULL)
	{
		m_pBody->release();
		delete m_pBody;
		m_pBody = NULL;
	}

	m_pSceneMgr = NULL;
	m_pWorld = NULL;
	m_pScore = NULL;//release in rigidbody
}

void MosquitoBase::update(float timePass)
{
	if(m_pBody != NULL)
	{
		if(m_pMove != NULL)
		{
			switch(m_eState)
			{
			case eMosquitoMove:
				{
					bool bDestory = false;
					m_pMove->update(bDestory, timePass * m_fMoveSpeed);

					Ogre::Vector3 pos= m_pMove->getPosition();
					Ogre::Quaternion q= m_pMove->getOrientation();
					m_pBody->update(timePass, (float *)&pos, (float *)&q);
					if(pos.z > MAX_ALERT_DIST)
					{
						m_eState = eMosquitoAlert;
						setParticle("Circle");
					}
				}
				break;
			case eMosquitoAlert:
				{
					bool bDestory = false;
					m_pMove->update(bDestory, timePass * m_fMoveSpeed);

					Ogre::Vector3 pos= m_pMove->getPosition();
					Ogre::Quaternion q= m_pMove->getOrientation();
					m_pBody->update(timePass, (float *)&pos, (float *)&q);
					if(bDestory)
						setState(eMosquitoBlood);
				}
				break;
			case  eMosquitoBlood:
				m_fTimer += timePass;
				if(m_fTimer > MAX_WAIT_BLOOD_TIME)
				{
					GameSystem::getInstance()->reduceBlood();
					setState(eMosuqitoDead);
				}
				break;
			case eMosuqitoHit:
			case eMosuqitoCut:
				m_fTimer += timePass;
				m_pBody->update(timePass);

				if(m_fTimer > MAX_WAIT_DEAD_TIME)
				{
					setState(eMosuqitoDead);
				}
				break;
			case eMosuqitoDead:
				m_bDestory = true;
				break;
			default:
				break;
			}
		}
	}
}

void MosquitoBase::setAnimation(const char *aniName, bool loop, bool blend)
{
	if(m_pBody != NULL)
		m_pBody->setAnimation(aniName, loop, blend);
}

void MosquitoBase::setParticle(const char *fileName)
{
	if(m_pBody != NULL)
	{
		OgreShapeBox *shape = (OgreShapeBox *)m_pBody->getShape();
		if(shape != NULL)
			shape->setParticle(fileName);
	}
}

void MosquitoBase::setBillboard(const char *fileName)
{
	if(m_pBody != NULL)
	{
		OgreShapeBox *shape = (OgreShapeBox *)m_pBody->getShape();
		if(shape != NULL)
			shape->setBillboard(fileName);
	}
}

void MosquitoBase::setVisible(bool visible)
{
	if(m_pBody != NULL)
		m_pBody->setVisible(visible);
}

void MosquitoBase::getPos(float *pos)
{
	if(m_pMove != NULL)
	{
		Ogre::Vector3 p= m_pMove->getPosition();
		pos[0] = p.x;
		pos[1] = p.y;
		pos[2] = p.z;
	}
}

void MosquitoBase::setState(MOSQUITO_STATE state)
{
	bool loop = true, blend = false;
	float pos[3];
	getPos(pos);
	if(state == eMosuqitoHit)
	{
		loop = false;
		setBillboard("blood_billboard");
		AudioSystem::getInstance()->play3D("../music/spring.wav", pos);
	}
	else if(state == eMosuqitoCut)
	{
		loop = false;
		setBillboard("blood_billboard");
		AudioSystem::getInstance()->play3D("../music/pop.wav", pos);
	}
	else if(state == eMosquitoBlood)
	{
		AudioSystem::getInstance()->play3D("../music/kiss.wav", pos);
	}
	if(m_uiMeshID < GameSystem::getInstance()->m_vMeshData.size())
	{
		if(state < GameSystem::getInstance()->m_vMeshData[m_uiMeshID].m_vAniName.size())
			setAnimation(GameSystem::getInstance()->m_vMeshData[m_uiMeshID].m_vAniName[state].c_str(), loop, blend);
	}
	m_eState = state;
	m_fTimer = 0.0f;
}

ScoreBase* MosquitoBase::getScoreBase(void)
{
	return m_pScore;
}
//*******************************************************
//***************  MOSQUITO_SPLIT  **********************
//*******************************************************
MosquitoSplit::MosquitoSplit(void):
	MosquitoBase(), 
	m_iSplitNumber(0)
{
}

MosquitoSplit::~MosquitoSplit(void)
{
	release();
}

void MosquitoSplit::init(Ogre::SceneManager *scene, btDynamicsWorld *world)
{
	MosquitoBase::init(scene, world);
}

void MosquitoSplit::create(const char *modelName, unsigned int moveType, float speed, float mass, float *scale, float *pos, float *size, float *quat, int score)
{
	MosquitoBase::create(modelName, moveType, speed, mass, scale, pos, size, quat, score);
}

void MosquitoSplit::release(void)
{

	MosquitoBase::release();
}

void MosquitoSplit::update(float timepass)
{
	MosquitoBase::update(timepass);
}
//*******************************************************
//****************  MOSQUITO_FAT  ***********************
//*******************************************************
MosquitoFat::MosquitoFat(void):
	MosquitoBase(), 
	m_iBloodNumber(0)
{
}

MosquitoFat::~MosquitoFat(void)
{
	release();
}

void MosquitoFat::init(Ogre::SceneManager *scene, btDynamicsWorld *world)
{
	MosquitoBase::init(scene, world);
}

void MosquitoFat::create(const char *modelName, unsigned int moveType, float speed, float mass, float *scale, float *pos, float *size, float *quat, int score)
{
	MosquitoBase::create(modelName, moveType, speed, mass, scale, pos, size, quat, score);
}

void MosquitoFat::release(void)
{

	MosquitoBase::release();
}

void MosquitoFat::update(float timePass)
{

	if(m_eState == eMosuqitoHit || m_eState == eMosuqitoCut)
	{
		if(m_iBloodNumber  > 0)
		{
			m_fTimer += timePass;
			if(m_fTimer > MAX_CUT_HIT_TIME)
			{
				setState(eMosquitoMove);
			}
			else
			{
				bool bDestory = false;
				m_pMove->update(bDestory, timePass * m_fMoveSpeed);
				Ogre::Vector3 pos= m_pMove->getPosition();
				Ogre::Quaternion q= m_pMove->getOrientation();
				m_pBody->update(timePass, (float *)&pos, (float *)&q);
				return;
			}
		}
	}
	MosquitoBase::update(timePass);
}
//*******************************************************
//********************  END  ****************************
//*******************************************************