#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "intersection/Triangle.h"

using namespace glm;
using namespace std;

class Mesh : public Hitable
{
public:
	Mesh(const vector<Triangle>& tris, int mId)
	{
		materialId = mId;
	}

	Mesh(string filePath, int mId)
	{
		
	}

	virtual bool Hit(const Ray& ray, float tmin, float tmax, Intersection& intersect) const override
	{
		bool isHit = false;
		float minT = 1000000;
		for (auto& tri : tris)
		{
			Intersection triIsect;
			if (tri->Hit(ray, tmin, tmax, triIsect))
			{
				isHit = true;
				if (triIsect.t < minT)
				{
					minT = triIsect.t;
					intersect = triIsect;
				}
			}
		}

		return isHit;
	}	

	virtual bool BoundingBox(AABB& aabb) const override
	{
		for (auto& tri : tris)
		{
			aabb.Add(tri.aabb);
		}
		
		return true;
	}

	vector<Triangle> tris;	
};

#endif