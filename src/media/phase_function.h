#ifndef PHASE_FUNCTION_H
#define PHASE_FUNCTION_H

#include <cmath>
#include <glm/glm.hpp>

#include "math/constants.h"

class PhaseFunction()
{
public:
    
    /**
     * Following pbrt convention, both wo and wi point away from the point of scattering
     * @return: The value of the phase function for the pair of directions
     */
    virtual float p(const glm::vec3& wo, const glm::vec3& wi) const = 0;
    virtual float sample_p(const glm::vec3& wo, glm::vec3* wi_out, const glm::vec2& u) const = 0;
};

class HenyeyGreenstein : public PhaseFunction
{
public:
    HenyeyGreenstein(float g) : m_g(g) {}
    
    virtual float p(const glm::vec3& wo, const glm::vec3& wi) const override
    {
        return PhaseHG(glm::dot(wo, wi), m_g);
    }
    virtual float sample_p(const glm::vec3& wo, glm::vec3* wi_out, const glm::vec2& u) const override
    {
        
    }

private:
    
    static inline float PhaseHG(float cosTheta, float g)
    {
        float g2 = g * g;
        float denom = 1 + g2 + 2 * g * cosTheta;
        return INV_4_PI * (1 - g2) / (denom * std::sqrt(denom));
    }
    
    /** Asummetry parameter, -1 to 1. Negative means backscattering */
    const float m_g;
};

#endif
