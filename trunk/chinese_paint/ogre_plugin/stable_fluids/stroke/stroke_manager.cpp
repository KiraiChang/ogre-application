#include "stroke_manager.h"


namespace Stroke
{
	StrokeManger *StrokeManger::m_pInstance = 0;

	StrokeManger::StrokeManger()
	{
	}

	StrokeManger::~StrokeManger()
	{
		if(m_pInstance != 0)
		{
			delete m_pInstance;
		}
	}

	StrokeManger *StrokeManger::getSingleton()
	{
		if(m_pInstance == 0)
		{
			m_pInstance = new StrokeManger();
		}

		return m_pInstance;
	}

	void StrokeManger::update(float timePass)
	{
		V_STROKE::iterator ite;
		
		for(ite = m_vStroke.begin(); ite != m_vStroke.end(); ++ite)
		{
			ite->update(timePass);
			if(!ite->isExist())
			{
				ite = m_vStroke.erase(ite);
			}
		}
	}

	void StrokeManger::createStroke(float **field, unsigned int size, float existTime, const LIST_POINT &listPoint)
	{
		m_vStroke.push_back(Stroke(field, size, existTime, listPoint));
	}
}