#include <chrono>
#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>

#include "camera/Camera.h"
#include "integrators/integrator.h"
#include "shapes/box.h"
#include "shapes/cylinder.h"
#include "shapes/hitable_transform.h"
#include "shapes/rectangle.h"
#include "shapes/triangle.h"
#include "shapes/scene.h"
#include "shapes/sphere.h"
#include "shapes/mesh.h"
#include "lights/diffuse_light.h"
#include "samplers/sampler.h"
#include "screen/screen.h"
#include "spectrum/spectrum.h"

#define REPORT 1

using namespace glm;
using namespace std;

struct
{
	int nx = 64;
	int ny = 64;
	int raytracingDepth;
	int numSamplesPerPixel;
	vec3 lookFrom;
	vec3 lookAt;
	float vfov;
	float aspect;
	float aperture;
	float focusDist;

} g_settings;

void RenderFullscreen(const Scene& scene)
{
	// Screen
    std::shared_ptr<Screen> screen;
	screen = std::make_shared<Screen>(g_settings.nx, g_settings.ny);
    
    Film film(g_settings.nx, g_settings.ny, "image.ppm");
    
	// Camera
    std::shared_ptr<Camera> camera;
	camera = std::make_shared<Camera>(
		g_settings.lookFrom, 
		g_settings.lookAt, 
		g_settings.vfov, 
		g_settings.aspect, 
		g_settings.aperture, 
		g_settings.focusDist,
        &film,
        0.0f, 1.0f);

	// Integrator
    std::shared_ptr<Integrator> integrator;
	integrator = std::make_shared<Integrator>(
		camera, screen);

	// Begin render
	integrator->Render(scene, g_settings.numSamplesPerPixel);
}

void InitSceneRandomBalls(Scene& scene)
{
	g_settings.raytracingDepth = 50;
	g_settings.numSamplesPerPixel = 100;
	g_settings.lookFrom = vec3(-3, 0.3, 1.5);
	g_settings.lookAt = vec3(0, 0, 0);
	g_settings.vfov = 40;
	g_settings.aspect = float(g_settings.nx) / float(g_settings.ny);
	g_settings.aperture = 0.02f;
	g_settings.focusDist = length(g_settings.lookAt - g_settings.lookFrom);

	// Scene
	scene.materials.emplace_back(new LambertianMaterial(new ConstantTexture(Spectrum(0.2, 0.3, 0.7))));
	scene.materials.emplace_back(new LambertianMaterial(new ConstantTexture(Spectrum(0.7, 0.2, 0.2))));
	scene.materials.emplace_back(new MetalMaterial(new ConstantTexture(Spectrum(0.8, 0.6, 0.2)), 0.3));
	scene.materials.emplace_back(new MetalMaterial(new ConstantTexture(Spectrum(0.8, 0.8, 0.8)), 1.0));
	scene.materials.emplace_back(new MetalMaterial(new ConstantTexture(Spectrum(0.4, 0.6, 0.2)), 1.0));
	scene.materials.emplace_back(new DielectricMaterial(2.5));
	scene.materials.emplace_back(new DielectricMaterial(1.2));

	scene.objects.emplace_back(new Sphere(vec3(0,-100.5,0), 100, 0));
	scene.objects.emplace_back(new Sphere(vec3(0,0,0), 0.5, 1));
	scene.objects.emplace_back(new Sphere(vec3(-1.1,0,0), 0.5, 5));
	scene.objects.emplace_back(new Sphere(vec3(1.1,0,0), 0.5, 2));

	// Generate lots of random sphere
	int n = 100;
	float spawnRadius = 3.0f;
	float halfSpawnRadius = spawnRadius * 0.5f;
	float ground = -0.5f;
	for (int i = 0; i < n; i++)
	{
		int randomMaterialId = floor(Sampler::Random01() * float(scene.materials.size() - 1)) + 1;
		if (randomMaterialId > 0 && randomMaterialId < scene.materials.size())
		{
			float randomRadius = Sampler::Random01() * 0.1f;
			vec3 randomPosition = spawnRadius * vec3(Sampler::Random01(), 0, Sampler::Random01()) - vec3(halfSpawnRadius, 0, halfSpawnRadius);
			randomPosition.y = ground + randomRadius;
			scene.objects.emplace_back(new Sphere(randomPosition, randomRadius, randomMaterialId));
		}
	}

}

void InitSceneMovingBalls(Scene& scene)
{
	g_settings.raytracingDepth = 50;
	g_settings.numSamplesPerPixel = 100;
	g_settings.lookFrom = vec3(0, 0, 3);
	g_settings.lookAt = vec3(0, 0, 0);
	g_settings.vfov = 40;
	g_settings.aspect = float(g_settings.nx) / float(g_settings.ny);
	g_settings.aperture = 0.02f;
	g_settings.focusDist = length(g_settings.lookAt - g_settings.lookFrom);

	// Scene
	scene.materials.emplace_back(
		new LambertianMaterial(
			new CheckerTexture(
				new ConstantTexture(Spectrum(0.2, 0.3, 0.7)),
				new ConstantTexture(Spectrum(0.8, 0.8, 0.8))
			)
		));
	scene.materials.emplace_back(new LambertianMaterial(new ConstantTexture(Spectrum(0.7, 0.2, 0.2))));
	scene.materials.emplace_back(new MetalMaterial(new ConstantTexture(Spectrum(0.8, 0.6, 0.2)), 0));
	scene.materials.emplace_back(new MetalMaterial(new ConstantTexture(Spectrum(0.8, 0.8, 0.8)), 0));
	scene.materials.emplace_back(new LambertianMaterial(new ConstantTexture(Spectrum(0.2, 0.8, 0.2))));
	scene.materials.emplace_back(new MetalMaterial(new ConstantTexture(Spectrum(0.8, 0.4, 0.6)), 0.0));
	scene.materials.emplace_back(new MetalMaterial(new ConstantTexture(Spectrum(0.7, 0.2, 0.8))));
	scene.materials.emplace_back(new DielectricMaterial(1.2));
	scene.materials.emplace_back(new MetalMaterial(new ConstantTexture(Spectrum(0.2, 0.8, 0.2)), 0.2));


	// Ground
	scene.objects.emplace_back(new Sphere(vec3(0,-100.5,0), 100, 0));

	scene.objects.emplace_back(new Sphere(vec3(0,-.05,-1), 0.45, 3));
	scene.objects.emplace_back(new Sphere(vec3(-1,-0.1,-1), 0.4, 5));
	scene.objects.emplace_back(new Sphere(vec3(1.1,0,-1), 0.5, 2));


	Sphere* pMovingSphere = nullptr;

	float xStart = -1;
	float xEnd = 1;
	float nSpheres = 25;
	float deltaX = (xEnd - xStart) / float(nSpheres);
	for (int i = 0; i < nSpheres; i++)
	{
		float randomX = xStart + deltaX * i + Sampler::Random01() * deltaX;
		float randomY = i == nSpheres - 1 ? 0.1 : Sampler::Random01() * 0.9 - 0.1;
		float randomYMotion = Sampler::Random01() * 0.3 - 0.2;
		float randomRadius = i == nSpheres - 1 ? 0.12f : Sampler::Random01() * 0.1f;
		int randomMaterialId = i == nSpheres - 1 ? 7 : floor(Sampler::Random01() * float(scene.materials.size() - 1)) + 1;

	 	pMovingSphere = new Sphere(vec3(randomX,randomY, randomY), randomRadius, randomMaterialId);
		if (Sampler::Random01() < 0.8f && i != nSpheres - 1)
		{
			pMovingSphere->pMotion = new Motion(vec3(0, 0, 0), vec3(0, -0.15f * Sampler::Random01(), 0), 0, 1);
		}
		scene.objects.emplace_back(pMovingSphere);
	}


	// Generate lots of random sphere
	int n = 100;
	float spawnRadius = 3.0f;
	float halfSpawnRadius = spawnRadius * 0.5f;
	float ground = -0.5f;
	for (int i = 0; i < n; i++)
	{
		int randomMaterialId = floor(Sampler::Random01() * float(scene.materials.size() - 1)) + 1;
		if (randomMaterialId > 0 && randomMaterialId < scene.materials.size())
		{
			float randomRadius = Sampler::Random01() * 0.08f;
			vec3 randomPosition = spawnRadius * vec3(Sampler::Random01(), 0, Sampler::Random01()) - vec3(halfSpawnRadius, 0, halfSpawnRadius);
			randomPosition.y = ground + randomRadius;
			scene.objects.emplace_back(new Sphere(randomPosition, randomRadius, randomMaterialId));
		}
	}
}

void InitUniverseScene(Scene& scene)
{
	g_settings.raytracingDepth = 50;
	g_settings.numSamplesPerPixel = 100;
	g_settings.lookFrom = vec3(0, 0, 12);
	g_settings.lookAt = vec3(0, 0, -1);
	g_settings.vfov = 40;
	g_settings.aspect = float(g_settings.nx) / float(g_settings.ny);
	g_settings.aperture = 0.02f;
	g_settings.focusDist = length(g_settings.lookAt - g_settings.lookFrom);

	// Scene
	scene.materials.emplace_back(
		new LambertianMaterial(
			new CheckerTexture(
				new ConstantTexture(Spectrum(0.2, 0.3, 0.7)),
				new ConstantTexture(Spectrum(0.8, 0.8, 0.8))
			)
		));
	scene.materials.emplace_back(new LambertianMaterial(new ImageTexture("data/earth.png")));
	scene.materials.emplace_back(new LambertianMaterial(new ImageTexture("data/mars.png")));
	scene.materials.emplace_back(new LambertianMaterial(new ImageTexture("data/mercury.png")));
	scene.materials.emplace_back(new LambertianMaterial(new ImageTexture("data/venus.jpg")));
	scene.materials.emplace_back(new LambertianMaterial(new ImageTexture("data/jupiter.jpg")));
	scene.materials.emplace_back(new LambertianMaterial(new ImageTexture("data/pluto.png")));

	scene.materials.emplace_back(new DiffuseLight(new ConstantTexture(Spectrum(0.9f, 0.9f, 0.7f))));

	float earthRadius = 0.45;
	float mercuryRadius = earthRadius / 3.0f;
	float venusRadius = earthRadius;
	float plutoRadius = earthRadius / 6.0f;
	float marsRadius = earthRadius * 1.5f;
	float sunRadius = earthRadius * 100.0f;
	float jupiterRadius = earthRadius * 9.f;

	vec3 jupiterPosition(-jupiterRadius - 4,0,-1);

	// Sphere* pluto = new Sphere(vec3(0,-.05,-1), 0.45, 2);
	Sphere* jupiter = new Sphere(jupiterPosition, jupiterRadius, 5);
	Sphere* mars = new Sphere(vec3(-1,0,-1), marsRadius, 2);
	Sphere* earth = new Sphere(vec3(1,0,-1), earthRadius, 1);
	Sphere* venus = new Sphere(vec3(2.6,0,-1), venusRadius, 4);
	Sphere* mercury = new Sphere(vec3(4,0,-1), mercuryRadius, 3);

	// Sun
	Sphere* sun = new Sphere(vec3(10 + sunRadius,0,-1), sunRadius, scene.materials.size() - 1);

	scene.objects.emplace_back(earth);
	scene.objects.emplace_back(mars);
	scene.objects.emplace_back(mercury);
	scene.objects.emplace_back(venus);
	scene.objects.emplace_back(jupiter);
	scene.objects.emplace_back(sun);

}

void InitCornellBox(Scene& scene)
{
	g_settings.raytracingDepth = 50;
	g_settings.numSamplesPerPixel = 300;
	g_settings.lookFrom = vec3(0, 5, 14);
	g_settings.lookAt = vec3(0, 5, -1);
	g_settings.vfov = 60;
	g_settings.aspect = float(g_settings.nx) / float(g_settings.ny);
	g_settings.aperture = 0.01f;
	g_settings.focusDist = length(g_settings.lookAt - g_settings.lookFrom);

	// Scene
	scene.materials.emplace_back(
		new LambertianMaterial(
			new CheckerTexture(
				new ConstantTexture(Spectrum(0.2, 0.3, 0.7)),
				new ConstantTexture(Spectrum(0.8, 0.8, 0.8))
			)
		));
	scene.materials.emplace_back(new LambertianMaterial(new ConstantTexture(Spectrum(1, 1, 1)))); // white
	scene.materials.emplace_back(new LambertianMaterial(new ConstantTexture(Spectrum(0.2, 0.2, 0.7)))); // blue
	scene.materials.emplace_back(new LambertianMaterial(new ConstantTexture(Spectrum(0.7, 0.2, 0.2)))); // red
	scene.materials.emplace_back(new LambertianMaterial(new ConstantTexture(Spectrum(0.2, 0.7, 0.2)))); // green
	scene.materials.emplace_back(new MetalMaterial(new ImageTexture("data/mars.png"), 0.2)); //5
	scene.materials.emplace_back(new DielectricMaterial(1.2));//6
	scene.materials.emplace_back(new MetalMaterial(new ImageTexture("data/mercury.png"), 0.2));//7
	scene.materials.emplace_back(new MetalMaterial(new ImageTexture("data/venus.jpg"), 0.2));//8
	scene.materials.emplace_back(new MetalMaterial(new ConstantTexture(Spectrum(0.8, 0.7, 0.2)), 0.2));//9
	scene.materials.emplace_back(new LambertianMaterial(new ImageTexture("data/jupiter.jpg"))); //10
	scene.materials.emplace_back(new LambertianMaterial(new ImageTexture("data/pluto.png"))); //11


	scene.materials.emplace_back(new MetalMaterial(new ConstantTexture(Spectrum(0.8, 0.6, 0.2)), 0));//12
	scene.materials.emplace_back(new MetalMaterial(new ImageTexture("data/mercury.png"), 0.4));//13
	scene.materials.emplace_back(new LambertianMaterial(new ConstantTexture(Spectrum(0.2, 0.8, 0.2))));
	scene.materials.emplace_back(new MetalMaterial(new ConstantTexture(Spectrum(0.8, 0.4, 0.6)), 0.0));
	scene.materials.emplace_back(new MetalMaterial(new ConstantTexture(Spectrum(0.7, 0.2, 0.8))));
	scene.materials.emplace_back(new DielectricMaterial(1.2));
	scene.materials.emplace_back(new MetalMaterial(new ConstantTexture(Spectrum(0.2, 0.8, 0.2)), 0.2));
	
	scene.materials.emplace_back(new DiffuseLight(new ConstantTexture(Spectrum(4.f, 4.f, 4.f))));

	// Room
	float roomWidth = 5;
	float roomDepth = 15;
	Hitable* roomFloor = new RectXZ(vec2(-roomWidth, -roomDepth), vec2(roomWidth, roomDepth), 0, 0);
	Hitable* ceiling = new FlipNormal(new RectXZ(vec2(-roomWidth, -roomDepth), vec2(roomWidth, roomDepth), roomWidth * 2, 1));
	Hitable* wallBack = new RectXY(vec2(-roomWidth, 0), vec2(roomWidth, roomWidth * 2), -roomWidth, 13);
	Hitable* wallFront = new FlipNormal(new RectXY(vec2(-roomWidth, 0), vec2(roomWidth, roomWidth * 2), roomDepth, 7));
	Hitable* wallRight = new RectYZ(vec2(0, -roomDepth), vec2(roomWidth * 2, roomDepth), -roomWidth, 3);
	Hitable* wallLeft = new FlipNormal(new RectYZ(vec2(0, -roomDepth), vec2(roomWidth * 2, roomDepth), roomWidth, 4));

	// Platform
	float platformHeight = 0.2f;
	for (int i = 0; i < 1; i++)
	{
		Hitable* platform = new Box(
			vec3(-roomWidth + 0.5f * (i + 1), platformHeight * i, -roomWidth + 0.5f * (i + 1)),
			vec3(roomWidth - 0.5f * (i + 1), platformHeight * (i + 1), roomWidth - 0.5f * (i + 1)),
			0);		
		scene.objects.emplace_back(platform);
	}

	// 4 columns
	float wallColumnsRadius = 0.5f;
	float wallColumnHeight = 4;
	Hitable* wallBackLeft = new Translate(
		new CappedCylinder(wallColumnsRadius, 0, wallColumnHeight, 10), 
		vec3(-roomWidth + 1.5f, 0, roomWidth - 1.5f));
	scene.objects.emplace_back(wallBackLeft);

	Hitable* wallBackRight = new Translate(
		new CappedCylinder(wallColumnsRadius, 0, wallColumnHeight, 9), 
		vec3(roomWidth - 1.5f, 0, roomWidth - 1.5f));
	// scene.objects.emplace_back(wallBackRight);

	// Boxes
	float box1Height = 7;
	Hitable* box1 = new Translate(new Box(vec3(-1, 0, -1.5), vec3(1, box1Height, 1.5), 5), vec3(2.4, 0, -2));

	float box2Height = 2.5;
	Hitable* box2 = new Translate(
		new Box(vec3(-(box2Height/2), 0, -(box2Height/2)), vec3((box2Height/2), box2Height, (box2Height/2)), 8), 
		vec3(-2, 0, 1.5));

	// Spheres
	float sphere2Radius = 1.0f;
	Hitable* sphere2 = new Translate(
		new Sphere(vec3(0, 0, 0), sphere2Radius, 6), 
		vec3(-2, box2Height + sphere2Radius, 1.5));

	float cylinder1Radius = 0.7f;
	Hitable* cylinder1 = new Translate(
		new CappedCylinder(cylinder1Radius, 0, 2, 9), 
		vec3(2.5, 0, 3));

	float cylinder2Radius = 1.0f;
	Hitable* cylinder2 = new Translate(
		new CappedCylinder(cylinder2Radius, 0, 5.0f, 6), 
		vec3(1.1, 0, 1));

	// Light
	// Hitable* ceilingLight = new FlipNormal(new RectXZ(vec2(-4, -4), vec2(4, 4), roomWidth * 2, scene.materials.size() - 1));
	// scene.objects.emplace_back(ceilingLight);

	float lightRegionMargin = 1.0f;
	float lightRegionHalfWidth = roomWidth - lightRegionMargin;
	float lightRegionWidth = lightRegionHalfWidth * 2;
	int numLightPerRow = 5;
	float lightIntervalDist = lightRegionWidth / float(numLightPerRow - 1);
	for (int w = 0; w < numLightPerRow; w++)
	{
		for (int h = 0; h < numLightPerRow; h++)
		{
			Hitable* ceilingLight = new FlipNormal(
				new Disk(vec3(
					-lightRegionHalfWidth + float(w) * lightIntervalDist, 
					roomWidth * 2.0f, 
					-lightRegionHalfWidth + float(h) * lightIntervalDist), 
				0.8, 
				scene.materials.size() - 1));
			scene.objects.emplace_back(ceilingLight);
		}
	}

	scene.objects.emplace_back(roomFloor);
	scene.objects.emplace_back(ceiling);
	scene.objects.emplace_back(wallBack);
	scene.objects.emplace_back(wallFront);
	scene.objects.emplace_back(wallRight);
	scene.objects.emplace_back(wallLeft);
	scene.objects.emplace_back(box1);
	scene.objects.emplace_back(box2);
	scene.objects.emplace_back(cylinder1);
	scene.objects.emplace_back(cylinder2);
	scene.objects.emplace_back(sphere2);

	// Generate lots of random sphere
	// int n = 100;
	// float spawnRadius = roomWidth;
	// float halfSpawnRadius = spawnRadius * 0.5f;
	// float ground = platformHeight;
	// for (int i = 0; i < n; i++)
	// {
	// 	int randomMaterialId = floor(Sampler::Random01() * float(scene.materials.size() - 1)) + 1;
	// 	if (randomMaterialId > 0 && randomMaterialId < scene.materials.size())
	// 	{
	// 		float randomRadius = Sampler::Random01() * 0.6f;
	// 		vec3 randomPosition = spawnRadius * vec3(Sampler::Random01(), 0, Sampler::Random01()) - vec3(halfSpawnRadius, 0, halfSpawnRadius);
	// 		randomPosition.y = ground + randomRadius;
	// 		scene.objects.emplace_back(new Sphere(randomPosition, randomRadius, randomMaterialId));
	// 	}
	// }
}

void InitCornellBoxMCIntegration(Scene& scene)
{
	g_settings.nx = 400;
	g_settings.ny = 400;
	g_settings.raytracingDepth = 50;
	g_settings.numSamplesPerPixel = 40;
	g_settings.lookFrom = vec3(0, 5, 14.9);
	g_settings.lookAt = vec3(0, 5, -1);
	// Head pbrt
	// g_settings.lookFrom = vec3(0.322839 , 0.0534825, 0.504299);
	// g_settings.lookAt = vec3(-0.140808, -0.162727, -0.354936);
	// Up: 0.0355799 0.964444 -0.261882
	g_settings.vfov = 60;
	g_settings.aspect = float(g_settings.nx) / float(g_settings.ny);
	g_settings.aperture = 0.01f;
	g_settings.focusDist = length(g_settings.lookAt - g_settings.lookFrom);

	// Scene
	scene.materials.emplace_back(
		new LambertianMaterial(
			new CheckerTexture(
				new ConstantTexture(Spectrum(0.2, 0.3, 0.7)),
				new ConstantTexture(Spectrum(0.8, 0.8, 0.8))
			)
		));
	scene.materials.emplace_back(new LambertianMaterial(new ConstantTexture(Spectrum(1, 1, 1)))); // white
	scene.materials.emplace_back(new LambertianMaterial(new ConstantTexture(Spectrum(0.2, 0.2, 0.7)))); // blue
	scene.materials.emplace_back(new LambertianMaterial(new ConstantTexture(Spectrum(0.7, 0.2, 0.2)))); // red
	scene.materials.emplace_back(new LambertianMaterial(new ConstantTexture(Spectrum(0.2, 0.7, 0.2)))); // green
	scene.materials.emplace_back(new LambertianMaterial(new ConstantTexture(Spectrum(0.2, 0.7, 0.2)))); // green
	scene.materials.emplace_back(new MetalMaterial(new ConstantTexture(Spectrum(0.8, 0.8, 0.8)), 1.0));
	scene.materials.emplace_back(new DielectricMaterial(2.5));
	scene.materials.emplace_back(new DielectricMaterial(1.2));

	scene.materials.emplace_back(new DiffuseLight(new ConstantTexture(Spectrum(10.f, 10.f, 10.f))));

	// Room
	float roomWidth = 5;
	float roomDepth = 15;
	Hitable* roomFloor = new RectXZ(vec2(-roomWidth, -roomDepth), vec2(roomWidth, roomDepth), 0, 1);
	Hitable* ceiling = new FlipNormal(new RectXZ(vec2(-roomWidth, -roomDepth), vec2(roomWidth, roomDepth), roomWidth * 2 - 0.001f, 1));
	Hitable* wallBack = new RectXY(vec2(-roomWidth, 0), vec2(roomWidth, roomWidth * 2), -roomWidth + 0.001f, 1);
	Hitable* wallFront = new FlipNormal(new RectXY(vec2(-roomWidth, 0), vec2(roomWidth, roomWidth * 2), roomDepth, 1));
	Hitable* wallRight = new RectYZ(vec2(0, -roomDepth), vec2(roomWidth * 2, roomDepth), -roomWidth + 0.001f, 3);
	Hitable* wallLeft = new FlipNormal(new RectYZ(vec2(0, -roomDepth), vec2(roomWidth * 2, roomDepth), roomWidth - 0.001f, 4));

	scene.objects.emplace_back(roomFloor);
	scene.objects.emplace_back(ceiling);
	scene.objects.emplace_back(wallBack);
	scene.objects.emplace_back(wallFront);
	scene.objects.emplace_back(wallRight);
	scene.objects.emplace_back(wallLeft);

	// Boxes
	float box1Height = 6;
	Hitable* box1 = new Translate(new RotateY(
		new Box(vec3(-1.5, 0, -1.2), vec3(1.5, box1Height, 1.2), 1), 
		-30), // angle
		vec3(2, 0, 0));

	float box2Height = 2.5;
	Hitable* box2 = new Translate(new RotateY(
		new Box(vec3(-(box2Height/2), 0, -(box2Height/2)), vec3((box2Height/2), box2Height, (box2Height/2)), 1),
		20), // angle 
		vec3(-2, 0, 1.5));
	
	Hitable* sphere = 
		// new Box(vec3(-5, 0, -5), vec3(5, 5, 5), 6),
		new Sphere(vec3(0, 5, 0), 1.5f, 6);

	//Hitable* triange = new Triangle(vec3(-1.5, 0, -1.5), vec3(1.5, 0, -1.5), vec3(0, 5, -1.5), 2);
	//scene.objects.emplace_back(triange);
	//Hitable* mesh = new mi::Mesh("../data/models/gltf/Duck/glTF/Duck.gltf", 1);
	//Hitable* mesh = new mi::Mesh("../data/pbrt-v3-pbf/head.pbf", 3);
    //scene.objects.emplace_back(mesh);

	// Light
	Hitable* ceilingLight = new FlipNormal(new RectXZ(vec2(-1, -1), vec2(1, 1), roomWidth * 2 - 0.01f, scene.materials.size() - 1));
	scene.objects.emplace_back(ceilingLight);
	scene.lights.emplace_back(ceilingLight);

	// scene.objects.emplace_back(box1);
	// scene.objects.emplace_back(box2);
	// scene.objects.emplace_back(sphere);
	
}

int main()
{
#if REPORT
    cout << "Begin ray tracing" << endl;
#endif
    
	Scene scene;

	// InitSceneRandomBalls(scene);
	// InitSceneMovingBalls(scene);
	// InitUniverseScene(scene);
	// InitCornellBox(scene);
	InitCornellBoxMCIntegration(scene);
	scene.BuildAccelerationStructure();
	RenderFullscreen(scene);
    
#if REPORT
    //cout << "Render " << g_screen->width << "x" << g_screen->height << " image took " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
    cout << "    " << g_settings.numSamplesPerPixel << " samples / pixel" << endl;
    cout << "Scene: " << endl;
    cout << "    Camera: " << endl;
    cout << "        Look from: " << g_settings.lookFrom.x << ", " << g_settings.lookFrom.y << ", " << g_settings.lookFrom.z << endl;
    cout << "        Look at: " << g_settings.lookAt.x << ", " << g_settings.lookAt.y << ", " << g_settings.lookAt.z << endl;
    cout << "        Aspect: " << g_settings.aspect << endl;
    cout << "        FOV: " << g_settings.vfov << endl;
    cout << "        Aperture: " << g_settings.aperture << endl;
    cout << "        Focus distance: " << g_settings.focusDist << endl;
    cout << "    " << scene.objects.size() << " objects" << endl;
    cout << "    " << scene.materials.size() << " materials" << endl;
#endif

	return 0;
}
