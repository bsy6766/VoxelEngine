#ifndef RAY_H
#define RAY_H

#include <vector>
#include <Geometry.h>
#include <glm\glm.hpp>

namespace Voxel
{
	/**
	*	@class Ray
	*	@brief A line with some intersection tests.
	*/
	class Ray
	{
	private:
		// Start position of ray
		glm::vec3 start;
		// End position of ray
		glm::vec3 end;

	public:
		// Do default constructor
		Ray() = delete;
		/**
		*	Constructor	
		*	@param [in] start Start position of ray.
		*	@param [in] end End position of ray.
		*/
		Ray(const glm::vec3& start, const glm::vec3& end);
		// Default destructor. Nothing to manually release.
		~Ray() = default;

		/**
		*	Get starting point of ray.
		*/
		glm::vec3 getStart() const;

		/**
		*	Get end point of ray.
		*/
		glm::vec3 getEnd() const;

		/**
		*	Check if line intersects triangle.
		*	@param [in] triangle Const ref of Geometry::Triangle to check.
		*	@param [in] intersectingPoint A glm::vec3 ref to get intersecting point. 0 by default.
		*	@return -1 if triangle is degenerate (a segment or point)
		*	@return 0 if line disjoints (no intersect)
		*	@return 1 if line intersects triangle. Check @param intersectingPoint for position of intersection.
		*	@return 2 if line and triangle are in the same plane.
		*/
		int doesIntersectsTriangle(const Geometry::Triangle& triangle, glm::vec3& intersectingPoint = glm::vec3(0));
		bool doesIntersectsTriangle(const Geometry::Triangle& triangle, float& t);
		bool doesIntersectsTriangle2(const Geometry::Triangle& triangle);
		bool doesIntersectsTriangle3(const Geometry::Triangle& triangle);
		float scalarTriple(const glm::vec3& u, const glm::vec3& v, const glm::vec3& w);

		/**
		*	Prints ray info.
		*/
		void print();
	};
}

#endif