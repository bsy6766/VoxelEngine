#include "Ray.h"
#include <iostream>

using namespace Voxel;

Voxel::Ray::Ray(const glm::vec3 & start, const glm::vec3 & end)
	: start(start)
	, end(end)
{}

glm::vec3 Voxel::Ray::getStart() const
{
	return start;
}

glm::vec3 Voxel::Ray::getEnd() const
{
	return end;
}

int Voxel::Ray::doesIntersectsTriangle(const Geometry::Triangle & triangle, glm::vec3& intersectingPoint)
{
	glm::vec3 u, v, n;				// triangle vectors
	glm::vec3 dir, w0, w;			// ray vectors
	float r, a, b;					// params to calc ray-plane intersect

	// get triangle edge vectors and plane normal
	u = triangle.p2 - triangle.p1;
	v = triangle.p3 - triangle.p1;
	n = glm::cross(u, v);

	if (n == glm::vec3(0))
	{
		// triangle is degenerate. do not deal with this case
		return -1;                  
	}

	dir = end - start;              // ray direction vector
	w0 = start - triangle.p1;
	a = -glm::dot(n, w0);
	b = glm::dot(n, dir);

	if (fabs(b) <  0.00000001f) 
	{
		// ray is  parallel to triangle plane
		if (a == 0)
		{
			// ray lies in triangle plane
			return 2;
		}
		else
		{
			// ray disjoint from plane
			return 0;
		}
	}

	// get intersect point of ray with triangle plane
	r = a / b;
	if (r < 0.0)
	{
		// ray goes away from triangle
		// => no intersect
		// for a segment, also test if (r > 1.0) => no intersect
		return 0;
	}

	intersectingPoint = start + (r * dir);  // intersect point of ray and plane

	// is I inside T?
	float    uu, uv, vv, wu, wv, D;
	uu = glm::dot(u, u);
	uv = glm::dot(u, v);
	vv = glm::dot(v, v);
	w = intersectingPoint - triangle.p1;
	wu = glm::dot(w, u);
	wv = glm::dot(w, v);
	D = uv * uv - uu * vv;

	// get and test parametric coords
	float s, t;
	s = (uv * wv - vv * wu) / D;

	if (s < 0.0 || s > 1.0)
	{
		// I is outside T
		return 0;
	}

	t = (uv * wu - uu * wv) / D;

	if (t < 0.0 || (s + t) > 1.0)
	{
		// I is outside T
		return 0;
	}

	// I is in T
	return 1;                       
}

void Voxel::Ray::print()
{
	std::cout << "[Ray] start: (" << start.x << ", " << start.y << ", " << start.z << "), end: (" << end.x << ", " << end.y << ", " << end.z << ")\n";
}
