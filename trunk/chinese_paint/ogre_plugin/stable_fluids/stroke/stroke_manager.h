#ifndef _STROKE_MANAGER_H_
#define _STROKE_MANAGER_H_

#include "stroke.h"
#include <vector>

namespace Stroke
{
	typedef std::vector< Stroke > V_STROKE;
	class StrokeManger
	{
	private:
								StrokeManger();
		static StrokeManger *	m_pInstance;
		V_STROKE				m_vStroke;
	public:
								~StrokeManger();
		static StrokeManger *	getSingleton();
		void					update(float timePass);
		void					createStroke(float **field, unsigned int size, float existTime, const LIST_POINT &listPoint);
	};
}
#endif //_STROKE_MANAGER_H_