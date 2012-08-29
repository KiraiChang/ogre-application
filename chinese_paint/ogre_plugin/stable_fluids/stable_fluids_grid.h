#ifndef _STABLE_FLUIDS_GRID_H_
#define _STABLE_FLUIDS_GRID_H_
#include "../simulation_interface/water_interface.h"
#include <ogre.h>

class StableFluidsGrid : public WaterInterface
{
private:
	//float									m_fTimeSpeed;
	float									m_fDiff;
	float									m_fVisc;
	float									m_fForce;
	float									m_fSource;
	float *									m_vfHeightMap[3] ; // 3 map to do wave equation
	float *									m_vfDumpening;
	int										m_iCurrentMap ;
	Ogre::HardwarePixelBufferSharedPtr		m_heightMap;
	Ogre::PixelBox*							m_pPixelBox;
	Ogre::ManualObject *					m_pManuObj;
	Ogre::SceneManager *					m_pSceneMgr;
	Ogre::SceneNode *						m_pManualNode;
	Ogre::Rectangle2D *						m_pMiniScreen;
	Ogre::SceneNode	*						m_pMiniScreenNode;


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