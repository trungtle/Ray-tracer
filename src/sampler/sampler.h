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

	static vec2 RandomSampleFromPixel(int x, int y, int width, int height)
	{
		float ru = Random01();
		float rv = Random01();
		float u = float(x + ru) / float(width);
		float v = float(y + rv) / float(height);
		return vec2(u, v);

	}

	static vec3 RandomSampleFromUnitDisk()
	{
		vec3 p;
		do
		{
			p = 2.0f * vec3(Random01(), Random01(), 0) - vec3(1, 1, 0);
		} while(dot(p, p) >= 1); // dot product with itself is squared length

		return p;
	}

	static vec3 RandomSampleInUnitSphere()
	{
		vec3 p;
		do
		{
			p = vec3(Random01(), Random01(), Random01());
			p = 2.0f * p - vec3(1.0f, 1.0f, 1.0f); // Scale to -1 , 1 range
		} while(glm::length2(p) >= 1.0f);

		return p;
	}

	static default_random_engine generator;
	static uniform_real_distribution<double> distribution;

};

default_random_engine Sampler::generator;
uniform_real_distribution<double> Sampler::distribution(0.0, 1.0);

#endif