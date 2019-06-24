#include <chrono>
#include <fstream>
#include <iostream>
#include <glm/glm.hpp>
#include <tbb/tbb.h>

#include "camera/Camera.h"
#include "intersection/scene.h"
#include "intersection/sphere.h"
#include "sampler/sampler.h"
#include "screen/screen.h"
#include "utility.h"

#define PARALLEL 1
#define REPORT 1

using namespace glm;
using namespace std;
using namespace tbb;

Scene g_scene;
Camera* g_camera;
Screen* g_screen;

vec3 Shade(const Ray& r, int depth)
{
	vec3 color;

	Intersection intersect;
	if (g_scene.Hit(r, 0.001, 10000.0f, intersect))
	{
		Ray scatterRay;
		vec3 attenuation;

		if (depth >= 0 && g_scene.materials[intersect.hit->materialId]->Scatter(r, intersect, attenuation, scatterRay))
		{
			color = 0.9f * attenuation * Shade(scatterRay, depth - 1);			
		}
		else
		{
			color = vec3(0.0, 0, 0);
		}
	}
	else
	{
		color = Screen::Color(r);
	}

	return color;
}

class SumColor
{
	int pixelX;
	int pixelY;
public:

	void operator()(const blocked_range<size_t>& range)
	{
		vec2 uv = Sampler::RandomSampleFromPixel(pixelX, pixelY, g_screen->width, g_screen->height);
		Ray ray = g_camera->GetRay(uv);

		vec3 sum = _sumColor;
		size_t end = range.end();		
		for (size_t i = range.begin(); i != end; i++)
		{
			sum += Shade(ray, 50);
		}
		_sumColor = sum;
	}

	SumColor(SumColor& other, split) : 
		pixelX(other.pixelX), pixelY(other.pixelY), _sumColor(vec3(0, 0, 0)) 
	{}

	void join(const SumColor& other)
	{
		_sumColor += other._sumColor;
	}

	SumColor(int x, int y) :
		pixelX(x), pixelY(y), _sumColor(vec3(0, 0, 0))
	{}

	vec3 _sumColor;
};

void RenderFullscreen()
{
	ofstream file;
	file.open("../images/image.ppm");	

	// Screen
	const int nx = 800;
	const int ny = 600;
	g_screen = new Screen(nx, ny);
	file << "P3\n" << g_screen->width << " " << g_screen->height << "\n255\n";

	// Camera
	vec3 lookFrom = vec3(-3, 0.3, 1.5);
	vec3 lookAt = vec3(0, 0, 0);
	float vfov = 40;
	float aspect = float(g_screen->width) / float(g_screen->height);
	float aperture = 0.02f;
	float focusDist = length(lookAt - lookFrom);
	g_camera = new Camera(lookFrom, lookAt, vfov, aspect, aperture, focusDist);
	int numSamplesPerPixel = 100;

	// Begin render
#if REPORT
	auto start = chrono::steady_clock::now();
#endif

#if PARALLEL

	vec3 imageBuffer[nx][ny];

	parallel_for(blocked_range2d<int>(0, g_screen->height, 0, g_screen->width),
		[numSamplesPerPixel, &imageBuffer](blocked_range2d<int> range)
		{
			int rowEnd = range.rows().end();
			for (int y = range.rows().begin(); y < rowEnd; y++)
			{
				int colEnd = range.cols().end();
				for (int x = range.cols().begin(); x < colEnd; x++)
				{
					SumColor sumColor(x, y);
					parallel_reduce(blocked_range<size_t>(0, numSamplesPerPixel), sumColor);
					vec3 color = sumColor._sumColor;
					color /= float(numSamplesPerPixel);

					// Gamma correction
					color = glm::sqrt(color);
					imageBuffer[x][y] = color;

				}
			}
		});

	// Output to file

	for (int y = g_screen->height - 1; y >= 0; y--)
	{
		for (int x = 0; x < g_screen->width; x++)
		{
			vec3 color = imageBuffer[x][y];
			int ir = int(color.r * 255.99);
			int ig = int(color.g * 255.99);
			int ib = int(color.b * 255.99);
			file << ir << " " << ig << " " << ib << "\n";
		}
	}

#else

	for (int y = g_screen->height - 1; y >= 0; y--)
	{
		for (int x = 0; x < g_screen->width; x++)
		{
			vec3 color(0.0f, 0.0f, 0.0f);
			for (int n = 0; n < numSamplesPerPixel; n++)
			{
				vec2 uv = Sampler::RandomSampleFromPixel(x, y, g_screen->width, g_screen->height);
				Ray r = g_camera->GetRay(uv);
				color += Shade(r, 50);
			}

			color /= float(numSamplesPerPixel);

			// Gamma correction
			color = glm::sqrt(color);

			int ir = int(color.r * 255.99);
			int ig = int(color.g * 255.99);
			int ib = int(color.b * 255.99);

			file << ir << " " << ig << " " << ib << "\n";
		}
	}
#endif


#if REPORT
	auto end = chrono::steady_clock::now();
	cout << "Render " << g_screen->width << "x" << g_screen->height << " image took " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
	cout << "	" << numSamplesPerPixel << " samples / pixel" << endl;
	cout << "Scene: " << endl;
	cout << "	Camera: " << endl;
	cout << "		Look from: " << lookFrom.x << ", " << lookFrom.y << ", " << lookFrom.z << endl;
	cout << "		Look at: " << lookAt.x << ", " << lookAt.y << ", " << lookAt.z << endl;
	cout << "		Aspect: " << aspect << endl;
	cout << "		FOV: " << vfov << endl;
	cout << "		Aperture: " << aperture << endl;
	cout << "		Focus distance: " << focusDist << endl;
	cout << "	" << g_scene.objects.size() << " objects" << endl;
	cout << "	" << g_scene.materials.size() << " materials" << endl;
#endif
	file.close();
}

void InitScene()
{
	// Scene
	g_scene.materials.emplace_back(new LambertianMaterial(vec3(0.2, 0.3, 0.7)));
	g_scene.materials.emplace_back(new LambertianMaterial(vec3(0.7, 0.2, 0.2)));
	g_scene.materials.emplace_back(new MetalMaterial(vec3(0.8, 0.6, 0.2), 0.3));
	g_scene.materials.emplace_back(new MetalMaterial(vec3(0.8, 0.8, 0.8), 1.0));
	g_scene.materials.emplace_back(new MetalMaterial(vec3(0.4, 0.6, 0.2), 1.0));
	g_scene.materials.emplace_back(new DielectricMaterial(2.5));
	g_scene.materials.emplace_back(new DielectricMaterial(1.2));

	g_scene.objects.emplace_back(new Sphere(vec3(0,-100.5,0), 100, 0));
	g_scene.objects.emplace_back(new Sphere(vec3(0,0,0), 0.5, 1));
	g_scene.objects.emplace_back(new Sphere(vec3(-1.1,0,0), 0.5, 5));
	g_scene.objects.emplace_back(new Sphere(vec3(1.1,0,0), 0.5, 2));

	int n = 100;
	float spawnRadius = 3.0f;
	float halfSpawnRadius = spawnRadius * 0.5f;
	float ground = -0.5f;
	for (int i = 0; i < n; i++)
	{
		int randomMaterialId = floor(Sampler::Random01() * float(g_scene.materials.size() - 1)) + 1;
		if (randomMaterialId > 0 && randomMaterialId < g_scene.materials.size())
		{
			float randomRadius = Sampler::Random01() * 0.1f;
			vec3 randomPosition = spawnRadius * vec3(Sampler::Random01(), 0, Sampler::Random01()) - vec3(halfSpawnRadius, 0, halfSpawnRadius);
			randomPosition.y = ground + randomRadius;
			g_scene.objects.emplace_back(new Sphere(randomPosition, randomRadius, randomMaterialId));
		}
	}

	// Generate lots of random sphere
}

int main()
{
	InitScene();
	RenderFullscreen();
	return 0;
}