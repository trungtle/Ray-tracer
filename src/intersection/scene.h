#ifndef SCENE_H
#define SCENE_H

#include <algorithm>
#include "intersection/AABB.h"
#include "intersection/bvh.h"
#include "intersection/Intersection.h"
#include "material/material.h"

#define USE_ACCELERATION_STRUCTURE 1

using namespace glm;
using namespace std;

class Scene : public Hitable
{
public:
	virtual bool Hit(const Ray& ray, float tmin, float tmax, Intersection& intersect) const
	{
#if USE_ACCELERATION_STRUCTURE
		bool hit = root->Hit(ray, tmin, tmax, intersect);		
#else
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
#endif		

		return hit;
	}

	virtual bool BoundingBox(AABB& aabb) const
	{
		// Empty list
		if (objects.size() < 1) return false;

		// Combine AABB
		AABB temp;
		for (Hitable* obj : objects)
		{
			if (obj->BoundingBox(temp))
			{
				aabb.Add(temp);
			}
			else
			{
				return false;
			}
		}

		return true;

	}

	void BuildAccelerationStructure()
	{
		root = new BVHNode(objects, 0, objects.size());

	}

	vector<Hitable*> objects;
	vector<Hitable*> lights;
	vector<Material*> materials;

	BVHNode* root;

};

#endif