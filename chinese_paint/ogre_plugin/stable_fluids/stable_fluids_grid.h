#ifndef _STABLE_FLUIDS_GRID_H_
#define _STABLE_FLUIDS_GRID_H_

class StableFluidsGrid
{
private:
	//float m_fTimeSpeed;
	float m_fDiff, m_fVisc;
	float m_fForce, m_fSource;

	float * m_vfU, * m_vfV, * m_vfUPrev, * m_vfVPrev;
	
public:
	int m_iGridNumber, m_iGridSize;
	float * m_vfDens, * m_vfDensPrev;
private:

public:
	StableFluidsGrid(unsigned int number);
	~StableFluidsGrid(void);
	void init(void);
	void release(void);
	void update(float timePass);
	void clear(void);
};

#endif //_STABLE_FLUIDS_GRID_H_