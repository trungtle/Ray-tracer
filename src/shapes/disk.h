#ifndef DISK_H
#define DISK_H

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "shapes/Intersection.h"

using namespace glm;
using namespace std;

class Disk : public Hitable
{
public:
	Disk(const vec3& c, float r, int mId) :
		center(c), radius(r)
	{
		materialId = mId;
	}

	virtual bool Hit(const Ray& ray, float tmin, float tmax, Interaction& intersect) const override
	{
		// Disk equation:
		// x^2 + z^2 = r^2 at height y
		// Perform plane intersection test first, then check to see if point is inside disk radius
		float t = (center.y - ray.origin.y) / ray.direction.y;
		if (t > tmin && t < tmax)
		{
			vec3 hitPoint = ray(t);
			float distToCenterSq = glm::distance2(hitPoint, center);
			if (distToCenterSq <= radius * radius)
			{
				intersect.P = hitPoint;
				intersect.t = t;
				intersect.N = N(hitPoint);
				intersect.UV = UV(hitPoint);
				intersect.hit = this;
				return true;
			}
		}
		return false;
	}	

	virtual bool BoundingBox(AABB& aabb) const override
	{
		aabb = AABB(
			vec3(center.x - radius, center.y - 0.00001f, center.z - radius),
			vec3(center.x + radius, center.y + 0.00001f, center.z + radius)
			);
		return true;
	}

	vec3 N(const vec3& point) const
	{
		return vec3(0, 1, 0);
	}

	vec2 UV(const vec3& point) const
	{
		vec2 uv;
		float theta = atan2(point.z, point.x);
		uv.x = radius * cos(theta);
		uv.y = radius * sin(theta);
		return uv;
	}


	vec3 center;
	float radius;
};

#endif
