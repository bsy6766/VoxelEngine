#ifndef FRUSTUM_H
#define FRUSTUM_H

// voxel
#include "Config.h"
#include "Shape.h"

// cpp
#include <array>

// glm
#include <glm\glm.hpp>

// gl
#include <GL\glew.h>

namespace Voxel
{
	class Chunk;
	class Program;

	/**
	*	@class Frustum
	*	@brief Contains frustum information with 6 planes of frustum
	*/
	class Frustum
	{
	private:
		// All 6 planes. 
		std::array<Voxel::Shape::Plane, 6> planes;

		// projection
		glm::mat4 projection;
	public:
		// Constructor
		Frustum();

		// Destructor
		~Frustum();

		// Update frustum projection
		void updateProjection(const float fovy, const float aspect, const float nears, const float fars);
		
		// Update
		void updateFrustumPlanes(const glm::mat4& MVP);

		// check if given chunk is is frustum. returns true if it's in. Else, false.
		bool isChunkBorderInFrustum(Chunk* chunk);

#if V_DEBUG && V_DEBUG_FRUSTUM_LINE
		GLuint vao;
		void initDebugLines(const float fovy, const float fovx, const float near, const float far);
		void render(const glm::mat4& modelMat, Program* prog);
#endif
	};
}

#endif