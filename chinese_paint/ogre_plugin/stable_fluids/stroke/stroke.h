#ifndef _STROKE_H_
#define _STROKE_H_

#include "point.h"
#include <list>

namespace Stroke
{
	typedef std::list < Point > LIST_POINT;
	class Stroke
	{
	private:
		LIST_POINT					m_vControlPoint;
		float **					m_pField;
		unsigned int				m_uiFieldSize;
		float						m_fExistTime;

	public:
									Stroke(float **field, unsigned int size, float time, const LIST_POINT &listPoint);
									~Stroke();
		void						update(float timePass);
		bool						isExist();
	};
}

#endif //_STROKE_H_