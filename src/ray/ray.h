#ifndef RAY_H
#define RAY_H

#include <glm/glm.hpp>

using namespace glm;

class Ray
{
public:
	Ray(const vec3 o, const vec3 d) :
		origin(o), direction(d)
	{
		normalizedDirection = normalize(direction);
	}

	vec3 PointAt(float t)
	{
		return origin + t * direction;
	}

	vec3 origin;
	vec3 direction;
	vec3 normalizedDirection;
};

#endif