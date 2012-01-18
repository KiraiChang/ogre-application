#include "object_manger.h"
#include "../base_math/base_define.h"

#include <iostream>
#include <assert.h>
namespace CL
{
//#########################################
//############  BaseAttrib   ##############
//#########################################
	BaseAttrib::BaseAttrib(void):mpObj(NULL)
	{
	}

	BaseAttrib::~BaseAttrib(void)
	{
	}

	void BaseAttrib::init(BaseObject *obj)
	{
		mpObj = obj;
	}

	void BaseAttrib::release(void)
	{
	}

	AttribType BaseAttrib::getType()const
	{
		return ATTRIB_TYPE_BASE;
	}

	BaseObject *BaseAttrib::getObj()const
	{
		return mpObj;
	}

	void BaseAttrib::setPos()
	{
	}

	void BaseAttrib::setScale()
	{
	}

	void BaseAttrib::setDir()
	{
	}

	void BaseAttrib::setSize()
	{
	}

	void BaseAttrib::update()
	{
	}

//#########################################
//############  BaseObject   ##############
//#########################################
	unsigned int BaseObject::gUID = 0;
	BaseObject::BaseObject(void):mpAttrib(NULL), mDir(0.0f), mPos(0.0f), mScale(1.0f), mID(INVALID_ID)
	{
	}

	BaseObject::~BaseObject(void)
	{
		release();
	}
	void BaseObject::init(BaseAttrib *pAttrib)
	{
		assert(pAttrib);
		assert(!mpAttrib);
		assert(mID==INVALID_ID);

		mpAttrib = pAttrib;
		mpAttrib->init(this);
		mID = gUID;
		gUID++;
		char name[64];
#ifdef _WIN32
		sprintf_s(name, "BaseObject_%d", mID);
#else
		sprintf(name, "BaseObject_%d", mID);
#endif
		mName = name;
	}

	void BaseObject::release(void)
	{
		if(mpAttrib)
		{
			mpAttrib->release();
			delete mpAttrib;
			mpAttrib = NULL;
		}
	}

	void BaseObject::setPos(float *pos)
	{
		mPos.x = pos[0];
		mPos.y = pos[1];
		mPos.z = pos[2];

		if(mpAttrib)
		{
			mpAttrib->setPos();
		}
	}

	void BaseObject::setScale(float *scale)
	{
		mScale.x = scale[0];
		mScale.y = scale[1];
		mScale.z = scale[2];
		if(mpAttrib)
		{
			mpAttrib->setScale();
		}
	}

	void BaseObject::setDir(float *dir)
	{
		mDir.x = dir[0];
		mDir.y = dir[1];
		mDir.z = dir[2];
		if(mpAttrib)
		{
			mpAttrib->setDir();
		}
	}

	void BaseObject::setSize(const float &x, const float &y, const float &z)
	{
		mSize.x = x;
		mSize.y = y;
		mSize.z = z;
		if(mpAttrib)
		{
			mpAttrib->setSize();
		}
	}

	vec3 BaseObject::getPos()const
	{
		return mPos;
	}

	vec3 BaseObject::getScale()const
	{
		return mScale;
	}

	vec3 BaseObject::getDir()const
	{
		return mDir;
	}

	vec3 BaseObject::getSize()const
	{
		return mSize;
	}

	BaseAttrib *BaseObject::getAttrib(void)const
	{
		assert(mpAttrib!=NULL);
		return mpAttrib;
	}

	void BaseObject::update()
	{
		if(mpAttrib)
		{
			mpAttrib->update();
		}
	}
//#########################################
//############  BaseObject   ##############
//#########################################
	ObjectManger *ObjectManger::mpData = NULL;

	ObjectManger::ObjectManger(void)
	{
	}

	ObjectManger::~ObjectManger(void)
	{
		fnRelease();
	}

	ObjectManger &ObjectManger::fnGet()
	{
		if(!mpData)
		{
			mpData = new ObjectManger();
		}
		return *mpData;
	}

	BaseObject &ObjectManger::fnAdd(BaseAttrib *pAttrib)
	{
		unsigned int id = BaseObject::gUID;
		//fnGet().mvObject.insert(std::pair< unsigned int, BaseObject >(id, BaseObject()));
		//fnGet().mvObject.insert(std::pair< unsigned int, BaseObject >(id, BaseObject()));
		fnGet().mvObject[id] = BaseObject();
		fnGet().mvObject[id].init(pAttrib);
		return fnGet().mvObject[id];
	}

	void ObjectManger::fnClear(void)
	{
		fnGet().mvObject.clear();
	}

	void ObjectManger::fnRelease(void)
	{
		if(!mpData)
		{
			fnClear();
			delete mpData;
			mpData = NULL;
		}
	}

	V_OBJECT::iterator ObjectManger::fnBegin()
	{
		return fnGet().mvObject.begin();
	}

	V_OBJECT::iterator ObjectManger::fnEnd()
	{
		return fnGet().mvObject.end();
	}

	BaseObject &ObjectManger::fnGetObj(const unsigned int &id)
	{
		assert(fnGet().mvObject.count(id) != 0);
		return fnGet().mvObject[id];
	}

	void ObjectManger::fnUpdate()
	{
		if(mpData)
		{
			V_OBJECT::iterator ite;
			for(ite =  mpData->mvObject.begin(); ite != mpData->mvObject.end();++ite)
			{
				ite->second.update();
			}
		}
	}

//#########################################
//###############  END   ##################
//#########################################
}