#ifndef FRUSTUM_H
#define FRUSTUM_H

#include <vector>
#include <glm\glm.hpp>

namespace Voxel
{
	class Chunk;

	struct FrustumPlane
	{
	public:
		enum Face : unsigned int
		{
			LEFT,
			RIGHT,
			BOTTOM,
			TOP,
			NEAR,
			FAR,
		};
	public:
		// plane's normal vector
		glm::vec3 normal;

		// d
		float distanceToOrigin;

		// Calculates shortest distance from point to plane
		float distanceToPoint(const glm::vec3& point);
	};

	/**
	*	@class Frustum
	*	@brief Contains frustum information with 6 planes of frustum
	*/
	class Frustum
	{
	private:
		// All 6 planes. 
		std::vector<FrustumPlane> planes;
	public:
		Frustum();
		~Frustum() = default;

		void update(const glm::mat4& MVP);
		bool isChunkBorderInFrustum(Chunk* chunk);
	};
}

#endif