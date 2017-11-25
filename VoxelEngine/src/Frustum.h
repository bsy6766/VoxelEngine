#ifndef FRUSTUM_H
#define FRUSTUM_H

#include <Config.h>
#include <vector>
#include <glm\glm.hpp>
#include <GL\glew.h>

namespace Voxel
{
	class Chunk;
	class Program;

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

		// projection
		glm::mat4 projection;
	public:
		// Constructor
		Frustum();
		// Destructor
		~Frustum();

		// Update frustum projection
		void updateProjection(const float fovy, const float aspect, const float near, const float far);
		
		// Update
		void updateFrustumPlanes(const glm::mat4& MVP);
		bool isChunkBorderInFrustum(Chunk* chunk);

#if V_DEBUG && V_DEBUG_FRUSTUM_LINE
		GLuint vao;
		void initDebugLines(const float fovy, const float fovx, const float near, const float far);
		void render(const glm::mat4& modelMat, Program* prog);
#endif
	};
}

#endif