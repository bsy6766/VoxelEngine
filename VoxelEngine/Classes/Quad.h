#ifndef QUAD_H
#define QUAD_H

// cpp
#include <array>
#include <vector>

// glm
#include <glm\glm.hpp>

namespace Voxel
{
	/**
	*	@class Quad
	*	@brief static class that defines and computes quad data.
	*/
	class Quad
	{
	private:
		// This is static class. No instances can be made.
		Quad() = delete;
		~Quad() = delete;
	public:
		const static std::array<unsigned int, 6> indices;
		const static std::array<float, 16> defaultColors;
		const static std::array<float, 8> uv;

		static std::vector<float> getVertices(const glm::vec2& size);
		static std::vector<float> getColors3(const glm::vec3& color);
		static std::vector<float> getColors4(const glm::vec4& color);
	};


}

#endif // !QUAD_H