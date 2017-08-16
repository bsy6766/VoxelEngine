#ifndef CUBE_H
#define CUBE_H

#include <vector>
#include <glm\glm.hpp>

namespace Voxel
{
	/**
	*	@class Line
	*	@brief Static class that provides line vertices
	*/
	/*
	class Line
	{
	private:
		// This is static class
		Line() = delete;
		~Line() = delete;
	public:
		static std::vector<float> getvertices(const glm::vec3& start, const glm::vec3& end);
		static std::vector<float> getColors(const glm::vec3& color);
		static std::vector<float> getColors(const glm::vec3& colorStart, const glm::vec3& colorEnd);
		static std::vector<float> getVertices(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color);
		static std::vector<float> getVertices(const glm::vec3& start, const glm::vec3& end, const glm::vec3& colorStart, const glm::vec3& colorEnd);
	};
	*/


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

		// Vertices
		const static std::vector<float> FrontVertices;
		const static std::vector<float> BackVertices;
		const static std::vector<float> LeftVertices;
		const static std::vector<float> RightVertices;
		const static std::vector<float> TopVertices;
		const static std::vector<float> BottomVertices;

		// Indices
		const static std::vector<unsigned int> faceIndices;
		const static std::vector<unsigned int> indices;

		// normals
		const static std::vector<float> FaceNormals;

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