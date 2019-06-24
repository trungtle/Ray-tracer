#ifndef SAMPLER_H
#define SAMPLER_H

#include <glm/gtx/norm.hpp>
#include <random>
#include "screen/screen.h"

using namespace glm;

class Sampler
{
public:
	Sampler()
	{
	}

	static double Random01()
	{
		return distribution(generator);
	}

	vec2 RandomSampleFromPixel(Screen& screen, int x, int y)
	{
		float ru = Random01();
		float rv = Random01();
		float u = float(x + ru) / float(screen.width);
		float v = float(y + rv) / float(screen.height);
		return vec2(u, v);

	}

	static vec3 RandomSampleInUnitSphere()
	{
		vec3 p;
		do
		{
			p = vec3(Random01(), Random01(), Random01());
			p = 2.0f * p - vec3(1, 1, 1); // Scale to -1 , 1 range
		} while(glm::length2(p) > 1.0f);

		return p;
	}

	int numSamplesPerPixel;
	static default_random_engine generator;
	static uniform_real_distribution<double> distribution;

};

default_random_engine Sampler::generator;
uniform_real_distribution<double> Sampler::distribution(0.0, 1.0);

#endif