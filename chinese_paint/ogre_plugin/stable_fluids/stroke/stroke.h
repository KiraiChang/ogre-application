#ifndef _STROKE_H_
#define _STROKE_H_

#include "point.h"
#include <list>

namespace Stroke
{
	class StrokeDraw;
	typedef std::list < Point > LIST_POINT;
	class Stroke
	{
	private:
		LIST_POINT					m_vControlPoint;
		StrokeDraw *				m_pDraw;
		float						m_fExistTime;

	public:
									Stroke(float time, const LIST_POINT &listPoint, StrokeDraw *draw);
									~Stroke(void);
		void						release(void);
		void						update(float timePass, float **field, float **prev, unsigned int size );
		bool						isExist(void);
	};
}

#endif //_STROKE_H_