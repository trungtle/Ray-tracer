#ifndef SCREEN_H
#define SCREEN_H

#include "ray/ray.h"

using namespace glm;
using namespace std;

vec3 lerp(float t, const vec3& fullT, const vec3& zeroT)
{
	return (1.0f - t) * zeroT + t * fullT;
}

struct Screen
{
	Screen(int w, int h) : width(w), height(h) {}

	// Return screen color
	static vec3 Color(const Ray& ray)
	{
		const vec3 white(0.5f, 0.7f, 1.0f);
		const vec3 skyblue(1.0f, 1.0f, 1.0f);

		vec3 normR = ray.direction;
		float t = 0.5f * (normR.y + 1.0f);
		return lerp(t, white, skyblue);
	}

	int width;
	int height;

};

#endif