#ifndef TEXTURE_H
#define TEXTURE_H

#include <glm/glm.hpp>

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

#endif