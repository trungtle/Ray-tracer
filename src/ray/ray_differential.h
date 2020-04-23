#ifndef RAY_DIFFERENTIAL_H
#define RAY_DIFFERENTIAL_H

#include "ray/ray.h"

#include "core/point.h"

class RayDifferential : public Ray
{
public:
	RayDifferential() { m_hasDifferentials = false; }
	RayDifferential(const Point3& origin, const glm::vec3& direction, float time) :
		Ray(origin, direction, time)
	{
		m_hasDifferentials = false;
	}


	RayDifferential(const Ray& ray) : Ray(ray)
	{
		m_hasDifferentials = false;
	}


	void ScaleDifferentials(float scale)
	{
		m_rxOrigin = origin + (m_rxOrigin - origin) * scale;
		m_ryOrigin = origin + (m_ryOrigin - origin) * scale;
		m_rxDirection = direction + (m_rxDirection - direction) * scale;
		m_ryDirection = direction + (m_ryDirection - direction) * scale;
	}


	bool m_hasDifferentials;
	Point3 m_rxOrigin, m_ryOrigin;
	glm::vec3 m_rxDirection, m_ryDirection;
};

#endif
