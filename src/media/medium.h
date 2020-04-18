#ifndef DISK_H
#define DISK_H

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "material/material.h"
#include "intersection/Intersection.h"
#include "samplers/sampler.h"
#include "texture/texture.h"

using namespace glm;
using namespace std;

class Medium
{
public:
    virtual ~Medium() {}
    virtual Spectrum Tr(const Ray& ray, Sampler& sampler) const = 0;
//    virtual Spectrum Sample(const Ray& ray, Sampler& sampler, MemoryArena& arena, MediumInteraction* mi) const = 0;
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


/** Represent the boundary between two media.
 *  N.B: It's possible to have inconsitent media configuration.
 *       It is up to the user to make sure the setup is consistent.
 */
struct MediumInterface
{
    MediumInterface(const Medium* medium) :
    inside(medium), outside(medium) {}
    
    MediumInterface(const Medium* inside, const Medium* outside) :
    inside(inside), outside(outside) {}
    
    /** Checks whether the MediumInterface marks the transition between two distinc media */
    bool IsMediumTransition() const { return inside != outside; }
    
    const Medium* inside, outside;
};

#endif
