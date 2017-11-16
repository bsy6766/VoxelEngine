#ifndef RAY_H
#define RAY_H

#include <vector>
#include <Geometry.h>
#include <glm\glm.hpp>

namespace Voxel
{
	class Ray
	{
	private:
		glm::vec3 start;
		glm::vec3 end;

	public:
		Ray() = delete;
		Ray(const glm::vec3& start, const glm::vec3& end);
		~Ray() = default;

		glm::vec3 getStart() const;
		glm::vec3 getEnd() const;

		//    Return: -1 = triangle is degenerate (a segment or point)
		//             0 =  disjoint (no intersect)
		//             1 =  intersect in unique point I1
		//             2 =  are in the same plane
		int doesIntersectsTriangle(const Geometry::Triangle& triangle, glm::vec3& intersectingPoint = glm::vec3(0));

		void print();
	};
}

#endif