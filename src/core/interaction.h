#ifndef INTERACTION_H
#define INTERACTION_H

#include <glm/glm.hpp>

class Hitable;
class Medium;
class PhaseFunction;

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
    
    const Medium* inside;
    const Medium* outside;
};

struct Interaction
{
    Interaction() :
        P(vec3(0.)), time(0), mediumInterface(nullptr)
    {}
    
    Interaction(const glm::vec3& p, float time,
                MediumInterface interface) :
        P(p), time(time), mediumInterface(interface)
    {}
    
    /** Return: medium of ray leaving the the surface in the direction of w */
    const Medium* GetMedium(const glm::vec3& w) const
    {
        return glm::dot(w, N) > 0 ? mediumInterface.outside : mediumInterface.inside;
    }
    
    /** Return: medium for interactions inside participating media */
    const Medium* GetMedium() const
    {
        return mediumInterface.inside;
    }
    
    float t;
    glm::vec3 P;
    glm::vec3 N;
    glm::vec2 UV;
    float time;
    const Hitable* hit;
    MediumInterface mediumInterface;
};


/** Represents an interaction at a point in a scattering medium */
class MediumInteraction : public Interaction
{
public:
    MediumInteraction(const glm::vec3& p, float time,
                      const Medium* medium, const PhaseFunction* phase) :
            Interaction(p, time, medium), phase(phase)
    {}
    
    const PhaseFunction* phase;
};

#endif
