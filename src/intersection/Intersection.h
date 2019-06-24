#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <vector>
#include "ray/ray.h"

using namespace glm;
using namespace std;

class Hitable;

struct Intersection
{	
	float t;
	vec3 P;
	vec3 N;
	const Hitable* hit;
};

class Hitable
{
public:
	virtual bool Hit(const Ray& ray, float tmin, float tmax, Intersection& intersect) const = 0;

	int materialId;
};

#endif