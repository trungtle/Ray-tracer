#include <iostream>
#include <fstream>
#include <glm/glm.hpp>

using namespace std;

struct Screen
{
	Screen(int w, int h) : width(w), height(h) {}

	int width;
	int height;
};

void RenderTexture()
{
	ofstream file;
	file.open("../images/image.ppm");	

	Screen screen(200, 100);
	file << "P3\n" << screen.width << " " << screen.height << "\n255\n";
	for (int y = screen.height - 1; y >= 0; y--)
	{
		for (int x = 0; x < screen.width; x++)
		{
			glm::vec3 color(
				float(x) / float(screen.width),
				float(y) / float(screen.height),
				0.2);
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