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
		release();

		Ogre::SceneManager *mgr = Ogre::Root::getSingleton().getSingleton().getSceneManager(PROJECT_NAME);
		if(mgr != NULL)
		{
			m_pManualNode = mgr->getRootSceneNode()->createChildSceneNode();

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
				m_pManuObj->clear();
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
		m_pManuObj->clear();
		m_pManuObj->begin(m_sMaterial, m_iType);
	}

	void OgreDraw::draw(const V_POINT &vect)
	{
		size_t i, size = vect.size();
		if(size < 2)
			return ;
		Ogre::Vector3 from, to;

		from.x = vect[0].x;
		from.y = 0;
		from.z = vect[0].y;

		for(i = 1; i < size; i++)
		{
			to.x = vect[i].x;
			to.y = 0;
			to.z = vect[i].y;
			m_pManuObj->position(from);
			m_pManuObj->position(to);
			from = to;
		}
	}

	void OgreDraw::draw(const LIST_POINT &list)
	{
		if(list.size() < 2)
			return ;
		LIST_POINT::const_iterator ite;
		Ogre::Vector3 from;
		Ogre::Vector3 to;
		ite = list.begin();
		from.x = ite->x;
		from.y = 0.0;
		from.z = ite->y;
		if(ite != list.end())
		{
			++ite;
			to.x = ite->x;
			to.y = 0.0;
			to.z = ite->y;
			m_pManuObj->position(from);
			m_pManuObj->position(to);
		}
		for(;ite != list.end();++ite)
		{
			from = to;
			to.x = ite->x;
			to.y = 0.0;
			to.z = ite->y;
			m_pManuObj->position(from);
			m_pManuObj->position(to);
		}
	}

	void OgreDraw::drawEnd(void)
	{
		m_pManuObj->end();
	}

	void OgreDraw::setAttribute(const std::string &materail, Ogre::RenderOperation::OperationType type)
	{
		m_sMaterial = materail;
		m_iType = type;
	}
}