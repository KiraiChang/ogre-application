#ifndef _OGRE_PHYSIC_SHAPE_
#define _OGRE_PHYSIC_SHAPE_

#include "..\physic\physic_shape_base.h"

namespace Ogre
{
	class SceneManager;
	class Entity;
	class SceneNode;
	class Vector3;
	class AnimationState;
	class ParticleSystem;
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
	Ogre::AnimationState *		m_pAnimationState;
	Ogre::ParticleSystem *		m_pParticleSystem;
public:
								OgreShapeBox(Ogre::SceneManager *scene);
	virtual						~OgreShapeBox(void);
	virtual void				init(const char *meshName, float *scale);
	virtual void				release(void);
	virtual void				update(float timePass, float *position, float *quaternion);
	virtual void				update(float timePass);
	virtual void				setAnimation(const char *aniName, bool loop, bool blend);
	virtual void				setParticle(const char *fileName);
	virtual void				setVisible(bool visible);
	Ogre::SceneNode *			getNode(void)const							{return m_pBodyNode;}
protected:
};
/*
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
	Ogre::AnimationState *		m_pAnimationState;
public:
								OgreShapeCone(Ogre::SceneManager *scene, unsigned int id, const char *meshName);
	virtual						~OgreShapeCone(void);
	virtual void				release(void);
	virtual void				update(float timePass, float *position, float *quaternion);
	virtual void				setAnimation(const char *aniName);
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
	Ogre::AnimationState *		m_pAnimationState;
public:
								OgreShapeCylinder(Ogre::SceneManager *scene, unsigned int id, const char *meshName);
	virtual						~OgreShapeCylinder(void);
	virtual void				release(void);
	virtual void				update(float timePass, float *position, float *quaternion);
	virtual void				setAnimation(const char *aniName);
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
	Ogre::AnimationState *		m_pAnimationState;
public:
								OgreShapeSphere(Ogre::SceneManager *scene);
	virtual						~OgreShapeSphere(void);
	virtual void				init(const char *meshName, float *scale);
	virtual void				release(void);
	virtual void				update(float timePass, float *position, float *quaternion);
	virtual void				setAnimation(const char *aniName);
protected:
};
//*******************************************************
//********************  END  ****************************
//*******************************************************
*/
#endif //_OGRE_PHYSIC_SHAPE_