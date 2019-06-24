#ifndef SCENE_H
#define SCENE_H

#include "intersection/Intersection.h"
#include "material/material.h"

using namespace glm;
using namespace std;

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
	vector<Material*> materials;
};

#endif