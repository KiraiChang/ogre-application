#include "stroke.h"
#include "stroke_draw.h"

#define PARTICLE_MOVE_SPEED 100
#define IX(i,j,k) ((i)+(k+2)*(j))

namespace Stroke
{
	//void process(const V_POINT &vPoint, LIST_POINT &vCPNegative, LIST_POINT &vCPPositive, const Point &center, const Point &dir)
	//{
	//	/*
	//	method 1:
	//	y - m * x - b = 0
	//		y2 - y1
	//	a = -------
	//		x2 - x1

	//		x2*y1 - x1*y2
	//	b = -------------
	//			x2 - x1

	//	method 2:
	//	a = (y2-y1)/(x2-x1);
	//	y = a * ( x - x2 ) + y2;

	//	*/

	//	V_POINT::const_iterator ite;
	//	if(dir.x == 0)
	//	{
	//		for(ite = vPoint.begin(); ite != vPoint.end();++ite)
	//		{
	//			if(ite->y > center.y)
	//			{
	//				vCPPositive.push_back(*ite);
	//			}
	//			else
	//			{
	//				vCPNegative.push_back(*ite);
	//			}
	//		}
	//	}
	//	else
	//	{
	//		//float c = -(dir.y * center.x + (-dir.x * center.y));
	//		float a = dir.y / dir.x;
	//		//Point p2 = center + dir;
	//		//float b = (center.x*p2.y - p2.x*center.y) / (center.x - p2.x);
	//		float b = center.y - a * center.x;

	//		for(ite = vPoint.begin(); ite != vPoint.end();++ite)
	//		{
	//			float value = 1;
	//			//value = dir.y * ite->x + (-dir.x * ite->y) + c;

	//			//value = a * (ite->x - dir.x) - ite->y  + dir.y;

	//			value = a * ite->x + b - ite->y;

	//			if(value > 0)
	//			{
	//				vCPPositive.push_back(*ite);
	//			}
	//			else if(value < 0)
	//			{
	//				vCPNegative.push_back(*ite);
	//			}
	//		}
	//	}
	//}

	Stroke::Stroke(float time, const V_POINT &vPoint, StrokeDraw *drawNegative, StrokeDraw *drawPositive, const Point &center, const Point &dir):m_fExistTime(time),
		m_center(center),
		m_pDrawNegative(drawNegative),
		m_pDrawPositive(drawPositive)
	{
		m_vControlPoint.clear();
		V_POINT::const_iterator ite;
		for(ite = vPoint.begin(); ite != vPoint.end();++ite)
		{
			m_vControlPoint.push_back(*ite);
		}

		//m_vCPPositive.clear();
		//m_vCPNegative.clear();

		if(dir.x == 0)
		{
			m_fA = 0.0;
		}
		else
		{
			m_fA = dir.y / dir.x;
		}
		m_fB = center.y - m_fA * center.x;
		m_preCenter = center + dir;
		//process(vPoint, m_vCPNegative, m_vCPPositive, center, dir);
	}

	Stroke::~Stroke()
	{
		release();
	}

	void Stroke::release(void)
	{
		if(m_pDrawNegative != 0)
		{
			delete m_pDrawNegative;
			m_pDrawNegative = 0;
		}

		if(m_pDrawPositive != 0)
		{
			delete m_pDrawPositive;
			m_pDrawPositive = 0;
		}
	}

	void Stroke::classifier(const LIST_POINT &vContorlPoint, LIST_POINT &vNegative, LIST_POINT &vPositive)
	{
		LIST_POINT::const_iterator ite;
		for(ite = vContorlPoint.begin(); ite != vContorlPoint.end();++ite)
		{
			//float value = 1;
			//value = dir.y * ite->x + (-dir.x * ite->y) + c;

			//value = a * (ite->x - dir.x) - ite->y  + dir.y;

			float value = m_fA * ite->x + m_fB - ite->y;

			if(value > 0)
			{
				vPositive.push_back(*ite);
			}
			else
			{
				vNegative.push_back(*ite);
			}
		}
	}

	void process(LIST_POINT &vControlPoint, float **field, unsigned int size)
	{

		LIST_POINT::iterator ite;
		unsigned int index;
		float u0, u1, v0, v1, u, v;
		int x0, x1, y0, y1;

		for(ite = vControlPoint.begin(); ite != vControlPoint.end();++ite)
		{
			x0 = ite->x; y0 = ite->y;
			x1 = x0 + 1; y1 = y0 + 1;
			u1 = ite->x-x0; u0 = 1-u1; v1 = ite->y-y0; v0 = 1-v1;
			u = u0*(v0*field[0][IX(x0,y0,size)]+v1*field[0][IX(x0,y1,size)])+
				u1*(v0*field[0][IX(x1,y0,size)]+v1*field[0][IX(x1,y1,size)]);

			v = u0*(v0*field[1][IX(x0,y0,size)]+v1*field[1][IX(x0,y1,size)])+
				u1*(v0*field[1][IX(x1,y0,size)]+v1*field[1][IX(x1,y1,size)]);

			ite->x += u * PARTICLE_MOVE_SPEED; ite->y += v * PARTICLE_MOVE_SPEED;	
		}
	}

	void Stroke::update(float timePass, float **field, unsigned int size)
	{
		if(m_pDrawNegative == 0 || m_pDrawPositive == 0)
			return;
		m_fExistTime -= timePass;
		LIST_POINT vNegative, vPositive;
		if(field != 0)
		{

			//process(m_vCPNegative, field, size);
			//process(m_vCPPositive, field, size);

			process(m_vControlPoint, field, size);
			classifier(m_vControlPoint, vNegative, vPositive);
		}
		if(m_pDrawNegative->isValid())
		{
			m_pDrawNegative->drawBegin();
			m_pDrawNegative->draw(vNegative);
			LIST_POINT vPoint;
			vPoint.push_back(m_preCenter);
			vPoint.push_back(m_center);
			m_pDrawNegative->draw(vPoint);
			m_pDrawNegative->drawEnd();
		}

		if(m_pDrawPositive->isValid())
		{
			m_pDrawPositive->drawBegin();
			m_pDrawPositive->draw(vPositive);
			m_pDrawPositive->drawEnd();
		}
	}

	bool Stroke::isExist()
	{
		return m_fExistTime > 0;
	}
}