#ifndef RAY_H
#define RAY_H

#include <glm/glm.hpp>
#include <cmath>
#include "math/constants.h"

using namespace glm;

class Medium;

class Ray
{
public:
	Ray() :
		origin(), direction(), time(0.f), tMax(INFINITY), medium(nullptr)
	{

	}
    
    Ray(const Ray& other)
    {
        origin = other.origin;
        direction = other.direction;
        time = other.time;
    }

	Ray(const vec3& o, const vec3& d, float t = 0, float tMax = INFINITY, const Medium* medium = nullptr) :
		origin(o + d * EPSILON), direction(d), time(t), tMax(tMax), medium(medium)
	{
		direction = normalize(direction);
	}

	Ray GetCopyTransformed(const vec3& offset) const
	{
		return Ray(origin + offset, direction, time);
	}

	vec3 operator()(float t) const
	{
		return origin + t * direction;
	}

	vec3 origin;
	vec3 direction;
	float time;
    
    // Mutable so we could adjust the distance where we evaluate the ray during interection test.
    // This is a common approach with raytracer.
    mutable float tMax;
    const Medium* medium;
};

#endif
