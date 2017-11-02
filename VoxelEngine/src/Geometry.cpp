#include "Geometry.h"

#include <Cube.h>
#include <iostream>

using namespace Voxel;

Geometry::Triangle::Triangle(const glm::vec3 & p1, const glm::vec3 & p2, const glm::vec3 & p3)
	: p1(p1)
	, p2(p2)
	, p3(p3)
{
}




Geometry::Plane::Plane(const glm::vec3 & normal, const float distance)
	: normal(normal)
	, distance(distance)
{
}

Geometry::Plane::Plane(const Triangle& triangle)
{
	normal = glm::normalize(glm::cross(triangle.p2 - triangle.p1, triangle.p3 - triangle.p1));
	distance = glm::dot(normal, triangle.p1);
}



Voxel::Geometry::Rect::Rect()
	: center(0)
	, size(0)
{}

Voxel::Geometry::Rect::Rect(const glm::vec2 & center, const glm::vec2 & size)
	: center(center)
	, size(size)
{}





Geometry::AABB::AABB()
	: center(0)
	, size(0)
{}

Geometry::AABB::AABB(const glm::vec3 & center, const glm::vec3 & size)
{
	this->center = center;
	this->size = size;
}

Voxel::Geometry::AABB::AABB(const glm::vec3 & center, const float width, const float height, const float length)
{
	this->center = center;
	this->size = glm::vec3(width, height, length);
}

std::vector<Geometry::Plane> Geometry::AABB::toPlanes() const
{
	std::vector<Plane> planes;

	std::vector<glm::vec3> cubeVertices = Cube::allVerticesGlmVec3;

	for (auto& vertex : cubeVertices)
	{
		vertex += center;
	}

	// Front
	planes.push_back(Plane(Triangle(cubeVertices.at(0), cubeVertices.at(1), cubeVertices.at(2))));
	//planes.push_back(Plane(Triangle(cubeVertices.at(1), cubeVertices.at(2), cubeVertices.at(3))));
	//planes.push_back(Plane(Triangle(cubeVertices.at(3), cubeVertices.at(2), cubeVertices.at(1))));

	// Left
	planes.push_back(Plane(Triangle(cubeVertices.at(4), cubeVertices.at(5), cubeVertices.at(0))));
	//planes.push_back(Plane(Triangle(cubeVertices.at(5), cubeVertices.at(0), cubeVertices.at(1))));
	//planes.push_back(Plane(Triangle(cubeVertices.at(1), cubeVertices.at(0), cubeVertices.at(5))));

	// Back
	planes.push_back(Plane(Triangle(cubeVertices.at(6), cubeVertices.at(7), cubeVertices.at(4))));
	//planes.push_back(Plane(Triangle(cubeVertices.at(7), cubeVertices.at(4), cubeVertices.at(5))));
	//planes.push_back(Plane(Triangle(cubeVertices.at(5), cubeVertices.at(4), cubeVertices.at(7))));

	// Right
	planes.push_back(Plane(Triangle(cubeVertices.at(2), cubeVertices.at(3), cubeVertices.at(6))));
	//planes.push_back(Plane(Triangle(cubeVertices.at(3), cubeVertices.at(6), cubeVertices.at(7))));
	//planes.push_back(Plane(Triangle(cubeVertices.at(7), cubeVertices.at(6), cubeVertices.at(3))));

	// Top
	planes.push_back(Plane(Triangle(cubeVertices.at(1), cubeVertices.at(5), cubeVertices.at(3))));
	//planes.push_back(Plane(Triangle(cubeVertices.at(5), cubeVertices.at(3), cubeVertices.at(7))));
	//planes.push_back(Plane(Triangle(cubeVertices.at(7), cubeVertices.at(3), cubeVertices.at(5))));

	// Bottom
	//planes.push_back(Plane(Triangle(cubeVertices.at(0), cubeVertices.at(4), cubeVertices.at(2))));
	planes.push_back(Plane(Triangle(cubeVertices.at(2), cubeVertices.at(4), cubeVertices.at(0))));
	//planes.push_back(Plane(Triangle(cubeVertices.at(4), cubeVertices.at(2), cubeVertices.at(6))));
	//planes.push_back(Plane(Triangle(cubeVertices.at(6), cubeVertices.at(2), cubeVertices.at(4))));

	return planes;
}

std::vector<Geometry::Triangle> Geometry::AABB::toTriangles() const
{
	std::vector<Triangle> triangles;

	std::vector<glm::vec3> cubeVertices = Cube::allVerticesGlmVec3;

	for (auto& vertex : cubeVertices)
	{
		vertex += center;
	}

	// Front
	triangles.push_back(Triangle(cubeVertices.at(0), cubeVertices.at(1), cubeVertices.at(2)));
	//triangles.push_back(Triangle(cubeVertices.at(1), cubeVertices.at(2), cubeVertices.at(3)));
	triangles.push_back(Triangle(cubeVertices.at(3), cubeVertices.at(2), cubeVertices.at(1)));

	// Left
	triangles.push_back(Triangle(cubeVertices.at(4), cubeVertices.at(5), cubeVertices.at(0)));
	//triangles.push_back(Triangle(cubeVertices.at(5), cubeVertices.at(0), cubeVertices.at(1)));
	triangles.push_back(Triangle(cubeVertices.at(1), cubeVertices.at(0), cubeVertices.at(5)));

	// Back
	triangles.push_back(Triangle(cubeVertices.at(6), cubeVertices.at(7), cubeVertices.at(4)));
	//triangles.push_back(Triangle(cubeVertices.at(7), cubeVertices.at(4), cubeVertices.at(5)));
	triangles.push_back(Triangle(cubeVertices.at(5), cubeVertices.at(4), cubeVertices.at(7)));

	// Right
	triangles.push_back(Triangle(cubeVertices.at(2), cubeVertices.at(3), cubeVertices.at(6)));
	//triangles.push_back(Triangle(cubeVertices.at(3), cubeVertices.at(6), cubeVertices.at(7)));
	triangles.push_back(Triangle(cubeVertices.at(7), cubeVertices.at(6), cubeVertices.at(3)));

	// Top
	triangles.push_back(Triangle(cubeVertices.at(1), cubeVertices.at(5), cubeVertices.at(3)));
	//triangles.push_back(Triangle(cubeVertices.at(5), cubeVertices.at(3), cubeVertices.at(7)));
	triangles.push_back(Triangle(cubeVertices.at(7), cubeVertices.at(3), cubeVertices.at(5)));

	// Bottom
	triangles.push_back(Triangle(cubeVertices.at(0), cubeVertices.at(4), cubeVertices.at(2)));
	//triangles.push_back(Triangle(cubeVertices.at(4), cubeVertices.at(2), cubeVertices.at(6)));
	triangles.push_back(Triangle(cubeVertices.at(6), cubeVertices.at(2), cubeVertices.at(4)));

	return triangles;
}

glm::vec3 Geometry::AABB::getMin() const
{
	return center - (size * 0.5f);
}

glm::vec3 Geometry::AABB::getMax() const
{
	return center + (size * 0.5f);
}

glm::vec3 Voxel::Geometry::AABB::getCenter() const
{
	return center;
}

glm::vec3 Voxel::Geometry::AABB::getSize() const
{
	return size;
}

bool Geometry::AABB::doesIntersectsWith(const AABB & other)
{
	auto min = getMin();
	auto max = getMax();
	auto oMin = other.getMin();
	auto oMax = other.getMax();

	return (min.x <= oMax.x && max.x >= oMin.x) &&
		(min.y <= oMax.y && max.y >= oMin.y) &&
		(min.z <= oMax.z && max.z >= oMin.z);
}

bool Voxel::Geometry::AABB::doesIntersectsWith(const Sphere & other)
{
	float distSquared = other.radius * other.radius;

	auto min = getMin();
	auto max = getMax();

	if (other.position.x < min.x)
		distSquared -= (other.position.x - min.x) * (other.position.x - min.x);
	else if (other.position.x > max.x)
		distSquared -= (other.position.x - max.x) * (other.position.x - max.x);

	if (other.position.y < min.y)
		distSquared -= (other.position.y - min.y) * (other.position.y - min.y);
	else if (other.position.y > max.y)
		distSquared -= (other.position.y - max.y) * (other.position.y - max.y);

	if (other.position.z < min.z)
		distSquared -= (other.position.z - min.z) * (other.position.z - min.z);
	else if (other.position.z > max.z)
		distSquared -= (other.position.z - max.z) * (other.position.z - max.z);

	return distSquared > 0;
}

bool Voxel::Geometry::AABB::isZero(const bool all) const
{
	if (all)
	{
		return (size.x == 0) && (size.y == 0) && (size.z == 0);
	}
	else
	{
		return (size.x == 0) || (size.y == 0) || (size.z == 0);
	}
}

void Geometry::AABB::print()
{
	std::cout << "[BoundingBox] center: (" << center.x << ", " << center.y << ", " << center.z << "), size: (" << size.x << ", " << size.y << ", " << size.z << ")\n";
}

Voxel::Geometry::Sphere::Sphere()
	: radius(0)
	, position(0)
{
}

Voxel::Geometry::Sphere::Sphere(const float radius, const glm::vec3 & position)
	: radius(radius)
	, position(position)
{
}
