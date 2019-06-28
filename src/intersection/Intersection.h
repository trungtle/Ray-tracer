#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <string>
#include <vector>
#include "intersection/aabb.h"
#include "motion/motion.h"
#include "ray/ray.h"

#define DEBUG 1

using namespace glm;
using namespace std;

class Hitable;
class AABB;

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
	virtual bool BoundingBox(AABB& aabb) const = 0;

	int materialId;
	Motion* pMotion = nullptr;
#if DEBUG
	string name;
#endif	
};

#endif