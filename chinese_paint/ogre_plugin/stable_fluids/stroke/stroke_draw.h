#ifndef _STROKE_DRAW_H_
#define _STROKE_DRAW_H_

#include "point.h"
namespace Stroke
{
	class StrokeDraw
	{
		virtual void init(void) = 0;
		virtual void drawBegin(void) = 0;
		virtual void draw(const Point &from, const Point &to) = 0;
		virtual void drawEnd(void) = 0;
	};
}
#endif //_STROKE_DRAW_H_