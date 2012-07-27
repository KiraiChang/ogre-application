#include "water_simulation.h"
#include "OgreBillboardParticleRenderer.h"
#include <Ogre.h>
#include <SdkSample.h>
#include "water_mesh.h"

#define MESH_NAME "WaterMesh"
#define ENTITY_NAME "WaterEntity"
#define MATERIAL_NAME "Examples/Water0"
#define COMPLEXITY 128 		// watch out - number of polys is 2*ACCURACY*ACCURACY !
#define PLANE_SIZE 3000.0f
//#define PLANE_SIZE 6000.0f
#define CIRCLES_MATERIAL "Examples/Water/Circles"

#define PANEL_WIDTH 200
#define RAIN_HEIGHT_RANDOM 5
#define RAIN_HEIGHT_CONSTANT 5

WaterSimulation::WaterSimulation(Ogre::SceneManager *sceneMgr, Ogre::Camera* cam):m_pSceneMgr(sceneMgr),
																m_pCamera(cam),
																m_fTimeoutDelay(0.0f),
																m_fHeadDepth(2.0f),
																m_fMoveSpeed(1.0f)
																
{


}

WaterSimulation::~WaterSimulation(void)
{
	release();
}

void WaterSimulation::init(void)
{
	// Set ambient light
	m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.75, 0.75, 0.75));

	// Create a light
	Ogre::Light* l = m_pSceneMgr->createLight("MainLight");
	// Accept default settings: point light, white diffuse, just set position
	// NB I could attach the light to a SceneNode if I wanted it to move automatically with
	//  other objects, but I don't
	l->setPosition(200,300,100);

	//// Create water mesh and entity
	m_pWaterInterface = new WaterMesh(MESH_NAME, PLANE_SIZE, COMPLEXITY);
	m_pWaterEntity = m_pSceneMgr->createEntity(ENTITY_NAME,
		MESH_NAME);
	m_pWaterEntity->setMaterialName(MATERIAL_NAME);
	Ogre::SceneNode *waterNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
	waterNode->attachObject(m_pWaterEntity);

	// Add a head, give it it's own node
	m_pHeadNode = waterNode->createChildSceneNode();
	//m_pHeadNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
	Ogre::Entity *ent = m_pSceneMgr->createEntity("head", "ogrehead.mesh");
	m_pHeadNode->attachObject(ent);

	// Make sure the camera track this node
	//~ mCamera->setAutoTracking(true, headNode);

	// Create the camera node, set its position & attach camera
	Ogre::SceneNode* camNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
	camNode->translate(1500, 3000, 1500);
	//camNode->yaw(Ogre::Degree(-45));
	camNode->pitch(Ogre::Degree(-90));
	camNode->attachObject(m_pCamera);

	// Create light node
	Ogre::SceneNode* lightNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
	lightNode->attachObject(l);

	// set up spline animation of light node
	Ogre::Animation* anim = m_pSceneMgr->createAnimation("WaterLight", 20);
	Ogre::NodeAnimationTrack *track ;
	Ogre::TransformKeyFrame *key ;
	// create a random spline for light
	track = anim->createNodeTrack(0, lightNode);
	key = track->createNodeKeyFrame(0);
	for(int ff=1;ff<=19;ff++) {
		key = track->createNodeKeyFrame(ff);
		Ogre::Vector3 lpos (
			rand()%(int)PLANE_SIZE , //- PLANE_SIZE/2,
			rand()%300+100,
			rand()%(int)PLANE_SIZE //- PLANE_SIZE/2
			);
		key->setTranslate(lpos);
	}
	key = track->createNodeKeyFrame(20);

	//// Create a new animation state to track this
	//m_pAnimState = m_pSceneMgr->createAnimationState("WaterLight");
	//m_pAnimState->setEnabled(true);

	////Put in a bit of fog for the hell of it
	////m_pSceneMgr->setFog(Ogre::FOG_EXP, Ogre::ColourValue::White, 0.0002);

	//// Let there be rain
	//m_pParticleSystem = m_pSceneMgr->createParticleSystem("rain",
	//	"Examples/Water/Rain");
	//m_pParticleEmitter = m_pParticleSystem->getEmitter(0);
	//Ogre::SceneNode* rNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
	//rNode->translate(PLANE_SIZE/2.0f, 3000, PLANE_SIZE/2.0f);
	//rNode->attachObject(m_pParticleSystem);
	//// Fast-forward the rain so it looks more natural
	//m_pParticleSystem->fastForward(20);
	//// It can't be set in .particle file, and we need it ;)
	//static_cast<Ogre::BillboardParticleRenderer*>(m_pParticleSystem->getRenderer())->setBillboardOrigin(Ogre::BillboardOrigin::BBO_BOTTOM_CENTER);

	prepareCircleMaterial();
}

void WaterSimulation::release(void)
{
	if(m_pSceneMgr != NULL)
	{

	}
}

void WaterSimulation::update(float timePass)
{
	if(m_fMoveSpeed > 0)
	{
		float t = timePass * m_fMoveSpeed;
		processCircles(t);

		processParticles();

		animateHead(t);

		m_fTimeoutDelay-=t ;
		if (m_fTimeoutDelay<=0)
			m_fTimeoutDelay = 0;

		m_pWaterInterface->updateMesh(t);
	}
}

/** Head animation */
void WaterSimulation::animateHead(Ogre::Real timeSinceLastFrame)
{
	// sine track? :)
	static double sines[4] = {0,100,200,300};
	static const double adds[4] = {0.3,-1.6,1.1,0.5};
	for(int i=0;i<4;i++) {
		sines[i]+=adds[i]*timeSinceLastFrame*0.3;
	}
	Ogre::Real tx = ((sin(sines[0]) + sin(sines[1])) / 4 + 0.5 ) * (float)(COMPLEXITY-2) + 1 ;
	Ogre::Real ty = ((sin(sines[2]) + sin(sines[3])) / 4 + 0.5 ) * (float)(COMPLEXITY-2) + 1 ;

	//static double sine = 0;
	//static const double MOVE = 0.5;
	//sine += MOVE * timeSinceLastFrame;

	////Fish Head
	//Ogre::Real tx = (sin(sine / 2) / 3 + 0.5 ) * (float)(COMPLEXITY-2) + 1 ;
	//Ogre::Real ty = (sin(sine * 3) / 32 + 0.5 ) * (float)(COMPLEXITY-2) + 1 ;
	((WaterMesh *)m_pWaterInterface)->push(tx,ty, -m_fHeadDepth);

	////Fish Body
	//Ogre::Real dx1 = (sin((sine / 2)-0.01) / 2 + 0.5 ) * (float)(COMPLEXITY-2) + 1 ;
	//Ogre::Real dx2 = (sin((sine / 2)-0.02) / 2 + 0.5 ) * (float)(COMPLEXITY-2) + 1 ;
	//Ogre::Real dx3 = (sin((sine / 2)-0.03) / 2 + 0.5 ) * (float)(COMPLEXITY-2) + 1 ;
	//Ogre::Real dy1 = (sin(sine * 2) / 32 + 0.5 ) * (float)(COMPLEXITY-2) + 1 ;
	//Ogre::Real dy2 = (sin(sine * 3) / 32 + 0.5 ) * (float)(COMPLEXITY-2) + 1 ;
	//Ogre::Real dy3 = (sin(sine * 4) / 32 + 0.5 ) * (float)(COMPLEXITY-2) + 1 ;
	//((WaterMesh *)m_pWaterInterface)->push(dx1,dy1, -m_fHeadDepth);
	//((WaterMesh *)m_pWaterInterface)->push(dx2,dy2, -m_fHeadDepth);
	//((WaterMesh *)m_pWaterInterface)->push(dx3,dy3, -m_fHeadDepth);

	Ogre::Real step = PLANE_SIZE / COMPLEXITY ;
	m_pHeadNode->resetToInitialState();
	//m_pHeadNode->scale(3,3,3);
	Ogre::Vector3 newPos = Ogre::Vector3(step*tx, m_fHeadDepth, step*ty);
	static Ogre::Vector3 oldPos = Ogre::Vector3::UNIT_Z;
	Ogre::Vector3 diffPos = newPos - oldPos ;
	Ogre::Quaternion headRotation = Ogre::Vector3::UNIT_Z.getRotationTo(diffPos);
	oldPos = newPos ;
	m_pHeadNode->translate(newPos);
	m_pHeadNode->rotate(headRotation);
}

void WaterSimulation::prepareCircleMaterial()
{
	char *bmap = new char[256 * 256 * 4] ;
	memset(bmap, 127, 256 * 256 * 4);
	for(int b=0;b<16;b++) {
		int x0 = b % 4 ;
		int y0 = b >> 2 ;
		Ogre::Real radius = 4.0f + 1.4 * (float) b ;
		for(int x=0;x<64;x++) {
			for(int y=0;y<64;y++) {
				Ogre::Real dist = Ogre::Math::Sqrt((x-32)*(x-32)+(y-32)*(y-32)); // 0..ca.45
				dist = fabs(dist -radius -2) / 2.0f ;
				dist = dist * 255.0f;
				if (dist>255)
					dist=255 ;
				int colour = 255-(int)dist ;
				colour = (int)( ((Ogre::Real)(15-b))/15.0f * (Ogre::Real) colour );

				bmap[4*(256*(y+64*y0)+x+64*x0)+0]=colour ;
				bmap[4*(256*(y+64*y0)+x+64*x0)+1]=colour ;
				bmap[4*(256*(y+64*y0)+x+64*x0)+2]=colour ;
				bmap[4*(256*(y+64*y0)+x+64*x0)+3]=colour ;
			}
		}
	}

	Ogre::DataStreamPtr imgstream(new Ogre::MemoryDataStream(bmap, 256 * 256 * 4));
	//~ Image img;
	//~ img.loadRawData( imgstream, 256, 256, PF_A8R8G8B8 );
	//~ TextureManager::getSingleton().loadImage( CIRCLES_MATERIAL , img );
	Ogre::TextureManager::getSingleton().loadRawData(CIRCLES_MATERIAL,
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		imgstream, 256, 256, Ogre::PF_A8R8G8B8);
	Ogre::MaterialPtr material =
		Ogre::MaterialManager::getSingleton().create( CIRCLES_MATERIAL,
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	Ogre::TextureUnitState *texLayer = material->getTechnique(0)->getPass(0)->createTextureUnitState( CIRCLES_MATERIAL );
	texLayer->setTextureAddressingMode( Ogre::TextureUnitState::TAM_CLAMP );
	material->setSceneBlending( Ogre::SBT_ADD );
	material->setDepthWriteEnabled( false ) ;
	material->load();
	// finished with bmap so release the memory
	delete [] bmap;
}

void WaterSimulation::processCircles(Ogre::Real timeSinceLastFrame)
{
	//for(unsigned int i=0;i<circles.size();i++) {
	//	circles[i]->animate(timeSinceLastFrame);
	//}
	//bool found ;
	//do 
	//{
	//	found = false ;
	//	for(WaterCircles::iterator it = circles.begin() ;
	//			it != circles.end();
	//			++it) 
	//	{
	//		if ((*it)->lvl>=16) {
	//			delete (*it);
	//			circles.erase(it);
	//			found = true ;
	//			break ;
	//		}
	//	}
	//} while (found) ;
}

void WaterSimulation::processParticles()
{
	//static int pindex = 0 ;
	//Ogre::ParticleIterator pit = m_pParticleSystem->_getIterator() ;
	//while(!pit.end()) {
	//	Ogre::Particle *particle = pit.getNext();
	//	Ogre::Vector3 ppos = particle->position;
	//	if (ppos.y<=0 && particle->timeToLive>0) { // hits the water!
	//		// delete particle
	//		particle->timeToLive = 0.0f;
	//		// push the water
	//		float x = ppos.x / PLANE_SIZE * COMPLEXITY ;
	//		float y = ppos.z / PLANE_SIZE * COMPLEXITY ;
	//		float h = rand() % RAIN_HEIGHT_RANDOM + RAIN_HEIGHT_CONSTANT ;
	//		if (x<1) x=1 ;
	//		if (x>COMPLEXITY-1) x=COMPLEXITY-1;
	//		if (y<1) y=1 ;
	//		if (y>COMPLEXITY-1) y=COMPLEXITY-1;
	//		((WaterMesh *)m_pWaterInterface)->push(x,y,-h) ;
	//		//WaterCircle *circle = new WaterCircle(
	//		//	"Circle#"+StringConverter::toString(pindex++),
	//		//	x, y);
	//		//circles.push_back(circle);
	//	}
	//}
}

void WaterSimulation::setWaterMaterialName(const std::string &name)
{
	m_pWaterEntity->setMaterialName(name);
}

void WaterSimulation::setMoveSpeed(const float &speed)
{
	m_fMoveSpeed = speed;
}