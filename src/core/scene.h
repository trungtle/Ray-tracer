#ifndef SCENE_H
#define SCENE_H

#include <algorithm>

#include "accelerators/AABB.h"
#include "accelerators/bvh.h"
#include "camera/camera.h"
#include "materials/material.h"
#include "shapes/Intersection.h"

#define USE_ACCELERATION_STRUCTURE 1

using namespace glm;
using namespace std;

class Scene : public Hitable
{
public:
    Scene() : root(nullptr)
    {}
    
    void LoadFromFile(const std::string& filename);

	virtual bool Hit(const Ray& ray, float tmin, float tmax, Interaction& intersect) const
	{
#if USE_ACCELERATION_STRUCTURE
        if (root == nullptr) return false;
        
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
		if (objects.size() == 0) return;

		root = new BVHNode(objects, 0, objects.size());

	}

	vector<Hitable*> objects;
	vector<Hitable*> lights;
	vector<Material*> materials;

	std::shared_ptr<Camera> camera;

	BVHNode* root;

};

#endif
