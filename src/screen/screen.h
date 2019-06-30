#ifndef SCREEN_H
#define SCREEN_H

#include "ray/ray.h"
#include "utility.h"

using namespace glm;
using namespace std;

vec3 lerp(float t, const vec3& fullT, const vec3& zeroT)
{
	return (1.0f - t) * zeroT + t * fullT;
}

struct Screen
{
	Screen(int w, int h) : width(w), height(h) {}

	static vec3 SkyColor(const Ray& ray)
	{
		const vec3 skyblue(0.5f, 0.7f, 1.0f);
		const vec3 white(1.0f, 1.0f, 1.0f);

		vec3 normR = ray.direction;
		float t = 0.5f * (normR.y + 1.0f);
		return Tr::lerp(t, skyblue, white);
	}

	static vec3 GalaxyColor(const Ray& ray)
	{
		const vec3 darksky(0.01f, 0.04f, 0.12f);
		const vec3 black(0.0f, 0.0f, 0.0f);

		vec3 normR = ray.direction;
		float t = 0.5f * (normR.y + 1.0f);
		return Tr::lerp(t, darksky, black);
	}

	int width;
	int height;

};

#endif