#ifndef RAY_H
#define RAY_H

#include <glm/glm.hpp>
#include <math.h>
#include "math/constants.h"

using namespace glm;

class Ray
{
public:
	Ray() :
		origin(), direction()
	{

	}
    
    Ray(const Ray& other)
    {
        origin = other.origin;
        direction = other.direction;
        time = other.time;
    }

	Ray(const vec3 o, const vec3 d, float t = 0) :
		origin(o + d * EPSILON), direction(d), time(t)
	{
		direction = normalize(direction);
	}

	Ray GetCopyTransformed(const vec3& offset) const
	{
		return Ray(origin + offset, direction, time);
	}

	vec3 PointAt(float t) const
	{
		return origin + t * direction;
	}

	vec3 origin;
	vec3 direction;
	float time;
};

#endif
