#ifndef _STROKE_DRAW_H_
#define _STROKE_DRAW_H_

#include "point.h"

namespace Stroke
{
	class StrokeDraw
	{
	public:
		virtual ~StrokeDraw(void) {};
		virtual bool isValid(void) = 0;
		virtual void init(void) = 0;
		virtual void release(void) = 0;
		virtual void drawBegin(void) = 0;
		virtual void draw(const LIST_POINT &list) = 0;
		virtual void drawEnd(void) = 0;
	};
}
#endif //_STROKE_DRAW_H_