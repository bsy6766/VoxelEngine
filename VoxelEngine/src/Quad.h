#ifndef QUAD_H
#define QUAD_H

#include <vector>
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
		const static std::vector<unsigned int> indices;
		const static std::vector<float> defaultColors;
		const static std::vector<float> uv;

		static std::vector<float> getVertices(const glm::vec2& size, const glm::vec2& position);
	};


}

#endif // !QUAD_H