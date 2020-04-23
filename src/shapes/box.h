#ifndef BOX_H
#define BOX_H

#include <iostream>
#include <glm/glm.hpp>

#include "shapes/hitable_transform.h"
#include "shapes/rectangle.h"
#include "shapes/scene.h"

using namespace glm;
using namespace std;

class Box : public Hitable
{
public:
	Box(const vec3& mi, const vec3& ma, int mId) :
		_min(mi), _max(ma)
	{
		materialId = mId;

		Hitable* faceLeft = new RectYZ(vec2(_min.y, _min.z), vec2(_max.y, _max.z), _max.x, materialId);
		Hitable* faceRight = new FlipNormal(new RectYZ(vec2(_min.y, _min.z), vec2(_max.y, _max.z), _min.x, materialId));
		Hitable* faceFront = new RectXY(vec2(_min.x, _min.y), vec2(_max.x, _max.y), _max.z, materialId);
		Hitable* faceBack = new FlipNormal(new RectXY(vec2(_min.x, _min.y), vec2(_max.x, _max.y), _min.z, materialId));
		Hitable* faceTop = new RectXZ(vec2(_min.x, _min.z), vec2(_max.x, _max.z), _max.y, materialId);
		Hitable* faceBottom = new FlipNormal(new RectXZ(vec2(_min.x, _min.z), vec2(_max.x, _max.z), _min.y, materialId));

		faces = new Scene();
		faces->objects.emplace_back(faceLeft);
		faces->objects.emplace_back(faceRight);
		faces->objects.emplace_back(faceFront);
		faces->objects.emplace_back(faceBack);
		faces->objects.emplace_back(faceTop);
		faces->objects.emplace_back(faceBottom);
		faces->BuildAccelerationStructure();

		name = "Box";
	}

	virtual bool Hit(const Ray& ray, float tmin, float tmax, Interaction& intersect) const
	{
		return faces->Hit(ray, tmin, tmax, intersect);
	}

	virtual bool BoundingBox(AABB& aabb) const
	{
		aabb = AABB(_min, _max);
		return true;
	}

private:
	Scene* faces;
	vec3 _min;
	vec3 _max;

};

#endif
