#ifndef SAMPLER_H
#define SAMPLER_H

#include <glm/gtx/norm.hpp>
#include <random>
#include "math/constants.h"
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

	static vec2 RandomSampleFromUnitDisk()
	{
		vec2 p;
		do
		{
			p = 2.0f * vec2(Random01(), Random01()) - vec2(1, 1);
		} while(dot(p, p) >= 1); // dot product with itself is squared length

		return p;
	}

	static vec2 RandomSampleFromConcentricUnitDisk()
	{
		float r1 = Random01();
		float r2 = Random01();
		r1 = 2.0 * r1 - 1.0f;
		r2 = 2.0 * r2 - 1.0f; // mapped to -1, 1

		// Degeneracy at origin
		if (r1 == 0 && r2 == 0)
		{
			return vec2(0);
		}

		float theta, radius;
		if (fabs(r1) > fabs(r2))
		{
			radius = r1;
			theta = PI_OVER_4 * r2 / r1;
		}
		else
		{
			radius = r2;
			theta = PI_OVER_2 - PI_OVER_4 * r1 / r2;
		}
		return radius * vec2(cos(theta), sin(theta));
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

	// Find a random direction, with z axis as normal.
	static vec3 RandomDirection()
	{
		// Pick a random point inside a unity sphere tangle to the xy plane.
		vec3 randomDirection = vec3(0,0,1) + Sampler::RandomSampleInUnitSphere();
		return randomDirection;
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
		// float r1 = Sampler::Random01();
		// float r2 = Sampler::Random01();
		// float z = glm::sqrt(1.0f - r2); // this is cos(theta)
		// float phi = 2.0f * M_PI * r1;
		// float x = cos(phi) * glm::sqrt(r2);
		// float y = sin(phi) * glm::sqrt(r2);
		// return vec3(x, y, z);

		// Malley's method: sample from concentric disk, then project upward
		vec2 r = RandomSampleFromConcentricUnitDisk();
		float z = glm::sqrt(std::max(0.0f, 1.0f - r.x * r.x - r.y * r.y));
		return vec3(r.x, r.y, z);
	}

	static default_random_engine generator;
	static uniform_real_distribution<double> distribution;

};

default_random_engine Sampler::generator;
uniform_real_distribution<double> Sampler::distribution(0.0, 1.0);

#endif