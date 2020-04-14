#ifndef LINE_H
#define LINE_H

#include "shapes/point.h"

class Line
{
public:
	Line(const Point3& s, const Point3& e) :
		start(s), end(e)
	{
		slope = (end.y - start.y) / (end.x - start.y);
		yIntercept = end.y - slope * end.x;
	}

	static Point3 FindIntersection(const Line& l1, const Line& l2)
	{
		
	}

	Point3 start;
	Point3 end;
	float slope;
	float yIntercept;
};

#endif