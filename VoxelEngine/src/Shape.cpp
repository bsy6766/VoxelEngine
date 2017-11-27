#include "Shape.h"

#include <Cube.h>
#include <iostream>

using namespace Voxel;

Shape::Triangle::Triangle(const glm::vec3 & p1, const glm::vec3 & p2, const glm::vec3 & p3, const glm::vec3& n)
	: p1(p1)
	, p2(p2)
	, p3(p3)
	, n(n)
{
	this->p1 = p1;
	this->p2 = p2;
	this->p3 = p3;
	//checkOrientation();
}

void Voxel::Shape::Triangle::checkOrientation()
{
	int total = 0;
	int index = 0;
	std::vector<glm::vec3> vertices = { p1, p2, p3 };
	auto it = vertices.begin();
	auto next = vertices.begin();
	std::advance(next, 1);

	for (; next != vertices.end(); )
	{
		total += static_cast<int>((next->x - it->x) * (next->y + it->y));
		index++;
		it++;
		next++;

		if (next == vertices.end())
		{
			next = vertices.begin();
			total += static_cast<int>((next->x - it->x) * (next->y + it->y));
			break;
		}
	}

	if (total >= 0)
	{
		// clock wise
		auto vert_it = std::begin(vertices);
		std::advance(vert_it, 1);
		std::reverse(vert_it, std::end(vertices));

		p1 = vertices.at(0);
		p2 = vertices.at(1);
		p3 = vertices.at(2);
	}
	else
	{
		// counter clock wise
		return;
	}
}




Shape::Plane::Plane(const glm::vec3 & normal, const float distance)
	: normal(normal)
	, distance(distance)
{
}

Shape::Plane::Plane(const Triangle& triangle)
{
	normal = glm::normalize(glm::cross(triangle.p2 - triangle.p1, triangle.p3 - triangle.p1));
	distance = glm::dot(normal, triangle.p1);
}



Voxel::Shape::Rect::Rect()
	: center(0)
	, size(0)
{}

Voxel::Shape::Rect::Rect(const glm::vec2 & center, const glm::vec2 & size)
	: center(center)
	, size(size)
{}

glm::vec2 Voxel::Shape::Rect::getMin() const
{
	return center - (size * 0.5f);
}

glm::vec2 Voxel::Shape::Rect::getMax() const
{
	return center + (size * 0.5f);
}

bool Voxel::Shape::Rect::containsPoint(const glm::vec2 & point)
{
	auto min = getMin();
	auto max = getMax();
	return ((min.x <= point.x && point.x <= max.x) && (min.y <= point.y && point.y <= max.y));
}





Shape::AABB::AABB()
	: center(0)
	, size(0)
{}

Shape::AABB::AABB(const glm::vec3 & center, const glm::vec3 & size)
{
	this->center = center;
	this->size = size;
}

Voxel::Shape::AABB::AABB(const glm::vec3 & center, const float width, const float height, const float length)
{
	this->center = center;
	this->size = glm::vec3(width, height, length);
}

/*
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
*/

std::vector<Shape::Triangle> Shape::AABB::toTriangles() const
{
	std::vector<Triangle> triangles;

	// copy
	std::vector<glm::vec3> cubeVertices = Cube::allVerticesGlmVec3;

	for (auto& vertex : cubeVertices)
	{
		vertex += center;
	}

	// Front
	triangles.push_back(Triangle(cubeVertices.at(0), cubeVertices.at(1), cubeVertices.at(2), glm::vec3(0.0f, 0.0f, -1.0f)));
	//triangles.push_back(Triangle(cubeVertices.at(1), cubeVertices.at(2), cubeVertices.at(3), glm::vec3(0.0f, 0.0f, -1.0f)));
	triangles.push_back(Triangle(cubeVertices.at(3), cubeVertices.at(2), cubeVertices.at(1), glm::vec3(0.0f, 0.0f, -1.0f)));

	// Left
	triangles.push_back(Triangle(cubeVertices.at(4), cubeVertices.at(5), cubeVertices.at(0), glm::vec3(-1.0f, 0.0f, 0.0f)));
	//triangles.push_back(Triangle(cubeVertices.at(5), cubeVertices.at(0), cubeVertices.at(1), glm::vec3(-1.0f, 0.0f, 0.0f)));
	triangles.push_back(Triangle(cubeVertices.at(1), cubeVertices.at(0), cubeVertices.at(5), glm::vec3(-1.0f, 0.0f, 0.0f)));

	// Back
	triangles.push_back(Triangle(cubeVertices.at(6), cubeVertices.at(7), cubeVertices.at(4), glm::vec3(0.0f, 0.0f, 1.0f)));
	//triangles.push_back(Triangle(cubeVertices.at(7), cubeVertices.at(4), cubeVertices.at(5), glm::vec3(0.0f, 0.0f, 1.0f)));
	triangles.push_back(Triangle(cubeVertices.at(5), cubeVertices.at(4), cubeVertices.at(7), glm::vec3(0.0f, 0.0f, 1.0f)));

	// Right
	triangles.push_back(Triangle(cubeVertices.at(2), cubeVertices.at(3), cubeVertices.at(6), glm::vec3(1.0f, 0.0f, 0.0f)));
	//triangles.push_back(Triangle(cubeVertices.at(3), cubeVertices.at(6), cubeVertices.at(7), glm::vec3(1.0f, 0.0f, 0.0f)));
	triangles.push_back(Triangle(cubeVertices.at(7), cubeVertices.at(6), cubeVertices.at(3), glm::vec3(1.0f, 0.0f, 0.0f)));

	// Top
	triangles.push_back(Triangle(cubeVertices.at(1), cubeVertices.at(5), cubeVertices.at(3), glm::vec3(0.0f, 1.0f, 0.0f)));
	//triangles.push_back(Triangle(cubeVertices.at(5), cubeVertices.at(3), cubeVertices.at(7), glm::vec3(0.0f, 1.0f, 0.0f)));
	triangles.push_back(Triangle(cubeVertices.at(7), cubeVertices.at(3), cubeVertices.at(5), glm::vec3(0.0f, 1.0f, 0.0f)));

	// Bottom
	//triangles.push_back(Triangle(cubeVertices.at(0), cubeVertices.at(4), cubeVertices.at(2), glm::vec3(0.0f, -1.0f, 0.0f)));
	//triangles.push_back(Triangle(cubeVertices.at(4), cubeVertices.at(2), cubeVertices.at(6), glm::vec3(0.0f, -1.0f, 0.0f)));
	//triangles.push_back(Triangle(cubeVertices.at(6), cubeVertices.at(2), cubeVertices.at(4), glm::vec3(0.0f, -1.0f, 0.0f)));

	triangles.push_back(Triangle(cubeVertices.at(4), cubeVertices.at(0), cubeVertices.at(6), glm::vec3(0.0f, -1.0f, 0.0f)));
	triangles.push_back(Triangle(cubeVertices.at(2), cubeVertices.at(6), cubeVertices.at(0), glm::vec3(0.0f, -1.0f, 0.0f)));

	return triangles;
}

glm::vec3 Shape::AABB::getMin() const
{
	return center - (size * 0.5f);
}

glm::vec3 Shape::AABB::getMax() const
{
	return center + (size * 0.5f);
}

glm::vec3 Voxel::Shape::AABB::getCenter() const
{
	return center;
}

glm::vec3 Voxel::Shape::AABB::getSize() const
{
	return size;
}

bool Shape::AABB::doesIntersectsWith(const AABB & other)
{
	auto min = getMin();
	auto max = getMax();
	auto oMin = other.getMin();
	auto oMax = other.getMax();

	//return (min.x <= oMax.x && max.x >= oMin.x) &&
	//	(min.y <= oMax.y && max.y >= oMin.y) &&
	//	(min.z <= oMax.z && max.z >= oMin.z);
	return (min.x < oMax.x && max.x > oMin.x) &&
		(min.y < oMax.y && max.y > oMin.y) &&
		(min.z < oMax.z && max.z > oMin.z);
}

bool Voxel::Shape::AABB::doesIntersectsWith(const Sphere & other)
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

bool Voxel::Shape::AABB::isAnyZero() const
{
	return (size.x == 0) || (size.y == 0) || (size.z == 0);
}

bool Voxel::Shape::AABB::isAllZero() const
{
	return (size.x == 0) && (size.y == 0) && (size.z == 0);
}

void Shape::AABB::print()
{
	std::cout << "[BoundingBox] center: (" << center.x << ", " << center.y << ", " << center.z << "), size: (" << size.x << ", " << size.y << ", " << size.z << ")\n";
}

Voxel::Shape::Sphere::Sphere()
	: radius(0)
	, position(0)
{}

Voxel::Shape::Sphere::Sphere(const float radius, const glm::vec3 & position)
	: radius(radius)
	, position(position)
{
	if (radius < 0)
	{
		this->radius = 0.0f;
	}
}
