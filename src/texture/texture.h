#ifndef TEXTURE_H
#define TEXTURE_H

#include <algorithm>
#include <glm/glm.hpp>
#include <iostream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

using namespace glm;

class Texture
{
public:
	virtual vec3 value(const vec2& uv, const vec3& point) const = 0;
};

class ConstantTexture : public Texture
{
public:
	ConstantTexture(const vec3& c) : color(c)
	{

	}

	virtual vec3 value(const vec2& uv, const vec3& point) const override
	{
		return color;
	}

	vec3 color;
};

class CheckerTexture : public Texture
{
public:
	CheckerTexture(Texture* c1, Texture* c2) : color1(c1), color2(c2)
	{

	}

	virtual vec3 value(const vec2& uv, const vec3& point) const override
	{
		float freq = 10;
		float sines = sin(freq * point.x) * sin(freq * point.y) * sin(freq * point.z);
		if (sines < 0)
		{
			return color1->value(uv, point);
		}
		else
		{
			return color2->value(uv, point);
		}
	}

	Texture* color1;
	Texture* color2;
};

class ImageTexture : public Texture
{
public:
	ImageTexture(const string& path)
	{
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		if (!data)
		{
			throw runtime_error("Can't load image");
		}
		int size = width * height * channels;
		pixels.resize(size);
		memcpy(&pixels[0], data, size);
	}

	virtual vec3 value(const vec2& uv, const vec3& point) const override
	{
		int x = uv.x * width;
		int y = uv.y * height;
		x = clamp(x, 0, width - 1);
		y = clamp(y, 0, height - 1);
		float r = int(pixels[channels * x + channels * width * y]) / 255.0f;
		float b = int(pixels[channels * x + channels * width * y + 1]) / 255.0f;
		float g = int(pixels[channels * x + channels * width * y + 2]) / 255.0f;
		return vec3(r, b, g);

	}

private:
	vector<unsigned char> pixels;
	int width;
	int height;
	int channels;
};

#endif