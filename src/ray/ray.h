#ifndef RAY_H
#define RAY_H

#include <glm/glm.hpp>
#include <math.h>

using namespace glm;

class Ray
{
public:
	Ray() :
		origin(), direction()
	{

	}

	Ray(const vec3 o, const vec3 d) :
		origin(o), direction(d)
	{
		direction = normalize(direction);
	}

	vec3 PointAt(float t) const
	{
		return origin + t * direction;
	}

	vec3 origin;
	vec3 direction;
};

#endif