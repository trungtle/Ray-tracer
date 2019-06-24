#include <iostream>
#include <fstream>
#include <glm/glm.hpp>

#include "camera/Camera.h"
#include "intersection/Intersection.h"
#include "screen/screen.h"
#include "sampler/sampler.h"

using namespace std;
using namespace glm;

Scene g_scene;

vec3 Shade(const Ray& r)
{
	vec3 color;

	Intersection intersect;
	if (g_scene.Hit(r, 0, 10000.0f, intersect))
	{
		vec3 newTarget = intersect.P + intersect.N + Sampler::RandomSampleInUnitSphere();
		color = 0.5f * Shade(Ray(intersect.P, newTarget - intersect.P));
	}
	else
	{
		color = Screen::Color(r);
	}

	return color;
}

void RenderFullscreen()
{
	ofstream file;
	file.open("../images/image.ppm");	

	// Screen
	Screen screen(800, 600);
	file << "P3\n" << screen.width << " " << screen.height << "\n255\n";

	// Scene
	g_scene.objects.push_back(new Sphere(vec3(0,0,-1), 0.5));
	g_scene.objects.push_back(new Sphere(vec3(0,-100.5,-1), 100));

	// Camera
	Camera camera;
	Sampler sampler;
	sampler.numSamplesPerPixel = 100;

	for (int y = screen.height - 1; y >= 0; y--)
	{
		for (int x = 0; x < screen.width; x++)
		{
			vec3 color(0.0f, 0.0f, 0.0f);
			for (int n = 0; n < sampler.numSamplesPerPixel; n++)
			{
				vec2 uv = sampler.RandomSampleFromPixel(screen, x, y);
				Ray r = camera.GetRay(uv);
				vec3 tempColor = Shade(r);
				color += tempColor;
			}
			color /= float(sampler.numSamplesPerPixel);

			int ir = int(color.r * 255.99);
			int ig = int(color.g * 255.99);
			int ib = int(color.b * 255.99);

			file << ir << " " << ig << " " << ib << "\n";
		}
	}

	file.close();
}

int main()
{
	RenderFullscreen();
	return 0;
}