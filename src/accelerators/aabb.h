#ifndef AABB_H
#define AABB_H

#include <limits>
#include <glm/glm.hpp>

#include "ray/ray.h"

using namespace glm;
using namespace std;

class AABB
{
public:
	AABB():
		_min(vec3(numeric_limits<float>::max())),
		_max(vec3(numeric_limits<float>::max()))
	{

	}

	AABB(const vec3& mi, const vec3& ma) :
		_min(mi), _max(ma)
	{

	}

	bool Hit(const Ray& ray, float tmin, float tmax) const
	{
		// Using the slab method. Basically we compute tx0-tx1, ty0-ty1, tz0-tz1.
		// These are all intersections bettwen the ray and each plane of AABB.	
		// If all these three intervals overlapped, then the ray hit AABB.

		// To compute the t value of intersection with an axis aligned plane x0 (scalar only because it's AA):
		// ray.origin + t0 * ray.direction = x0  
		// -> t0 = (x0 - ray.origin) / ray.direction
		// 
		// To check for overlap between interval A and B, we can do this:
		// bool overlap(minA, maxA, minB, maxB)
		//     return (max(minA, minB) < min(maxA, maxB))
		// or sum of length is less than length bettween the max of maxes and min of mins.
		// or  return (max(maxA, maxB) - min(minA, minB) <= (maxA - minA) + (maxB - minB))


		for (int a = 0; a < 3; a++)
		{
			float invD = 1.0f / ray.direction[a];
			float t0 = (_min[a] - ray.origin[a]) * invD;
			float t1 = (_max[a] - ray.origin[a]) * invD;
			if (invD < 0.0f)
			{
				swap(t0, t1);
			}
			tmin = t0 > tmin ? t0 : tmin;
			tmax = t1 < tmax ? t1 : tmax;
			if (tmin >= tmax)
			{
				return false;
			}
		}
		return true;
	}

	void Add(const AABB& other)
	{
		_min = glm::min(_min, other._min);
		_max = glm::max(_max, other._max);
	}

	static AABB Combine(const AABB& first, const AABB& second)
	{
		AABB combined(glm::min(first._min, second._min), glm::max(first._max, second._max));
		return combined;
	}

	vec3 _min;
	vec3 _max;
};

#endif