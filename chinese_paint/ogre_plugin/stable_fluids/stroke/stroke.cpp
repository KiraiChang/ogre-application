#include "stroke.h"
#include "stroke_draw.h"
#include <algorithm>

#define PARTICLE_MOVE_SPEED 100
#define THRESHOLD 0.3
#define ADD_DENSITY 0.001

extern void draw_line(Stroke::V_POINT &vControlPoint, float *grid, unsigned int gridSize, float value, bool absolute);
extern void particle_move(Stroke::LIST_POINT &vControlPoint, float **field, unsigned int N, float moveSpeed);

namespace Stroke
{
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

		/*
		method y=ax+b :
		a = (y2-y1)/(x2-x1);
		y = a * ( x - x2 ) + y2;
		*/

		if(dir.x < 0.0001 && dir.x > -0.0001)
		{
			m_fA = 1000.0;
		}
		else
		{
			m_fA = dir.y / dir.x;
		}
		m_fB = center.y - m_fA * center.x;
		m_preCenter = center + dir;
		//printf("%fx+%f=y\n", m_fA, m_fB);
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

	void Stroke::classifier(const LIST_POINT &vContorlPoint, V_POINT &vNegative, V_POINT &vPositive)
	{
		LIST_POINT::const_iterator ite;
		for(ite = vContorlPoint.begin(); ite != vContorlPoint.end();++ite)
		{
			float value = 1;
			if(m_fA < 1000)
				value = m_fA * ite->x + m_fB - ite->y;
			else
			{
				value = ite->x - m_center.x;
			}

			if(value > THRESHOLD)
			{
				vPositive.push_back(*ite);
			}
			else if(value < -THRESHOLD)
			{
				vNegative.push_back(*ite);
			}
		}
		std::sort(vPositive.begin(), vPositive.end());
		std::sort(vNegative.begin(), vNegative.end());
	}

	void Stroke::update(float timePass, float **field, float *density, unsigned int size)
	{
		m_fExistTime -= timePass;
		V_POINT vNegative, vPositive;
		if(field != 0)
		{
			particle_move(m_vControlPoint, field, size, PARTICLE_MOVE_SPEED);
			classifier(m_vControlPoint, vNegative, vPositive);
			draw_line(vNegative, density, size, timePass * ADD_DENSITY, true);
			draw_line(vPositive, density, size, timePass * ADD_DENSITY, true);
		}
		if(m_pDrawNegative != 0 && m_pDrawNegative->isValid())
		{
			m_pDrawNegative->drawBegin();
			m_pDrawNegative->draw(vNegative, density);
			LIST_POINT vPoint;
			vPoint.push_back(m_preCenter);
			vPoint.push_back(m_center);
			m_pDrawNegative->draw(vPoint, NULL);
			m_pDrawNegative->drawEnd();
		}

		if(m_pDrawPositive != 0 && m_pDrawPositive->isValid())
		{
			m_pDrawPositive->drawBegin();
			m_pDrawPositive->draw(vPositive, density);
			m_pDrawPositive->drawEnd();
		}
	}

	bool Stroke::isExist()
	{
		return m_fExistTime > 0;
	}
}