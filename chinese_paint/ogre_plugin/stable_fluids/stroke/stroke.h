#ifndef _STROKE_H_
#define _STROKE_H_

#include "point.h"

namespace Stroke
{
	class StrokeDraw;
	class Stroke
	{
	private:
		LIST_POINT					m_vControlPoint;
		StrokeDraw *				m_pDraw;
		float						m_fExistTime;

	public:
									Stroke(float time, const V_POINT &vPoint, StrokeDraw *draw);
									~Stroke(void);
		void						release(void);
		void						update(float timePass, float **field, unsigned int size );
		bool						isExist(void);
	};
}

#endif //_STROKE_H_