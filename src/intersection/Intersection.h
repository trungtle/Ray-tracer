#ifndef INTERSECTION_H
#define INTERSECTION_H
#include <glm/glm.hpp>
#include <math.h>
#include <vector>
#include "ray/ray.h"

using namespace glm;
using namespace std;

struct Intersection
{	
	float t;
	vec3 P;
	vec3 N;
};

class Hitable
{
public:
	virtual bool Hit(const Ray& ray, float tmin, float tmax, Intersection& intersect) const = 0;
};

class Scene : public Hitable
{
public:
	virtual bool Hit(const Ray& ray, float tmin, float tmax, Intersection& intersect) const
	{
		Intersection tempIntersect;
		float closestT = tmax;
		bool hit = false;		
		for (Hitable* obj : objects)
		{
			if(obj->Hit(ray, tmin, closestT, tempIntersect))
			{
				hit = true;
				closestT = tempIntersect.t;
				intersect = tempIntersect;
			}
		}

		return hit;
	}

	vector<Hitable*> objects;
};


class Sphere : public Hitable
{
public:
	Sphere(const vec3& c, float r) :
		center(c), radius(r)
	{
		radiusSq = radius * radius;
	}

	// Normal at point on surface
	vec3 N(const vec3& point) const
	{
		return normalize(point - center);
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
				return true;				
			}
			t = (-b + glm::sqrt(discriminant)) / (2.0f * a);
			if (t > tmin && t < tmax)
			{
				intersect.t = t;
				intersect.P = ray.PointAt(t);
				intersect.N = N(intersect.P);
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