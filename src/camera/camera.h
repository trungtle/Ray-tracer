#ifndef CAMERA_H
#define CAMERA_H

#include "ray/ray.h"
#include "sampler/sampler.h"

class Camera
{
public:
	Camera(const vec3& lookFrom, const vec3& lookAt, float vfov /*vertical fov*/, float aspect, float lR, float focusDist)
		: origin(lookFrom), lensRadius(lR)
	{
		float theta = vfov * M_PI / 180.0f;
		float halfHeight = tan(theta / 2.0f);
		float halfWidth = halfHeight * aspect;

		static const vec3 worldUp(0, 1.0f, 0);
		forward = normalize(lookAt - lookFrom);
		right = normalize(cross(worldUp, forward));
		up = normalize(cross(forward, right));

		lowerLeft = origin - halfWidth * focusDist * right - halfHeight * focusDist * up + focusDist * forward;
		horizontal = 2.0f * halfWidth * focusDist * right;
		vertical = 2.0f * halfHeight * focusDist * up;

	}

	Ray GetRay(vec2 uv)
	{
		vec3 rp = lensRadius * Sampler::RandomSampleFromUnitDisk();
		vec3 offset = right * rp.x + up * rp.y;
		Ray r(origin + offset, lowerLeft + uv.x * horizontal + uv.y * vertical - origin - offset);
		return r;
	}

private:
	vec3 lowerLeft;
	vec3 horizontal;
	vec3 vertical;
	vec3 origin;
	vec3 up;
	vec3 forward;
	vec3 right;
	float lensRadius;

};

#endif