#ifndef SCREEN_H
#define SCREEN_H

#include "ray/ray.h"
#include "spectrum/spectrum.h"

using namespace glm;
using namespace std;

static vec3 lerp(float t, const vec3& fullT, const vec3& zeroT)
{
	return (1.0f - t) * zeroT + t * fullT;
}

static inline Spectrum SpectrumLerp(float t, const Spectrum& sp1, const Spectrum& sp2)
{
    return (1.0f - t) * sp1 + t * sp2;
}

struct Screen
{
	Screen(int w, int h) : width(w), height(h) {}

	static Spectrum SkyColor(const Ray& ray)
	{
        const Spectrum skyblue(0.5f, 0.7f, 1.0f);
		const Spectrum skyorange(1.0f, 1.0f, 1.0f);

		const Spectrum white(1.0f, 1.0f, 1.0f);
		const Spectrum black(0.8f, 0.8f, 0.8f);

		vec3 normR = ray.direction;
		float t = 0.5f * (normR.y + 1.0f);
        
		return SpectrumLerp(t, skyorange, white);
	}

	static Spectrum NightColor(const Ray& ray)
	{
		const Spectrum darksky(0.14f, 0.12f, 0.14f);
		const Spectrum black(0.0f, 0.0f, 0.001f);

		vec3 normR = ray.direction;
		float t = 0.7f * (normR.y + 1.0f);
		return SpectrumLerp(t, darksky, black);
	}

	static Spectrum GalaxyColor(const Ray& ray)
	{
		const Spectrum darksky(0.01f, 0.04f, 0.12f);
		const Spectrum black(0.0f, 0.0f, 0.001f);
		const Spectrum sun(1.0f, 0.76f, 0.3f);

		vec3 normR = ray.direction;
		float t = 0.5f * (normR.x + 0.7f);
		return SpectrumLerp(t * t * t * t, sun, black);
	}

	int width;
	int height;

};

#endif
