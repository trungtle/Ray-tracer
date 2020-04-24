#ifndef MATERIAL_H
#define MATERIAL_H

#include <iostream>

#include "ray/ray.h"
#include "samplers/sampler.h"
#include "shapes/intersection.h"
#include "spectrum/spectrum.h"
#include "textures/texture.h"
#include "transform/onb.h"

using namespace glm;

static vec3 Reflect(const vec3& in, const vec3& normal)
{
	return in - 2 * dot(in, normal) * normal;
}

static bool Refract(const vec3& in, const vec3& normal, float ni_over_nt, vec3& refracted)
{
	// Snell's law
	vec3 uv = normalize(in);
	float dt = dot(uv, normal);
	float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1.0f - dt * dt);
	if (discriminant > 0)
	{
		refracted = ni_over_nt * (uv - normal * dt) - normal * glm::sqrt(discriminant);
		return true;
	}
	else
	{
		return false;
	}
}

// Approximate glass
static float Schlick(float cosine, float refIdx)
{
	float r0 = (1.0 - refIdx) / (1.0 + refIdx);
	r0 = r0 * r0;
	return r0 + (1.0f - r0) * pow((1.0 - cosine), 5);
}

class Material
{
public:

	enum EType
	{
		kDiffuse,
		kMetal,
		kDielectric,
        kIsotropic,
		kLight
	};

	virtual bool Scatter(const Ray& ray, const Interaction& intersect, Ray& scatterRay) const = 0;
	virtual Spectrum Emitted(const vec2& uv, const vec3& point) const
	{
		return Spectrum(0.0);
	}
	Texture* texture;
	EType type;
};

class LambertianMaterial : public Material
{
public:
	LambertianMaterial(Texture* t)
	{
		texture = t;
		type = kDiffuse;
	}

	virtual bool Scatter(const Ray& ray, const Interaction& intersect, Ray& scatterRay) const override
	{
		// Scatter toward a random point inside a unit sphere tangent to the point of intersection.
		// vec3 newTarget = intersect.P + intersect.N + Sampler::RandomSampleInUnitSphere();
		// scatterRay = Ray(intersect.P, newTarget - intersect.P, ray.time);
		// pdf = dot(intersect.N, scatterRay.direction) / M_PI;

		// Scatter toward a cosine weighted direction
		// ONB uvw;
		// uvw.BuildFromW(intersect.N);
		// vec3 direction = uvw.Local(Sampler::RandomCosineDirection());
		// scatterRay = Ray(intersect.P, direction, ray.time);

		return true;
	}
};

class MetalMaterial : public Material
{
public:
	MetalMaterial(Texture* t, float f = 0) :
		fuzz(f < 1 ? f : 1)
	{		
		texture = t;
		type = kMetal;
	}

	virtual bool Scatter(const Ray& ray, const Interaction& intersect, Ray& scatterRay) const
	{
		// scatter ray reflect around the surface normal of the intersection point.
		vec3 reflected = Reflect(ray.direction, intersect.N);
		scatterRay = Ray(intersect.P, reflected + Sampler::RandomSampleInUnitSphere() * fuzz, ray.time);

		// Make sure we're reflected away from the intersection
		return dot(scatterRay.direction, intersect.N) > 0; 
	}

	float fuzz;
};

class DielectricMaterial : public Material
{
public:
	DielectricMaterial(float ri) :
		refIdx(ri)
	{		
		type = kDielectric;
	}

	virtual bool Scatter(const Ray& ray, const Interaction& intersect, Ray& scatterRay) const
	{
		vec3 outwardNormal;
		float ni_over_nt;
		float cosine;
		if (dot(ray.direction, intersect.N) > 0)
		{
			outwardNormal = -intersect.N;
			ni_over_nt = refIdx;
			cosine = refIdx * dot(ray.direction, intersect.N) / length(ray.direction);
		}
		else
		{
			outwardNormal = intersect.N;
			ni_over_nt = 1.0f / refIdx;
			cosine = -dot(ray.direction, intersect.N) / length(ray.direction);
		}

		float reflect_prob;
		vec3 refracted;
		if (Refract(ray.direction, outwardNormal, ni_over_nt, refracted))
		{
			reflect_prob = Schlick(cosine, refIdx);
		}
		else
		{
			reflect_prob = 1.0f;
		}

		if (Sampler::Random01() < reflect_prob)
		{
			vec3 reflected = Reflect(ray.direction, intersect.N);
			scatterRay = Ray(intersect.P, reflected, ray.time);
		}
		else
		{
			scatterRay = Ray(intersect.P, refracted, ray.time);
		}
		return true;
	}

	float refIdx;
};

class IsotropicMaterial : public Material
{
public:
	IsotropicMaterial(Texture* albedo)
    {
        texture = albedo;
        type = kIsotropic;
    }

	virtual bool Scatter(const Ray& ray, const Interaction& intersect, Ray& scatterRay) const override
	{
		scatterRay = Ray(intersect.P, Sampler::RandomSampleInUnitSphere());
		return true;
	}
};

#endif
