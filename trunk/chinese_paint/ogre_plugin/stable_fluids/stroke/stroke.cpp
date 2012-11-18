#include "stroke.h"
#include "stroke_draw.h"

#define IX(i,j,k) ((i)+(k+2)*(j))
namespace Stroke
{
	Stroke::Stroke(float time, const LIST_POINT &listPoint, StrokeDraw *draw):m_fExistTime(time),
		m_pDraw(draw),
		m_vControlPoint(listPoint)
	{

	}

	Stroke::~Stroke()
	{
		release();
	}

	void Stroke::release(void)
	{
		if(m_pDraw != 0)
		{
			delete m_pDraw;
			m_pDraw = 0;
		}
	}

	void Stroke::update(float timePass, float **field, float **prev, unsigned int size)
	{
		if(m_pDraw == 0)
			return;
		m_fExistTime -= timePass;
		LIST_POINT::iterator ite;
		unsigned int index;
		float u0, u1, v0, v1, u, v;
		int x0, x1, y0, y1;
		if(field != 0 && prev != 0)
		{
			m_pDraw->drawBegin();
			for(ite = m_vControlPoint.begin(); ite != m_vControlPoint.end();++ite)
			{
				x0 = ite->x; x1 = ite->y;
				x1 = x0 + 1; y1 = y0 + 1;
				u1 = ite->x-x0; u0 = 1-u1; v1 = ite->y-y0; v0 = 1-v1;
				u = u0*(v0*field[0][IX(x0,y0,size)]+v1*field[0][IX(x0,y1,size)])+
					u1*(v0*field[0][IX(x1,y0,size)]+v1*field[0][IX(x1,y1,size)]);

				v = u0*(v0*field[1][IX(x0,y0,size)]+v1*field[1][IX(x0,y1,size)])+
					u1*(v0*field[1][IX(x1,y0,size)]+v1*field[1][IX(x1,y1,size)]);

				ite->x += u; ite->y += v;
				m_pDraw->draw(*ite);
			}
			m_pDraw->drawEnd();
		}
	}

	bool Stroke::isExist()
	{
		return m_fExistTime <= 0;
	}
}