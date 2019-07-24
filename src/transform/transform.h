#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>

class Transform
{
public:
    glm::mat4 m_localTransform;
    glm::mat4 m_globalTransform;
    glm::vec3 m_translation;
    glm::quat m_orientation;
    glm::vec3 m_scale;
};

#endif