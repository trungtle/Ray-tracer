#ifndef UTILITY_H
#define UTILITY_H

#include <glm/glm.hpp>
using namespace glm;

#define EPSILON 0.00001f;

namespace Tr
{
	vec3 lerp(float t, const vec3& fullT, const vec3& zeroT)
	{
		return (1.0f - t) * zeroT + t * fullT;
	}

	bool AreSame(float a, float b)
	{
	    return fabs(a - b) < EPSILON;
	}

	vec3 deNaN(const vec3& v)
	{
		vec3 temp = v;
		if (!(temp.x == temp.x)) temp.x = 0;
		if (!(temp.y == temp.y)) temp.y = 0;
		if (!(temp.z == temp.z)) temp.z = 0;
		return temp;
	}
};

#endif