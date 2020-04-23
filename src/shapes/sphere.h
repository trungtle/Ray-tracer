#ifndef SPHERE_H
#define SPHERE_H

#include <math.h>

#include "accelerators/AABB.h"
#include "shapes/Intersection.h"

#define DEBUG 1

using namespace glm;
using namespace std;

class Sphere : public Hitable
{
public:
	Sphere(const vec3& c, float r, int m = 0) :
		center(c), radius(r)
	{
		materialId = m;
		radiusSq = radius * radius;
#if DEBUG		
		name = "sphere";
#endif
	}

	virtual bool Hit(const Ray& ray, float tmin, float tmax, Interaction& intersect) const
	{
		// Sphere equation
		// (x - cx)^2 + (y - cy)^2 + (z - cz)^2 = R^2
		// or dot((p - c), (p - c)) - R^2 = 0
		// p(t) = origin + t * direction
		// Solve for this equation using quadratic formula
		// t = -b +/- sqrt(b*b - 4*a*c) / 2 * a

		vec3 oc = ray.origin - GetCenterAtTime(ray.time);
		float a = dot(ray.direction, ray.direction);
		float b = 2 * dot(ray.direction, oc);
		float c = dot(oc, oc) - radiusSq;

		float discriminant = b * b - 4 * a * c;
		if (discriminant > 0)
		{
			float t = (-b - glm::sqrt(discriminant)) / (2.0f * a);

			if (t > tmin && t < tmax)
			{
				intersect.t = t;
				intersect.P = ray(t);
				intersect.N = N(intersect.P, ray.time);
				intersect.UV = UV(intersect.P);
				intersect.hit = this;
				return true;				
			}
			t = (-b + glm::sqrt(discriminant)) / (2.0f * a);
			if (t > tmin && t < tmax)
			{
				intersect.t = t;
				intersect.P = ray(t);
				intersect.N = N(intersect.P, ray.time);
				intersect.UV = UV(intersect.P);
				intersect.hit = this;
				return true;				
			}
		}
		return false;
	}

	virtual bool BoundingBox(AABB& aabb) const
	{
		aabb = 	AABB(GetMinCenter() - vec3(radius), GetMaxCenter() + vec3(radius));
		return true;
	}

	// Get UV for a unit sphere
	vec2 UV(const vec3& point) const
	{
		vec2 uv;
		float phi = atan2(point.z, point.x);
		float theta = asin(point.y);
		uv.x = 1 - (phi + M_PI) / (2.0f * M_PI);
		uv.y = (theta + M_PI / 2.0f) / M_PI;
		return uv;
	}

	// Normal at point on surface
	vec3 N(const vec3& point, float time) const
	{
		return (point - GetCenterAtTime(time)) / radius;
	}

	vec3 GetCenterAtTime(float t) const
	{
		if (pMotion)
		{
			return (*pMotion)(t) + center;
		}
		else
		{
			return center;
		}
	}

	vec3 GetMaxCenter() const
	{
		if (pMotion)
		{
			return glm::max(pMotion->m_start, pMotion->m_end) + center;
		}
		else
		{
			return center;
		}
	}

	vec3 GetMinCenter() const
	{
		if (pMotion)
		{
			return glm::min(pMotion->m_start, pMotion->m_end) + center;
		}
		else
		{
			return center;
		}		
	}

	vec3 center;
	float radius;
	float radiusSq;
};

#endif
