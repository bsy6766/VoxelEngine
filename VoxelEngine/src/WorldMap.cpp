#include "WorldMap.h"

#include <World.h>
#include <Region.h>
#include <UI.h>
#include <Color.h>
#include <ProgramManager.h>
#include <Program.h>
#include <glm\gtx\transform.hpp>
#include <Camera.h>
#include <EarClip.h>

using namespace Voxel;

Voxel::Program* RegionMesh::polygonProgram = nullptr;
Voxel::Program* RegionMesh::sideProgram = nullptr;

Voxel::RegionMesh::RegionMesh()
	: fillVao(0)
	, sideVao(0)
	, fillSize(0)
	, modelMat(glm::mat4(1.0f))
	, position(glm::vec3(0.0f)) 
{}

Voxel::RegionMesh::RegionMesh(const glm::mat4& modelMat, const glm::vec3& position)
	: fillVao(0)
	, sideVao(0)
	, fillSize(0)
	, modelMat(modelMat)
	, position(position) 
{}

Voxel::RegionMesh::~RegionMesh()
{
	if (fillVao)
	{
		glDeleteVertexArrays(1, &fillVao);
	}

	if (sideVao)
	{
		glDeleteVertexArrays(1, &sideVao);
	}
}

void Voxel::RegionMesh::buildMesh(const std::vector<float>& fillVertices, const std::vector<unsigned int>& fillIndices)
{
	GLint vertLoc = polygonProgram->getAttribLocation("vert");

	glGenVertexArrays(1, &fillVao);
	glBindVertexArray(fillVao);

	GLuint fillVbo;
	glGenBuffers(1, &fillVbo);
	glBindBuffer(GL_ARRAY_BUFFER, fillVbo);

	// Load cube vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * fillVertices.size(), &fillVertices.front(), GL_STATIC_DRAW);

	// vert
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLuint fillIbo;
	glGenBuffers(1, &fillIbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fillIbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * fillIndices.size(), &fillIndices.front(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	glDeleteBuffers(1, &fillVbo);
	glDeleteBuffers(1, &fillIbo);

	fillSize = fillIndices.size();
}

void Voxel::RegionMesh::render(const glm::mat4& worldModelMat)
{
	if (fillVao)
	{
		RegionMesh::polygonProgram->use(true);
		RegionMesh::polygonProgram->setUniformMat4("modelMat", worldModelMat);
		RegionMesh::polygonProgram->setUniformVec4("color", color);

		glBindVertexArray(fillVao);
		glDrawElements(GL_TRIANGLES, fillSize, GL_UNSIGNED_INT, 0);
	}
}

Voxel::WorldMap::WorldMap()
	: vao(0)
	, uiCanvas(nullptr)
	, compass(nullptr)
	, cameraIcon(nullptr)
	, worldName(nullptr)
	, modelMat(1.0f)
{}

Voxel::WorldMap::~WorldMap()
{
	releaseMesh();
	std::cout << "[WorldMap] Releasing world map\n";
}

void Voxel::WorldMap::init()
{
	// Initailize world map
	auto& pm = ProgramManager::getInstance();
	RegionMesh::polygonProgram = pm.getDefaultProgram(Voxel::ProgramManager::PROGRAM_NAME::SINGLE_COLOR_SHADER);
	RegionMesh::polygonProgram->use(true);
	RegionMesh::polygonProgram->setUniformMat4("projMat", Camera::mainCamera->getProjection());
}

void Voxel::WorldMap::buildMesh(World * world)
{
	// If there is a world map already, clear it.
	clear();

	// Initialize background

	// Initialize mesh for each region
	unsigned int gridSize = world->getGridSize();

	regionMeshes.resize(gridSize, nullptr);

	const float yBot = 0.0f;
	const float yTop = 10.0f;

	for (unsigned int i = 0; i < gridSize; i++)
	{
		Region* region = world->getRegion(i);
		if (region)
		{
			if (region->isCellValid())
			{
				std::vector<float> fillVertices;
				std::vector<unsigned int> fillIndices;

				unsigned int index = 0;

				std::vector<glm::vec2> edgePoints;
				region->getVoronoiEdgePoints(edgePoints);

				// ear clip edge points
				std::vector<glm::vec2> triangles = EarClip::earClipPolygon(edgePoints);

				if (triangles.empty())
				{
					continue;
				}

				auto randColor = Color::getRandomColor();

				// Top polygon
				for (unsigned int j = 0; j < triangles.size(); j += 3)
				{
					fillVertices.push_back(triangles.at(j).x);
					fillVertices.push_back(yTop);
					fillVertices.push_back(triangles.at(j).y);

					fillVertices.push_back(triangles.at(j + 1).x);
					fillVertices.push_back(yTop);
					fillVertices.push_back(triangles.at(j + 1).y);

					fillVertices.push_back(triangles.at(j + 2).x);
					fillVertices.push_back(yTop);
					fillVertices.push_back(triangles.at(j + 2).y);

					fillIndices.push_back(index);
					fillIndices.push_back(index + 1);
					fillIndices.push_back(index + 2);

					index += 3;
				}

				/*
				auto randSideColor = randColor * 1.1f;
				randSideColor = glm::clamp(randSideColor, 0.0f, 1.0f);

				// Sides
				unsigned int size = edgePoints.size() - 1;
				for (unsigned int j = 0; j < size; j++)
				{
					vertices.push_back(edgePoints.at(j).x);
					vertices.push_back(yBot);
					vertices.push_back(edgePoints.at(j).y);

					vertices.push_back(edgePoints.at(j).x);
					vertices.push_back(yTop);
					vertices.push_back(edgePoints.at(j).y);

					vertices.push_back(edgePoints.at(j + 1).x);
					vertices.push_back(yBot);
					vertices.push_back(edgePoints.at(j + 1).y);

					vertices.push_back(edgePoints.at(j + 1).x);
					vertices.push_back(yTop);
					vertices.push_back(edgePoints.at(j + 1).y);

					for (int k = 0; k < 4; k++)
					{
						colors.push_back(randSideColor.r);
						colors.push_back(randSideColor.g);
						colors.push_back(randSideColor.b);
						colors.push_back(1.0f);
					}

					indices.push_back(index);
					indices.push_back(index + 1);
					indices.push_back(index + 2);
					indices.push_back(index + 1);
					indices.push_back(index + 2);
					indices.push_back(index + 3);

					index += 2;
				}

				// loop it

				*/

				RegionMesh* newMesh = new RegionMesh();
				newMesh->buildMesh(fillVertices, fillIndices);
				newMesh->color = glm::vec4(randColor, 1.0f);

				regionMeshes.at(i) = newMesh;
			}
		}
		// Else, continue
	}
}

void Voxel::WorldMap::updatePosition(const glm::vec3 & playerPos)
{
	position = glm::vec3(playerPos.x, 0, playerPos.z);

	modelMat = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 1.0f, 0.1f)) * glm::translate(glm::mat4(1.0f), position);
}

void Voxel::WorldMap::updateViewMatrix(const glm::mat4 & viewMat)
{
	RegionMesh::polygonProgram->use(true);
	RegionMesh::polygonProgram->setUniformMat4("viewMat", viewMat);
}

void Voxel::WorldMap::clear()
{
	// Delete meshes
	releaseMesh();

	// Delete all UI.
}

void Voxel::WorldMap::render()
{
	for (auto regionMesh : regionMeshes)
	{
		if (regionMesh)
		{
			regionMesh->render(modelMat);
		}
	}
}

void Voxel::WorldMap::releaseMesh()
{
	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
	}

	for (auto regionMesh : regionMeshes)
	{
		if (regionMesh)
		{
			delete regionMesh;
		}
	}

	regionMeshes.clear();
}