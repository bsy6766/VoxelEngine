#ifndef CUBE_H
#define CUBE_H

#include <vector>
#include <glm\glm.hpp>

namespace Voxel
{
	/**
	*	@class Cube
	*	@brief Static class that provides cube vertices
	*/
	class Cube
	{
	public:
		// Faces bit enum.
		// |= sets bit, &= ~ resets bit
		enum Face : unsigned int
		{
			NONE = 0,			// 0 means no faces at all
			FRONT = 1 << 0,		// 0000 0001
			LEFT = 1 << 1,		// 0000 0010
			BACK = 1 << 2,		// 0000 0100
			RIGHT = 1 << 3,		// 0000 1000
			TOP = 1 << 4,		// 0001 0000
			BOTTOM = 1 << 5,	// 0010 0000
			ALL = 1 << 6,		// 0100 0000
		};

	private:
		// This is static class. 
		Cube() = delete;
		~Cube() = delete;

		static int countFaceBit(Face face);
	public:
		// Cube size of 1 oriented zero to positive
		//const static std::vector<float> vertices;
		const static std::vector<std::vector<float>> allVertices;

		const static std::vector<float> FrontVertices;
		const static std::vector<float> BackVertices;
		const static std::vector<float> LeftVertices;
		const static std::vector<float> RightVertices;
		const static std::vector<float> TopVertices;
		const static std::vector<float> BottomVertices;

		const static std::vector<unsigned int> faceIndices;
		const static std::vector<unsigned int> indices;

		// Get cube vertices without any color
		static std::vector<float> getVertices();
		// Get cube vertices on specific face
		static std::vector<float> getVertices(Face face, const glm::vec3& translation);
		// Get color
		static std::vector<float> getColors(const Face face, glm::vec3 color);
		// Get cube indices
		static std::vector<unsigned int> getIndices(Face face, const int cubeOffset);
	};
}

#endif