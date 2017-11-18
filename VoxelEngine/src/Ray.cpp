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

bool Voxel::Ray::doesIntersectsTriangle(const Geometry::Triangle & triangle, float & t)
{
	// compute plane's normal
	glm::vec3 v0v1 = triangle.p2 - triangle.p1;
	glm::vec3 v0v2 = triangle.p3 - triangle.p1;
	// no need to normalize
	//glm::vec3 N = glm::cross(v0v1, v0v2); // N 
	//float area2 = N.length();
	glm::vec3 N = glm::vec3(0, 1, 0);

	glm::vec3 dir = end - start;

	// Step 1: finding P

	// check if ray and plane are parallel ?
	float NdotRayDirection = glm::dot(N, dir);
	if (fabs(NdotRayDirection) < 0.00000001f) // almost 0 
		return false; // they are parallel so they don't intersect ! 

	// compute d parameter using equation 2
	float d = glm::dot(N, triangle.p1);

	// compute t (equation 3)
	//t = (glm::dot(N, start) + d) / NdotRayDirection;
	// check if the triangle is in behind the ray
//	if (t < 0) return false; // the triangle is behind 

							 // compute the intersection point using equation 1
	//glm::vec3 P = start + t * dir;
	glm::vec3 P = end;

	// Step 2: inside-outside test
	glm::vec3 C; // vector perpendicular to triangle's plane 

			 // edge 0
	glm::vec3 edge0 = triangle.p2 - triangle.p1;
	glm::vec3 vp0 = P - triangle.p1;
	C = glm::cross(edge0, vp0);
	if (glm::dot(N, C) < 0) return false; // P is on the right side 

										   // edge 1
	glm::vec3 edge1 = triangle.p3 - triangle.p2;
	glm::vec3 vp1 = P - triangle.p2;
	C = glm::cross(edge0, vp1);
	if (glm::dot(N, C) < 0)  return false; // P is on the right side 

											// edge 2
	glm::vec3 edge2 = triangle.p1 - triangle.p3;
	glm::vec3 vp2 = P - triangle.p3;
	C = glm::cross(edge0, vp2);
	if (glm::dot(N, C) < 0) return false; // P is on the right side; 

	return true; // this ray hits the triangle 
}

bool Voxel::Ray::doesIntersectsTriangle2(const Geometry::Triangle & triangle)
{
	const glm::vec3 e_1 = triangle.p2 - triangle.p1;
	const glm::vec3 e_2 = triangle.p3 - triangle.p1;

	//const glm::vec3 n = glm::normalize(glm::cross(e_1, e_2)); // N 
	const glm::vec3 n = glm::vec3(0, 1, 0);

	const glm::vec3 dir = end - start;
	const glm::vec3 q = glm::cross(dir, e_2);
	const float a = glm::dot(e_1, q);

	if ((glm::dot(n, dir) >= 0) || (glm::abs(a) < 0.00000001f))
	{
		return false;
	}

	const glm::vec3 s = (start - triangle.p1) / a;
	const glm::vec3 r = glm::cross(r, e_1);

	float b[3];

	b[0] = glm::dot(s, q);
	b[1] = glm::dot(r, dir);
	b[2] = 1.0f - b[0] - b[1];

	if ((b[0] < 0.0f) || (b[1] < 0.0f) || (b[2] < 0.0f)) 
		return false;

	float t = glm::dot(e_2, r);
	return t >= 0.0f;
}

bool Voxel::Ray::doesIntersectsTriangle3(const Geometry::Triangle & triangle)
{
	glm::vec3 pq = end - start;
	glm::vec3 pa = triangle.p1 - start;
	glm::vec3 pb = triangle.p2 - start;
	glm::vec3 pc = triangle.p3 - start;

	// Test if pq is inside the edges bc, ca and ab. Done by testing
	// that the signed tetrahedral volumes, computed using scalar triple
	// products, are all positive
	float u = scalarTriple(pq, pc, pb);

	if (u < 0.0f) 
		return 0;

	float v = scalarTriple(pq, pa, pc);

	if (v < 0.0f) 
		return 0;

	float w = scalarTriple(pq, pb, pa);

	if (w < 0.0f) 
		return 0;

	// Compute the barycentric coordinates (u, v, w) determining the
	// intersection point r, r = u*a + v*b + w*c
	float denom = 1.0f / (u + v + w);
	u *= denom;
	v *= denom;
	w *= denom; // w = 1.0f - u - v;
	return 1;
}

float Voxel::Ray::scalarTriple(const glm::vec3 & u, const glm::vec3 & v, const glm::vec3 & w)
{
	return glm::dot(u, glm::cross(v, w));
}

void Voxel::Ray::print()
{
	std::cout << "[Ray] start: (" << start.x << ", " << start.y << ", " << start.z << "), end: (" << end.x << ", " << end.y << ", " << end.z << ")\n";
}
