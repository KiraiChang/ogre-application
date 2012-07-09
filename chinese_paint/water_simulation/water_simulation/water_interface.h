#ifndef _WATER_INTERFACE_H_
#define _WATER_INTERFACE_H_

class WaterInterface
{
public:
	virtual void updateMesh(float timeSinceLastFrame) = 0;
};

#endif //_WATER_INTERFACE_H_