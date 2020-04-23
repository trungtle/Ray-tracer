#ifndef MEDIUM_H
#define MEDIUM_H

#include <memory>

#include <glm/glm.hpp>

#include "materials/material.h"
#include "shapes/Intersection.h"
#include "samplers/sampler.h"
#include "textures/texture.h"

using namespace glm;
using namespace std;


static inline float Lerp(float t, float v1, float v2)
{
    return (1 - t) * v1 + t * v2;
}

class Medium
{
public:
    virtual ~Medium() {}
    virtual Spectrum Tr(const Ray& ray, Sampler& sampler) const = 0;
    virtual Spectrum Sample(const Ray& ray, Sampler& sampler,
                            /*MemoryArena& arena,*/
                            MediumInteraction* mi) const = 0;
};


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

	virtual bool Hit(const Ray& ray, float tmin, float tmax, Interaction& intersect) const
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


class GridDensityMedium : public Medium
{
public:
    /**
     * sigma_a : absorption
     * sigma_s : scattering
     */
    GridDensityMedium(const Spectrum& sigma_a, const Spectrum& sigma_s,
                      float g, int nx, int ny, int nz, const float* d) :
        sigma_a(sigma_a), sigma_s(sigma_s), g(g), nx(nx), ny(ny), nz(nz),
        density(new float[nx * ny * nz])
    {
        memcpy((float *)density.get(), d, sizeof(float) * nx * ny * nz);
        
        sigma_t = (sigma_a + sigma_s)[0];
        
        float maxDensity = 0;
        for (int i = 0; i < nx * ny * nz; i++)
        {
            maxDensity = std::max(density[i], maxDensity);
        }
        invMaxDensity = 1.0f / maxDensity;
        
    }
    
    float Density(const glm::vec3 point)
    {
        // Range between [0,1]^3
        glm::vec3 pSamples(point.x * nx - 0.5f, point.y * ny - 0.5f, point.z * nz - 0.5f);
        glm::ivec3 pi = static_cast<glm::ivec3>(glm::floor(pSamples));
        glm::vec3 diff = pSamples - static_cast<glm::vec3>(pi);
        
        // Trilinear interpolate density at the point
        float d00 = Lerp(diff.x, D(pi),                         D(pi + glm::ivec3(1, 0, 0)));
        float d10 = Lerp(diff.x, D(pi + glm::ivec3(0, 1, 0)),   D(pi + glm::ivec3(1, 1, 0)));
        float d01 = Lerp(diff.x, D(pi + glm::ivec3(0, 0, 1)),   D(pi + glm::ivec3(1, 0, 1)));
        float d11 = Lerp(diff.x, D(pi + glm::ivec3(0, 1, 1)),   D(pi + glm::ivec3(1, 1, 1)));
        float d0 = Lerp(diff.y, d00, d10);
        float d1 = Lerp(diff.y, d01, d11);
        return Lerp(diff.z, d0, d1);
    }
    
    /** Utility function to return density at a given integer sample position */
    float D(const glm::ivec3& p)
    {
        if (p.x >= nx || p.y > ny || p.z > nz ||
            p.x < 0 || p.y < 0 || p.z < 0)
        {
            return 0;
        }
        
        return density[(p.z * ny + p.y) * nx + p.x];
    }
    
    virtual Spectrum Tr(const Ray& ray, Sampler& sampler) const override
    {
        return Spectrum(0.);
    }
    
    virtual Spectrum Sample(const Ray& ray, Sampler& sampler,
                            /*MemoryArena& arena,*/
                            MediumInteraction* mi) const override
    {
        return Spectrum(0.);
    }
    
private:
    /** Absorption */
    const Spectrum sigma_a;
    
    /** Scattering */
    const Spectrum sigma_s;
    
    /** Attentuation coefficient */
    float sigma_t;
    
    /** Asymmetry parameter */
    float g;
    
    /** Volume dimension */
    const int nx, ny, nz;
    
    /** 1D density values */
    std::unique_ptr<float[]> density;
    
    float invMaxDensity;
    
};


#endif
