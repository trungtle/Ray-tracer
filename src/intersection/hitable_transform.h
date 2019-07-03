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

class RotateY : public Hitable
{
public:
	RotateY(Hitable* h, float angle) :
		hitable(h)
	{
		float radian = (M_PI / 180.0f) * angle;
		sinTheta = sin(radian);
		cosTheta = cos(radian);
		hasBox = hitable->BoundingBox(bbox);
		vec3 _min(FLT_MAX, FLT_MAX, FLT_MAX);
		vec3 _max(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				for (int k = 0; k < 2; k++)
				{
					float x = i * bbox._max.x + (1 - i) * bbox._min.x;
					float y = j * bbox._max.y + (1 - j) * bbox._min.y;
					float z = k * bbox._max.z + (1 - k) * bbox._min.z;
					float newX = cosTheta * x + sinTheta * z;
					float newZ = -sinTheta * x + cosTheta * z;
					vec3 tester(newX, y, newZ);
					for (int c = 0; c < 3; c++)
					{
						if (tester[c] > _max[c])
						{
							_max[c] = tester[c];
						}
						if (tester[c] < _min[c])
						{
							_min[c] = tester[c];
						}
					}
				}
			}
		}
		bbox = AABB(_min, _max);
	}

	virtual bool Hit(const Ray& ray, float tmin, float tmax, Intersection& intersect) const
	{
		// Transform ray into local space
		vec3 origin = ray.origin;
		origin.x = cosTheta * ray.origin.x - sinTheta * ray.origin.z;
		origin.z = sinTheta * ray.origin.x + cosTheta * ray.origin.z;

		vec3 direction = ray.direction;
		direction.x = cosTheta * ray.direction.x - sinTheta * ray.direction.z;
		direction.z = sinTheta * ray.direction.x + cosTheta * ray.direction.z;

		Ray rotatedRay(origin, direction, ray.time);
		if (hitable->Hit(rotatedRay, tmin, tmax, intersect))
		{
			// Convert result back into world space
			vec3 p = intersect.P;
			p.x = cosTheta * intersect.P.x + sinTheta * intersect.P.z;
			p.z = -sinTheta * intersect.P.x + cosTheta * intersect.P.z;
			intersect.P = p;
			intersect.t = glm::distance(intersect.P, ray.origin);

			vec3 normal = intersect.N;
			normal.x = cosTheta * intersect.N.x + sinTheta * intersect.N.z;
			normal.z = -sinTheta * intersect.N.x + cosTheta * intersect.N.z;
			intersect.N = normal;

			return true;
		}
		return false;
	}

	virtual bool BoundingBox(AABB& aabb) const
	{
		aabb = bbox; 
		return hasBox;
	}	

private:
	Hitable* hitable;
	float sinTheta;
	float cosTheta;
	bool hasBox;
	AABB bbox;
};

#endif