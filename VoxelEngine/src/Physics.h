#ifndef PHYSICS_H
#define PHYSICS_H

#include <glm\glm.hpp>
#include <vector>

namespace Voxel
{
	class Triangle
	{
	public:
		Triangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);
		~Triangle() = default;

		glm::vec3 p1;
		glm::vec3 p2;
		glm::vec3 p3;
	};
	/**
	*	@class Plane
	*	@brief Simple plane contains normal and distance from origin
	*/
	class Plane
	{
	public:
		Plane(const glm::vec3& normal, const float distance);
		Plane(const Triangle& triangle);
		~Plane() = default;

		glm::vec3 normal;
		float distance;
	};


	/**
	*	@class AABB
	*	@brief Simple Axis Aligned Bounding box
	*/
	class AABB
	{
	public:
		AABB();
		AABB(const glm::vec3& center, const glm::vec3& size);
		~AABB() = default;

		glm::vec3 center;
		glm::vec3 size;

		// Order: Front > Left > Back > Right > Top > Bottom
		// 2 Triangles per side. Return vector must be size of 12.
		std::vector<Plane> toPlanes() const;
		std::vector<Triangle> toTriangles() const;

		glm::vec3 getMin() const;
		glm::vec3 getMax() const;

		void print();
	};
}

#endif