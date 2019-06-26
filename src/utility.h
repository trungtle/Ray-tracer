#ifndef UTILITY_H
#define UTILITY_H

namespace Tr
{
	vec3 lerp(float t, const vec3& fullT, const vec3& zeroT)
	{
		return (1.0f - t) * zeroT + t * fullT;
	}

};

#endif