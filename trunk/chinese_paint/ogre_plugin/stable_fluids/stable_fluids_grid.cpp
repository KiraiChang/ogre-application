#include "stable_fluids_grid.h"

#include <iostream>

extern void dens_step ( int N, float * x, float * x0, float * u, float * v, float diff, float dt );
extern void vel_step ( int N, float * u, float * v, float * u0, float * v0, float visc, float dt );

StableFluidsGrid::StableFluidsGrid(unsigned int number):
	m_iGridNumber(number),
	//m_fTimeSpeed(0.1f), 
	m_fDiff(0.0f), 
	m_fVisc(0.0f),
	m_fForce(5.0f), 
	m_fSource(100.0f),
	m_vfU(NULL), 
	m_vfV(NULL), 
	m_vfUPrev(NULL), 
	m_vfVPrev(NULL),
	m_vfDens(NULL),
	m_vfDensPrev(NULL)
{
	m_iGridSize = (m_iGridNumber+2)*(m_iGridNumber+2);
}

StableFluidsGrid::~StableFluidsGrid(void)
{
	release();
}

void StableFluidsGrid::init(void)
{
	m_vfU			= new float[m_iGridSize];
	m_vfV			= new float[m_iGridSize];
	m_vfUPrev		= new float[m_iGridSize];
	m_vfVPrev		= new float[m_iGridSize];
	m_vfDens		= new float[m_iGridSize];
	m_vfDensPrev	= new float[m_iGridSize];
	clear();
}

void StableFluidsGrid::release(void)
{
	if ( m_vfU ) delete ( m_vfU );
	if ( m_vfV ) delete ( m_vfV );
	if ( m_vfUPrev ) delete ( m_vfUPrev );
	if ( m_vfVPrev ) delete ( m_vfVPrev );
	if ( m_vfDens ) delete ( m_vfDens );
	if ( m_vfDensPrev ) delete ( m_vfDensPrev );
}

void StableFluidsGrid::update(float timePass)
{
	vel_step ( m_iGridNumber, m_vfU, m_vfV, m_vfUPrev, m_vfVPrev, m_fVisc, timePass);
	dens_step ( m_iGridNumber, m_vfDens, m_vfDensPrev, m_vfU, m_vfV, m_fDiff, timePass);
}

void StableFluidsGrid::clear(void)
{
	int i = 0;
	for ( i=0 ; i<m_iGridSize ; i++ ) 
	{
		m_vfU[i] = m_vfV[i] = m_vfUPrev[i] = m_vfVPrev[i] = m_vfDens[i] = m_vfDensPrev[i] = 0.0f;
	}
}