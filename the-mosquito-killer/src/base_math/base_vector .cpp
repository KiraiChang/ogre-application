#include "base_vector.h"

namespace CL
{
//#########################################
//###############  vec3   #################
//#########################################
	vec3::vec3(void)
	{
		set(0.0, 0.0, 0.0);
	}

	vec3::vec3(const vec3 &v)
	{
		set(v.x, v.y, v.z);
	}

	vec3::vec3(const float &f)
	{
		set(f, f, f);
	}

	vec3::vec3(const float &dx, const float &dy, const float &dz)
	{
		set(dx, dy, dz);
	}

	vec3::~vec3(void)
	{
	}

	void vec3::set(const float &dx, const float &dy, const float &dz)
	{
		x = dx;
		y = dy;
		z = dz;
	}
//#########################################
//###############  END   ##################
//#########################################
}