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

	// Find a random direction, cosine weighted, with z axis as normal
	// If we sample the variables with cosine weighted, then we can use
	// our pdf as cos(theta) / pi
	static vec3 RandomCosineDirection()
	{
		// Sampling with 2 variables over a cosine weighted direction
		// r1 = Integral_0_phi(1 /(2 * PI)) -> phi = 2 * PI * r1
		// r2 = Integral_0_theta(2 * PI * f(t) * sin(t)) with f(t) = cos(theta) / PI
		// -> r2 = 1 - cos^2(theta) -> cos(theta) = sqrt(1 - r2)
		float r1 = Sampler::Random01();
		float r2 = Sampler::Random01();
		float z = glm::sqrt(1.0f - r2); // this is cos(theta)
		float phi = 2.0 * M_PI * r1;
		float x = cos(phi) * 2 * glm::sqrt(r2);
		float y = sin(phi) * 2 * glm::sqrt(r2);
		return vec3(x, y, z);
	}

	static default_random_engine generator;
	static uniform_real_distribution<double> distribution;

};

default_random_engine Sampler::generator;
uniform_real_distribution<double> Sampler::distribution(0.0, 1.0);

#endif