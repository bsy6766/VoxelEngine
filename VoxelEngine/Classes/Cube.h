#ifndef CUBE_H
#define CUBE_H

// cpp
#include <vector>
#include <array>

// glm
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
		const static std::array<float, 24> allVertices;
		const static std::vector<glm::vec3> allVerticesGlmVec3;

		// Vertices
		const static std::vector<float> FrontVertices;
		const static std::vector<float> BackVertices;
		const static std::vector<float> LeftVertices;
		const static std::vector<float> RightVertices;
		const static std::vector<float> TopVertices;
		const static std::vector<float> BottomVertices;
		
		// Normals
		const static std::vector<float> FrontNormals;
		const static std::vector<float> BackNormals;
		const static std::vector<float> LeftNormals;
		const static std::vector<float> RightNormals;
		const static std::vector<float> TopNormals;
		const static std::vector<float> BottomNormals;

		// Indices
		const static std::vector<unsigned int> faceIndices;
		const static std::vector<unsigned int> indices;

		// Shade ratio
		const static float TopShadeRatio;
		const static float FrontAndBackShadeRatio;
		const static float LeftAndRightShadeRatio;
		const static float BottomShadeRatio;
		const static float ShadePower;

		// Get cube vertices without any color
		static std::vector<float> getVertices();
		static std::vector<float> getVertices(const float size);
		// Get cube vertices on specific face and position
		static std::vector<float> getVertices(Face face, const glm::vec3& translation);
		static unsigned int getVertices(Face face, const glm::vec3& translation, std::vector<float>& vertices);
		// Get cube normals on specific face and position
		static std::vector<float> getNormals(Face face, const glm::vec3& translation);
		static void getNormals(Face face, const glm::vec3& translation, std::vector<float>& normals);
		// Get color
		static std::vector<float> getColors3(const Face face, const glm::vec3& color);
		static std::vector<float> getColors4WithoutShade(const Face face, const glm::vec4& color);
		static void getColors4WithoutShade(const Face face, const glm::vec4& color, std::vector<float>& colors);
		static std::vector<float> getColors4WithDefaultShade(const Face& face, const glm::vec4& color);
		static void getColors4WithDefaultShade(const Face& face, const glm::vec4& color, std::vector<float>& colors);
		static std::vector<float> getColors4WithShade(const Face face, const glm::vec4& color, const std::vector<unsigned int>& shadowWeight);
		static void getColors4WithShade(const Face face, const glm::vec4& color, const std::vector<unsigned int>& shadeWeight, std::vector<float>& colors);
		// Get cube indices
		static std::vector<unsigned int> getIndices(Face face, const int cubeOffset);
		static void getIndices(Face face, const int cubeOffset, std::vector<unsigned int>& indices);
		// Face to string
		static std::string faceToString(const Face& face);
	};
}

#endif