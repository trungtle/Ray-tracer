#ifndef CAMERA_H
#define CAMERA_H

#include "ray/ray.h"

class Camera
{
public:
	Camera()
	{
		lowerLeft = vec3(-4.0f, -3.0f, -1.0f);
		horizontal = vec3(8.0f, 0.0f, 0.0f);
		vertical = vec3(0.0f, 6.0f, 0.0f);
		origin  = vec3(0.0f, 0.0f, 0.0f);	
	}

	Ray GetRay(vec2 uv)
	{
		Ray r(origin, lowerLeft + uv.x * horizontal + uv.y * vertical);
		return r;
	}

private:
	vec3 lowerLeft;
	vec3 horizontal;
	vec3 vertical;
	vec3 origin;

};

#endif