#ifndef HITABLE_TRANSFORM_H
#define HITABLE_TRANSFORM_H

#include "intersection/Intersection.h"

class FlipNormal : public Hitable
{
public:
	FlipNormal(Hitable* h) : hitable(h)
	{
		name = "FlipNormal, " + hitable->name;
	}

	virtual bool Hit(const Ray& ray, float tmin, float tmax, Intersection& intersect) const
	{
		if (hitable->Hit(ray, tmin, tmax, intersect))
		{
			intersect.N = -intersect.N;
			return true;
		}
		return false;
	}
	virtual bool BoundingBox(AABB& aabb) const
	{
		return hitable->BoundingBox(aabb);
	}

private:
	Hitable* hitable;
};

class Translate : public Hitable
{
public:
	Translate(Hitable* h, const vec3& o) :
		hitable(h), offset(o)
	{
		name = "Translate, " + hitable->name;
	}

	virtual bool Hit(const Ray& ray, float tmin, float tmax, Intersection& intersect) const
	{
		Ray transformedRay = ray.GetCopyTransformed(-offset);	
		if (hitable->Hit(transformedRay, tmin, tmax, intersect))
		{
			intersect.P += offset;
			return true;
		}
		return false;
	}
	virtual bool BoundingBox(AABB& aabb) const
	{
		if (hitable->BoundingBox(aabb))
		{
			aabb = AABB(aabb._min + offset, aabb._max + offset);
			return true;
		}
		return false;
	}

private:
	Hitable* hitable;
	vec3 offset;
};

#endif