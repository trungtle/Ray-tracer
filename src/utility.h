#ifndef UTILITY_H
#define UTILITY_H

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
};

#endif