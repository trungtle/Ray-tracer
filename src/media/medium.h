#ifndef DISK_H
#define DISK_H

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "material/material.h"
#include "intersection/Intersection.h"
#include "texture/texture.h"

using namespace glm;
using namespace std;



class ConstantMedium : public Hitable
{
public:
	ConstantMedium(
		Hitable* boundary, 
		float density, 
		Texture* albedo)
	{
		m_phaseFunction = new IsotropicMaterial(albedo);

	}

	virtual bool Hit(const Ray& ray, float tmin, float tmax, Intersection& intersect) const
	{
		return false;
	}

	virtual bool BoundingBox(AABB& aabb) const
	{
		return m_pBoundary->BoundingBox(aabb);
	}


	Hitable* m_pBoundary;
	float m_density;
	Material* m_phaseFunction;

};


#endif