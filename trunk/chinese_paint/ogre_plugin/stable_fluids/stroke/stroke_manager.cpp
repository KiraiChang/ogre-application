#include "stroke_manager.h"
#include "stroke_draw.h"

#define DEFAULT_DELAY_CREATE 1.0//0.33;

namespace Stroke
{
	StrokeManager *StrokeManager::m_pInstance = 0;

	StrokeManager::StrokeManager():m_fDelayTime(0.0f)
	{
	}

	StrokeManager::~StrokeManager()
	{
		release();
		if(m_pInstance != 0)
		{
			delete m_pInstance;
		}
	}

	StrokeManager *StrokeManager::getSingleton()
	{
		if(m_pInstance == 0)
		{
			m_pInstance = new StrokeManager();
		}

		return m_pInstance;
	}

	void StrokeManager::release(void)
	{
		//V_STROKE::iterator ite;
		//Stroke *stroke = NULL;
		//for(ite = m_vStroke.begin(); ite != m_vStroke.end(); ++ite)
		//{
		//	stroke = *ite;
		//	ite = m_vStroke.erase(ite);
		//	delete stroke;
		//	stroke = NULL;
		//}
		LIST_STROKE::iterator ite;
		LIST_STROKE::iterator del_ite;
		Stroke *stroke = NULL;
		for(ite = m_listStroke.begin(); ite != m_listStroke.end();)
		{
			del_ite = ite;
			++ite;
			stroke = *del_ite;
			m_listStroke.erase(del_ite);
			delete stroke;
			stroke = NULL;
		}
	}

	void StrokeManager::update(float timePass, float **field, float *density, unsigned int size)
	{
		m_fDelayTime -= timePass;
		//V_STROKE::iterator ite;
		//V_STROKE::iterator del_ite;
		//Stroke *stroke = NULL;

		//for(ite = m_vStroke.begin(); ite != m_vStroke.end();)
		//{
		//	del_ite = ite;
		//	++ite;
		//	stroke = *del_ite;
		//	stroke->update(timePass, field, size);
		//	if(!stroke->isExist())
		//	{
		//		m_vStroke.erase(del_ite);
		//		delete stroke;
		//		stroke = NULL;
		//	}
		//}

		LIST_STROKE::iterator ite;
		LIST_STROKE::iterator del_ite;
		Stroke *stroke = NULL;

		for(ite = m_listStroke.begin(); ite != m_listStroke.end();)
		{
			del_ite = ite;
			++ite;
			stroke = *del_ite;
			stroke->update(timePass, field, density, size);
			if(!stroke->isExist())
			{
				m_listStroke.erase(del_ite);
				delete stroke;
				stroke = NULL;
			}
		}
	}

	void StrokeManager::createStroke(float existTime, const V_POINT &vPoint, StrokeDraw *drawNegative, StrokeDraw *drawPositive, const Point &center)
	{
		if(m_fDelayTime <= 0)
		{
			m_fDelayTime = DEFAULT_DELAY_CREATE;
			//m_vStroke.push_back(new Stroke(existTime, vPoint, draw));
			Point dir = m_preCenter - center;
			m_preCenter = center;
			m_listStroke.push_back(new Stroke(existTime, vPoint, drawNegative, drawPositive, center, dir));
		}
		else
		{
			delete drawNegative;
			drawNegative = 0;

			delete drawPositive;
			drawPositive = 0;
		}
	}
}