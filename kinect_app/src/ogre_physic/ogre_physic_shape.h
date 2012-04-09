#ifndef _OGRE_PHYSIC_SHAPE_
#define _OGRE_PHYSIC_SHAPE_

#include "..\physic\physic_shape_base.h"

namespace Ogre
{
	class SceneManager;
	class Entity;
	class SceneNode;
	class Vector3;
}

//*******************************************************
//********************  BOX  ****************************
//*******************************************************
class OgreShapeBox : public PhysicShapeBox
{
public:

private:
	Ogre::SceneManager *		m_pSceneMgr;
	Ogre::SceneNode *			m_pBodyNode;
	Ogre::Entity *				m_pBodyEnt;
public:
								OgreShapeBox(Ogre::SceneManager *scene);
	virtual						~OgreShapeBox(void);
	virtual void				init(const char *meshName, float *scale);
	virtual void				release(void);
	virtual void				update(float *position, float *quaternion);
protected:
};
//*******************************************************
//********************  CONE  ***************************
//*******************************************************
class OgreShapeCone : public PhysicShapeCone
{
public:

private:
	Ogre::SceneManager *		m_pSceneMgr;
	Ogre::SceneNode *			m_pBodyNode;
	Ogre::Entity *				m_pBodyEnt;
public:
								OgreShapeCone(Ogre::SceneManager *scene, unsigned int id, const char *meshName);
	virtual						~OgreShapeCone(void);
	virtual void				release(void);
	virtual void				update(float *position, float *quaternion);
protected:
};
//*******************************************************
//******************  CYLINDER  *************************
//*******************************************************
class OgreShapeCylinder : public PhysicShapeCylinder
{
public:

private:
	Ogre::SceneManager *		m_pSceneMgr;
	Ogre::SceneNode *			m_pBodyNode;
	Ogre::Entity *				m_pBodyEnt;
public:
								OgreShapeCylinder(Ogre::SceneManager *scene, unsigned int id, const char *meshName);
	virtual						~OgreShapeCylinder(void);
	virtual void				release(void);
	virtual void				update(float *position, float *quaternion);
protected:
};
//*******************************************************
//*******************  SPHERE  **************************
//*******************************************************
class OgreShapeSphere : public PhysicShapeSphere
{
public:

private:
	Ogre::SceneManager *		m_pSceneMgr;
	Ogre::SceneNode *			m_pBodyNode;
	Ogre::Entity *				m_pBodyEnt;
public:
								OgreShapeSphere(Ogre::SceneManager *scene);
	virtual						~OgreShapeSphere(void);
	virtual void				init(const char *meshName, float *scale);
	virtual void				release(void);
	virtual void				update(float *position, float *quaternion);
protected:
};
//*******************************************************
//********************  END  ****************************
//*******************************************************


#endif //_OGRE_PHYSIC_SHAPE_