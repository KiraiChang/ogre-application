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
		if(m_pManuObj != NULL)
		{
			Ogre::SceneManager *mgr = Ogre::Root::getSingleton().getSingleton().getSceneManager(PROJECT_NAME);
			if(mgr != NULL)
			{
				m_pManualNode->detachObject(m_pManuObj);
			}
		}
	}

	void OgreDraw::init(void)
	{

	}

	void OgreDraw::drawBegin(void)
	{

	}

	void OgreDraw::draw(const Point &from, const Point &to)
	{

	}

	void OgreDraw::drawEnd(void)
	{

	}
}