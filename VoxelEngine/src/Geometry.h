#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <glm\glm.hpp>
#include <vector>

namespace Voxel
{
	namespace Geometry
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
			// Default construcotr
			AABB();
			// Constructor with values
			AABB(const glm::vec3& center, const glm::vec3& size);
			// Nothing to release, use default destructor
			~AABB() = default;

			// Center of AABB
			glm::vec3 center;
			// Size of AABB (Not radius)
			glm::vec3 size;

			// Order: Front > Left > Back > Right > Top > Bottom
			// 2 Triangles per side. Return vector must be size of 12.
			std::vector<Plane> toPlanes() const;
			std::vector<Triangle> toTriangles() const;

			/**
			*	@return A minimum point of AABB.
			*/
			glm::vec3 getMin() const;

			/**
			*	@return A maximum point of AABB.
			*/
			glm::vec3 getMax() const;

			/**
			*	@return glm::vec3 center point of AABB
			*/
			glm::vec3 getCenter() const;

			/**
			*	@return glm::vec3 size of AABB
			*/
			glm::vec3 getSize() const;

			/**
			*	Check if this AABB intersects with other AABB.
			*
			*	@param [int] other An reference of other AABB to check intersection
			*	@return true if this AABB intersects other AABB. Else, false.
			*/
			bool doesIntersectsWith(const AABB& other);

			/**
			*	Checks if this AABB has any or all axis equal to zero
			*
			*	@param [in] all Set true to check all 3 axis. Else, false. If false, AABB will be zero if there is one or more axis sized 0.
			*	@return true If AABB is zero;
			*/
			bool isZero(const bool all) const;

			/**
			*	Print AABB info.
			*/
			void print();
		};
	}
}

#endif