#ifndef CYLINDER_H
#define CYLINDER_H

#include <iostream>

#include "shapes/disk.h"
#include "shapes/hitable_transform.h"
#include "shapes/Intersection.h"
#include "shapes/scene.h"

using namespace glm;
using namespace std;

static bool AreSame(float a, float b)
{
	static const float eps = 0.00001f;
    
    return fabs(a - b) < eps;
}

class Cylinder : public Hitable
{
public:
	Cylinder(float r, float yMi, float yMa, int mId) :
		radius(r), yMin(yMi), yMax(yMa), radiusSquared(r * r)
	{
		materialId = mId;

	}

	virtual bool Hit(const Ray& ray, float tmin, float tmax, Intersection& intersect) const override
	{
		// Cylinder equation is x^2 + z^2 - r^2 = 0
		// 
		float a = ray.direction.x * ray.direction.x + ray.direction.z * ray.direction.z;
		float b = 2.0f * (ray.direction.x * ray.origin.x + ray.direction.z * ray.origin.z);
		float c = ray.origin.x * ray.origin.x + ray.origin.z * ray.origin.z - radiusSquared;

		float discriminant = b * b - 4 * a * c;
		if (discriminant > 0)
		{
			float t = (-b - glm::sqrt(discriminant)) / (2.0f * a);
			if (t > tmin && t < tmax)
			{
				vec3 hitPoint = ray.PointAt(t);
				if (hitPoint.y >= yMin && hitPoint.y <= yMax)
				{
					intersect.t = t;
					intersect.P = hitPoint;
					intersect.N = N(intersect.P);
					intersect.UV = UV(intersect.P);
					intersect.hit = this;

					return true;				
				}
			}
			t = (-b + glm::sqrt(discriminant)) / (2.0f * a);
			if (t > tmin && t < tmax)
			{
				vec3 hitPoint = ray.PointAt(t);
				if (hitPoint.y >= yMin && hitPoint.y <= yMax)
				{
					intersect.t = t;
					intersect.P = hitPoint;
					intersect.N = N(intersect.P);
					intersect.UV = UV(intersect.P);
					intersect.hit = this;

					return true;				
				}				
			}
		}		
		return false;
	}

	virtual bool BoundingBox(AABB& aabb) const override
	{
		aabb = AABB(vec3(-radius, yMin, -radius), vec3(radius, yMax, radius));
		return true;
	}

	vec3 N(const vec3& point) const
	{
		vec3 normal(point.x / radius, 0, point.z / radius);
		return normal;

	}

	vec2 UV(const vec3& point) const
	{
		if (AreSame(point.y, yMax) || AreSame(point.y, yMin))
		{
			vec2 uv;
			float theta = atan2(point.z, point.x);
			uv.x = radius * cos(theta);
			uv.y = radius * sin(theta);
			return uv;
		}

		vec2 uv;
		float theta = atan2(point.z, point.x);
		uv.x = 1 - (theta + M_PI) / (2.0f * M_PI);
		uv.y = (point.y - yMin) / (yMax - yMin);
		return uv;		
	}

	float radiusSquared;
	float radius;
	float yMin;
	float yMax;
};

class CappedCylinder : public Hitable
{
public:
	CappedCylinder(float r, float yMi, float yMa, int mId) :
		radius(r), yMin(yMi), yMax(yMa)
	{
		materialId = mId;
		
		Hitable* top = new Disk(vec3(0, yMax, 0), r, mId);
		Hitable* bottom = new FlipNormal(new Disk(vec3(0, yMin, 0), r, mId));
		Hitable* cylinder = new Cylinder(r, yMi, yMa, mId);

		scene = new Scene();
		scene->objects.emplace_back(top);
		scene->objects.emplace_back(bottom);
		scene->objects.emplace_back(cylinder);
		scene->BuildAccelerationStructure();

		name = "CappedCylinder";
	}

	virtual bool Hit(const Ray& ray, float tmin, float tmax, Intersection& intersect) const
	{
		return scene->Hit(ray, tmin, tmax, intersect);
	}

	virtual bool BoundingBox(AABB& aabb) const override
	{
		aabb = AABB(vec3(-radius, yMin, -radius), vec3(radius, yMax, radius));
		return true;
	}

	Scene* scene;
	float radius;
	float yMin;
	float yMax;
};

#endif