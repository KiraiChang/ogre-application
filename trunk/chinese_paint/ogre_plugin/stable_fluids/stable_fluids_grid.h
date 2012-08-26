#ifndef _STABLE_FLUIDS_GRID_H_
#define _STABLE_FLUIDS_GRID_H_
#include "../simulation_interface/water_interface.h"

namespace Ogre
{
	class ManualObject ;
	class SceneManager ;
	class SceneNode;
}

class StableFluidsGrid : public WaterInterface
{
private:
	//float									m_fTimeSpeed;
	float									m_fDiff;
	float									m_fVisc;
	float									m_fForce;
	float									m_fSource;
	Ogre::ManualObject *					m_pManuObj;
	Ogre::SceneManager *					m_pSceneMgr;
	Ogre::SceneNode *						m_pManualNode;

public:
	float *									m_vfU;
	float *									m_vfV;
	float *									m_vfUPrev;
	float *									m_vfVPrev;
	int										m_iGridNumber;
	int										m_iGridSize;
	//float *								m_vfDens;
	//float *								m_vfDensPrev;
private:

public:
	StableFluidsGrid(unsigned int number);
	~StableFluidsGrid(void);
	void init(Ogre::SceneManager *mgr);
	void release(void);
	void updateMesh(float timePass);
	void updateDebug();
	void push(float x, float y, float depth, bool absolute=false) ;
	void clear(void);
};

#endif //_STABLE_FLUIDS_GRID_H_