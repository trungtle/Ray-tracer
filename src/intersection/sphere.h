#ifndef SPHERE_H
#define SPHERE_H

#include "intersection/Intersection.h"

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
	}

	// Normal at point on surface
	vec3 N(const vec3& point) const
	{
		return (point - center) / radius;
	}

	virtual bool Hit(const Ray& ray, float tmin, float tmax, Intersection& intersect) const
	{
		// Sphere equation
		// (x - cx)^2 + (y - cy)^2 + (z - cz)^2 = R^2
		// or dot((p - c), (p - c)) - R^2 = 0
		// p(t) = origin + t * direction
		// Solve for this equation using quadratic formula
		// t = -b +/- sqrt(b*b - 4*a*c) / 2 * a

		vec3 oc = ray.origin - center;
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
				intersect.P = ray.PointAt(t);
				intersect.N = N(intersect.P);
				intersect.hit = this;
				return true;				
			}
			t = (-b + glm::sqrt(discriminant)) / (2.0f * a);
			if (t > tmin && t < tmax)
			{
				intersect.t = t;
				intersect.P = ray.PointAt(t);
				intersect.N = N(intersect.P);
				intersect.hit = this;
				return true;				
			}
		}
		return false;
	}

	vec3 center;
	float radius;
	float radiusSq;
};

#endif