#ifndef SCREEN_H
#define SCREEN_H

#include <glm/glm.hpp>

#include "ray/ray.h"

using namespace glm;
using namespace std;

static vec3 lerp(float t, const vec3& fullT, const vec3& zeroT)
{
	return (1.0f - t) * zeroT + t * fullT;
}

struct Screen
{
	Screen(int w, int h) : width(w), height(h) {}

	static vec3 SkyColor(const Ray& ray)
	{
		const vec3 skyblue(0.5f, 0.7f, 1.0f);
		const vec3 skyorange(1.0f, 1.0f, 1.0f);

		const vec3 white(1.0f, 1.0f, 1.0f);
		const vec3 black(0.8f, 0.8f, 0.8f);

		vec3 normR = ray.direction;
		float t = 0.5f * (normR.y + 1.0f);
		return lerp(t, skyorange, white);
	}

	static vec3 NightColor(const Ray& ray)
	{
		const vec3 darksky(0.14f, 0.12f, 0.14f);
		const vec3 black(0.0f, 0.0f, 0.001f);

		vec3 normR = ray.direction;
		float t = 0.7f * (normR.y + 1.0f);
		return lerp(t, darksky, black);
	}

	static vec3 GalaxyColor(const Ray& ray)
	{
		const vec3 darksky(0.01f, 0.04f, 0.12f);
		const vec3 black(0.0f, 0.0f, 0.001f);
		const vec3 sun(1.0f, 0.76f, 0.3f);

		vec3 normR = ray.direction;
		float t = 0.5f * (normR.x + 0.7f);
		return lerp(t * t * t * t, sun, black);
	}

	int width;
	int height;

};

#endif