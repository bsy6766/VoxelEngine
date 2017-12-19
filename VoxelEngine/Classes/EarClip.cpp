// pch
#include "PreCompiled.h"

#include "EarClip.h"

using namespace Voxel;

bool Voxel::EarClip::isPolygonCounterClockWise(std::vector<glm::vec2>& vertices)
{
	int total = 0;
	int index = 0;
	auto it = vertices.begin();
	auto next = vertices.begin();
	std::advance(next, 1);

	for (; next != vertices.end(); )
	{
		total += static_cast<int>((next->x - it->x) * (next->y + it->y));
		index++;
		it++;
		next++;

		if (next == vertices.end())
		{
			next = vertices.begin();
			total += static_cast<int>((next->x - it->x) * (next->y + it->y));
			break;
		}
	}

	if (total >= 0)
	{
		// clock wise
		return false;
	}
	else
	{
		// counter clock wise
		return true;
	}
}

std::vector<glm::vec2> Voxel::EarClip::earClipPolygon(std::vector<glm::vec2>& vertices)
{
	std::vector<glm::vec2> triangles;

	if (vertices.size() < 3)
	{
		return triangles;
	}

	if (!isPolygonCounterClockWise(vertices))
	{
		reverseVertices(vertices);
	}

	std::vector<Vertex*> vertexList;

	makeVertices(vertices, vertexList);
	
	Vertex* head = nullptr;

	unsigned int actualVerticesSize = vertexList.size();
	
	while (actualVerticesSize >= 3)
	{
		if (head == nullptr)
		{
			head = vertexList.front();
		}

		glm::vec2 p = head->point;
		glm::vec2 prevP = head->prev->point;
		glm::vec2 nextP = head->next->point;

		if (isEar(prevP, p, nextP, vertices))
		{
			// Ear found
			triangles.push_back(prevP);
			triangles.push_back(p);
			triangles.push_back(nextP);

			// Remove ear
			head->prev->next = head->next;
			head->next->prev = head->prev;

			actualVerticesSize--;
		}

		head = head->next;
	}

	for (auto v : vertexList)
	{
		if (v)
			delete v;
	}

	return triangles;
}

void Voxel::EarClip::reverseVertices(std::vector<glm::vec2>& vertices)
{
	auto vert_it = std::begin(vertices);
	std::advance(vert_it, 1);
	std::reverse(vert_it, std::end(vertices));
}

void Voxel::EarClip::makeVertices(std::vector<glm::vec2>& vertices, std::vector<Vertex*>& vertexList)
{
	auto v_it = vertices.begin();
	for (; v_it != vertices.end();)
	{
		Vertex* newVertex = new Vertex();
		newVertex->point = *v_it;
		newVertex->prev = nullptr;
		newVertex->next = nullptr;
		vertexList.push_back(newVertex);

		v_it++;
	}

	unsigned int verticesSize = vertexList.size();
	for (unsigned int i = 0; i < verticesSize; i++)
	{
		if (i >= 1 && i < verticesSize - 1)
		{
			auto p_it = vertexList.begin();
			std::advance(p_it, i);

			auto prevP_it = vertexList.begin();
			std::advance(prevP_it, i - 1);

			auto nextP_it = vertexList.begin();
			std::advance(nextP_it, i + 1);

			(*p_it)->prev = *prevP_it;
			(*p_it)->next = *nextP_it;
		}
	}


	vertexList.front()->prev = vertexList.back();
	auto it = vertexList.begin();
	std::advance(it, 1);
	vertexList.front()->next = *it;

	it = vertexList.begin();
	std::advance(it, verticesSize - 2);
	vertexList.back()->prev = *it;
	vertexList.back()->next = vertexList.front();
}

bool Voxel::EarClip::isEar(const glm::vec2 & prevP, const glm::vec2 & p, const glm::vec2 & nextP, std::vector<glm::vec2>& vertices)
{
	if (isConvex(prevP, p, nextP))
	{
		// Check if triangle doesn't have any points in.
		for (auto point : vertices)
		{
			if (point == prevP || point == p || point == nextP)
			{
				continue;
			}

			if (isPointInOrOnTriangle(nextP, p, prevP, point))
			{
				return false;
			}
		}

		return true;
	}
	else
	{
		return false;
	}
}

bool Voxel::EarClip::isConvex(const glm::vec2 & prevP, const glm::vec2 & p, const glm::vec2 & nextP)
{
	float result = ((prevP.x * (nextP.y - p.y)) + (p.x * (prevP.y - nextP.y)) + (nextP.x * (p.y - prevP.y)));
	return result < 0;
}

bool Voxel::EarClip::isPointInOrOnTriangle(const glm::vec2 & a, const glm::vec2 & b, const glm::vec2 & c, const glm::vec2 & p)
{
	return determinant(p - a, b - a) >= 0 &&
		determinant(p - b, c - b) >= 0 &&
		determinant(p - c, a - c) >= 0;
}

float Voxel::EarClip::determinant(const glm::vec2 & a, const glm::vec2 & b)
{
	return (a.x * b.y) - (a.y * b.x);
}
