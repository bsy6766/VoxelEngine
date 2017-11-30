#ifndef EAR_CLIP_H
#define EAR_CLIP_H

// glm
#include <glm\glm.hpp>

// cpp
#include <vector>

namespace Voxel
{
	struct Vertex
	{
	public:
		glm::vec2 point;
		Vertex* prev;
		Vertex* next;

		Vertex() : point(0), prev(nullptr), next(nullptr) {}
	};

	class EarClip
	{
	private:
		EarClip() = delete;
		~EarClip() = delete;

		static bool isPolygonCounterClockWise(std::vector<glm::vec2>& vertices);
		static void reverseVertices(std::vector<glm::vec2>& vertices);
		static void makeVertices(std::vector<glm::vec2>& vertices, std::vector<Vertex*>& vertexList);
		static bool isEar(const glm::vec2& prevP, const glm::vec2& p, const glm::vec2& nextP, std::vector<glm::vec2>& vertices);
		static bool isConvex(const glm::vec2& prevP, const glm::vec2& p, const glm::vec2& nextP);
		static bool isPointInOrOnTriangle(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& p);
		static float determinant(const glm::vec2& a, const glm::vec2& b);
	public:
		static std::vector<glm::vec2> earClipPolygon(std::vector<glm::vec2>& vertices);
	};
}

#endif