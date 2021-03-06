// pch
#include "PreCompiled.h"

#include "Frustum.h"

// voxel
#include "Chunk.h"
#include "ChunkUtil.h"
#include "ChunkSection.h"
#include "Utility.h"
#include "ProgramManager.h"
#include "Program.h"

using namespace Voxel;

Frustum::Frustum()
	: projection(1.0f)
#if V_DEBUG && V_DEBUG_FRUSTUM_LINE
	, vao(0)
#endif
{
	for (int i = 0; i < 6; i++)
	{
		planes.back().normal = glm::vec3(0);
		planes.back().distanceToOrigin = 0;
	}
}

Frustum::~Frustum()
{
#if V_DEBUG && V_DEBUG_FRUSTUM_LINE
	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
	}
#endif
}

void Voxel::Frustum::updateProjection(const float fovy, const float aspect, const float nears, const float fars)
{
	this->projection = glm::perspective(glm::radians(fovy), aspect, nears, fars);
}

void Voxel::Frustum::updateFrustumPlanes(const glm::mat4 & playerVP)
{
	glm::mat4 matrix = projection * playerVP;

	// left
	planes.at(Voxel::Shape::Plane::Face::LEFT).normal.x = matrix[0][3] + matrix[0][0];
	planes.at(Voxel::Shape::Plane::Face::LEFT).normal.y = matrix[1][3] + matrix[1][0];
	planes.at(Voxel::Shape::Plane::Face::LEFT).normal.z = matrix[2][3] + matrix[2][0];
	planes.at(Voxel::Shape::Plane::Face::LEFT).distanceToOrigin = matrix[3][3] + matrix[3][0];

	// right
	planes.at(Voxel::Shape::Plane::Face::RIGHT).normal.x = matrix[0][3] - matrix[0][0];
	planes.at(Voxel::Shape::Plane::Face::RIGHT).normal.y = matrix[1][3] - matrix[1][0];
	planes.at(Voxel::Shape::Plane::Face::RIGHT).normal.z = matrix[2][3] - matrix[2][0];
	planes.at(Voxel::Shape::Plane::Face::RIGHT).distanceToOrigin = matrix[3][3] - matrix[3][0];

	// bottom
	planes.at(Voxel::Shape::Plane::Face::BOTTOM).normal.x = matrix[0][3] + matrix[0][1];
	planes.at(Voxel::Shape::Plane::Face::BOTTOM).normal.y = matrix[1][3] + matrix[1][1];
	planes.at(Voxel::Shape::Plane::Face::BOTTOM).normal.z = matrix[2][3] + matrix[2][1];
	planes.at(Voxel::Shape::Plane::Face::BOTTOM).distanceToOrigin = matrix[3][3] + matrix[3][1];

	// top
	planes.at(Voxel::Shape::Plane::Face::TOP).normal.x = matrix[0][3] - matrix[0][1];
	planes.at(Voxel::Shape::Plane::Face::TOP).normal.y = matrix[1][3] - matrix[1][1];
	planes.at(Voxel::Shape::Plane::Face::TOP).normal.z = matrix[2][3] - matrix[2][1];
	planes.at(Voxel::Shape::Plane::Face::TOP).distanceToOrigin = matrix[3][3] - matrix[3][1];

	// near
	planes.at(Voxel::Shape::Plane::Face::NEARS).normal.x = matrix[0][3] + matrix[0][2];
	planes.at(Voxel::Shape::Plane::Face::NEARS).normal.y = matrix[1][3] + matrix[1][2];
	planes.at(Voxel::Shape::Plane::Face::NEARS).normal.z = matrix[2][3] + matrix[2][2];
	planes.at(Voxel::Shape::Plane::Face::NEARS).distanceToOrigin = matrix[3][3] + matrix[3][2];

	// far
	planes.at(Voxel::Shape::Plane::Face::FARS).normal.x = matrix[0][3] - matrix[0][2];
	planes.at(Voxel::Shape::Plane::Face::FARS).normal.y = matrix[1][3] - matrix[1][2];
	planes.at(Voxel::Shape::Plane::Face::FARS).normal.z = matrix[2][3] - matrix[2][2];
	planes.at(Voxel::Shape::Plane::Face::FARS).distanceToOrigin = matrix[3][3] - matrix[3][2];

	for (auto& plane : planes)
	{
		float len = glm::length(plane.normal);
		plane.normal /= len;
		plane.distanceToOrigin /= len;
	}
}

bool Voxel::Frustum::isChunkBorderInFrustum(Chunk * chunk)
{
	//auto start = Utility::Time::now();
	//std::cout << "Frustum culling chunk at (" << chunk->getPosition().x << ", " << chunk->getPosition().z << ")\n";

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

	//std::vector<int> visibleChunkSection;

	std::vector<glm::vec3> chunkBorderPoints(8, glm::vec3(0));

	for (int i = 0; i < Constant::TOTAL_CHUNK_SECTION_PER_CHUNK; i++)
	{
		auto chunkSection = chunk->getChunkSectionAtY(i);

		if (chunkSection == nullptr)
		{
			// Skip if chunk section is null
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

		bool inPlane = true;

		// iterate through planes and check if 8 points of chunk section is in plane
		for (auto& plane : planes)
		{
			// counter
			int outCounter = 0;

			// iterate 8 points
			for (auto& point : chunkBorderPoints)
			{
				// check 
				if (plane.distanceToPoint(point) < 0)
				{
					// is out of plane
					outCounter++;
				}
			}

			if (outCounter == 8)
			{
				// All 8 points are out of plane. Stop plane iteration and check next chunk section.
				inPlane = false;
				break;
			}
			else
			{
				continue;
			}
		}

		if (inPlane)
		{
			// This section is visible. 
			//visibleChunkSection.push_back(i);

			// Todo: I'm just returning true when there is at least 1 chunk section visible from player. 
			// To findout which chunk is visible, don't return and use the vector that contains y level of chunk sections
			// that are visible.

			return true;
		}
		else
		{
			// This sections is not visible. Check next.
			continue;
		}
	}

	// None is visible
	return false;

	//auto end = Utility::Time::now();
	//std::cout << "fc = " << Utility::Time::toNanoSecondSTring(start, end) << std::endl;
	// avg 900 ns

	/*
	if (visibleChunkSection.empty())
	{
		return false;
	}
	else
	{
		return true;
	}
	*/
}

#if V_DEBUG && V_DEBUG_FRUSTUM_LINE
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

	auto program = ProgramManager::getInstance().getProgram(ProgramManager::PROGRAM_NAME::LINE_SHADER);

	GLint vertLoc = program->getAttribLocation("vert");
	GLint colorLoc = program->getAttribLocation("color");

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

void Voxel::Frustum::render(const glm::mat4 & modelMat, Program* prog)
{
	if (vao)
	{
		prog->setUniformMat4("modelMat", modelMat);
		prog->setUniformVec4("lineColor", glm::vec4(1.0f));

		glBindVertexArray(vao);
		glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
	}
}
#endif
