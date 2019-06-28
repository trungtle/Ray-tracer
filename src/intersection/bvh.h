#ifndef BVH_H
#define BVH_H

#include <algorithm>
#include <vector>
#include "intersection/AABB.h"
#include "intersection/Intersection.h"
#include "sampler/sampler.h"

#define DEBUG 0

using namespace std;

class BVHNode : public Hitable
{
public:
	BVHNode() :
		left(nullptr), right(nullptr)
	{
	}

	BVHNode(vector<Hitable*>& objects, int start, int end)
	{
		int axis = int(Sampler::Random01() * 3);
		int count = end - start;
		int half = start + count / 2;

#if DEBUG
		cout << "Start " << start << ", end " << end << ", half " << half << endl;
#endif
		// Sort only the nth element. We don't really care about ordering
		nth_element(
			objects.begin() + start, 
			objects.begin() + start + half, 
			objects.begin() + end, 
			[axis](Hitable* first, Hitable* second)
			{
				AABB boxFirst;
				AABB boxSecond;
#if DEBUG
				if (!first->BoundingBox(boxFirst) || !second->BoundingBox(boxSecond))
				{
					cerr << "No bounding box for this bvh node" << endl;
					cerr << first->name << ", " << second->name << endl;
				}
#endif				
				return boxFirst._min[axis] < boxSecond._min[axis];
			});

		if (count == 1)
		{
			left = right = objects[start];
#if DEBUG
			cout << "Single Leaf " << start << endl;
#endif			
		}
		else if (count == 2)
		{
			left = objects[start];
			right = objects[start + 1];
#if DEBUG			
			cout << "Two Leaf " << start << ", " << start + 1 << endl;
#endif			
		}
		else
		{
			left = new BVHNode(objects, start, half);
			right = new BVHNode(objects, half, end);
		}

		AABB boxLeft, boxRight;
#if DEBUG		
		if (!left->BoundingBox(boxLeft) || !right->BoundingBox(boxRight))
		{
			cerr << "No bounding box in BVH constructor" << endl;
		}
#endif		
		aabb = AABB::Combine(boxLeft, boxRight);
	}

	virtual bool Hit(const Ray& ray, float tmin, float tmax, Intersection& intersect) const
	{
		if (aabb.Hit(ray, tmin, tmax))
		{
			Intersection leftIntersect;
			bool hitLeft = left ? left->Hit(ray, tmin, tmax, leftIntersect) : false;

			Intersection rightIntersect;
			bool hitRight = right ? right->Hit(ray, tmin, tmax, rightIntersect) : false;

			if (hitLeft && hitRight)
			{
				intersect = leftIntersect.t < rightIntersect.t ? leftIntersect : rightIntersect;
				return true;
			}
			else if (hitLeft)
			{
				intersect = leftIntersect;
				return true;
			}
			else if (hitRight)
			{
				intersect = rightIntersect;
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}


	virtual bool BoundingBox(AABB& box) const override
	{
		box = aabb;
		return true;
	}

	Hitable* left;
	Hitable* right;
	AABB aabb;
};

#endif