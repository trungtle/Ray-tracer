#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include "ray/ray.h"

using namespace std;
using namespace glm;

vec3 lerp(float t, const vec3& fullT, const vec3& zeroT)
{
	return (1.0f - t) * zeroT + t * fullT;
}

struct Screen
{
	Screen(int w, int h) : width(w), height(h) {}

	int width;
	int height;

	// Return screen color
	static vec3 Color(const Ray& ray)
	{
		vec3 normR = ray.normalizedDirection;
		float t = 0.5f * (normR.y + 1.0f);
		return lerp(t, vec3(0.5f, 0.7f, 1.0f), vec3(1.0f, 1.0f, 1.0f));
	}
};

void RenderTexture()
{
	ofstream file;
	file.open("../images/image.ppm");	

	Screen screen(200, 100);
	file << "P3\n" << screen.width << " " << screen.height << "\n255\n";


	vec3 lowerLeft(-2, -1, -1);
	vec3 horizontal(4, 0, 0);
	vec3 vertical(0, 2, 0);
	vec3 origin(0, 0, 0);

	for (int y = screen.height - 1; y >= 0; y--)
	{
		for (int x = 0; x < screen.width; x++)
		{
			float u = float(x) / float(screen.width);
			float v = float(y) / float(screen.height);
			Ray r(origin, lowerLeft + u * horizontal + v * vertical);

			vec3 color = Screen::Color(r);
			int ir = int(color.r * 255.99);
			int ig = int(color.g * 255.99);
			int ib = int(color.b * 255.99);

			file << ir << " " << ig << " " << ib << "\n";
		}
	}

	file.close();
}

int main()
{
	RenderTexture();
	return 0;
}