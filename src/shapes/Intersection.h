#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <string>
#include <vector>

#include "accelerators/aabb.h"
#include "core/interaction.h"
#include "motion/motion.h"
#include "ray/ray.h"

#define DEBUG 1

using namespace glm;
using namespace std;

class Hitable;
class AABB;

class Hitable
{
public:
	virtual bool Hit(const Ray& ray, float tmin, float tmax, Interaction& intersect) const = 0;
	virtual bool BoundingBox(AABB& aabb) const = 0;
	virtual float PdfValue(const vec3& o, const vec3& v) const
	{
		return 0.0f;
	}
	virtual vec3 Random(const vec3& o) const
	{
		return vec3(1, 0, 0);
	}

	int materialId;
	Motion* pMotion = nullptr;
#if DEBUG
	string name;
#endif	
};

#endif
