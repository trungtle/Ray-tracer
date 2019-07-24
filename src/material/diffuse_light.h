#ifndef DIFFUSE_LIGHT_H
#define DIFFUSE_LIGHT_H

#include "material/material.h"
#include "texture/texture.h"

class DiffuseLight : public Material
{
public:
	DiffuseLight(Texture* texture) : emit(texture)
	{
		type = kLight;
	}
	virtual bool Scatter(const Ray& ray, const Intersection& intersect, Ray& scatterRay) const override
	{
		return false;
	}
	virtual vec3 Emitted(const vec2& uv, const vec3& point) const override
	{
		return emit->value(uv, point);
	}

	Texture* emit;
};


#endif