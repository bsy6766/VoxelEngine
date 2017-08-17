#include "Frustum.h"
#include <Chunk.h>

using namespace Voxel;

float FrustumPlane::distanceToPoint(const glm::vec3& point)
{
	return glm::dot(point, normal) + distanceToOrigin;
}


Frustum::Frustum()
{
	for (int i = 0; i < 6; i++)
	{
		planes.push_back(FrustumPlane());
		planes.back().normal = glm::vec3(0);
		planes.back().distanceToOrigin = 0;
	}
}

void Frustum::update(const glm::mat4& MVP)
{    // left
	planes.at(FrustumPlane::Face::LEFT).normal.x = MVP[0][3] + MVP[0][0];
	planes.at(FrustumPlane::Face::LEFT).normal.y = MVP[1][3] + MVP[1][0];
	planes.at(FrustumPlane::Face::LEFT).normal.z = MVP[2][3] + MVP[2][0];
	planes.at(FrustumPlane::Face::LEFT).distanceToOrigin = MVP[3][3] + MVP[3][0];

	// right
	planes.at(FrustumPlane::Face::RIGHT).normal.x = MVP[0][3] - MVP[0][0];
	planes.at(FrustumPlane::Face::RIGHT).normal.y = MVP[1][3] - MVP[1][0];
	planes.at(FrustumPlane::Face::RIGHT).normal.z = MVP[2][3] - MVP[2][0];
	planes.at(FrustumPlane::Face::RIGHT).distanceToOrigin = MVP[3][3] - MVP[3][0];

	// bottom
	planes.at(FrustumPlane::Face::BOTTOM).normal.x = MVP[0][3] + MVP[0][1];
	planes.at(FrustumPlane::Face::BOTTOM).normal.y = MVP[1][3] + MVP[1][1];
	planes.at(FrustumPlane::Face::BOTTOM).normal.z = MVP[2][3] + MVP[2][1];
	planes.at(FrustumPlane::Face::BOTTOM).distanceToOrigin = MVP[3][3] + MVP[3][1];

	// top
	planes.at(FrustumPlane::Face::TOP).normal.x = MVP[0][3] - MVP[0][1];
	planes.at(FrustumPlane::Face::TOP).normal.y = MVP[1][3] - MVP[1][1];
	planes.at(FrustumPlane::Face::TOP).normal.z = MVP[2][3] - MVP[2][1];
	planes.at(FrustumPlane::Face::TOP).distanceToOrigin = MVP[3][3] - MVP[3][1];

	// near
	planes.at(FrustumPlane::Face::NEAR).normal.x = MVP[0][3] + MVP[0][2];
	planes.at(FrustumPlane::Face::NEAR).normal.y = MVP[1][3] + MVP[1][2];
	planes.at(FrustumPlane::Face::NEAR).normal.z = MVP[2][3] + MVP[2][2];
	planes.at(FrustumPlane::Face::NEAR).distanceToOrigin = MVP[3][3] + MVP[3][2];

	// far
	planes.at(FrustumPlane::Face::FAR).normal.x = MVP[0][3] - MVP[0][2];
	planes.at(FrustumPlane::Face::FAR).normal.y = MVP[1][3] - MVP[1][2];
	planes.at(FrustumPlane::Face::FAR).normal.z = MVP[2][3] - MVP[2][2];
	planes.at(FrustumPlane::Face::FAR).distanceToOrigin = MVP[3][3] - MVP[3][2];

	for (auto& plane : planes)
	{
		float len = glm::length(plane.normal);
		plane.normal /= len;
		plane.distanceToOrigin /= len;
	}
}

bool Voxel::Frustum::isChunkBorderInFrustum(Chunk * chunk)
{
	bool result = true;
	for (auto& plane : planes)
	{
		auto chunkWorldPos = chunk->getWorldPosition();
		chunkWorldPos.y = 8.0f;

		auto chunkBBMin = glm::vec3(chunkWorldPos.x - 8.0f, 0, chunkWorldPos.z - 8.0f);
		auto chunkBBMax = glm::vec3(chunkWorldPos.x + 8.0f, 256.0f, chunkWorldPos.z + 8.0f);

		auto a = glm::vec3(chunkBBMin.x, 0.0f, chunkBBMin.z);
		auto b = glm::vec3(chunkBBMin.x, 0.0f, chunkBBMax.z);
		auto c = glm::vec3(chunkBBMax.x, 0.0f, chunkBBMin.z);
		auto d = glm::vec3(chunkBBMax.x, 0.0f, chunkBBMax.z);
		auto e = glm::vec3(chunkBBMin.x, 16.0f, chunkBBMin.z);
		auto f = glm::vec3(chunkBBMin.x, 16.0f, chunkBBMax.z);
		auto g = glm::vec3(chunkBBMax.x, 16.0f, chunkBBMin.z);
		auto h = glm::vec3(chunkBBMax.x, 16.0f, chunkBBMax.z);

		std::vector<glm::vec3> chunkBorderPoints = { a, b, c, d, e, f, g, h };

		// if at least one point of chunk border is visible, 

		int out = 0;

		for (auto& point : chunkBorderPoints)
		{
			if (plane.distanceToPoint(point) < 0)
			{
				out++;
			}
		}

		if (out == 8)
		{
			return false;
		}

		//auto VN = chunkWorldPos;
		/*
		if (plane.normal.x < 0)
		{
			VN.x += 4.0f;
		}

		if (plane.normal.y < 0)
		{
			VN.y += 4.0f;
		}

		if (plane.normal.z < 0)
		{
			VN.z += 4.0f;
		}
		*/

		//auto VP = chunkWorldPos;

		/*
		if (plane.normal.x > 0)
		{
			VP.x += 4.0f;
		}

		if (plane.normal.y > 0)
		{
			VP.y += 4.0f;
		}

		if (plane.normal.z > 0)
		{
			VP.z += 4.0f;
		}
		*/

		/*
		if (plane.distanceToPoint(VN) < 0)
		{
			return false;
		}
		*/
		/*
		else if (plane.distanceToPoint(VP) < 0)
		{
			result = true;
		}
		*/
	}

	return true;
}
