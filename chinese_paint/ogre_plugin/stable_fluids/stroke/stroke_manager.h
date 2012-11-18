#ifndef _STROKE_MANAGER_H_
#define _STROKE_MANAGER_H_

#include "stroke.h"
#include <vector>

namespace Stroke
{
	typedef std::vector< Stroke > V_STROKE;
	class StrokeDraw;
	class StrokeManger
	{
	private:
								StrokeManger();
		static StrokeManger *	m_pInstance;
		V_STROKE				m_vStroke;
	public:
								~StrokeManger();
		static StrokeManger *	getSingleton();
		void					update(float timePass, float **field, float **prev, unsigned int size);
		void					createStroke(float existTime, const LIST_POINT &listPoint, StrokeDraw *draw);
	};
}
#endif //_STROKE_MANAGER_H_