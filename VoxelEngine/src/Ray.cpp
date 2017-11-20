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

/*
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
*/

bool Voxel::Ray::doesIntersectsTriangle(const Shape::Triangle & triangle, glm::vec3& intersectingPoint)
{
	glm::vec3 pq = end - start;
	glm::vec3 pa = triangle.p1 - start;
	glm::vec3 pb = triangle.p2 - start;
	glm::vec3 pc = triangle.p3 - start;
	
	// normal
	glm::vec3 p21 = triangle.p2 - triangle.p1;
	glm::vec3 p31 = triangle.p3 - triangle.p1;
	//glm::vec3 n = glm::normalize(glm::cross(p21, p31));

	float d = glm::dot(triangle.n, glm::normalize(start - end));

	if (d <= 0)
	{
		return false;
	}

	// Test if pq is inside the edges bc, ca and ab. Done by testing
	// that the signed tetrahedral volumes, computed using scalar triple
	// products, are all positive
	float u = scalarTriple(pq, pc, pb);

	if (u < 0.0f)
	{
		return false;
	}

	float v = scalarTriple(pq, pa, pc);

	if (v < 0.0f)
	{
		return false;
	}

	float w = scalarTriple(pq, pb, pa);

	if (w < 0.0f)
	{
		return false;
	}

	// Compute the barycentric coordinates (u, v, w) determining the
	// intersection point r, r = u*a + v*b + w*c
	float sum = u + v + w;
	if (sum == 0)
	{
		return false;
	}

	float denom = 1.0f / sum;
	u *= denom;
	v *= denom;
	w *= denom; // w = 1.0f - u - v;

	intersectingPoint = (triangle.p1 * u) + (triangle.p2 * v) + (triangle.p3 * w);

	return true;
}

Cube::Face Voxel::Ray::getIntersectingAABBFace(const Shape::AABB & aabb)
{
	Cube::Face result = Cube::Face::NONE;
	auto triangles = aabb.toTriangles();

	float minDist = std::numeric_limits<float>::max();
	unsigned int closestTriangle = 1000;

	for (unsigned int i = 0; i < triangles.size(); i++)
	{
		glm::vec3 intersectingPoint = glm::vec3(0);

		bool rayResult = doesIntersectsTriangle(triangles.at(i), intersectingPoint);

		if (rayResult)
		{
			float dist = glm::abs(glm::distance(intersectingPoint, start));
			if (dist < minDist)
			{
				//std::cout << "hit: " << i << std::endl;
				//std::cout << "point: " << Utility::Log::vec3ToStr(intersectingPoint) << std::endl;
				minDist = dist;
				closestTriangle = i;
			}
		}
	}

	switch (closestTriangle)
	{
	case 0:
	case 1:
		result = Cube::Face::FRONT;
		//std::cout << "FRONT\n";
		break;
	case 2:
	case 3:
		result = Cube::Face::LEFT;
		//std::cout << "LEFT\n";
		break;
	case 4:
	case 5:
		result = Cube::Face::BACK;
		//std::cout << "BACK\n";
		break;
	case 6:
	case 7:
		result = Cube::Face::RIGHT;
		//std::cout << "RIGHT\n";
		break;
	case 8:
	case 9:
		result = Cube::Face::TOP;
		//std::cout << "TOP\n";
		break;
	case 10:
	case 11:
		result = Cube::Face::BOTTOM;
		//std::cout << "BOTTOM\n";
		break;
	default:
		break;
	}

	return result;
}

float Voxel::Ray::getMinimumIntersectingDistance(const Shape::AABB & aabb)
{
	auto triangles = aabb.toTriangles();

	float minDist = std::numeric_limits<float>::max();
	
	for (unsigned int i = 0; i < triangles.size(); i++)
	{
		glm::vec3 intersectingPoint;
		bool rayResult = doesIntersectsTriangle(triangles.at(i), intersectingPoint);

		if (rayResult)
		{
			//std::cout << "hit: " << i << std::endl;
			//std::cout << "point: " << Utility::Log::vec3ToStr(intersectingPoint)<< std::endl;
			float dist = glm::abs(glm::distance(intersectingPoint, start));
			if (dist < minDist)
			{
				minDist = dist;
			}
		}
	}
	
	return minDist;
}

float Voxel::Ray::scalarTriple(const glm::vec3 & u, const glm::vec3 & v, const glm::vec3 & w)
{
	return glm::dot(u, glm::cross(v, w));
}

void Voxel::Ray::print()
{
	std::cout << "[Ray] start: (" << start.x << ", " << start.y << ", " << start.z << "), end: (" << end.x << ", " << end.y << ", " << end.z << ")\n";
}
