#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include <glm/glm.hpp>
#include <memory>

class Camera;
class Ray;
class Scene;
class Screen;

class Integrator
{
	std::shared_ptr<const Camera> m_camera;
	std::shared_ptr<const Screen> m_screen;

public:
	Integrator(
		std::shared_ptr<const Camera> camera,
		std::shared_ptr<const Screen> screen) :
			m_camera(camera),
			m_screen(screen) {}


	void Render(const Scene& scene, uint32_t numSamplesPerPixel);


	glm::vec3 Li(const Scene& scene, const Ray& r, int maxDepth) const;
};

#endif