#include "stroke.h"
#define IX(i,j,k) ((i)+(k+2)*(j))
namespace Stroke
{
	Stroke::Stroke(float **field, unsigned int size, float time, const LIST_POINT &listPoint):m_pField(field), 
		m_fExistTime(time),
		m_uiFieldSize(size),
		m_vControlPoint(listPoint)
	{

	}

	Stroke::~Stroke()
	{

	}

	void Stroke::update(float timePass)
	{
		m_fExistTime -= timePass;
		LIST_POINT::iterator ite;
		unsigned int index;
		float u0, u1, v0, v1, u, v;
		int x0, x1, y0, y1;
		if(m_pField != 0)
		{
			for(ite = m_vControlPoint.begin(); ite != m_vControlPoint.end();++ite)
			{
				x0 = ite->x; x1 = ite->y;
				x1 = x0 + 1; y1 = y0 + 1;
				u1 = ite->x-x0; u0 = 1-u1; v1 = ite->y-y0; v0 = 1-v1;
				u = u0*(v0*m_pField[0][IX(x0,y0,m_uiFieldSize)]+v1*m_pField[0][IX(x0,y1,m_uiFieldSize)])+
					u1*(v0*m_pField[0][IX(x1,y0,m_uiFieldSize)]+v1*m_pField[0][IX(x1,y1,m_uiFieldSize)]);

				v = u0*(v0*m_pField[1][IX(x0,y0,m_uiFieldSize)]+v1*m_pField[1][IX(x0,y1,m_uiFieldSize)])+
					u1*(v0*m_pField[1][IX(x1,y0,m_uiFieldSize)]+v1*m_pField[1][IX(x1,y1,m_uiFieldSize)]);
			}
		}
	}

	bool Stroke::isExist()
	{
		return m_fExistTime <= 0;
	}
}