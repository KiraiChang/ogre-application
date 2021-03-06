#include "dot_scene_loader.h"
#include "tinyxml.h"
#include <Ogre.h>
 
//using namespace std;
using namespace Ogre;

void DotSceneLoader::update(float timePass)
{
	{
		VP_DYNAMIC_OBJECT::iterator ite;
		for( ite = m_vDynamicDatas.begin(); ite != m_vDynamicDatas.end();++ite)
		{
			ite->m_fReplayTime -= timePass;
			if(ite->m_fReplayTime <= 0)
			{
				ite->m_fReplayTime = (rand() % ite->m_iMaxTime) + ite->m_iMaxAddTime;
				ite->m_fDelayReplayTime = rand() % ite->m_iMaxTime;
				Ogre::AnimationState *state = m_vpEntity[ite->m_uiEntityIndex]->getAnimationState(ite->m_sAniName);
				state->setLoop(true);
				state->setEnabled(true);
			}
			else if(ite->m_fReplayTime > ite->m_fDelayReplayTime)
			{
				Ogre::AnimationState *state =  m_vpEntity[ite->m_uiEntityIndex]->getAnimationState(ite->m_sAniName);
				state->addTime(timePass);
			}
		}
	}
}

void DotSceneLoader::setAllVisible(bool visible)
{
	{
		VP_SCENE_NODE::iterator ite;
		for(ite = m_vpSceneNode.begin();ite != m_vpSceneNode.end();++ite)
		{
			(*ite)->setVisible(visible);
		}
	}
	//VP_ENTITY m_vpEntity;
	{
		VP_LIGHT::iterator ite;
		for(ite = m_vpLight.begin();ite != m_vpLight.end();++ite)
		{
			(*ite)->setVisible(visible);
		}
	}
	//VP_CAMERA m_vpCamera;
}

void DotSceneLoader::release()
{
	SceneNode *m_pBodyNode = NULL;
	Entity *m_pBodyEnt = NULL;
	Light *pLight = NULL;
	Camera *pCamera = NULL;
	VP_SCENE_NODE::iterator nodeIte;
	VP_ENTITY::iterator entityIte;
	VP_LIGHT::iterator lightIte;
	VP_CAMERA::iterator cameraIte;
	if(m_pSceneMgr)
	{
		for(nodeIte = m_vpSceneNode.begin(); nodeIte != m_vpSceneNode.end(); ++nodeIte)
		{
			m_pBodyNode = *nodeIte;
			if(m_pBodyNode!= NULL)
			{
				m_pBodyNode->detachAllObjects();
			}
		}

		for(entityIte = m_vpEntity.begin(); entityIte != m_vpEntity.end(); ++entityIte)
		{
			m_pBodyEnt = *entityIte;
			if(m_pBodyEnt != NULL)
			{
				m_pSceneMgr->destroyEntity(m_pBodyEnt);
				m_pBodyEnt = NULL;
			}
		}
		m_vpEntity.clear();

		for(lightIte = m_vpLight.begin(); lightIte != m_vpLight.end(); ++lightIte)
		{
			pLight = *lightIte;
			if(pLight!= NULL)
			{
				m_pSceneMgr->destroyLight(pLight);
				pLight = NULL;
			}
		}
		m_vpLight.clear();

		for(cameraIte = m_vpCamera.begin(); cameraIte != m_vpCamera.end(); ++cameraIte)
		{
			pCamera = *cameraIte;
			if(pCamera!= NULL)
			{
				m_pSceneMgr->destroyCamera(pCamera);
				pCamera = NULL;
			}
		}
		m_vpCamera.clear();

		 

		for(nodeIte = m_vpSceneNode.begin(); nodeIte != m_vpSceneNode.end(); ++nodeIte)
		{
			m_pBodyNode = *nodeIte;
			if(m_pBodyNode!= NULL)
			{
				m_pBodyNode->detachAllObjects();
				m_pSceneMgr->destroySceneNode(m_pBodyNode);
				m_pBodyNode = NULL;
			}
		}
		m_vpSceneNode.clear();

		if(m_pCurrentVP != NULL)
		{
			m_pWindow->removeViewport(m_pCurrentVP->getZOrder());
			m_pCurrentVP = NULL;
		}
	}
}

void DotSceneLoader::parseDotScene(const String &SceneName, const String &groupName, SceneManager *yourSceneMgr, Ogre::RenderWindow* pWindow, SceneNode *pAttachNode, const String &sPrependNode)
{
	if(yourSceneMgr == NULL)
		return;
	// set up shared object values
	m_sGroupName = groupName;
	m_pSceneMgr = yourSceneMgr;
	m_sPrependNode = sPrependNode;
	m_pWindow = pWindow;
	staticObjects.clear();
	dynamicObjects.clear();
	m_vDynamicDatas.clear();
 
	TiXmlDocument   *XMLDoc = 0;
	TiXmlElement   *XMLRoot;
 
	try
	{
		// Strip the path
		String basename, path;
		StringUtil::splitFilename(SceneName, basename, path);
 
		DataStreamPtr pStream = ResourceGroupManager::getSingleton().
			openResource( basename, groupName );
 
		//DataStreamPtr pStream = ResourceGroupManager::getSingleton().
		//	openResource( SceneName, groupName );
 
		String data = pStream->getAsString();
		// Open the .scene File
		XMLDoc = new TiXmlDocument();
		XMLDoc->Parse( data.c_str() );
		pStream->close();
		pStream.setNull();
 
		if( XMLDoc->Error() )
		{
			//We'll just log, and continue on gracefully
			LogManager::getSingleton().logMessage("[DotSceneLoader] The TiXmlDocument reported an error");
			delete XMLDoc;
			return;
		}
	}
	catch(...)
	{
		//We'll just log, and continue on gracefully
		LogManager::getSingleton().logMessage("[DotSceneLoader] Error creating TiXmlDocument");
		delete XMLDoc;
		return;
	}
 
	// Validate the File
	XMLRoot = XMLDoc->RootElement();
	if( String( XMLRoot->Value()) != "scene"  ) {
		LogManager::getSingleton().logMessage( "[DotSceneLoader] Error: Invalid .scene File. Missing <scene>" );
		delete XMLDoc;      
		return;
	}
 
	// figure out where to attach any nodes we create
	m_pSceneRoot = pAttachNode;
	if(!m_pSceneRoot)
		m_pSceneRoot = m_pSceneMgr->getRootSceneNode();
 
	// Process the scene
	processScene(XMLRoot);
 
	// Close the XML File
	delete XMLDoc;
}
 
void DotSceneLoader::processScene(TiXmlElement *XMLRoot)
{
	// Process the scene parameters
	String version = getAttrib(XMLRoot, "formatVersion", "unknown");
 
	String message = "[DotSceneLoader] Parsing dotScene file with version " + version;
	if(XMLRoot->Attribute("ID"))
		message += ", id " + String(XMLRoot->Attribute("ID"));
	if(XMLRoot->Attribute("sceneManager"))
		message += ", scene manager " + String(XMLRoot->Attribute("sceneManager"));
	if(XMLRoot->Attribute("minOgreVersion"))
		message += ", min. Ogre version " + String(XMLRoot->Attribute("minOgreVersion"));
	if(XMLRoot->Attribute("author"))
		message += ", author " + String(XMLRoot->Attribute("author"));
 
	LogManager::getSingleton().logMessage(message);
 
	TiXmlElement *pElement;
 
	// Process nodes (?)
	pElement = XMLRoot->FirstChildElement("nodes");
	if(pElement)
		processNodes(pElement);
 
	//// Process externals (?)
	//pElement = XMLRoot->FirstChildElement("externals");
	//if(pElement)
	//	processExternals(pElement);
 
	// Process environment (?)
	pElement = XMLRoot->FirstChildElement("environment");
	if(pElement)
		processEnvironment(pElement);
 
	// Process light (?)
	pElement = XMLRoot->FirstChildElement("light");
	if(pElement)
		processLight(pElement);

	//// Process terrain (?)
	//pElement = XMLRoot->FirstChildElement("terrain");
	//if(pElement)
	//	processTerrain(pElement);
 //
	//// Process userDataReference (?)
	//pElement = XMLRoot->FirstChildElement("userDataReference");
	//if(pElement)
	//	processUserDataReference(pElement);
 //
	//// Process octree (?)
	//pElement = XMLRoot->FirstChildElement("octree");
	//if(pElement)
	//	processOctree(pElement);
 
	// Process camera (?)
	//pElement = XMLRoot->FirstChildElement("camera");
	//if(pElement)
	//	processCamera(pElement);
}
 
void DotSceneLoader::processNodes(TiXmlElement *XMLNode)
{
	TiXmlElement *pElement;
 
	// Process node (*)
	pElement = XMLNode->FirstChildElement("node");
	while(pElement)
	{
		processNode(pElement);
		pElement = pElement->NextSiblingElement("node");
	}
 
	// Process position (?)
	pElement = XMLNode->FirstChildElement("position");
	if(pElement)
	{
		m_pSceneRoot->setPosition(parseVector3(pElement));
		m_pSceneRoot->setInitialState();
	}
 
	// Process rotation (?)
	pElement = XMLNode->FirstChildElement("rotation");
	if(pElement)
	{
		m_pSceneRoot->setOrientation(parseQuaternion(pElement));
		m_pSceneRoot->setInitialState();
	}
 
	// Process scale (?)
	pElement = XMLNode->FirstChildElement("scale");
	if(pElement)
	{
		m_pSceneRoot->setScale(parseVector3(pElement));
		m_pSceneRoot->setInitialState();
	}
}
 
void DotSceneLoader::processExternals(TiXmlElement *XMLNode)
{
	//! @todo Implement this
}
 
void DotSceneLoader::processEnvironment(TiXmlElement *XMLNode)
{
	TiXmlElement *pElement;
 
	// Process fog (?)
	pElement = XMLNode->FirstChildElement("fog");
	if(pElement)
		processFog(pElement);
 
	// Process skyBox (?)
	pElement = XMLNode->FirstChildElement("skyBox");
	if(pElement)
		processSkyBox(pElement);
 
	// Process skyDome (?)
	pElement = XMLNode->FirstChildElement("skyDome");
	if(pElement)
		processSkyDome(pElement);
 
	// Process skyPlane (?)
	pElement = XMLNode->FirstChildElement("skyPlane");
	if(pElement)
		processSkyPlane(pElement);
 
	// Process clipping (?)
	pElement = XMLNode->FirstChildElement("clipping");
	if(pElement)
		processClipping(pElement);
 
	// Process colourAmbient (?)
	pElement = XMLNode->FirstChildElement("colourAmbient");
	if(pElement)
		m_pSceneMgr->setAmbientLight(parseColour(pElement));
 
	// Process colourBackground (?)
	//! @todo Set the background colour of all viewports (RenderWindow has to be provided then)
	pElement = XMLNode->FirstChildElement("colourBackground");
	if(pElement)
		;//m_pSceneMgr->set(parseColour(pElement));
 
	// Process userDataReference (?)
	pElement = XMLNode->FirstChildElement("userDataReference");
	if(pElement)
		processUserDataReference(pElement);
}
 
void DotSceneLoader::processTerrain(TiXmlElement *XMLNode)
{
	//! @todo Implement this
}
 
void DotSceneLoader::processUserDataReference(TiXmlElement *XMLNode, SceneNode *pParent)
{
	//! @todo Implement this
}
 
void DotSceneLoader::processOctree(TiXmlElement *XMLNode)
{
	//! @todo Implement this
}
 
void DotSceneLoader::processLight(TiXmlElement *XMLNode, SceneNode *pParent)
{
	// Process attributes
	String name = getAttrib(XMLNode, "name");
	String id = getAttrib(XMLNode, "id");
 
	// Create the light
	Light *pLight = m_pSceneMgr->createLight(name);
	m_vpLight.push_back(pLight);
	if(pParent)
		pParent->attachObject(pLight);
 
	String sValue = getAttrib(XMLNode, "type");
	if(sValue == "point")
		pLight->setType(Light::LT_POINT);
	else if(sValue == "directional")
		pLight->setType(Light::LT_DIRECTIONAL);
	else if(sValue == "spot")
		pLight->setType(Light::LT_SPOTLIGHT);
	else if(sValue == "radPoint")
		pLight->setType(Light::LT_POINT);
 
	pLight->setVisible(getAttribBool(XMLNode, "visible", true));
	pLight->setCastShadows(getAttribBool(XMLNode, "castShadows", true));
 
	TiXmlElement *pElement;
 
	// Process position (?)
	pElement = XMLNode->FirstChildElement("position");
	if(pElement)
		pLight->setPosition(parseVector3(pElement));
 
	// Process normal (?)
	pElement = XMLNode->FirstChildElement("normal");
	if(pElement)
		pLight->setDirection(parseVector3(pElement));
 
	// Process colourDiffuse (?)
	pElement = XMLNode->FirstChildElement("colourDiffuse");
	if(pElement)
		pLight->setDiffuseColour(parseColour(pElement));
 
	// Process colourSpecular (?)
	pElement = XMLNode->FirstChildElement("colourSpecular");
	if(pElement)
		pLight->setSpecularColour(parseColour(pElement));
 
	// Process lightRange (?)
	pElement = XMLNode->FirstChildElement("lightRange");
	if(pElement)
		processLightRange(pElement, pLight);
 
	// Process lightAttenuation (?)
	pElement = XMLNode->FirstChildElement("lightAttenuation");
	if(pElement)
		processLightAttenuation(pElement, pLight);
 
	// Process userDataReference (?)
	pElement = XMLNode->FirstChildElement("userDataReference");
	if(pElement)
		;//processUserDataReference(pElement, pLight);
}
 
void DotSceneLoader::processCamera(TiXmlElement *XMLNode, SceneNode *pParent)
{
	// Process attributes
	String name = getAttrib(XMLNode, "name");
	String id = getAttrib(XMLNode, "id");
	Real fov = getAttribReal(XMLNode, "fov", 45);
	Real aspectRatio = getAttribReal(XMLNode, "aspectRatio", 1.3333);
	String projectionType = getAttrib(XMLNode, "projectionType", "perspective");
 
	// Create the camera
	Camera *pCamera = m_pSceneMgr->createCamera(name);
	m_vpCamera.push_back(pCamera);
	if(pParent)
		pParent->attachObject(pCamera);
 
	// Set the field-of-view
	//! @todo Is this always in degrees?
	pCamera->setFOVy(Ogre::Degree(fov));
 
	// Set the aspect ratio
	pCamera->setAspectRatio(aspectRatio);
 
	// Set the projection type
	if(projectionType == "perspective")
		pCamera->setProjectionType(PT_PERSPECTIVE);
	else if(projectionType == "orthographic")
		pCamera->setProjectionType(PT_ORTHOGRAPHIC);
 
 
	TiXmlElement *pElement;
 
	// Process clipping (?)
	pElement = XMLNode->FirstChildElement("clipping");
	if(pElement)
	{
		Real nearDist = getAttribReal(pElement, "near");
		pCamera->setNearClipDistance(nearDist);
 
		Real farDist =  getAttribReal(pElement, "far");
		pCamera->setFarClipDistance(farDist);
	}
 
	// Process position (?)
	pElement = XMLNode->FirstChildElement("position");
	if(pElement)
		pCamera->setPosition(parseVector3(pElement));
 
	// Process rotation (?)
	pElement = XMLNode->FirstChildElement("rotation");
	if(pElement)
		pCamera->setOrientation(parseQuaternion(pElement));
 
	// Process normal (?)
	pElement = XMLNode->FirstChildElement("normal");
	if(pElement)
		;//!< @todo What to do with this element?
 
	// Process lookTarget (?)
	pElement = XMLNode->FirstChildElement("lookTarget");
	if(pElement)
		;//!< @todo Implement the camera look target
 
	// Process trackTarget (?)
	pElement = XMLNode->FirstChildElement("trackTarget");
	if(pElement)
		;//!< @todo Implement the camera track target
 
	// Process userDataReference (?)
	pElement = XMLNode->FirstChildElement("userDataReference");
	if(pElement)
		;//!< @todo Implement the camera user data reference

	//processViewport(pCamera);
}

void DotSceneLoader::processViewport(Camera *pCamera)
{
	if(m_pCurrentVP == NULL)
	{
		m_pCurrentVP = m_pWindow->addViewport(pCamera);
		m_pCurrentVP->setBackgroundColour(Ogre::ColourValue(0,0,0));
	}
}
 
void DotSceneLoader::processNode(TiXmlElement *XMLNode, SceneNode *pParent)
{
	// Construct the node's name
	String name = m_sPrependNode + getAttrib(XMLNode, "name");
 
	// Create the scene node
	
	SceneNode *pNode;
	if(name.empty())
	{
		// Let Ogre choose the name
		if(pParent)
			pNode = pParent->createChildSceneNode();
		else
			pNode = m_pSceneRoot->createChildSceneNode();
	}
	else
	{
		// Provide the name
		if(pParent)
			pNode = pParent->createChildSceneNode(name);
		else
			pNode = m_pSceneRoot->createChildSceneNode(name);
	}
	m_vpSceneNode.push_back(pNode);

	// Process other attributes
	String id = getAttrib(XMLNode, "id");
	bool isTarget = getAttribBool(XMLNode, "isTarget");
 
	TiXmlElement *pElement;
 
	// Process position (?)
	pElement = XMLNode->FirstChildElement("position");
	if(pElement)
	{
		pNode->setPosition(parseVector3(pElement));
		pNode->setInitialState();
	}
 
	// Process rotation (?)
	pElement = XMLNode->FirstChildElement("rotation");
	if(pElement)
	{
		pNode->setOrientation(parseQuaternion(pElement));
		pNode->setInitialState();
	}
 
	// Process scale (?)
	pElement = XMLNode->FirstChildElement("scale");
	if(pElement)
	{
		pNode->setScale(parseVector3(pElement));
		pNode->setInitialState();
	}
 
	// Process lookTarget (?)
	pElement = XMLNode->FirstChildElement("lookTarget");
	if(pElement)
		processLookTarget(pElement, pNode);
 
	// Process trackTarget (?)
	pElement = XMLNode->FirstChildElement("trackTarget");
	if(pElement)
		processTrackTarget(pElement, pNode);
 
	// Process node (*)
	pElement = XMLNode->FirstChildElement("node");
	while(pElement)
	{
		processNode(pElement, pNode);
		pElement = pElement->NextSiblingElement("node");
	}
 
	// Process entity (*)
	pElement = XMLNode->FirstChildElement("entity");
	while(pElement)
	{
		processEntity(pElement, pNode);
		pElement = pElement->NextSiblingElement("entity");
	}
 
	// Process light (*)
	pElement = XMLNode->FirstChildElement("light");
	while(pElement)
	{
		processLight(pElement, pNode);
		pElement = pElement->NextSiblingElement("light");
	}
 
	// Process camera (*)
	pElement = XMLNode->FirstChildElement("camera");
	while(pElement)
	{
		processCamera(pElement, pNode);
		pElement = pElement->NextSiblingElement("camera");
	}
 
	// Process particleSystem (*)
	pElement = XMLNode->FirstChildElement("particleSystem");
	while(pElement)
	{
		processParticleSystem(pElement, pNode);
		pElement = pElement->NextSiblingElement("particleSystem");
	}
 
	// Process billboardSet (*)
	pElement = XMLNode->FirstChildElement("billboardSet");
	while(pElement)
	{
		processBillboardSet(pElement, pNode);
		pElement = pElement->NextSiblingElement("billboardSet");
	}
 
	// Process plane (*)
	pElement = XMLNode->FirstChildElement("plane");
	while(pElement)
	{
		processPlane(pElement, pNode);
		pElement = pElement->NextSiblingElement("plane");
	}
 
	// Process userDataReference (?)
	pElement = XMLNode->FirstChildElement("userDataReference");
	if(pElement)
		processUserDataReference(pElement, pNode);
}
 
void DotSceneLoader::processLookTarget(TiXmlElement *XMLNode, SceneNode *pParent)
{
	//! @todo Is this correct? Cause I don't have a clue actually
 
	// Process attributes
	String nodeName = getAttrib(XMLNode, "nodeName");
 
	Node::TransformSpace relativeTo = Node::TS_PARENT;
	String sValue = getAttrib(XMLNode, "relativeTo");
	if(sValue == "local")
		relativeTo = Node::TS_LOCAL;
	else if(sValue == "parent")
		relativeTo = Node::TS_PARENT;
	else if(sValue == "world")
		relativeTo = Node::TS_WORLD;
 
	TiXmlElement *pElement;
 
	// Process position (?)
	Vector3 position;
	pElement = XMLNode->FirstChildElement("position");
	if(pElement)
		position = parseVector3(pElement);
 
	// Process localDirection (?)
	Vector3 localDirection = Vector3::NEGATIVE_UNIT_Z;
	pElement = XMLNode->FirstChildElement("localDirection");
	if(pElement)
		localDirection = parseVector3(pElement);
 
	// Setup the look target
	try
	{
		if(!nodeName.empty())
		{
			SceneNode *pLookNode = m_pSceneMgr->getSceneNode(nodeName);
			position = pLookNode->_getDerivedPosition();
		}
 
		pParent->lookAt(position, relativeTo, localDirection);
	}
	catch(Ogre::Exception &/*e*/)
	{
		LogManager::getSingleton().logMessage("[DotSceneLoader] Error processing a look target!");
	}
}
 
void DotSceneLoader::processTrackTarget(TiXmlElement *XMLNode, SceneNode *pParent)
{
	// Process attributes
	String nodeName = getAttrib(XMLNode, "nodeName");
 
	TiXmlElement *pElement;
 
	// Process localDirection (?)
	Vector3 localDirection = Vector3::NEGATIVE_UNIT_Z;
	pElement = XMLNode->FirstChildElement("localDirection");
	if(pElement)
		localDirection = parseVector3(pElement);
 
	// Process offset (?)
	Vector3 offset = Vector3::ZERO;
	pElement = XMLNode->FirstChildElement("offset");
	if(pElement)
		offset = parseVector3(pElement);
 
	// Setup the track target
	try
	{
		SceneNode *pTrackNode = m_pSceneMgr->getSceneNode(nodeName);
		pParent->setAutoTracking(true, pTrackNode, localDirection, offset);
	}
	catch(Ogre::Exception &/*e*/)
	{
		LogManager::getSingleton().logMessage("[DotSceneLoader] Error processing a track target!");
	}
}
 
void DotSceneLoader::processEntity(TiXmlElement *XMLNode, SceneNode *pParent)
{
	// Process attributes
	String name = getAttrib(XMLNode, "name");
	String id = getAttrib(XMLNode, "id");
	String meshFile = getAttrib(XMLNode, "meshFile");
	String materialFile = getAttrib(XMLNode, "materialFile");
	bool isStatic = getAttribBool(XMLNode, "static", true);
	bool castShadows = getAttribBool(XMLNode, "castShadows", true);
 
	// TEMP: Maintain a list of static and dynamic objects
	if(isStatic)
		staticObjects.push_back(name);
	else
		dynamicObjects.push_back(name);
 
	TiXmlElement *pElement;
 
	// Process vertexBuffer (?)
	pElement = XMLNode->FirstChildElement("vertexBuffer");
	if(pElement)
		;//processVertexBuffer(pElement);
 
	// Process indexBuffer (?)
	pElement = XMLNode->FirstChildElement("indexBuffer");
	if(pElement)
		;//processIndexBuffer(pElement);
 
	// Create the entity
	Entity *pEntity = 0;
	try
	{
		MeshManager::getSingleton().load(meshFile, m_sGroupName);
		pEntity = m_pSceneMgr->createEntity(name, meshFile);
		pEntity->setCastShadows(castShadows);
		pParent->attachObject(pEntity);
 
		if(!materialFile.empty())
			pEntity->setMaterialName(materialFile);
	}
	catch(Ogre::Exception &/*e*/)
	{
		LogManager::getSingleton().logMessage("[DotSceneLoader] Error loading an entity!");
	}
	m_vpEntity.push_back(pEntity);
	if(!isStatic)
	{
		DynamicObjectData data;
		data.m_fDelayReplayTime = 0.0;
		data.m_fReplayTime = 0.0;
		data.m_iMaxAddTime = 3.0;
		data.m_iMaxTime = 5;
		data.m_sAniName = "move";
		data.m_uiEntityIndex = m_vpEntity.size() - 1;
		m_vDynamicDatas.push_back(data);
	}
 
	// Process userDataReference (?)
	pElement = XMLNode->FirstChildElement("userDataReference");
	if(pElement)
		processUserDataReference(pElement, pEntity);
 
 
}
 
void DotSceneLoader::processParticleSystem(TiXmlElement *XMLNode, SceneNode *pParent)
{
	// Process attributes
	String name = getAttrib(XMLNode, "name");
	String id = getAttrib(XMLNode, "id");
	String file = getAttrib(XMLNode, "file");
 
	// Create the particle system
	try
	{
		ParticleSystem *pParticles = m_pSceneMgr->createParticleSystem(name, file);
		pParent->attachObject(pParticles);
	}
	catch(Ogre::Exception &/*e*/)
	{
		LogManager::getSingleton().logMessage("[DotSceneLoader] Error creating a particle system!");
	}
}
 
void DotSceneLoader::processBillboardSet(TiXmlElement *XMLNode, SceneNode *pParent)
{
	//! @todo Implement this
}
 
void DotSceneLoader::processPlane(TiXmlElement *XMLNode, SceneNode *pParent)
{
	//! @todo Implement this
}
 
void DotSceneLoader::processFog(TiXmlElement *XMLNode)
{
	// Process attributes
	Real expDensity = getAttribReal(XMLNode, "expDensity", 0.001);
	Real linearStart = getAttribReal(XMLNode, "linearStart", 0.0);
	Real linearEnd = getAttribReal(XMLNode, "linearEnd", 1.0);
 
	FogMode mode = FOG_NONE;
	String sMode = getAttrib(XMLNode, "mode");
	if(sMode == "none")
		mode = FOG_NONE;
	else if(sMode == "exp")
		mode = FOG_EXP;
	else if(sMode == "exp2")
		mode = FOG_EXP2;
	else if(sMode == "linear")
		mode = FOG_LINEAR;
 
	TiXmlElement *pElement;
 
	// Process colourDiffuse (?)
	ColourValue colourDiffuse = ColourValue::White;
	pElement = XMLNode->FirstChildElement("colourDiffuse");
	if(pElement)
		colourDiffuse = parseColour(pElement);
 
	// Setup the fog
	m_pSceneMgr->setFog(mode, colourDiffuse, expDensity, linearStart, linearEnd);
}
 
void DotSceneLoader::processSkyBox(TiXmlElement *XMLNode)
{
	// Process attributes
	String material = getAttrib(XMLNode, "material");
	Real distance = getAttribReal(XMLNode, "distance", 5000);
	bool drawFirst = getAttribBool(XMLNode, "drawFirst", true);
 
	TiXmlElement *pElement;
 
	// Process rotation (?)
	Quaternion rotation = Quaternion::IDENTITY;
	pElement = XMLNode->FirstChildElement("rotation");
	if(pElement)
		rotation = parseQuaternion(pElement);
 
	// Setup the sky box
	m_pSceneMgr->setSkyBox(true, material, distance, drawFirst, rotation, m_sGroupName);
}
 
void DotSceneLoader::processSkyDome(TiXmlElement *XMLNode)
{
	// Process attributes
	String material = XMLNode->Attribute("material");
	Real curvature = getAttribReal(XMLNode, "curvature", 10);
	Real tiling = getAttribReal(XMLNode, "tiling", 8);
	Real distance = getAttribReal(XMLNode, "distance", 4000);
	bool drawFirst = getAttribBool(XMLNode, "drawFirst", true);
 
	TiXmlElement *pElement;
 
	// Process rotation (?)
	Quaternion rotation = Quaternion::IDENTITY;
	pElement = XMLNode->FirstChildElement("rotation");
	if(pElement)
		rotation = parseQuaternion(pElement);
 
	// Setup the sky dome
	m_pSceneMgr->setSkyDome(true, material, curvature, tiling, distance, drawFirst, rotation, 16, 16, -1, m_sGroupName);
}
 
void DotSceneLoader::processSkyPlane(TiXmlElement *XMLNode)
{
	// Process attributes
	String material = getAttrib(XMLNode, "material");
	Real planeX = getAttribReal(XMLNode, "planeX", 0);
	Real planeY = getAttribReal(XMLNode, "planeY", -1);
	Real planeZ = getAttribReal(XMLNode, "planeX", 0);
	Real planeD = getAttribReal(XMLNode, "planeD", 5000);
	Real scale = getAttribReal(XMLNode, "scale", 1000);
	Real bow = getAttribReal(XMLNode, "bow", 0);
	Real tiling = getAttribReal(XMLNode, "tiling", 10);
	bool drawFirst = getAttribBool(XMLNode, "drawFirst", true);
 
	// Setup the sky plane
	Plane plane;
	plane.normal = Vector3(planeX, planeY, planeZ);
	plane.d = planeD;
	m_pSceneMgr->setSkyPlane(true, plane, material, scale, tiling, drawFirst, bow, 1, 1, m_sGroupName);
}
 
void DotSceneLoader::processClipping(TiXmlElement *XMLNode)
{
	//! @todo Implement this
 
	// Process attributes
	Real fNear = getAttribReal(XMLNode, "near", 0);
	Real fFar = getAttribReal(XMLNode, "far", 1);
}
 
void DotSceneLoader::processLightRange(TiXmlElement *XMLNode, Light *pLight)
{
	// Process attributes
	Real inner = getAttribReal(XMLNode, "inner");
	Real outer = getAttribReal(XMLNode, "outer");
	Real falloff = getAttribReal(XMLNode, "falloff", 1.0);
 
	// Setup the light range
	pLight->setSpotlightRange(Angle(inner), Angle(outer), falloff);
}
 
void DotSceneLoader::processLightAttenuation(TiXmlElement *XMLNode, Light *pLight)
{
	// Process attributes
	Real range = getAttribReal(XMLNode, "range");
	Real constant = getAttribReal(XMLNode, "constant");
	Real linear = getAttribReal(XMLNode, "linear");
	Real quadratic = getAttribReal(XMLNode, "quadratic");
 
	// Setup the light attenuation
	pLight->setAttenuation(range, constant, linear, quadratic);
}
 
 
String DotSceneLoader::getAttrib(TiXmlElement *XMLNode, const String &attrib, const String &defaultValue)
{
	if(XMLNode->Attribute(attrib.c_str()))
		return XMLNode->Attribute(attrib.c_str());
	else
		return defaultValue;
}
 
Real DotSceneLoader::getAttribReal(TiXmlElement *XMLNode, const String &attrib, Real defaultValue)
{
	if(XMLNode->Attribute(attrib.c_str()))
		return StringConverter::parseReal(XMLNode->Attribute(attrib.c_str()));
	else
		return defaultValue;
}
 
bool DotSceneLoader::getAttribBool(TiXmlElement *XMLNode, const String &attrib, bool defaultValue)
{
	if(!XMLNode->Attribute(attrib.c_str()))
		return defaultValue;
 
	if(String(XMLNode->Attribute(attrib.c_str())) == "true")
		return true;
 
	return false;
}
 
 
Vector3 DotSceneLoader::parseVector3(TiXmlElement *XMLNode)
{
	return Vector3(
		StringConverter::parseReal(XMLNode->Attribute("x")),
		StringConverter::parseReal(XMLNode->Attribute("y")),
		StringConverter::parseReal(XMLNode->Attribute("z"))
	);
}
 
Quaternion DotSceneLoader::parseQuaternion(TiXmlElement *XMLNode)
{
	//! @todo Fix this crap!
 
	Quaternion orientation;
 
	if(XMLNode->Attribute("qx"))
	{
		orientation.x = StringConverter::parseReal(XMLNode->Attribute("qx"));
		orientation.y = StringConverter::parseReal(XMLNode->Attribute("qy"));
		orientation.z = StringConverter::parseReal(XMLNode->Attribute("qz"));
		orientation.w = StringConverter::parseReal(XMLNode->Attribute("qw"));
	}
	else if(XMLNode->Attribute("axisX"))
	{
		Vector3 axis;
		axis.x = StringConverter::parseReal(XMLNode->Attribute("axisX"));
		axis.y = StringConverter::parseReal(XMLNode->Attribute("axisY"));
		axis.z = StringConverter::parseReal(XMLNode->Attribute("axisZ"));
		Real angle = StringConverter::parseReal(XMLNode->Attribute("angle"));;
		orientation.FromAngleAxis(Ogre::Angle(angle), axis);
	}
	else if(XMLNode->Attribute("angleX"))
	{
		Vector3 axis;
		axis.x = StringConverter::parseReal(XMLNode->Attribute("angleX"));
		axis.y = StringConverter::parseReal(XMLNode->Attribute("angleY"));
		axis.z = StringConverter::parseReal(XMLNode->Attribute("angleZ"));
		//orientation.FromAxes(&axis);
		//orientation.F
	}
 
	return orientation;
}
 
ColourValue DotSceneLoader::parseColour(TiXmlElement *XMLNode)
{
	return ColourValue(
		StringConverter::parseReal(XMLNode->Attribute("r")),
		StringConverter::parseReal(XMLNode->Attribute("g")),
		StringConverter::parseReal(XMLNode->Attribute("b")),
		XMLNode->Attribute("a") != NULL ? StringConverter::parseReal(XMLNode->Attribute("a")) : 1
	);
}
 
String DotSceneLoader::getProperty(const String &ndNm, const String &prop)
{
	for ( unsigned int i = 0 ; i < nodeProperties.size(); i++ )
	{
		if ( nodeProperties[i].nodeName == ndNm && nodeProperties[i].propertyNm == prop )
		{
			return nodeProperties[i].valueName;
		}
	}
 
	return "";
}
 
void DotSceneLoader::processUserDataReference(TiXmlElement *XMLNode, Entity *pEntity)
{
	String str = XMLNode->Attribute("id");
	pEntity->setUserAny(Any(str));
}