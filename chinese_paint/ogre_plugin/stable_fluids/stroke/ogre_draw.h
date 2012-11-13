#ifndef _OGRE_DRAW_H_
#define _OGRE_DRAW_H_
#include "stroke_draw.h"
#include <ogre.h>

namespace Stroke
{
	class OgreDraw : StrokeDraw
	{
	private:
		Ogre::ManualObject *					m_pManuObj;
		Ogre::SceneNode *						m_pManualNode;
	public:
		OgreDraw(void);
		~OgreDraw(void);
		virtual void init(void);
		virtual void drawBegin(void);
		virtual void draw(const Point &from, const Point &to);
		virtual void drawEnd(void);
	};
}

#endif //_OGRE_DRAW_H_