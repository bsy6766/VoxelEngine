#include "Physics.h"
#include <Cube.h>

using namespace Voxel;



Voxel::Triangle::Triangle(const glm::vec3 & p1, const glm::vec3 & p2, const glm::vec3 & p3)
	: p1(p1)
	, p2(p2)
	, p3(p3)
{
}


Voxel::Plane::Plane(const glm::vec3 & normal, const float distance)
	: normal(normal)
	, distance(distance)
{
}

Voxel::Plane::Plane(const Triangle& triangle)
{
	normal = glm::normalize(glm::cross(triangle.p2 - triangle.p1, triangle.p3 - triangle.p1));
	distance = glm::dot(normal, triangle.p1);
}


Voxel::AABB::AABB()
	: center(0)
	, size(0)
{}

Voxel::AABB::AABB(const glm::vec3 & center, const glm::vec3 & size)
	: center(center)
	, size(size)
{}

std::vector<Plane> Voxel::AABB::toPlanes() const
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

std::vector<Triangle> Voxel::AABB::toTriangles() const
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

glm::vec3 Voxel::AABB::getMin()
{
	return center - (size * 0.5f);
}

glm::vec3 Voxel::AABB::getMax()
{
	return center + (size * 0.5f);
}
