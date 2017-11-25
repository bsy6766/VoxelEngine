#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <glm\glm.hpp>
#include <vector>

namespace Voxel
{
	namespace Shape
	{
		/**
		*	@class Triangle
		*	@brief Contains three points of 3D triangle
		*/
		class Triangle
		{
		public:
			/**
			*	Constructor	
			*	@param [in] p1 First point of triangle
			*	@param [in] p2 Second point of triangle
			*	@param [in] p3 Third point of triangle
			*	@param [in] n Normal of triangle surface. Used for raycasting.
			*/
			Triangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& n);

			// Default destructor.
			~Triangle() = default;

			// First point of triangle
			glm::vec3 p1;
			// Second point of triangle
			glm::vec3 p2;
			// Third point of triangle
			glm::vec3 p3;
			// Normal of triangle surface
			glm::vec3 n;

			// Checks orientation of triangle. If triangle is clockwise, reverse to counter clockwise
			void checkOrientation();
		};

		/**
		*	@class Plane
		*	@brief An infinitely extending plane in 3D.
		*/
		class Plane
		{
		public:
			/**
			*	Constructor
			*	@param normal 
			*/
			Plane(const glm::vec3& normal, const float distance);
			Plane(const Triangle& triangle);
			~Plane() = default;

			glm::vec3 normal;
			float distance;
		};

		/**
		*	@class Rect
		*	@brief 2D rectangle with center point and the size of the rectangle from center
		*/
		class Rect
		{
		public:
			// Constructor
			Rect();

			/**
			*	Constructor
			*	@param [in] center Center position of rectangle
			*	@param [in] size Size of rectangle from center point
			*/
			Rect(const glm::vec2& center, const glm::vec2& size);

			// Destructor
			~Rect() = default;

			// Center of rectangle
			glm::vec2 center;
			// Size of rectangle
			glm::vec2 size;
		};

		/**
		*	@class Sphere
		*	@brief 3D Sphere with position and radius
		*/
		class Sphere
		{
		public:
			// Constructor
			Sphere();
			/**
			*	Constructor
			*	@param [in] radius Radius of sphere
			*	@param [in] position Position of sphere
			*/
			Sphere(const float radius, const glm::vec3& position);

			// Destructor
			~Sphere() = default;

			// Radius of sphere. Must be positive number.
			float radius;
			// Position of sphere.
			glm::vec3 position;
		};

		/**
		*	@class AABB
		*	@brief Simple 3D Axis Aligned Bounding box
		*/
		class AABB
		{
		public:
			// Default construcotr
			AABB();
			// Constructor with values
			AABB(const glm::vec3& center, const glm::vec3& size);
			// Constructor with values without glm 
			AABB(const glm::vec3& center, const float width, const float height, const float length);
			// Nothing to release, use default destructor
			~AABB() = default;

			// Center of AABB
			glm::vec3 center;
			// Size of AABB (Not radius)
			glm::vec3 size;

			// Order: Front > Left > Back > Right > Top > Bottom
			// 2 Triangles per side. Return vector must be size of 12.
			//std::vector<Plane> toPlanes() const;
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

			bool doesIntersectsWith(const Sphere& other);

			/**
			*	Checks if this AABB's size has zero value.
			*	@return true if one or more than one size in x, y and z is zero. If x, y and z is all zero, returns false.
			*/
			bool isAnyZero() const;

			/**
			*	Checks if size is all zero for all axis.
			*	@return true if size x, y and z are all 0
			*/
			bool isAllZero() const;

			/**
			*	Print AABB info.
			*/
			void print();
		};
	}
}

#endif