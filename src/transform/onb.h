#ifndef ONB_H
#define ONB_H

#include <glm/glm.hpp>

using namespace glm;
using namespace std;

// Othorgonal-normal basics
class ONB
{
public:

	vec3 u() const { return axis[0];}
	vec3 v() const { return axis[1];}
	vec3 w() const { return axis[2];}
	vec3 Local(float a, float b, float c) const
	{
		return a * u() + b * v() + c * w();
	}
	vec3 Local(const vec3& a) const
	{
		return a.x * u() + a.y * v() + a.z * w();
	}

	void BuildFromW(const vec3& n)
	{
		axis[2] = normalize(n);

		// Pick a vector not parallel to n
		vec3 a;
		if (fabs(w().x) > 0.9)
		{
			a = vec3(0, 1, 0);
		}
		else
		{
			a = vec3(1, 0, 0);
		}
		axis[1] = normalize(cross(w(), a));
		axis[0] = normalize(cross(w(), v()));
	}


	vec3 axis[3]; // 3 basics
};

#endif