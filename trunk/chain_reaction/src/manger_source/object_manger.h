#ifndef _OBJECT_MANGER_H_
#define _OBJECT_MANGER_H_
#include "../base_math/base_vector.h"
#include <map>
namespace CL
{
//#########################################
//############  BaseAttrib   ##############
//#########################################
class BaseObject;
enum AttribType
{
	ATTRIB_TYPE_BASE,
	ATTRIB_TYPE_OGRE_PHYSIC,
	ATTRIB_TYPE_VEHICELES,
	ATTRIB_TYPE_SOFTBODY,
};
class BaseAttrib
{
protected:
	BaseObject *mpObj;

public:
	BaseAttrib(void);
	virtual ~BaseAttrib(void);
	virtual void init(BaseObject *obj);
	virtual void release(void);
	virtual AttribType getType()const;
	BaseObject *getObj()const;
};
//#########################################
//############  BaseObject   ##############
//#########################################
class BaseObject
{
friend class ObjectManger;
private:
	static unsigned int gUID;
	BaseAttrib *mpAttrib;
	vec3 mPos;
	vec3 mScale;
	vec3 mDir;
	unsigned int mID;
public:
	//register Ogre
	std::string mName;

public:
	BaseObject(void);
	~BaseObject(void);
	void init(BaseAttrib *pAttrib);
	void release(void);
	void setPos(float *pos);
	void setScale(float *scale);
	void setDir(float *scale);
	vec3 getPos()const;
	vec3 getScale()const;
	vec3 getDir()const;
	BaseAttrib *getAttrib(void)const;
};
typedef std::map< unsigned int, BaseObject> V_OBJECT;
//#########################################
//############  BaseObject   ##############
//#########################################
class ObjectManger
{
private:
	V_OBJECT mvObject;
	static ObjectManger *mpData;

	ObjectManger(void);
public:
	~ObjectManger(void);
	static ObjectManger &fnGet();
	static BaseObject &fnAdd(BaseAttrib *pAttrib);
	static void fnClear(void);
	static void fnRelease(void);
	static V_OBJECT::iterator fnBegin();
	static V_OBJECT::iterator fnEnd();
	static BaseObject &fnGetObj(const unsigned int &id);
};
//#########################################
//###############  END   ##################
//#########################################
}
#endif //_OBJECT_MANGER_H_