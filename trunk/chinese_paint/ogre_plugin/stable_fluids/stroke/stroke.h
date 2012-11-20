#ifndef _STROKE_H_
#define _STROKE_H_

#include "point.h"

namespace Stroke
{
	class StrokeDraw;
	class Stroke
	{
	private:
		//LIST_POINT					m_vControlPoint;
		LIST_POINT					m_vCPPositive;
		LIST_POINT					m_vCPNegative;
		Point						m_center;
		Point						m_preCenter;
		StrokeDraw *				m_pDrawPositive;
		StrokeDraw *				m_pDrawNegative;
		float						m_fExistTime;

	public:
									Stroke(float time, const V_POINT &vPoint, StrokeDraw *drawNegative, StrokeDraw *drawPositive, const Point &center, const Point &dir);
									~Stroke(void);
		void						release(void);
		void						update(float timePass, float **field, unsigned int size );
		bool						isExist(void);
	};
}

#endif //_STROKE_H_