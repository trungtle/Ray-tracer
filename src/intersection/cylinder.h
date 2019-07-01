#ifndef CYLINDER_H
#define CYLINDER_H

#include <iostream>
#include "intersection/disk.h"
#include "intersection/hitable_transform.h"
#include "intersection/Intersection.h"
#include "utility.h"

using namespace glm;
using namespace std;

class Cylinder : public Hitable
{
public:
	Cylinder(float r, float yMi, float yMa, int mId) :
		radius(r), yMin(yMi), yMax(yMa), radiusSquared(r * r)
	{
		materialId = mId;
		top = new Disk(vec3(0, yMax, 0), r, mId);
		bottom = new FlipNormal(new Disk(vec3(0, yMin, 0), r, mId));
	}

	virtual bool Hit(const Ray& ray, float tmin, float tmax, Intersection& intersect) const override
	{
		float tTop = -1;
		Intersection intersectTop;
		if(top->Hit(ray, tmin, tmax, intersectTop))
		{
			tTop = intersectTop.t;
		}

		float tBottom = -1;
		Intersection intersectBottom;
		if(bottom->Hit(ray, tmin, tmax, intersectBottom))
		{
			tBottom = intersectBottom.t;
		}

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
				if (tTop != -1 && tTop <= t && tTop < tBottom)
				{
					intersect = intersectTop;
					intersect.hit = this;
					return true;
				}
				if (tBottom != -1 && tBottom <= t && tBottom < tTop)
				{
					intersect = intersectTop;
					intersect.hit = this;
					return true;
				}

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
				if (tTop != -1 && tTop <= t && tTop < tBottom)
				{
					intersect = intersectTop;
					intersect.hit = this;
					return true;
				}
				if (tBottom != -1 && tBottom <= t && tBottom < tTop)
				{
					intersect = intersectTop;
					intersect.hit = this;
					return true;
				}

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
		else
		{
			if (tTop != -1 && tTop < tBottom)
			{
				intersect = intersectTop;
				intersect.hit = this;
				return true;
			}
			if (tBottom != -1 && tBottom < tTop)
			{
				intersect = intersectTop;
				intersect.hit = this;
				return true;
			}
		}		
		return false;
	}

	virtual bool BoundingBox(AABB& aabb) const
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
		if (Tr::AreSame(point.y, yMax) || Tr::AreSame(point.y, yMin))
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
	Hitable* top;
	Hitable* bottom;
};

#endif