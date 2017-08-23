#include "Frustum.h"
#include <Chunk.h>
#include <ChunkUtil.h>
#include <ChunkSection.h>
#include <iostream>
#include <Utility.h>
#include <ProgramManager.h>
#include <Program.h>

using namespace Voxel;

float FrustumPlane::distanceToPoint(const glm::vec3& point)
{
	return glm::dot(point, normal) + distanceToOrigin;
}


Frustum::Frustum()
	: vao(0)
{
	for (int i = 0; i < 6; i++)
	{
		planes.push_back(FrustumPlane());
		planes.back().normal = glm::vec3(0);
		planes.back().distanceToOrigin = 0;
	}
}

Frustum::~Frustum()
{
	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
	}
}

void Voxel::Frustum::initDebugLines(const float fovy, const float fovx, const float near, const float far)
{
	std::vector<float> vertices;

	float nearY = tan(glm::radians(fovy*0.5f)) * near;
	float nearX = tan(glm::radians(fovx*0.5f)) * near;

	glm::vec2 minNear = glm::vec2(-nearX, -nearY);
	glm::vec2 maxNear = glm::vec2(nearX, nearY);

	float farY = tan(glm::radians(fovx*0.5f)) * far;
	float farX = tan(glm::radians(fovx*0.5f)) * far;

	glm::vec2 minFar = glm::vec2(-farX, -farY);
	glm::vec2 maxFar = glm::vec2(farX, farY);

	// Near left bot > left top > right top > right bottom (clock wise)
	vertices.push_back(minNear.x);
	vertices.push_back(minNear.y);
	vertices.push_back(-near);

	vertices.push_back(minNear.x);
	vertices.push_back(maxNear.y);
	vertices.push_back(-near);

	vertices.push_back(maxNear.x);
	vertices.push_back(maxNear.y);
	vertices.push_back(-near);

	vertices.push_back(maxNear.x);
	vertices.push_back(minNear.y);
	vertices.push_back(-near);

	// same for far
	vertices.push_back(minFar.x);
	vertices.push_back(minFar.y);
	vertices.push_back(-far);

	vertices.push_back(minFar.x);
	vertices.push_back(maxFar.y);
	vertices.push_back(-far);

	vertices.push_back(maxFar.x);
	vertices.push_back(maxFar.y);
	vertices.push_back(-far);

	vertices.push_back(maxFar.x);
	vertices.push_back(minFar.y);
	vertices.push_back(-far);

	std::vector<float> color;

	for (int i = 0; i < 8; i++)
	{
		color.push_back(0);
		color.push_back(0);
		color.push_back(1);
		color.push_back(1);
	}

	std::vector<unsigned int> indices = { 0,1, 1,2, 2,3, 3,0, 4,5, 5,6, 6,7, 7,4, 0,4, 1,5, 2,6, 3,7 };

	// Generate vertex array object
	glGenVertexArrays(1, &vao);
	// Bind it
	glBindVertexArray(vao);

	GLuint vbo;
	// Generate buffer object
	glGenBuffers(1, &vbo);
	// Bind it
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices.front(), GL_STATIC_DRAW);

	auto colorShader = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::SHADER_COLOR);

	GLint vertLoc = colorShader->getAttribLocation("vert");
	GLint colorLoc = colorShader->getAttribLocation("color");

	// vert
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLuint cbo;
	// Generate buffer object
	glGenBuffers(1, &cbo);
	// Bind it
	glBindBuffer(GL_ARRAY_BUFFER, cbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * color.size(), &color.front(), GL_STATIC_DRAW);

	// vert
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLuint ibo;
	glGenBuffers(1, &ibo);
	// Bind indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	// Load indices
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices.front(), GL_STATIC_DRAW);

	glBindVertexArray(0);
	// Delte buffers
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &cbo);
	glDeleteBuffers(1, &ibo);
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
	//std::cout << "Frustum culling chunk at (" << chunk->getPosition().x << ", " << chunk->getPosition().z << ")" << std::endl;

	/*
	// This was used for testing frustum culling. This only checks the most bottom chunk .
	bool result = true;

	{
		auto chunkWorldPos = chunk->getWorldPosition();

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

		for (auto& plane : planes)
		{
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
				result = false;
				break;
			}
		}
	}

	return result;
	*/

	const float chunkHalf = Constant::CHUNK_BORDER_SIZE * 0.5f;

	std::vector<int> visibleChunkSection;

	std::vector<glm::vec3> chunkBorderPoints(8, glm::vec3(0));

	for (int i = 0; i < Constant::TOTAL_CHUNK_SECTION_PER_CHUNK; i++)
	{
		auto chunkSection = chunk->getChunkSectionByY(i);
		if (chunkSection == nullptr)
		{
			continue;
		}
		//std::cout << "checking chunk section at y: " << i << std::endl;

		auto chunkSectionWorldPos = chunkSection->getWorldPosition();

		float y = static_cast<float>(i);

		auto chunkSectionMin = glm::vec3(chunkSectionWorldPos.x - chunkHalf, Constant::CHUNK_BORDER_SIZE * y, chunkSectionWorldPos.z - chunkHalf);
		auto chunkSectionMax = glm::vec3(chunkSectionWorldPos.x + chunkHalf, Constant::CHUNK_BORDER_SIZE * y + Constant::CHUNK_BORDER_SIZE, chunkSectionWorldPos.z + chunkHalf);

		chunkBorderPoints.at(0).x = chunkSectionMin.x;
		chunkBorderPoints.at(0).y = chunkSectionMin.y;
		chunkBorderPoints.at(0).z = chunkSectionMin.z;
		chunkBorderPoints.at(1).x = chunkSectionMin.x;
		chunkBorderPoints.at(1).y = chunkSectionMin.y;
		chunkBorderPoints.at(1).z = chunkSectionMax.z;
		chunkBorderPoints.at(2).x = chunkSectionMax.x;
		chunkBorderPoints.at(2).y = chunkSectionMin.y;
		chunkBorderPoints.at(2).z = chunkSectionMin.z;
		chunkBorderPoints.at(3).x = chunkSectionMax.x;
		chunkBorderPoints.at(3).y = chunkSectionMin.y;
		chunkBorderPoints.at(3).z = chunkSectionMax.z;
		chunkBorderPoints.at(4).x = chunkSectionMin.x;
		chunkBorderPoints.at(4).y = chunkSectionMax.y;
		chunkBorderPoints.at(4).z = chunkSectionMin.z;
		chunkBorderPoints.at(5).x = chunkSectionMin.x;
		chunkBorderPoints.at(5).y = chunkSectionMax.y;
		chunkBorderPoints.at(5).z = chunkSectionMax.z;
		chunkBorderPoints.at(6).x = chunkSectionMax.x;
		chunkBorderPoints.at(6).y = chunkSectionMax.y;
		chunkBorderPoints.at(6).z = chunkSectionMin.z;
		chunkBorderPoints.at(7).x = chunkSectionMax.x;
		chunkBorderPoints.at(7).y = chunkSectionMax.y;
		chunkBorderPoints.at(7).z = chunkSectionMax.z;

		bool sectionResult = true;
		for (auto& plane : planes)
		{
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
				sectionResult = false;
				break;
			}
		}

		if (sectionResult)
		{
			visibleChunkSection.push_back(i);

			// Todo: I'm just returning true when there is at least 1 chunk section visible from player. 
			// To findout which chunk is visible, don't return and use the vector that contains y level of chunk sections
			// that are visible.

			return true;
		}
	}

	if (visibleChunkSection.empty())
	{
		return false;
	}
	else
	{
		return true;
	}
}

void Voxel::Frustum::render(const glm::mat4 & modelMat, Program* prog)
{
	prog->setUniformMat4("modelMat", modelMat);

	glBindVertexArray(vao);
	glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
}
