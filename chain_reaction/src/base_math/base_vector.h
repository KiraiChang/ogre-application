#ifndef _BASE_VECTOR_H_
#define _BASE_VECTOR_H_

namespace CL
{
//#########################################
//###############  vec3   #################
//#########################################
class vec3
{
public:
	union
	{
		struct
		{
			float x, y, z;
		};
		float mvValue[3];
	};
public:
	vec3(void);
	vec3(const vec3 &v);
	vec3(const float &f);
	vec3(const float &dx, const float &dy, const float &dz);
	~vec3(void);
	void set(const float &dx, const float &dy, const float &dz);
};
//#########################################
//###############  END   ##################
//#########################################
}

#endif //_BASE_VECTOR_H_