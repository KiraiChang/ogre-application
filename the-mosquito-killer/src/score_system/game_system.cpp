#include "game_system.h"
#include "score_system.h"
#include "score_object.h"
#include "btBulletDynamicsCommon.h"
#include "../move_system/move_system.h"
#include "../ogre_physic/ogre_physic_shape.h"
#include "../ogre_physic/ogre_physic_debug.h"
#include "../physic/physic_debug.h"
#include "audio_system.h"
#include <Ogre.h>
#include <CEGUI.h>
#include <json_spirit.h>
#include "../compositer/depth_of_field_effect.h"
#include <random>
extern Ogre::Log* m_pLog;
const float HAND_CHECK_AIM_POSE_DIST = 1.5f;
const float HAND_CHECK_CLOSE_DIST = 2.5f;
const float HAND_CHECK_OPEN_DIST = 10.0f;
//const float HAND_CHECK_RIGHT_HAND_DIST = 1.5f;
const float HAND_CHECK_RIGHT_HAND_SPEED = 5.0f;
const float HAND_CHECK_SHOOT_TIMEPASS = 0.5f;
const float HAND_WAIT_ATTACK_TIMEOUT = 3.0f;

bool MOSQUITO_DEBUG_MODE = false;

bool GameSystem::MaterialCombinerCallback(btManifoldPoint& cp,	const btCollisionObject* colObj0,int partId0,int index0,const btCollisionObject* colObj1,int partId1,int index1)
{

	float friction0 = colObj0->getFriction();
	float friction1 = colObj1->getFriction();
	float restitution0 = colObj0->getRestitution();
	float restitution1 = colObj1->getRestitution();

	if (colObj0->getCollisionFlags() & btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK)
	{
		friction0 = 1.0;//partId0,index0
		restitution0 = 0.f;
	}
	if (colObj1->getCollisionFlags() & btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK)
	{
		if (index1&1)
		{
			friction1 = 1.0f;//partId1,index1
		} else
		{
			friction1 = 0.f;
		}
		restitution1 = 0.f;
	}

	//cp.m_combinedFriction = calculateCombinedFriction(friction0,friction1);
	//cp.m_combinedRestitution = calculateCombinedRestitution(restitution0,restitution1);

	//this return value is currently ignored, but to be on the safe side: return false if you don't calculate friction
	return true;
}

void checkDestory(ScoreBase *object0, ScoreBase *object1)
{
	if(ScoreSystem::calcScore(object0, object1) != 0)
	{
		//object1->m_bDestory = true;
		float pos[3];
		float scale[3] = {0.3, 0.3, 0.3};
		float quat[4] = {1.0, 0.0, 0.0, 0.0};
		float size[3] = {50.0, 50.0, 50.0};
		GameSystem::getInstance()->setHandState(GameSystem::eOnHandClose);
		if(object1->getParent() != NULL)
		{
			switch(((MosquitoBase *)object1->getParent())->getType())
			{	
			case eMosquitoBase:
				{
					((MosquitoBase *)object1->getParent())->getPos(pos);
					((MosquitoBase *)object1->getParent())->setState(eMosuqitoHit);
					//((MosquitoBase *)object1->getParent())->setDestory();
					AudioSystem::getInstance()->play3D("../music/explosion.wav", pos);
				}
				break;
			case eMosquitoSplit:
				{
					int number = ((MosquitoSplit *)object1->getParent())->getSplitNumber();
					((MosquitoSplit *)object1->getParent())->getPos(pos);
					((MosquitoSplit *)object1->getParent())->setState(eMosuqitoHit);
					float high = pos[1];
					float distance = pos[0];
					//((MosquitoSplit *)object1->getParent())->setDestory();
					AudioSystem::getInstance()->play3D("../music/explosion.wav", pos);
					//create "number" 
					for(int i = 0; i < number; i++)
					{
						pos[2] -= 40;
						pos[0] += distance;
						if(distance >= 20)
						{
							distance = -20;
							high -= 5;
						}
						pos[1] = high;
						GameSystem::getInstance()->createMosquito(eMosquitoBase,
							eMoveStraight,
							1.5,
							0,
							1.0,
							scale,
							pos,
							quat,
							100);
						distance += 5.0f;
					}
				}
				break;
			case eMosquitoFat:
				{
					((MosquitoFat *)object1->getParent())->getPos(pos);
					((MosquitoFat *)object1->getParent())->decreaseBlood();
					((MosquitoFat *)object1->getParent())->setState(eMosuqitoHit);
					AudioSystem::getInstance()->play3D("../music/explosion.wav", pos);
				}
				break;
			default:
				break;
			}
			//random to get book or bomb add
		 	int getW = Ogre::Math::RangeRandom(1,10);
			if(getW > 9)
				GameSystem::getInstance()->NumBomb++;
			else if(getW >= 8)
				GameSystem::getInstance()->NumBook++;
		}
	}
}

bool GameSystem::MaterialProcessedCallback(btManifoldPoint& cp,btCollisionObject* body0,btCollisionObject* body1)
{
	btRigidBody* rigidbody0 = dynamic_cast<btRigidBody*>(body0);
	btRigidBody* rigidbody1 = dynamic_cast<btRigidBody*>(body1);
	if(rigidbody0->getUserPointer() != NULL && rigidbody1->getUserPointer() != NULL)
	{
		//取出rigidbody內的指標...並檢查兩個的指標是否需要作用
		ScoreBase *object0 = (ScoreBase *)rigidbody0->getUserPointer();
		ScoreBase *object1 = (ScoreBase *)rigidbody1->getUserPointer();
		GameSystem::HandState eState = GameSystem::getInstance()->getHandState();
		if(object0->getType() == SCORE_TYPE_HAND && object1->getType() == SCORE_TYPE_ENEMY)
		{
			if(eState == eOnHandWaitAttack)
			{
				checkDestory(object0, object1);
				GameSystem::getInstance()->setHandState(eOnHandAttacked);
			}
		}
		else if(object1->getType() == SCORE_TYPE_HAND && object0->getType() == SCORE_TYPE_ENEMY)
		{
			if(eState == eOnHandWaitAttack)
			{
				checkDestory(object1, object0);
				GameSystem::getInstance()->setHandState(eOnHandAttacked);
			}
		}
		else if(object0->getType() == SCORE_TYPE_WEAPON)
		{
			if(object0->getParent() != NULL)
			{
				switch(((WeaponKnife *)object0->getParent())->getType()) //武器功能add
				{
				case eWeaponKnife:
					checkDestory(object0, object1);
					((WeaponKnife *)object0->getParent())->setDestory();
					break;
				case eWeaponBook:
					checkDestory(object0, object1);
					//((WeaponBook *)object0->getParent())->setDestory();
					break;
				case eWeaponBomb://蚊子全死光,要記分數
					//((WeaponBomb *)object0->getParent())->setDestory();
					//GameSystem::getInstance()->releaseMosquito();
					break;
				default:
					break;
				}
			}
		}
		else if(object0->getType() == SCORE_TYPE_ICON)
		{
			if(object1->getType() == SCORE_TYPE_HAND /*&& eState == eOnHandWaitShoot*/)
			{
				WEAPON_TYPE type = ((WeaponKnife *)object0->getParent())->getType();
				switch(type) //改變使用武器add
				{
				case eWeaponKnife:
					GameSystem::getInstance()->CurrentWeapon = 0;
					break;
				case eWeaponBook:
					GameSystem::getInstance()->CurrentWeapon = 1;
					break;
				case eWeaponBomb:
					GameSystem::getInstance()->CurrentWeapon = 2;
					break;
				default:
					break;
				}
			}
		}
	}
	return true;
}

GameSystem *GameSystem::g_instance = NULL;

GameSystem::GameSystem(void):
		m_pWorld(NULL),
		m_bIsPause(FALSE),
		m_pSceneMgr(NULL),
		m_pWindow(NULL),
		m_bIsDebug(FALSE),
		m_iPlayerBlood(0),
		//m_fFullTime(0),
		m_fTimePass(0),
		//m_bShoot(false),
		m_eState(eOnMenu),
		m_eDebugHandVState(eHandMoveVNothing),
		m_eDebugHandHState(eHandMoveHNothing),
		m_eDebugHandAttackState(eHandAttackWait),
		m_eHandState(eOnHandOpen),
		m_fHandCloseTime(0),
		m_iCurrentID(0),
		m_fRightHandZPos(0),
		m_fShootTimePass(0),
		//m_pSheet(NULL),
		NumBook(0),
		NumBomb(0),
		CurrentWeapon(0), //add
		ChooesKnife(NULL),//當作選取武器的快捷鍵 add
		ChooesBook(NULL),//當作選取武器的快捷鍵 add
		ChooesBomb(NULL), //當作選取武器的快捷鍵 add
		m_pHintObject(NULL),
		m_pHintNode(NULL),
		m_pCompositer(NULL)
{
	initMeshData();
	//for(int i = 0; i < NUI_SKELETON_COUNT; i++)
	//{
	//	m_vpPlayer[i] = NULL;
	//}
	m_vpPlayer = NULL;

	m_vfHandDebugPos[0] = 0.0;
	m_vfHandDebugPos[1] = 20.0;
	m_vfHandDebugPos[2] = 80.0;

	m_fTwoHandDistance = HAND_DEBUG_DISTANCE;
}

GameSystem::~GameSystem(void)
{
	release();
}

GameSystem *GameSystem::getInstance(void)
{
	if(g_instance == NULL)
	{
		g_instance = new GameSystem();
	}
	return g_instance;
}

void GameSystem::init(btDynamicsWorld* world, Ogre::SceneManager *sceneMgr, Ogre::RenderWindow *pWindow, Ogre::Root *mRoot, Ogre::Viewport *mViewport)
{
	if(m_pWorld == NULL)
		m_pWorld = world;

	if(m_pSceneMgr == NULL)
		m_pSceneMgr = sceneMgr;

	if(m_pWindow == NULL)
		m_pWindow = pWindow;

	initWeapon();

	CEGUI::WindowManager &windowMgr = CEGUI::WindowManager::getSingleton();

	//m_pSheet = windowMgr.loadWindowLayout("kinect_game.layout");
	m_mSheet["playing"] = windowMgr.loadWindowLayout("kinect_game.layout");
	//m_mSheet["logo"] = windowMgr.loadWindowLayout("MosquitoLogo.layout");
	////CEGUI::Imageset &imageset = CEGUI::ImagesetManager::getSingleton().createFromImageFile("Logo", "Mosquito3.png");

	//m_mSheet["logo"]->getChild("MainMenu/Logo")->setProperty("Image","set:Mosquito image:Logo");
	//CEGUI::System::getSingleton().setGUISheet(m_mSheet["logo"]);

	//Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
	//// Create an overlay
	//m_mOverlay["MainMenu"] = overlayManager.create( "MainMenu" );

	//// Create a panel
	//Ogre::OverlayContainer* panel = static_cast<Ogre::OverlayContainer*>( overlayManager.createOverlayElement( "Panel", "MosquitoLogo" ) );
	//panel->setPosition( 0.0, 0.0 );
	//panel->setDimensions( 0.1, 0.1 );
	//panel->setMaterialName( "MosquitoLogo" );
	//// Add the panel to the overlay
	//m_mOverlay["MainMenu"]->add2D( panel );

	//// Show the overlay
	//m_mOverlay["MainMenu"]->show();

	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
	// Create an overlay
	m_mOverlay["MainMenu"] = overlayManager.getByName( "MainMenu" );

	// Show the overlay
	m_mOverlay["MainMenu"]->hide();

	m_pCompositer = new DOFManager(mRoot, mViewport);
	m_pCompositer->setFocus(0);
	setGameState(eOnMenu);
}

void GameSystem::initWeapon(void)
{
	ChooesKnife = new WeaponKnife(); //武器快捷add
	ChooesBook = new WeaponBook();
	ChooesBomb = new WeaponBomb();

	ChooesKnife->init(m_pSceneMgr, m_pWorld);
	ChooesBook->init(m_pSceneMgr, m_pWorld);
	ChooesBomb->init(m_pSceneMgr, m_pWorld);

	float mass = 1;
	float scale[3] = {1.0,1.0,1.0};
	float pos[3] = {5.0,15.0,80.0};
	float quat[4] = {1.0, 1.0, 0.5, 0.0};
	float tar[3] = {0,0,0};
	ChooesKnife->create(m_vWeapeanMeshData[0].m_sMeshName.c_str(), 0, mass,scale,pos, m_vWeapeanMeshData[0].m_fvSize,quat,0,tar);
	pos[0] = 20;
	ChooesBook->create(m_vWeapeanMeshData[1].m_sMeshName.c_str(), 0, mass,scale,pos, m_vWeapeanMeshData[0].m_fvSize,quat,0,tar);
	pos[0] = 35;
	ChooesBomb->create(m_vWeapeanMeshData[2].m_sMeshName.c_str(), 0, mass,scale,pos, m_vWeapeanMeshData[0].m_fvSize,quat,0,tar);

	//OgreShapeBox *shape  = new OgreShapeBox(m_pSceneMgr);
	//shape->init("Ninja.mesh", scale);
	//quat[0] = 1.0;
	//quat[1] = 0.0;
	//quat[2] = 0.0;
	//quat[3] = 0.0;

	//pos[0] =  0.0;
	//pos[1] =  0.0;
	//pos[2] =  0.0;
	//shape->update(0.0, pos, quat);
}

void GameSystem::initMeshData(void)
{
	m_vMeshData.clear();
	m_vWeapeanMeshData.clear();
	std::fstream file("../configure/mesh_data.dat");
	if(file.is_open())
	{
		json_spirit::mValue value;
		json_spirit::read(file, value);
		json_spirit::mArray arr;
		json_spirit::mObject obj;
		if(value.get_obj().count("debug") > 0)
		{
			MOSQUITO_DEBUG_MODE = value.get_obj()["debug"].get_bool();
		}
		arr = value.get_obj()["mesh_data"].get_array();
		for(int i = 0; i <arr.size();i++)
		{
			obj = arr[i].get_obj();
			MeshData data;
			data.m_sMeshName = obj["MeshName"].get_str();
			data.m_fvSize[0] = obj["Size"].get_array()[0].get_real();
			data.m_fvSize[1] = obj["Size"].get_array()[1].get_real();
			data.m_fvSize[2] = obj["Size"].get_array()[2].get_real();
			for(int j = 0; j < obj["AniName"].get_array().size();j++)
			{
				data.m_vAniName.push_back(obj["AniName"].get_array()[j].get_str());
			}
			m_vMeshData.push_back(data);
		}
		std::cout<<json_spirit::write(value)<<std::endl;
		arr = value.get_obj()["weapon_data"].get_array();
		for(int i = 0; i <arr.size();i++)
		{
			obj = arr[i].get_obj();
			MeshData data;
			data.m_sMeshName = obj["MeshName"].get_str();
			data.m_fvSize[0] = obj["Size"].get_array()[0].get_real();
			data.m_fvSize[1] = obj["Size"].get_array()[1].get_real();
			data.m_fvSize[2] = obj["Size"].get_array()[2].get_real();
			for(int j = 0; j < obj["AniName"].get_array().size();j++)
			{
				data.m_vAniName.push_back(obj["AniName"].get_array()[j].get_str());
			}
			m_vWeapeanMeshData.push_back(data);
		}
	}
	else
	{
		{
			MeshData data;
			data.m_sMeshName = "mosquito01.mesh";
			data.m_fvSize[0] = 5;
			data.m_fvSize[1] = 5;
			data.m_fvSize[2] = 5;
			data.m_vAniName.push_back("move");
			m_vMeshData.push_back(data);
		}
	}
}

void GameSystem::release(void)
{
	restart();
	releaseCharacter();
	m_waveSystem.release();
	m_dotSceneLoader.release();
	if(NULL != m_pCompositer)
	{
		delete m_pCompositer;
		m_pCompositer = NULL;
	}
	m_pWorld = NULL;
	m_pSceneMgr = NULL;
	AudioSystem::getInstance()->release();
	//m_pSheet = NULL;
	m_mSheet.clear();
}

void GameSystem::releaseMosquito(void)
{
	V_MOSQUITO::iterator rIte;
	MosquitoBase *body;
	for(rIte = m_vMosquito.begin(); rIte != m_vMosquito.end(); rIte++)
	{
		body = *rIte;
		body->release();
		delete body;
	}
	m_vMosquito.clear();
}

void GameSystem::releaseWeapon(void)
{
	V_WEAPON::iterator rIte;
	WeaponKnife *body;
	for(rIte = m_vWeapon.begin(); rIte != m_vWeapon.end(); rIte++)
	{
		body = *rIte;
		body->release();
		delete body;
	}
	m_vWeapon.clear();
}

void GameSystem::releaseCharacter()
{
	for(int i = 0; i < NUI_SKELETON_COUNT; i++)
	{
		//if(m_vpPlayer[i] != NULL)
		//{
		//	delete m_vpPlayer[i];
		//	m_vpPlayer[i] = NULL;
		//}
		if(m_vpPlayer == NULL)
		{
			delete m_vpPlayer;
			m_vpPlayer = NULL;
		}
	}
}

void GameSystem::restart(unsigned int stageID)
{
	ScoreSystem::resetScore();
	m_fTimePass = 0.0f;
	m_iPlayerBlood = 10;
	m_fCurrentBlood = m_iPlayerBlood;
	releaseMosquito();
	releaseWeapon();
	m_dotSceneLoader.release();


	std::string scene;
	std::string sceneGroup;
	std::string audio;
	m_waveSystem.init(stageID, scene, sceneGroup, audio);

	
	m_dotSceneLoader.parseDotScene(scene, sceneGroup, m_pSceneMgr, m_pWindow);

	m_dotSceneLoader.setAllVisible(true);

	AudioSystem::getInstance()->restart();
	AudioSystem::getInstance()->play3DBGM(audio.c_str(), m_vfCameraPos, 50.0f);

	//if(m_pSheet != NULL)
	//{
	//	m_pSheet->setVisible(true);
	//	CEGUI::Slider *pSlider = (CEGUI::Slider *)m_pSheet->getChild("Root/Timepass");
	//	pSlider->setMaxValue(m_waveSystem.getFullTime());
	//	CEGUI::MouseCursor::getSingletonPtr()->setVisible(false);

	//	CEGUI::System::getSingleton().setGUISheet(m_pSheet);

	//	CEGUI::Slider *slider = (CEGUI::Slider *)m_pSheet->getChild("Root/Timepass");

	//	slider->setCurrentValue(DEF_MAX_PLAY_TIME);
	//	slider->setMaxValue(DEF_MAX_PLAY_TIME);
	//}
	if(m_mSheet.count("playing") > 0)
	{
		CEGUI::System::getSingleton().setGUISheet(m_mSheet["playing"]);
		m_mSheet["playing"]->setVisible(true);
		CEGUI::Slider *pSlider = (CEGUI::Slider *)m_mSheet["playing"]->getChild("Playing/Timepass");
		pSlider->setCurrentValue(m_waveSystem.getFullTime());
		pSlider->setMaxValue(m_waveSystem.getFullTime());
		CEGUI::MouseCursor::getSingletonPtr()->setVisible(false);
	}
	if(m_pHintObject != NULL)
	{
		m_pHintObject->clear();
		if(m_pHintNode)
			m_pHintNode->detachAllObjects();

		m_pSceneMgr->destroyManualObject(m_pHintObject);
		m_pHintObject = NULL;
	}

	if(m_pHintNode!= NULL)
	{
		m_pSceneMgr->destroySceneNode(m_pHintNode);
		m_pHintNode = NULL;
	}

	if(m_vpPlayer != NULL)
		m_vpPlayer->setVisible(true);
	if(ChooesKnife != NULL)
		ChooesKnife->setVisible(true);
	if(ChooesBook != NULL)
		ChooesBook->setVisible(true);
	if(ChooesBomb != NULL)
		ChooesBomb->setVisible(true);
}

float GameSystem::getTimePass(void)const
{
	return m_fTimePass;
}

GameSystem::HandState GameSystem::getHandState(void)const
{
	return m_eHandState;
}

void GameSystem::setHandState(GameSystem::HandState state)
{
	m_eHandState = state;
}

void GameSystem::createShape(const char *modelName, float *scale, float *pos, float *quat)
{
	//m_vShape.push_back(new OgreShapeBox(m_pSceneMgr));
	//OgreShapeBox *shape  = (OgreShapeBox *)m_vShape.back();
	//shape->init(modelName, scale);
	//shape->update(0.0, pos, quat);
}

PhysicRigidBody *GameSystem::createRidigBody(const char *modelName, float mass, float *scale, float *pos, float *quat, PhysicDebug *debug, void *userPoint, int flag)
{
	//OgreShapeBox *shape  = new OgreShapeBox(m_pSceneMgr);
	//shape->init(modelName, scale);
	//m_vRigidBody.push_back(new PhysicRigidBody(m_pWorld));
	//PhysicRigidBody *body = m_vRigidBody.back();
	//body->init(shape, debug, mass, pos, quat, userPoint, flag);
	//
	//return body;
	return NULL;
}

void GameSystem::createMosquito(MOSQUITO_TYPE type, unsigned int moveType, float speed, unsigned int meshID, float mass, float *scale, float *pos, float *quat, int score, int otherData)
{
	if(meshID >= m_vMeshData.size())
		return;
	switch(type)
	{
	case eMosquitoSplit:
		m_vMosquito.push_back(new MosquitoSplit());
		((MosquitoSplit *)m_vMosquito.back())->setSplitNumber(otherData);
		break;
	case eMosquitoFat:
		m_vMosquito.push_back(new MosquitoFat());
		((MosquitoFat *)m_vMosquito.back())->setBloodNumber(otherData);
		break;
	default:
		m_vMosquito.push_back(new MosquitoBase());
		break;
	}
	m_vMosquito.back()->setMeshID(meshID);
	m_vMosquito.back()->init(m_pSceneMgr, m_pWorld);
	m_vMosquito.back()->create(m_vMeshData[meshID].m_sMeshName.c_str(), moveType, speed, mass, scale, pos, m_vMeshData[meshID].m_fvSize, quat, score);
	m_vMosquito.back()->setAnimation(m_vMeshData[meshID].m_vAniName[eMosquitoAniMove].c_str(), true, false);
	/*m_vMosquito.back()->setParticle("Circle");*/
}

void GameSystem::createWeapon(WEAPON_TYPE type, const char *modelName, float mass, float *scale, float *pos, float *size, float *quat, int score, int otherData, float *tar)
{
	switch(type)
	{
	case eWeaponBook:
		m_vWeapon.push_back(new WeaponBook());
		break;
	case eWeaponBomb:
		m_vWeapon.push_back(new WeaponBomb());
		break;
	default:
		m_vWeapon.push_back(new WeaponKnife());
		break;
	}
	m_vWeapon.back()->init(m_pSceneMgr, m_pWorld);
	m_vWeapon.back()->create(modelName, 1, mass, scale, pos, size, quat, score, tar);
}



void GameSystem::randomShoot(MOSQUITO_TYPE type)
{
	//m_bShoot = true;
	srand((unsigned)time(0));
	int r = rand() % 4;
	float scale[3] = {1.0, 1.0, 1.0};
	float pos[3] = {0.0, 20, -100.0};
	float dir[3] = {0, 0.4, 1.0};
	float quat[4] = {1.0, 0.0, 0.0, 0.0};
	if(r == 0)
	{
		pos[0] = 50;
	}
	else if(r == 1)
	{
		pos[0] = -50;
	}
	else if(r == 2)
	{
		pos[0] = -50;
		pos[1] = 60;
	}
	else
	{
		pos[0] = 50;
		pos[1] = 60;
	}

	float roat[3] = {0.0, 0.0, 0.0};
	float speed = 35;
	char modelName[64] = "";
	//int scoreType = rand() % 3 + 3;//3 - 5
	
	//switch(scoreType)
	//{
	//	case SCORE_TYPE_COIN:
	//		sprintf_s(modelName, "coin.mesh");
	//		break;
	//	case SCORE_TYPE_ARROW:
	//		sprintf_s(modelName, "arror.mesh");
	//		break;
	//	case SCORE_TYPE_BOMB:
	//		sprintf_s(modelName, "bomb.mesh");
	//		break;
	//	default:
	//		sprintf_s(modelName, "bomb.mesh");
	//		break;
	//}
	//OgrePhysicDebug *debug = new OgrePhysicDebug();
	//debug->init(m_pSceneMgr);
	//sprintf_s(modelName, "mosquito01.mesh");
	//PhysicRigidBody *body = createRidigBody(modelName, 1.0, scale, pos, quat, debug, ScoreSystem::createScoreObject(SCORE_TYPE_ENEMY, 100), 8);
	//body->force(dir[0], dir[1], dir[2], roat[0], roat[1], roat[2], speed);
	//createMosquito(type, modelName, 1.0, scale, pos, quat, 100);
}

void GameSystem::initScene(void)
{
	/*
	float quat[4] = {1.0, 0.0, 0.0, 0.0};
	float pos[3] = {0.0, 0.0, -50.0};
	float scale[3] = {1.0, 1.0, 1.0};
	createShape("pipeAndBrick.mesh", scale, pos, quat);

	pos[0] = 30;
	quat[2] = -0.25;
	createShape("pipeAndBrick.mesh", scale, pos, quat);

	pos[0] = -30;
	quat[2] = 0.25;
	createShape("pipeAndBrick.mesh", scale, pos, quat);

	pos[0] = -40;
	pos[2] = -60;
	createShape("rock.mesh", scale, pos, quat);

	pos[0] = -20;
	pos[2] = -80;
	scale[0] = 2.0;
	scale[1] = 3.0;
	scale[2] = 4.0;
	quat[2] = 0.05;
	createShape("rock.mesh", scale, pos, quat);

	pos[0] = 40;
	pos[2] = -60;
	scale[0] = 2.0;
	scale[1] = 3.0;
	scale[2] = 1.0;
	quat[2] = -0.1;
	createShape("rock.mesh", scale, pos, quat);

	pos[0] = 30;
	pos[2] = -100;
	scale[0] = 5.0;
	scale[1] = 6.0;
	scale[2] = 4.0;
	quat[2] = -0.15;
	createShape("rock.mesh", scale, pos, quat);

	pos[0] = 130;
	pos[2] = -300;
	scale[0] = 20.0;
	scale[1] = 22.0;
	scale[2] = 24.0;
	quat[2] = -0.1;
	createShape("rock.mesh", scale, pos, quat);

	pos[0] = -100;
	pos[2] = -300;
	scale[0] = 20.0;
	scale[1] = 24.0;
	scale[2] = 22.0;
	quat[2] = 0.2;
	createShape("rock.mesh", scale, pos, quat);*/
}

void GameSystem::initPlayer(void)
{
	//for(int i = 0; i < NUI_SKELETON_COUNT; i++)
	//{
	//	m_vpPlayer[i] = new PhysicKinect(m_pSceneMgr, m_pWorld);
	//}
	m_vpPlayer = new PhysicKinect(m_pSceneMgr, m_pWorld);

	EntSight = m_pSceneMgr->createEntity("Target","bomb.mesh");
	NodeSight = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
	NodeSight->attachObject(EntSight);
	NodeSight->setPosition(Ogre::Vector3::ZERO);
	NodeSight->setVisible(false);
}

void GameSystem::initPlayer(unsigned int playerCount)
{
	//for(int i = 0; i < playerCount; i++)
	//{
	//	m_vpPlayer[i]->init(i);
	//}
	m_vpPlayer->init(0);
}

void GameSystem::update(float timePass, KinectDevice *device)
{
	switch(m_eState)
	{
	case eOnMenu:
		updateMenu(timePass, device);
		break;
	case eOnPlaying:
		updatePlaying(timePass, device);
		break;
	case eOnEnd:
		updateEnd(timePass, device);
		break;

	default:
		break;
	}
	AudioSystem::getInstance()->update(timePass);
}

void GameSystem::updateMenu(float timePass, KinectDevice *deivce)
{
	if(m_pCompositer != NULL);
		m_pCompositer->setFocus(m_iPlayerBlood * 10);

	if(m_waveSystem.work(timePass))
	{
		if(m_vMosquito.size() <= 0)
		{
			restart(0);
		}
	}
	updateMosquito(timePass);
	if(checkPlayerState(deivce))
	{
		setGameState(eOnPlaying);
	}
}

void GameSystem::updatePlaying(float timePass, KinectDevice *deivce)
{
	if(m_bIsPause)
	{
		if(checkPlayerState(deivce))
		{
			m_bIsPause = false;
			m_vpPlayer->setVisible(true);
		}
		else
			return;
	}
	if(deivce != NULL)
		updatePlayer(deivce);
	else
		updatePlayerDebug(timePass);
	m_fTimePass += timePass;

	if(m_waveSystem.work(timePass))
	{
		if(m_vMosquito.size() <= 0)
		{
			CEGUI::MouseCursor::getSingletonPtr()->setVisible(true);
			m_eState = eOnEnd;
		}
	}

	if(m_iPlayerBlood <= 0)
	{
		CEGUI::MouseCursor::getSingletonPtr()->setVisible(true);
		m_eState = eOnEnd;
	}

	updateMosquito(timePass);
	updateWeapon(timePass);
	updateHandState(timePass);
	
	if(m_mSheet.count("playing") > 0)
	{
		{
			CEGUI::Slider *pSlider = (CEGUI::Slider *)m_mSheet["playing"]->getChild("Playing/Timepass");
			float current = m_waveSystem.getFullTime() - m_fTimePass;
			pSlider->setCurrentValue(current);
		}

		{
			int score = ScoreSystem::getScore();
			char text[128];
			sprintf_s(text, "SCORE:%016d", score);
			CEGUI::Editbox *edit = (CEGUI::Editbox *)m_mSheet["playing"]->getChild("Playing/ScoreText");
			edit->setText(CEGUI::String (text));
		}
	}
	if(m_pCompositer != NULL);
	{
		if(m_fCurrentBlood > m_iPlayerBlood)
			m_fCurrentBlood -= timePass;
		else if(m_fCurrentBlood < m_iPlayerBlood)
			m_fCurrentBlood = m_iPlayerBlood;
		m_pCompositer->setFocus(m_fCurrentBlood * 10);
	}
	testCollision();
}

void GameSystem::updateEnd(float timePass, KinectDevice *deivce)//結算畫面
{
	m_eState = eOnMenu;
}

void GameSystem::updateMosquito(float timePass)
{
	V_MOSQUITO::iterator rIte;
	V_MOSQUITO::iterator eraseIte;
	MosquitoBase *body;
	for(rIte = m_vMosquito.begin(); rIte != m_vMosquito.end();)
	{
		body = *rIte;
		body->update(timePass);

		if(body->isDestory())
		{
			eraseIte = rIte;
			rIte++;
			m_vMosquito.erase(eraseIte);
			body->release();
			delete body;
		}
		else
			rIte++;
	}
}

void GameSystem::updateWeapon(float timePass)
{
	V_WEAPON::iterator rIte;
	V_WEAPON::iterator eraseIte;
	WeaponKnife *body;
	for(rIte = m_vWeapon.begin(); rIte != m_vWeapon.end();)
	{
		body = *rIte;
		body->update(timePass);

		if(body->isDestory())
		{
			eraseIte = rIte;
			rIte++;
			m_vWeapon.erase(eraseIte);
			body->release();
			delete body;
		}
		else
			rIte++;
	}
	if(ChooesKnife != NULL)
		ChooesKnife->update(timePass);
	if(ChooesBook != NULL)
		ChooesBook->update(timePass);
	if(ChooesBomb != NULL)
		ChooesBomb->update(timePass);
}

void GameSystem::updatePlayer(KinectDevice *deivce)
{
	if(m_eState == eOnPlaying)
	{
		NUI_SKELETON_FRAME frame = {0};
		deivce->getSkeletonFrame(frame);
		bool isTracking = false;
		for(int i = 0; i < NUI_SKELETON_COUNT; i++ )
		{
			//if(frame.SkeletonData[i].eTrackingState != NUI_SKELETON_NOT_TRACKED)
			//{
			//	if(m_vpPlayer[i]->getID() != frame.SkeletonData[i].dwTrackingID)
			//	{
			//		m_vpPlayer[i]->release();
			//		m_vpPlayer[i]->init(frame.SkeletonData[i].dwTrackingID);
			//	}
			//	m_iCurrentID = i;
			//	m_vpPlayer[i]->update(frame.SkeletonData[i]);
			//}
			//else
			//{
			//	m_vpPlayer[i]->release();
			//}
			if(frame.SkeletonData[i].eTrackingState != NUI_SKELETON_NOT_TRACKED)
			{
				if(m_iCurrentID == frame.SkeletonData[i].dwTrackingID)
				{
					isTracking = true;
					m_vpPlayer->update(frame.SkeletonData[i]);
					Shoulder[0] = frame.SkeletonData[i].SkeletonPositions[4].x; // change to shoulder index (4)
					Shoulder[1] = frame.SkeletonData[i].SkeletonPositions[4].y;
					Shoulder[2] = frame.SkeletonData[i].SkeletonPositions[4].z;
				}
				//else
				//{
				//	if(m_iCurrentID == 0)
				//	{
				//		m_iCurrentID = frame.SkeletonData[i].dwTrackingID;
				//		m_vpPlayer->init(m_iCurrentID);
				//		deivce->NuiSkeletonSetTrackedSkeletons(m_iCurrentID);
				//	}
				//}
			}
		}
		if(!isTracking)
		{
			m_iCurrentID = 0;
			m_vpPlayer->setVisible(false);
			m_bIsPause = true;
		}
	}
}

void GameSystem::updatePlayerDebug(float timePass)
{
	switch(m_eDebugHandVState)
	{
	case eHandMoveUp:
		m_vfHandDebugPos[1] += timePass * 20;
		break;
	case eHandMoveDown:
		m_vfHandDebugPos[1] -= timePass * 20;
		break;
	}

	switch(m_eDebugHandHState)
	{
	case eHandMoveRight:
		m_vfHandDebugPos[0] += timePass * 1;
		break;
	case eHandMoveLeft:
		m_vfHandDebugPos[0] -= timePass * 1;
		break;
	}

	switch(m_eDebugHandAttackState)
	{
	case eHandAttacking:
		{
			m_fTwoHandDistance -= timePass * 50;
			if(m_fTwoHandDistance <= 0)
				m_eDebugHandAttackState = eHandAttacked;
		}
		break;
	case eHandAttacked:
		{
			m_fTwoHandDistance += timePass * 50;
			if(m_fTwoHandDistance >= HAND_DEBUG_DISTANCE)
			{
				m_fTwoHandDistance = HAND_DEBUG_DISTANCE;
				m_eDebugHandAttackState = eHandAttackWait;
			}
		}
		break;
	}
	//m_vpPlayer[0]->updateDebug(m_vfHandDebugPos, m_fTwoHandDistance);
	m_vpPlayer->updateDebug(m_vfHandDebugPos, m_fTwoHandDistance);
	m_vpPlayer->getPartPos(4, Shoulder);
}

void GameSystem::updateHandState(float timePass)
{
	float rightPos[3];
	float leftPos[3];
	switch(m_eHandState)
	{
	case eOnHandOpen:
		{
			//m_vpPlayer[m_iCurrentID]->getPartPos(NUI_SKELETON_POSITION_HAND_RIGHT, rightPos);
			//m_vpPlayer[m_iCurrentID]->getPartPos(NUI_SKELETON_POSITION_HAND_LEFT, leftPos);
			m_vpPlayer->getPartPos(NUI_SKELETON_POSITION_HAND_RIGHT, rightPos);
			m_vpPlayer->getPartPos(NUI_SKELETON_POSITION_HAND_LEFT, leftPos);
			Ogre::Vector3 right(rightPos);
			Ogre::Vector3 left(leftPos);
			Ogre::Vector3 shoulder(Shoulder);
			float dist = 0.0;
			//if(!m_bIsDebug)
			//{
				//dist = abs(left.x - right.x) * m_vpPlayer[m_iCurrentID]->getScale(PhysicKinect::eScaleX);
				dist = abs(left.x - right.x) * m_vpPlayer->getScale(PhysicKinect::eScaleX);
			//}
			//else
			//	dist = left.distance(right);
			if(dist < HAND_CHECK_CLOSE_DIST)
			{
				m_eHandState = eOnHandWaitAttack;
			}

			//char msg[64];
			//sprintf(msg, "rightPos[2] - leftPos[2]:%f\n", rightPos[2] - leftPos[2]);
			//m_pLog->logMessage(msg);
			//if((Shoulder[2] - leftPos[2]) > 0.55 /*&& (rightPos[2] - leftPos[2]) > 0.6*/)
			//{
			//	m_eHandState = eOnHandWaitShoot;
			//}
			float distHand = left.distance(shoulder);
			if(distHand > 0.55 && dist > 0.6) //add
			{
				m_eHandState = eOnHandWaitShoot;
			}
			//else
			{
				notifyMosquitoAlert();
			}
		}
		break;
	case eOnHandClose:
		{
			//m_vpPlayer[m_iCurrentID]->getPartPos(NUI_SKELETON_POSITION_HAND_RIGHT, rightPos);
			//m_vpPlayer[m_iCurrentID]->getPartPos(NUI_SKELETON_POSITION_HAND_LEFT, leftPos);
			m_vpPlayer->getPartPos(NUI_SKELETON_POSITION_HAND_RIGHT, rightPos);
			m_vpPlayer->getPartPos(NUI_SKELETON_POSITION_HAND_LEFT, leftPos);
			Ogre::Vector3 right(rightPos);
			Ogre::Vector3 left(leftPos);
			float dist = 0.0;
			//if(!m_bIsDebug)
			//{
				//dist = abs(left.x - right.x) * m_vpPlayer[m_iCurrentID]->getScale(PhysicKinect::eScaleX);
				dist = abs(left.x - right.x) * m_vpPlayer->getScale(PhysicKinect::eScaleX);
			//}
			//else
			//	dist = left.distance(right);
			if(dist > HAND_CHECK_OPEN_DIST)
			{
				m_eHandState = eOnHandOpen;
			}
			else
			{
				int i =0;
			}
		}
		break;
	case eOnHandWaitAttack:
		{
			m_fHandCloseTime += timePass;
			//m_vpPlayer[m_iCurrentID]->getPartPos(NUI_SKELETON_POSITION_HAND_RIGHT, rightPos);
			//m_vpPlayer[m_iCurrentID]->getPartPos(NUI_SKELETON_POSITION_HAND_LEFT, leftPos);
			m_vpPlayer->getPartPos(NUI_SKELETON_POSITION_HAND_RIGHT, rightPos);
			m_vpPlayer->getPartPos(NUI_SKELETON_POSITION_HAND_LEFT, leftPos);
			Ogre::Vector3 right(rightPos);
			Ogre::Vector3 left(leftPos);
			float dist = 0.0;
			//if(!m_bIsDebug)
			//{
				//dist = abs(left.x - right.x) * m_vpPlayer[m_iCurrentID]->getScale(PhysicKinect::eScaleX);
				dist = abs(left.x - right.x) * m_vpPlayer->getScale(PhysicKinect::eScaleX);
			//}
			//else
			//	dist = left.distance(right);
			if(dist > HAND_CHECK_OPEN_DIST)
				m_eHandState = eOnHandOpen;
			else if(m_fHandCloseTime > HAND_WAIT_ATTACK_TIMEOUT)
			{
				m_eHandState = eOnHandClose;
				m_fHandCloseTime = 0.0;
			}
		}
		break;
	case eOnHandAttacked:
		{
			//m_vpPlayer[m_iCurrentID]->getPartPos(NUI_SKELETON_POSITION_HAND_RIGHT, rightPos);
			//m_vpPlayer[m_iCurrentID]->getPartPos(NUI_SKELETON_POSITION_HAND_LEFT, leftPos);
			m_vpPlayer->getPartPos(NUI_SKELETON_POSITION_HAND_RIGHT, rightPos);
			m_vpPlayer->getPartPos(NUI_SKELETON_POSITION_HAND_LEFT, leftPos);
			Ogre::Vector3 right(rightPos);
			Ogre::Vector3 left(leftPos);
			float dist = 0.0;
			//if(!m_bIsDebug)
			//{
				//dist = abs(left.x - right.x) * m_vpPlayer[m_iCurrentID]->getScale(PhysicKinect::eScaleX);
				dist = abs(left.x - right.x) * m_vpPlayer->getScale(PhysicKinect::eScaleX);
			//}
			//else
			//	dist = left.distance(right);
			if(dist > HAND_CHECK_OPEN_DIST)
				m_eHandState = eOnHandOpen;
			else if(dist < HAND_CHECK_CLOSE_DIST)
				m_eHandState = eOnHandClose;
		}
		break;
	case eOnHandWaitShoot:
		{
			//m_vpPlayer[m_iCurrentID]->getPartPos(NUI_SKELETON_POSITION_HAND_RIGHT, rightPos);
			//m_vpPlayer[m_iCurrentID]->getPartPos(NUI_SKELETON_POSITION_HAND_LEFT, leftPos);
			m_vpPlayer->getPartPos(NUI_SKELETON_POSITION_HAND_RIGHT, rightPos);
			m_vpPlayer->getPartPos(NUI_SKELETON_POSITION_HAND_LEFT, leftPos);
			m_fShootTimePass += timePass;

			//left_hand - left_Shoulder = target_direction
			float TargetDirect[3];
			TargetDirect[0] = leftPos[0] - Shoulder[0];
			TargetDirect[1] = leftPos[1] - Shoulder[1];
			TargetDirect[2] = leftPos[2] - Shoulder[2];

			Ogre::Vector3 vShoulder = Ogre::Vector3(Shoulder[0],Shoulder[1],Shoulder[2]);
			Ogre::Vector3 vTargetDirect = Ogre::Vector3(TargetDirect[0],TargetDirect[1],TargetDirect[2]);

			Ogre::Ray SightRay(vShoulder, vTargetDirect*500);

			mRaySceneQuery = m_pSceneMgr->createRayQuery(Ogre::Ray());
			mRaySceneQuery->setRay(SightRay);
			Ogre::RaySceneQueryResult &result = mRaySceneQuery->execute();
			Ogre::RaySceneQueryResult::iterator itr = result.begin();
			float TargetPos[3];

			if (itr != result.end() && !itr->worldFragment)
			{
				Ogre::Vector3 TargetSite = itr->movable->getParentSceneNode()->getPosition();
				Ogre::Vector3 VRectify = Ogre::Vector3(vTargetDirect.x*50,vTargetDirect.y*65,vTargetDirect.z*-1); // 58 is adjust value
				Ogre::Vector3 SightSite = TargetSite + VRectify ; //maybe the site is worng 

				NodeSight->setPosition(SightSite);
				NodeSight->setVisible(true);
			}
			else
				NodeSight->setVisible(false);


			if(m_fShootTimePass > HAND_CHECK_SHOOT_TIMEPASS)
			{
				float speed = (m_fRightHandZPos - rightPos[2]) / m_fShootTimePass;
				m_fShootTimePass = 0.0f;
				char msg[128];
				sprintf(msg, "m_fRightHandZPos:%f - rightPos[2]:%f, speed:%f\n", m_fRightHandZPos, leftPos[2], speed);
				m_pLog->logMessage(msg);
				if(speed > HAND_CHECK_RIGHT_HAND_SPEED)
				{
					sprintf(msg, "shoot, m_fRightHandZPos:%f - rightPos[2]:%f, speed:%f\n", m_fRightHandZPos, leftPos[2], speed);
					m_pLog->logMessage(msg);

					float quat[4] = {1.0, 0.0, 0.0, 0.0};
					float pos[3] = {0.0,20.0,0.0};
					float scale[3] = {2.0, 2.0, 2.0};
					float size[3] = {5.0, 5.0, 5.0};
					CurrentWeapon = 0; // use knife add

					if(CurrentWeapon == 2 && NumBomb >=1) //add
					{
						createWeapon(eWeaponBomb, m_vWeapeanMeshData[2].m_sMeshName.c_str(), 1.0, scale, rightPos,  m_vWeapeanMeshData[2].m_fvSize, quat, 100,1,TargetDirect); //add
						NumBomb--;
					}
					else if(CurrentWeapon == 1 && NumBook >=1) //add
					{
						createWeapon(eWeaponBook,m_vWeapeanMeshData[1].m_sMeshName.c_str(), 1.0, scale, rightPos, m_vWeapeanMeshData[1].m_fvSize, quat, 100,1,TargetDirect); //add
						NumBook--;
					}
					else
					{
						createWeapon(eWeaponKnife,m_vWeapeanMeshData[0].m_sMeshName.c_str(), 1.0, scale, rightPos, m_vWeapeanMeshData[0].m_fvSize, quat, 100,1,TargetDirect);
					}
				}
				m_fRightHandZPos = rightPos[2];
			}
			if((rightPos[2] - leftPos[2]) <= HAND_CHECK_AIM_POSE_DIST - 1.0)
			{
				m_eHandState = eOnHandOpen;
			}
		}
		break;
	default:
		break;
	}

	if(m_eHandState != eOnHandWaitShoot)
		NodeSight->setVisible(false);
}

void GameSystem::notifyMosquitoAlert(void)
{
	float dist = 0;
	float distN = 0;
	MosquitoBase *data = NULL;
	float pos[3];
	V_MOSQUITO::iterator ite;
	float rightPos[3];
	float leftPos[3];
	m_vpPlayer->getPartPos(NUI_SKELETON_POSITION_HAND_RIGHT, rightPos);
	m_vpPlayer->getPartPos(NUI_SKELETON_POSITION_HAND_LEFT, leftPos);
	rightPos[0] *= m_vpPlayer->getScale(PhysicKinect::eScaleX);
	rightPos[1] *= m_vpPlayer->getScale(PhysicKinect::eScaleY);
	rightPos[2] *= m_vpPlayer->getScale(PhysicKinect::eScaleZ);
	leftPos[0] *= m_vpPlayer->getScale(PhysicKinect::eScaleX);
	leftPos[1] *= m_vpPlayer->getScale(PhysicKinect::eScaleY);
	leftPos[2] *= m_vpPlayer->getScale(PhysicKinect::eScaleZ);
	Ogre::Vector3 right(rightPos);
	Ogre::Vector3 left(leftPos);
	Ogre::Vector3 center = right.midPoint(left);
	for(ite = m_vMosquito.begin(); ite != m_vMosquito.end();++ite)
	{
		if((*ite)->getState() == eMosquitoAlert)
		{
			if(dist == 0)
			{
				data = *ite;
				data->getPos(pos);
				Ogre::Vector3 posM(pos);
				dist = posM.distance(center);
			}
			else
			{
				(*ite)->getPos(pos);
				Ogre::Vector3 posM(pos);
				distN = posM.distance(center);
				if(distN <= dist)
				{
					dist = distN;
					data = *ite;
				}
			}
		}
	}
	if(data != NULL)
	{
		if(m_pHintObject == NULL)
		{
			m_pHintObject = m_pSceneMgr->createManualObject("m_pHintObject");
			m_pHintNode = m_pSceneMgr->getRootSceneNode()->
				createChildSceneNode("m_pHintNode", Ogre::Vector3(0.0, 0.0, 0.0));
			m_pHintNode->attachObject(m_pHintObject);
		}
		m_pHintObject->clear();
		data->getPos(pos);
		m_pHintObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST);
		float color = 0.0;
		if(pos[2] < center.z)
		{
			color = 1 - center.z / pos[2];
			m_pHintObject->colour(color, color, color);
		}
		else if(pos[2] > center.z)
		{
			color =  center.z / pos[2];
			m_pHintObject->colour(1.0, color, color);
		}

		

		
		m_pHintObject->position(center);
		m_pHintObject->position(Ogre::Vector3(pos));
		//}
		m_pHintObject->end();
	}
	else
	{
		if(m_pHintObject != NULL)
		{
			m_pHintObject->clear();
		}
	}
}

bool GameSystem::checkPlayerState(KinectDevice *device)
{
	if(device == NULL)
	{
		//return true;
	}
	else
	{
		NUI_SKELETON_FRAME frame = {0};
		device->getSkeletonFrame(frame);
		for(int i = 0; i < NUI_SKELETON_COUNT; i++ )
		{
			if(frame.SkeletonData[i].eTrackingState != NUI_SKELETON_NOT_TRACKED)
			{
				float weight = frame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_RIGHT].z + 
								frame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_RIGHT].z + 
								frame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_WRIST_RIGHT].z + 
								frame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].z;
				if(weight > 0.5)
				{
					m_iCurrentID = frame.SkeletonData[i].dwTrackingID;
					m_vpPlayer->init(m_iCurrentID);
					device->NuiSkeletonSetTrackedSkeletons(m_iCurrentID);
					return true;
				}
			}
		}
	}
	return false;
}

float GameSystem::getFullTime(void)const												
{
	return  m_waveSystem.getFullTime();
}

void GameSystem::setGameState(GameState state)
{
	m_eState = state;
	setAllVisible(false);
	switch(state)
	{
	case eOnMenu://主選單畫面
		{
			if(m_mOverlay.count("MainMenu") > 0)
			{
				m_mOverlay["MainMenu"]->show();
			}
			
			std::string scene;
			std::string sceneGroup;
			std::string audio;
			m_waveSystem.init(0, scene, sceneGroup, audio);


			m_dotSceneLoader.parseDotScene(scene, sceneGroup, m_pSceneMgr, m_pWindow);

			m_dotSceneLoader.setAllVisible(true);

			AudioSystem::getInstance()->restart();
			AudioSystem::getInstance()->play3DBGM(audio.c_str(), m_vfCameraPos, 50.0f);
		}
		break;
	case eOnPlaying://遊戲中畫面
		{
			restart();
		}
		break;
	case eOnEnd://結算畫面
		{

		}
		break;
	default:
		break;
	}
}

void GameSystem::setAllVisible(bool visible)
{
	if(m_vpPlayer != NULL)
	{
		//for(int i = 0; i < NUI_SKELETON_COUNT; i++)
		//{
		//	m_vpPlayer[i]->setVisible(visible);
		//}
		m_vpPlayer->setVisible(visible);
	}

	{
		V_MOSQUITO::iterator ite;								
		for( ite = m_vMosquito.begin();ite != m_vMosquito.end();++ite)
		{
			(*ite)->setVisible(visible);
		}
	}

	{
		M_OGRE_WINDOW::iterator ite;								
		for( ite = m_mOverlay.begin();ite != m_mOverlay.end();++ite)
		{
			if(visible)
				ite->second->show();
			else
				ite->second->hide();
		}
	}

	//if(m_pSheet != NULL)
	//	m_pSheet->setVisible(visible);

	M_CEGUI_WINDOW::iterator ite;
	for(ite =  m_mSheet.begin(); ite != m_mSheet.end(); ++ite)
	{
		ite->second->setVisible(visible);
	}
	m_dotSceneLoader.setAllVisible(visible);

	if(ChooesKnife != NULL)
		ChooesKnife->setVisible(visible);

	if(ChooesBook != NULL)
		ChooesBook->setVisible(visible);

	if(ChooesBomb != NULL)
		ChooesBomb->setVisible(visible);

	if(m_pHintObject != NULL)
		m_pHintObject->setVisible(visible);

}

void GameSystem::testCollision()
{
	int numManifolds = m_pWorld->getDispatcher()->getNumManifolds();
	for (int i=0;i<numManifolds;i++)
	{
		btPersistentManifold* contactManifold =  m_pWorld->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* obA = static_cast<btCollisionObject*>(contactManifold->getBody0());
		btCollisionObject* obB = static_cast<btCollisionObject*>(contactManifold->getBody1());

		int flagsA = obA->getCompanionId();
		int flagsB = obB->getCompanionId();
		int numContacts = contactManifold->getNumContacts();
		for (int j=0;j<numContacts;j++)
		{
			//btManifoldPoint& pt = contactManifold->getContactPoint(j);
			//if (pt.getDistance()<0.f)
			//{
			//	const btVector3& ptA = pt.getPositionWorldOnA();
			//	const btVector3& ptB = pt.getPositionWorldOnB();
			//	const btVector3& normalOnB = pt.m_normalWorldOnB;
			//}
		}

		//if(obA->getUserPointer() != NULL && obB->getUserPointer() != NULL)
		//{
		//	//取出rigidbody內的指標...並檢查兩個的指標是否需要作用
		//	ScoreBase *object0 = (ScoreBase *)obA->getUserPointer();
		//	ScoreBase *object1 = (ScoreBase *)obB->getUserPointer();
		//	if(object0->getType() == SCORE_TYPE_BODY || object0->getType() == SCORE_TYPE_HAND)
		//	{
		//		if(ScoreSystem::calcScore(object0, object1) != 0)
		//			object1->m_bDestory = true;
		//	}
		//}
	}
}

void GameSystem::reduceBlood(void)
{
		m_iPlayerBlood--;
}