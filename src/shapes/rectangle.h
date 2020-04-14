#ifndef RECTANGLE_H 
#define RECTANGLE_H

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <iostream>

#include "shapes/Intersection.h"

using namespace glm;
using namespace std;

class RectXY : public Hitable
{
public:
	RectXY(const vec2& mi, const vec2& ma, float z, int mId) 
	: _min(mi), _max(ma), _z(z)
	{
		materialId = mId;
		name = "RectXY";
	}

	virtual bool Hit(const Ray& ray, float tmin, float tmax, Intersection& intersect) const
	{
		float t = (_z - ray.origin.z) / ray.direction.z;
		if (t > tmin && t < tmax)
		{
			float x = ray.origin.x + t * ray.direction.x;
			float y = ray.origin.y + t * ray.direction.y;
			if (x > _min.x && x < _max.x && y > _min.y && y < _max.y)
			{
				intersect.P = vec3(x, y, _z);
				intersect.UV = UV(intersect.P);
				intersect.N = N(intersect.P);
				intersect.t = t;
				intersect.hit = this;
				return true;
			}
		}
		return false;
	}
	virtual bool BoundingBox(AABB& aabb) const
	{
		aabb = AABB(vec3(_min.x, _min.y, _z - 0.0001f), vec3(_max.x, _max.y, _z + 0.0001f));
		return true;
	}

	vec3 N(const vec3& point) const
	{
		return vec3(0, 0, 1);
	}

	vec2 UV(const vec3& point) const
	{
		return vec2(
			(point.x - _min.x) / (_max.x - _min.x),
			(point.y - _min.y) / (_max.y - _min.y));
	}

	vec2 _min;
	vec2 _max;
	float _z;
};



class RectXZ : public Hitable
{
public:
	RectXZ(const vec2& mi, const vec2& ma, float y, int mId) 
	: _min(mi), _max(ma), _y(y)
	{
		materialId = mId;
		name = "RectXZ";
	}

	virtual bool Hit(const Ray& ray, float tmin, float tmax, Intersection& intersect) const
	{
		float t = (_y - ray.origin.y) / ray.direction.y;
		if (t > tmin && t < tmax)
		{
			float x = ray.origin.x + t * ray.direction.x;
			float z = ray.origin.z + t * ray.direction.z;
			if (x > _min.x && x < _max.x && z > _min.y && z < _max.y)
			{
				intersect.P = vec3(x, _y, z);
				intersect.UV = UV(intersect.P);
				intersect.N = N(intersect.P);
				intersect.t = t;
				intersect.hit = this;
				return true;
			}
		}
		return false;
	}

	virtual bool BoundingBox(AABB& aabb) const
	{
		aabb = AABB(vec3(_min.x, _y - 0.0001f, _min.y), vec3(_max.x, _y + 0.0001f, _max.y));
		return true;
	}

	virtual float PdfValue(const vec3& origin, const vec3& direction) const override
	{
		Intersection isect;
		if (this->Hit(Ray(origin, direction), 0, FLT_MAX, isect))
		{
			float area = (_max.x - _min.x) * (_max.y - _min.y);
			float distSq = isect.t * isect.t * glm::length2(direction);
			float cosine = fabs(dot(direction, isect.N) / glm::length(direction));
			float result = distSq / (cosine * area);
			return distSq / (cosine * area);
		}
		return 0;
	}

	virtual vec3 Random(const vec3& origin) const override
	{
		vec3 randomPoint(
			_min.x + Sampler::Random01() * (_max.x - _min.x),
			_y,
			_min.y + Sampler::Random01() * (_max.y - _min.y)
			);

		return glm::normalize(randomPoint - origin);
	}

	vec3 N(const vec3& point) const
	{
		return vec3(0, 1, 0);
	}

	vec2 UV(const vec3& point) const
	{
		return vec2(
			(point.x - _min.x) / (_max.x - _min.x),
			(point.z - _min.y) / (_max.y - _min.y));
	}

	vec2 _min;
	vec2 _max;
	float _y;
};

class RectYZ : public Hitable
{
public:
	RectYZ(const vec2& mi, const vec2& ma, float x, int mId) 
	: _min(mi), _max(ma), _x(x)
	{
		materialId = mId;
		name = "RectYZ";
	}

	virtual bool Hit(const Ray& ray, float tmin, float tmax, Intersection& intersect) const
	{
		float t = (_x - ray.origin.x) / ray.direction.x;
		if (t > tmin && t < tmax)
		{
			float y = ray.origin.y + t * ray.direction.y;
			float z = ray.origin.z + t * ray.direction.z;
			if (y > _min.x && y < _max.x && z > _min.y && z < _max.y)
			{
				intersect.P = vec3(_x, y, z);
				intersect.UV = UV(intersect.P);
				intersect.N = N(intersect.P);
				intersect.t = t;
				intersect.hit = this;
				return true;
			}
		}
		return false;
	}

	virtual bool BoundingBox(AABB& aabb) const
	{
		aabb = AABB(vec3(_x - 0.0001f, _min.x, _min.y), vec3(_x + 0.0001f, _max.x, _max.y));
		return true;
	}

	vec3 N(const vec3& point) const
	{
		return vec3(1, 0, 0);
	}

	vec2 UV(const vec3& point) const
	{
		return vec2(
			(point.x - _min.x) / (_max.x - _min.x),
			(point.z - _min.y) / (_max.y - _min.y));
	}

	vec2 _min;
	vec2 _max;
	float _x;
};

#endif