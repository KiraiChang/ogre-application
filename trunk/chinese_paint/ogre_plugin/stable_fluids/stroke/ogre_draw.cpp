#include "ogre_draw.h"

#define PROJECT_NAME "ChinesePaint"

namespace Stroke
{
	OgreDraw::OgreDraw(void):m_pManuObj(NULL), 
		m_pManualNode(NULL)
	{

	}

	OgreDraw::~OgreDraw(void)
	{
		release();
	}

	bool OgreDraw::isValid(void)
	{
		return m_pManuObj != NULL && m_pManualNode != NULL;
	}

	void OgreDraw::init(void)
	{
		if(m_pManuObj == NULL)
			release();

		Ogre::SceneManager *mgr = Ogre::Root::getSingleton().getSingleton().getSceneManager(PROJECT_NAME);
		if(mgr != NULL)
		{
			m_pManualNode = mgr->createSceneNode();

			m_pManuObj = mgr->createManualObject();

			m_pManualNode->attachObject(m_pManuObj);
		}
	}

	void OgreDraw::release(void)
	{
		if(m_pManuObj != NULL)
		{
			Ogre::SceneManager *mgr = Ogre::Root::getSingleton().getSingleton().getSceneManager(PROJECT_NAME);
			if(mgr != NULL)
			{
				m_pManualNode->detachObject(m_pManuObj);
				mgr->destroySceneNode(m_pManualNode);
				m_pManualNode = NULL;
				mgr->destroyManualObject(m_pManuObj);
				m_pManuObj = NULL;
			}
		}
	}

	void OgreDraw::drawBegin(void)
	{
		m_pManuObj->begin(m_sMaterial, m_iType);
	}

	void OgreDraw::draw(const Point &pos)
	{
		Ogre::Vector3 o;
		o.x = pos.x;
		o.y = 0.0;
		o.z = pos.y;
		m_pManuObj->position(o);
	}

	void OgreDraw::drawEnd(void)
	{
		m_pManuObj->end();
	}

	void OgreDraw::setMaterial(const std::string &materail, Ogre::RenderOperation::OperationType type)
	{
		m_sMaterial = materail;
		m_iType = type;
	}
}