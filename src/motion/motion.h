#ifndef MOTION_H
#define MOTION_H

#include <glm/glm.hpp>

using namespace glm;

class Motion
{
public:
	Motion(vec3 start, vec3 end, float tStart, float tEnd) :
		m_start(start), m_end(end), timeStart(tStart), timeEnd(tEnd)
	{

	}

	vec3 operator()(float t)
	{
		return m_start + ((t - timeStart)/(timeEnd - timeStart)) * (m_end - m_start);
	}

private:
	vec3 m_start;
	vec3 m_end;
	float timeStart;
	float timeEnd;
};

#endif