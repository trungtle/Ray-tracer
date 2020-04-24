#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include <glm/glm.hpp>
#include <memory>

#include "spectrum/spectrum.h"

class Camera;
class Ray;
class Scene;
class Screen;
class SumColor;

class Integrator
{
	friend class SumColor;
	std::shared_ptr<const Screen> m_screen;

public:
	Integrator(
		std::shared_ptr<const Screen> screen, 
		int maxDepth = 50) :
			m_screen(screen), 
			maxDepth(maxDepth) 
		{}


	void Render(Scene& scene, uint32_t numSamplesPerPixel);


protected:
	Spectrum Li(const Scene& scene, const Ray& r) const;

	int maxDepth;
};

#endif
