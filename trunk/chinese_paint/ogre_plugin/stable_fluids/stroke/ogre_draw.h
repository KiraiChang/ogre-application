#ifndef _OGRE_DRAW_H_
#define _OGRE_DRAW_H_
#include "stroke_draw.h"
#include <ogre.h>
#include <string>

namespace Stroke
{
	class OgreDraw : StrokeDraw
	{
	private:
		Ogre::ManualObject *					m_pManuObj;
		Ogre::SceneNode *						m_pManualNode;
		std::string								m_sMaterial;
		Ogre::RenderOperation::OperationType	m_iType;

	public:
		OgreDraw(void);
		virtual ~OgreDraw(void);
		virtual bool isValid(void);
		virtual void init(void);
		virtual void release(void);
		virtual void drawBegin(void);
		virtual void draw(const V_POINT &list, float *density);
		virtual void draw(const LIST_POINT &list, float *density);
		virtual void drawEnd(void);
		void setAttribute(const std::string &materail, Ogre::RenderOperation::OperationType type);
	};
}

#endif //_OGRE_DRAW_H_