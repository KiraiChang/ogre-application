#ifndef _PHYSIC_SHAPE_BASE_
#define _PHYSIC_SHAPE_BASE_

#include <vector>

typedef std::vector< float > V_FLOAT;

enum PHYSIC_SHPAE_TYPE
{
	ePhysicShapeBox,
	ePhysicShapeCone,
	ePhysicShapeCylinder,
	ePhysicShapeSphere,
	ePhysicShapeStaticPlane,
};

//*******************************************************
//********************  BASE  ***************************
//*******************************************************
class PhysicShapeBase
{
private:
	V_FLOAT						m_vData;
public:
								PhysicShapeBase(unsigned int dataNumber);
	virtual						~PhysicShapeBase();

	float						getData(unsigned int dataType);
	void						setData(unsigned int dataType, float data);
	virtual void				release(void);
	virtual PHYSIC_SHPAE_TYPE	getType(void) = 0;
	virtual void				update(float timePass, float *position, float *quaternion);
	virtual void				setAnimation(const char *aniName);
protected:
};
//*******************************************************
//********************  BOX  ****************************
//*******************************************************
class PhysicShapeBox : public PhysicShapeBase
{
public:
	enum
	{
		SIZE_X = 0,
		SIZE_Y,
		SIZE_Z,
		DATA_COUNT,
	};
private:
	
public:
								PhysicShapeBox(void);
	virtual						~PhysicShapeBox(void);
	virtual void				release(void);
	virtual PHYSIC_SHPAE_TYPE	getType(void);
protected:
};
//*******************************************************
//********************  CONE  ***************************
//*******************************************************
class PhysicShapeCone : public PhysicShapeBase
{
public:
	enum
	{
		RADIUS = 0,
		HEIGHT,
		AXE_X,
		AXE_Y,
		AXE_Z,
		DATA_COUNT,
	};
private:
	
public:
								PhysicShapeCone(void);
	virtual						~PhysicShapeCone(void);
	virtual void				release(void);
	virtual PHYSIC_SHPAE_TYPE	getType(void);
protected:
};
//*******************************************************
//******************  CYLINDER  *************************
//*******************************************************
class PhysicShapeCylinder : public PhysicShapeBase
{
public:
	enum
	{
		HALF_X = 0,
		HALF_Y,
		HALF_Z,
		AXE_X,
		AXE_Y,
		AXE_Z,
		DATA_COUNT,
	};
private:
	
public:
								PhysicShapeCylinder(void);
	virtual						~PhysicShapeCylinder(void);
	virtual void				release(void);
	virtual PHYSIC_SHPAE_TYPE	getType(void);
protected:
};
//*******************************************************
//*******************  SPHERE  **************************
//*******************************************************
class PhysicShapeSphere : public PhysicShapeBase
{
public:
	enum
	{
		RADIUS = 0,
		DATA_COUNT,
	};
private:
	
public:
								PhysicShapeSphere(void);
	virtual						~PhysicShapeSphere(void);
	virtual void				release(void);
	virtual PHYSIC_SHPAE_TYPE	getType(void);
protected:
};
//*******************************************************
//********************  END  ****************************
//*******************************************************

#endif //_PHYSIC_SHAPE_BASE_