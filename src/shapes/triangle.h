#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <array>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "shapes/Intersection.h"

using namespace glm;
using namespace std;

class Triangle : public Hitable
{
public:
	Triangle(const vec3& point1, const vec3& point2, const vec3& point3, int mId)
	{
		points[0] = point1;
		points[1] = point2;
		points[2] = point3;
		materialId = mId;
	}

	static bool RayTriangleIntersect(
		const Ray& ray, 
		const vec3& p0,
		const vec3& p1,
		const vec3& p2,
		float tmin, 
		float tmax, 
		Intersection& intersect
		)
	{
		// Moller-Trumbore fast ray-triangle intersection
		vec3 e0 = p1 - p0;
		vec3 e1 = p2 - p0;
		vec3 pvec = cross(ray.direction, e1);
		float det = dot(e0, pvec);
		if (fabs(det) < 0.00001f) return false;

		float invDet = 1 / det;
		vec3 tvec = ray.origin - p0;
		float u = dot(tvec, pvec) * invDet;
		if (u < 0 || u > 1) return false;

		vec3 qvec = cross(tvec, e0);
		float v = dot(ray.direction, qvec) * invDet;
		if (v < 0 || u + v > 1) return false;

		float t = dot(e1, qvec) * invDet;

		intersect.t = t;
		intersect.P = ray.PointAt(t);
		intersect.N = cross(p2 - p0, p1 - p0);;
		intersect.UV = vec2(u, v);

		return true;
	}

	virtual bool Hit(const Ray& ray, float tmin, float tmax, Intersection& intersect) const override
	{
		bool result = RayTriangleIntersect(
			ray, 
			points[0],
			points[1],
			points[2],
			tmin,
			tmax,
			intersect
			);
		intersect.hit = this;
		return result;
	}	

	virtual bool BoundingBox(AABB& aabb) const override
	{
		aabb = AABB::Combine(
			AABB(
				points[0],
				points[1]),
			AABB(
				points[0],
				points[2]
			));
		return true;
	}

	vec3 N(const vec3& point) const
	{
		return cross(points[2] - points[0], points[1] - points[0]);
	}

	array<vec3, 3> points;	
};

#endif