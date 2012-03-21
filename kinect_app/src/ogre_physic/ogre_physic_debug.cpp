#include "ogre_physic_debug.h"
#include "btBulletDynamicsCommon.h"
#include <OgreSceneManager.h>
#include <OgreManualObject.h>

OgrePhysicDebug::OgrePhysicDebug():
		m_pSceneMgr(NULL), 
		m_pManualObject(NULL),
		m_pBodyNode(NULL)
{
}

OgrePhysicDebug::~OgrePhysicDebug(void)
{
	release();
}

void OgrePhysicDebug::init(Ogre::SceneManager *sceneMgr)
{
	m_pSceneMgr = sceneMgr;
	char name[64];
	sprintf(name, "m_pManualObject%d", g_iDebugID);
	m_pManualObject = m_pSceneMgr->createManualObject(name);

	sprintf(name, "m_pDebugBodyNode%d", g_iDebugID);
	m_pBodyNode = m_pSceneMgr->getRootSceneNode()->
		createChildSceneNode(name, Ogre::Vector3(0.0f, 0.0f, 0.0f));
	m_pBodyNode->attachObject(m_pManualObject);
	g_iDebugID++;
}

void OgrePhysicDebug::beginDraw(void)
{
	m_mBoundingBox.clear();
	if(NULL != m_pManualObject)
		m_pManualObject->clear();
}

//AABB
//vec3(min.x, min.y, min.z);
//vec3(max.x, min.y, min.z);
//vec3(max.x, max.y, min.z);
//vec3(max.x, min.y, max.z);
//vec3(max.x, max.y, max.z);
//vec3(min.x, max.y, min.z);
//vec3(min.x, max.y, max.z);
//vec3(min.x, min.y, max.z);

void OgrePhysicDebug::draw(const int &part, btRigidBody *body)
{
	if(body != NULL)
	{
		SET_VECTOR3 boundingBox;
		if(m_mBoundingBox.count(part) > 0)
		{
			boundingBox = m_mBoundingBox[part];
		}
		btVector3 min, max;
		body->getAabb(min, max);

		//boundingBox.push_back(Ogre::Vector3(min.x()*100,max.y() + 100,max.z()*100));
		//boundingBox.push_back(Ogre::Vector3(max.x()*100,max.y() + 100,max.z()*100));
		//boundingBox.push_back(Ogre::Vector3(max.x()*100,max.y() + 100,min.z()*100));
		//boundingBox.push_back(Ogre::Vector3(min.x()*100,max.y() + 100,min.z()*100));
		//boundingBox.push_back(Ogre::Vector3(min.x()*100,max.y() + 100,max.z()*100));

		//boundingBox.push_back(Ogre::Vector3(min.x()*100,min.y() - 100,max.z()*100));
		//boundingBox.push_back(Ogre::Vector3(max.x()*100,min.y() - 100,max.z()*100));
		//boundingBox.push_back(Ogre::Vector3(max.x()*100,min.y() - 100,min.z()*100));
		//boundingBox.push_back(Ogre::Vector3(min.x()*100,min.y() - 100,min.z()*100));
		//boundingBox.push_back(Ogre::Vector3(min.x()*100,min.y() - 100,max.z()*100));

		//boundingBox.push_back(Ogre::Vector3(min.x()*100,max.y() + 100,max.z()*100));
		//boundingBox.push_back(Ogre::Vector3(max.x()*100,min.y() - 100,max.z()*100));
		//boundingBox.push_back(Ogre::Vector3(max.x()*100,max.y() + 100,max.z()*100));

		//boundingBox.push_back(Ogre::Vector3(max.x()*100,min.y() - 100,min.z()*100));
		//boundingBox.push_back(Ogre::Vector3(max.x()*100,max.y() + 100,min.z()*100));
		//boundingBox.push_back(Ogre::Vector3(min.x()*100,min.y() - 100,min.z()*100));
		//boundingBox.push_back(Ogre::Vector3(min.x()*100,max.y() + 100,min.z()*100));
		//boundingBox.push_back(Ogre::Vector3(min.x()*100,min.y() - 100,max.z()*100));

		boundingBox.push_back(Ogre::Vector3(min.x(), max.y(), max.z()));
		boundingBox.push_back(Ogre::Vector3(max.x(), max.y(), max.z()));
		boundingBox.push_back(Ogre::Vector3(max.x(), max.y(), min.z()));
		boundingBox.push_back(Ogre::Vector3(min.x(), max.y(), min.z()));
		boundingBox.push_back(Ogre::Vector3(min.x(), max.y(), max.z()));

		boundingBox.push_back(Ogre::Vector3(min.x(), min.y(), max.z()));
		boundingBox.push_back(Ogre::Vector3(max.x(), min.y(), max.z()));
		boundingBox.push_back(Ogre::Vector3(max.x(), min.y(), min.z()));
		boundingBox.push_back(Ogre::Vector3(min.x(), min.y(), min.z()));
		boundingBox.push_back(Ogre::Vector3(min.x(), min.y(), max.z()));

		boundingBox.push_back(Ogre::Vector3(min.x(), max.y(), max.z()));
		boundingBox.push_back(Ogre::Vector3(max.x(), min.y(), max.z()));
		boundingBox.push_back(Ogre::Vector3(max.x(), max.y(), max.z()));

		boundingBox.push_back(Ogre::Vector3(max.x(), min.y(), min.z()));
		boundingBox.push_back(Ogre::Vector3(max.x(), max.y(), min.z()));
		boundingBox.push_back(Ogre::Vector3(min.x(), min.y(), min.z()));
		boundingBox.push_back(Ogre::Vector3(min.x(), max.y(), min.z()));
		boundingBox.push_back(Ogre::Vector3(min.x(), min.y(), max.z()));

		m_mBoundingBox[part] = boundingBox;
	}
}

void OgrePhysicDebug::endDraw(void)
{
	if(NULL != m_pManualObject)
	{
		MAP_BOUNDING_BOX::iterator ite;
		SET_VECTOR3::iterator setIte;
		for(ite = m_mBoundingBox.begin(); ite != m_mBoundingBox.end(); ++ite)
		{
			m_pManualObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST);
			for(setIte = ite->second.begin(); setIte != ite->second.end(); ++setIte)
			{
				m_pManualObject->position(*setIte);
			}
			m_pManualObject->end();
		}
	}
}

void OgrePhysicDebug::release(void)
{
	if(NULL != m_pSceneMgr)
	{
		if(NULL != m_pBodyNode)
		{
			if(NULL != m_pManualObject)
			{
				m_pManualObject->clear();
				m_pBodyNode->detachAllObjects();
				m_pSceneMgr->destroyManualObject(m_pManualObject);
				m_pManualObject = NULL;
			}

			m_pSceneMgr->destroySceneNode(m_pBodyNode);
			m_pBodyNode = NULL;
		}
		m_pSceneMgr = NULL;
	}
}




//VERTEX_AABB vb[] = {
//        {min.x,max.y,max.z, 1.0f, 1.0f, 0.0f}, // 0
//        {max.x,max.y,max.z, 1.0f, 1.0f, 0.0f}, // 1
//        {max.x,max.y,min.z, 1.0f, 1.0f, 0.0f}, // 2
//        {min.x,max.y,min.z, 1.0f, 1.0f, 0.0f}, // 3 up
//        {min.x,max.y,max.z, 1.0f, 1.0f, 0.0f}, // 0
//        
//        {min.x,min.y,max.z, 1.0f, 1.0f, 0.0f}, // 4
//        {max.x,min.y,max.z, 1.0f, 1.0f, 0.0f}, // 5
//        {max.x,min.y,min.z, 1.0f, 1.0f, 0.0f}, // 6
//        {min.x,min.y,min.z, 1.0f, 1.0f, 0.0f}, // 7 down
//        {min.x,min.y,max.z, 1.0f, 1.0f, 0.0f}, // 8 = 4
//        
//        {min.x,max.y,max.z, 1.0f, 1.0f, 0.0f}, // 9 = 0
//        {max.x,min.y,max.z, 1.0f, 1.0f, 0.0f}, // 10 = 5
//        {max.x,max.y,max.z, 1.0f, 1.0f, 0.0f}, // 11 = 1
//        
//        {max.x,min.y,min.z, 1.0f, 1.0f, 0.0f}, // 12 = 6
//        {max.x,max.y,min.z, 1.0f, 1.0f, 0.0f}, // 13 = 2
//        {min.x,min.y,min.z, 1.0f, 1.0f, 0.0f}, // 14 = 7
//        {min.x,max.y,min.z, 1.0f, 1.0f, 0.0f}, // 15 = 3
//        {min.x,min.y,max.z, 1.0f, 1.0f, 0.0f}, // 16 = 4
//	};
//glDrawArrays(GL_LINE_STRIP, 0, 17);

//Vector3 *min = pBox->getMin();
//	Vector3 *max = pBox->getMax();
//    
//	fnBeginDraw(ePrimitive_Lines);
//    
//	fnPushVertex(Vector3(min->x, min->y, min->z));
//	fnPushVertex(Vector3(max->x, min->y, min->z));
//    
//	fnPushVertex(Vector3(min->x, min->y, min->z));
//	fnPushVertex(Vector3(min->x, max->y, min->z));
//    
//	fnPushVertex(Vector3(min->x, min->y, min->z));
//	fnPushVertex(Vector3(min->x, min->y, max->z));
//    
//	fnPushVertex(Vector3(max->x, max->y, max->z));
//	fnPushVertex(Vector3(min->x, max->y, max->z));
//    
//	fnPushVertex(Vector3(max->x, max->y, max->z));
//	fnPushVertex(Vector3(max->x, min->y, max->z));
//    
//	fnPushVertex(Vector3(max->x, max->y, max->z));
//	fnPushVertex(Vector3(max->x, max->y, min->z));
//    
//	fnPushVertex(Vector3(max->x, max->y, min->z));
//	fnPushVertex(Vector3(min->x, max->y, min->z));
//    
//	fnPushVertex(Vector3(max->x, min->y, max->z));
//	fnPushVertex(Vector3(min->x, min->y, max->z));
//    
//	fnPushVertex(Vector3(max->x, max->y, min->z));
//	fnPushVertex(Vector3(max->x, min->y, min->z));
//    
//	fnPushVertex(Vector3(min->x, max->y, max->z));
//	fnPushVertex(Vector3(min->x, min->y, max->z));
//    
//	fnPushVertex(Vector3(min->x, max->y, max->z));
//	fnPushVertex(Vector3(min->x, max->y, min->z));
//    
//	fnPushVertex(Vector3(max->x, min->y, max->z));
//	fnPushVertex(Vector3(max->x, min->y, min->z));
//    
//	fnEndDraw(pRC);
//glDrawArrays(GL_LINES, 0, 24);