#include "physic_shape_base.h"

//*******************************************************
//********************  BASE  ***************************
//*******************************************************
PhysicShapeBase::PhysicShapeBase(unsigned int dataNumber)
{
	m_vData.resize(dataNumber);
}

PhysicShapeBase::~PhysicShapeBase()
{
}

float PhysicShapeBase::getData(unsigned int dataType)
{
	if(m_vData.size() > dataType)
	{
		return m_vData[dataType];
	}
	return 0;
}

void PhysicShapeBase::setData(unsigned int dataType, float data)
{
	if(m_vData.size() > dataType)
	{
		m_vData[dataType] = data;
	}
}

void PhysicShapeBase::release(void)
{
	release();
}

void PhysicShapeBase::update(float *pos, float *qua)
{
}
void PhysicShapeBase::setAnimation(const char *aniName)
{
}
//*******************************************************
//********************  BOX  ****************************
//*******************************************************
PhysicShapeBox::PhysicShapeBox():PhysicShapeBase(DATA_COUNT)
{
}

PhysicShapeBox::~PhysicShapeBox()
{
}

PHYSIC_SHPAE_TYPE PhysicShapeBox::getType(void)
{
	return ePhysicShapeBox;
}

void PhysicShapeBox::release(void)
{
	PhysicShapeBase::release();
}
//*******************************************************
//********************  CONE  ***************************
//*******************************************************
PhysicShapeCone::PhysicShapeCone():PhysicShapeBase(DATA_COUNT)
{
}

PhysicShapeCone::~PhysicShapeCone()
{
}

PHYSIC_SHPAE_TYPE PhysicShapeCone::getType(void)
{
	return ePhysicShapeCone;
}

void PhysicShapeCone::release(void)
{
	PhysicShapeBase::release();
}
//*******************************************************
//******************  CYLINDER  *************************
//*******************************************************
PhysicShapeCylinder::PhysicShapeCylinder():PhysicShapeBase(DATA_COUNT)
{
}

PhysicShapeCylinder::~PhysicShapeCylinder()
{
}

PHYSIC_SHPAE_TYPE PhysicShapeCylinder::getType(void)
{
	return ePhysicShapeCylinder;
}

void PhysicShapeCylinder::release(void)
{
	PhysicShapeBase::release();
}
//*******************************************************
//******************  SPHERE  *************************
//*******************************************************
PhysicShapeSphere::PhysicShapeSphere():PhysicShapeBase(DATA_COUNT)
{
}

PhysicShapeSphere::~PhysicShapeSphere()
{
}

PHYSIC_SHPAE_TYPE PhysicShapeSphere::getType(void)
{
	return ePhysicShapeSphere;
}

void PhysicShapeSphere::release(void)
{
	PhysicShapeBase::release();
}
//*******************************************************
//********************  END  ****************************
//*******************************************************