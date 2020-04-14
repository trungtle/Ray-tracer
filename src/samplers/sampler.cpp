#include "samplers/sampler.h"

default_random_engine Sampler::generator;
uniform_real_distribution<double> Sampler::distribution(0.0, 1.0);
