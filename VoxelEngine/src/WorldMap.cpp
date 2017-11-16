#include "WorldMap.h"

#include <Application.h>
#include <Camera.h>
#include <World.h>
#include <Region.h>
#include <UI.h>
#include <Color.h>
#include <ProgramManager.h>
#include <Program.h>
#include <glm\gtx\transform.hpp>
#include <glm/gtx/compatibility.hpp>
#include <Utility.h>
#include <EarClip.h>

using namespace Voxel;

Voxel::Program* RegionMesh::polygonProgram = nullptr;
Voxel::Program* RegionMesh::sideProgram = nullptr;

Voxel::RegionMesh::RegionMesh()
	: fillVao(0)
	, sideVao(0)
	, fillSize(0)
	, sideSize(0)
	, color(0)
	, sideColor(0)
	, modelMat(glm::mat4(1.0f))
{}

Voxel::RegionMesh::RegionMesh(const glm::mat4& modelMat, const glm::vec3& position)
	: fillVao(0)
	, sideVao(0)
	, fillSize(0)
	, sideSize(0)
	, color(0)
	, sideColor(0)
	, modelMat(modelMat)
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

void Voxel::RegionMesh::buildMesh(const std::vector<float>& fillVertices, const std::vector<unsigned int>& fillIndices, const std::vector<float>& sideVertices, const std::vector<unsigned int>& sideIndices)
{
	GLint fillVertLoc = polygonProgram->getAttribLocation("vert");

	glGenVertexArrays(1, &fillVao);
	glBindVertexArray(fillVao);

	GLuint fillVbo;
	glGenBuffers(1, &fillVbo);
	glBindBuffer(GL_ARRAY_BUFFER, fillVbo);

	// Load cube vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * fillVertices.size(), &fillVertices.front(), GL_STATIC_DRAW);

	// vert
	glEnableVertexAttribArray(fillVertLoc);
	glVertexAttribPointer(fillVertLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLuint fillIbo;
	glGenBuffers(1, &fillIbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fillIbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * fillIndices.size(), &fillIndices.front(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	glDeleteBuffers(1, &fillVbo);
	glDeleteBuffers(1, &fillIbo);

	fillSize = fillIndices.size();



	GLint sideVertLoc = sideProgram->getAttribLocation("vert");

	glGenVertexArrays(1, &sideVao);
	glBindVertexArray(sideVao);

	GLuint sideVbo;
	glGenBuffers(1, &sideVbo);
	glBindBuffer(GL_ARRAY_BUFFER, sideVbo);

	// Load cube vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * sideVertices.size(), &sideVertices.front(), GL_STATIC_DRAW);

	// vert
	glEnableVertexAttribArray(sideVertLoc);
	glVertexAttribPointer(sideVertLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLuint sideIbo;
	glGenBuffers(1, &sideIbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sideIbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * sideIndices.size(), &sideIndices.front(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	glDeleteBuffers(1, &sideVbo);
	glDeleteBuffers(1, &sideIbo);

	sideSize = sideIndices.size();
}

void Voxel::RegionMesh::renderPolygon(const glm::mat4& worldModelMat)
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

void Voxel::RegionMesh::renderPolygonSide(const glm::mat4 & worldModelMat)
{
	if (sideVao)
	{
		RegionMesh::sideProgram->use(true);
		RegionMesh::sideProgram->setUniformMat4("modelMat", worldModelMat);
		RegionMesh::sideProgram->setUniformVec4("color", sideColor);

		glBindVertexArray(sideVao);
		glDrawElements(GL_TRIANGLES, sideSize, GL_UNSIGNED_INT, 0);
	}

}





Voxel::WorldMap::WorldMap()
	: vao(0)
	, uiCanvas(nullptr)
	, compass(nullptr)
	, cameraIcon(nullptr)
	, worldName(nullptr)
	, modelMat(1.0f)
	, state(State::IDLE)
	, zoomZ(0)
	, zoomZTarget(0)
	, position(0)
	, nextPosition(0)
	, rotation(0)
	, nextRotation(0)
	, posBoundary(0)
	, prevMouseClickedPos(0)
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
	RegionMesh::polygonProgram = pm.getDefaultProgram(Voxel::ProgramManager::PROGRAM_NAME::POLYGON_SHADER);
	RegionMesh::polygonProgram->use(true);
	RegionMesh::polygonProgram->setUniformMat4("projMat", Camera::mainCamera->getProjection());

	RegionMesh::sideProgram = pm.getDefaultProgram(Voxel::ProgramManager::PROGRAM_NAME::POLYGON_SIDE_SHADER);
	RegionMesh::sideProgram->use(true);
	RegionMesh::sideProgram->setUniformMat4("projMat", Camera::mainCamera->getProjection());

	resetPosAndRot();
	
	updateViewMatrix();
	updateModelMatrix();
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
	const float yTopSide = yTop - 0.1f;

	for (unsigned int i = 0; i < gridSize; i++)
	{
		Region* region = world->getRegion(i);
		if (region)
		{
			if (region->isCellValid())
			{
				std::vector<float> fillVertices;
				std::vector<unsigned int> fillIndices;

				std::vector<float> sideVertices;
				std::vector<float> sideNormals;
				std::vector<unsigned int> sideIndices;

				unsigned int index = 0;

				std::vector<glm::vec2> edgePoints;
				region->getVoronoiEdgePoints(edgePoints);

				for (auto& edge : edgePoints)
				{
					edge *= 0.05f;
				}

				// ear clip edge points
				std::vector<glm::vec2> triangles = EarClip::earClipPolygon(edgePoints);

				if (triangles.empty())
				{
					continue;
				}

				RegionMesh* newMesh = new RegionMesh();
				auto randColor = Color::getRandomColor();
				newMesh->color = glm::vec4(randColor, 1.0f);
				newMesh->sideColor = glm::vec4(randColor * 0.75f, 1.0f);
				
				// Top polygon
				auto size = triangles.size();
				for (unsigned int j = 0; j < size; j += 3)
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

					newMesh->triangles.push_back(std::move(Geometry::Triangle(glm::vec3(triangles.at(j).x, yTop, triangles.at(j).y), glm::vec3(triangles.at(j + 1).x, yTop, triangles.at(j + 1).y), glm::vec3(triangles.at(j + 2).x, yTop, triangles.at(j + 2).y))));

					fillIndices.push_back(index);
					fillIndices.push_back(index + 1);
					fillIndices.push_back(index + 2);

					index += 3;
				}

				index = 0;

				size = edgePoints.size() - 1;

				// side
				for (unsigned int j = 0; j < size; j++)
				{
					sideVertices.push_back(edgePoints.at(j).x);
					sideVertices.push_back(yBot);
					sideVertices.push_back(edgePoints.at(j).y);

					sideVertices.push_back(edgePoints.at(j).x);
					sideVertices.push_back(yTop);
					sideVertices.push_back(edgePoints.at(j).y);

					sideVertices.push_back(edgePoints.at(j + 1).x);
					sideVertices.push_back(yBot);
					sideVertices.push_back(edgePoints.at(j + 1).y);

					sideVertices.push_back(edgePoints.at(j + 1).x);
					sideVertices.push_back(yTop);
					sideVertices.push_back(edgePoints.at(j + 1).y);

					sideIndices.push_back(index);
					sideIndices.push_back(index + 1);
					sideIndices.push_back(index + 2);
					sideIndices.push_back(index + 1);
					sideIndices.push_back(index + 2);
					sideIndices.push_back(index + 3);

					index += 4;
				}

				// loop it
				sideVertices.push_back(edgePoints.back().x);
				sideVertices.push_back(yBot);
				sideVertices.push_back(edgePoints.back().y);

				sideVertices.push_back(edgePoints.back().x);
				sideVertices.push_back(yTop);
				sideVertices.push_back(edgePoints.back().y);

				sideIndices.push_back(index);
				sideIndices.push_back(index + 1);
				sideIndices.push_back(0);
				sideIndices.push_back(index + 1);
				sideIndices.push_back(0);
				sideIndices.push_back(1);

				newMesh->buildMesh(fillVertices, fillIndices, sideVertices, sideIndices);

				regionMeshes.at(i) = newMesh;
			}
		}
		// Else, continue
	}
}

void Voxel::WorldMap::update(const float delta)
{
	if (position != nextPosition)
	{
		position = glm::lerp(position, nextPosition, 10.0f * delta);

		if (glm::abs(glm::distance(position, nextPosition)) < 0.01f)
		{
			position = nextPosition;
		}
		
		checkPosBoundary();

		updateModelMatrix();
	}

	if (rotation != nextRotation)
	{
		rotation = glm::lerp(rotation, nextRotation, 10.0f * delta);

		if (glm::abs(glm::distance(rotation, nextRotation)) < 0.01f)
		{
			rotation = nextRotation;
		}

		updateViewMatrix();
	}

	if (zoomZ != zoomZTarget)
	{
		zoomZ = glm::lerp(zoomZ, zoomZTarget, 10.0f * delta);

		if (glm::abs(zoomZ - zoomZTarget) < 0.01f)
		{
			zoomZ = zoomZTarget;
		}

		checkPosBoundary();

		updateViewMatrix();
	}

}

void Voxel::WorldMap::updatePosition(const glm::vec3 & playerPos)
{
	position = glm::vec3(playerPos.x, 0, playerPos.z);

	modelMat = /*glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 1.0f, 0.05f)) * */glm::translate(glm::mat4(1.0f), position);
}

void Voxel::WorldMap::updateViewMatrix()
{
	glm::mat4 viewMatrix = getViewMatrix();

	RegionMesh::polygonProgram->use(true);
	RegionMesh::polygonProgram->setUniformMat4("viewMat", viewMatrix);

	RegionMesh::sideProgram->use(true);
	RegionMesh::sideProgram->setUniformMat4("viewMat", viewMatrix);
}

void Voxel::WorldMap::updateWithCamViewMatrix(const glm::mat4 & viewMat)
{
	RegionMesh::polygonProgram->use(true);
	RegionMesh::polygonProgram->setUniformMat4("viewMat", viewMat);

	RegionMesh::sideProgram->use(true);
	RegionMesh::sideProgram->setUniformMat4("viewMat", viewMat);
}

glm::mat4 Voxel::WorldMap::getViewMatrix()
{
	glm::mat4 viewMatrix = mat4(1.0f);
	viewMatrix = glm::translate(viewMatrix, -(glm::vec3(0.0f, 0.0f, zoomZ)));
	viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation.x), vec3(1, 0, 0));
	viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation.y), vec3(0, 1, 0));

	return  viewMatrix;
}

void Voxel::WorldMap::updateModelMatrix() 
{
	modelMat = glm::translate(glm::mat4(1.0f), -position);
	//modelMat = glm::rotate(modelMat, glm::radians(rotation.x), vec3(1, 0, 0));
	//modelMat = glm::rotate(modelMat, glm::radians(rotation.y), vec3(0, 1, 0));
}

void Voxel::WorldMap::updateMouseClick(const int button, const bool clicked, const glm::vec2& mousePos)
{
	if (button == 0)
	{
		if (clicked)
		{
			state = State::PAN;
			prevMouseClickedPos = mousePos;
		}
		else
		{
			if (prevMouseClickedPos == mousePos)
			{
				// Raycast
				raycastRegion();
			}

			state = State::IDLE;
		}
	}
	else if (button == 1)
	{
		if (clicked)
		{
			state = State::ROTATE;
		}
		else
		{
			state = State::IDLE;
		}
	}
}

void Voxel::WorldMap::updateMouseMove(const glm::vec2 & delta)
{
	if (state == State::PAN)
	{
		nextPosition.x -= (delta.x * 10.0f);
		nextPosition.z -= (delta.y * 10.0f);

		checkNextPosBoundary();
	}
	else if (state == State::ROTATE)
	{
		nextRotation.x += (delta.y * 10.0f);

		if (nextRotation.x > 80.0f)
		{
			nextRotation.x = 80.0f;
		}
		else if (nextRotation.x < 20.0f)
		{
			nextRotation.x = 20.0f;
		}

		nextRotation.y += (delta.x * 10.0f);
	}
}

void Voxel::WorldMap::resetPosAndRot()
{
	position = glm::vec3(0.0f);
	nextPosition = glm::vec3(0.0f);

	zoomZ = 500.0f;
	zoomZTarget = 500.0f;

	rotation = glm::vec2(35.0f, 0.0f);
	nextRotation = glm::vec2(35.0f, 0.0f);

	posBoundary = glm::vec3(250.0f, 0.0f, 200.0f);
}

void Voxel::WorldMap::zoomIn(const float delta)
{
	if (zoomZTarget > 200.0f)
	{
		zoomZTarget -= 50.0f;

		posBoundary.x -= 10.0f;
		posBoundary.z -= 5.0f;
	}
}

void Voxel::WorldMap::zoomOut(const float delta)
{
	if (zoomZTarget < 500.0f)
	{
		zoomZTarget += 50.0f;

		posBoundary.x += 10.0f;
		posBoundary.z += 5.0f;
	}
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
			regionMesh->renderPolygonSide(modelMat);
		}
	}

	glClear(GL_DEPTH_BUFFER_BIT);

	for (auto regionMesh : regionMeshes)
	{
		if (regionMesh)
		{
			regionMesh->renderPolygon(modelMat);
		}
	}
}

void Voxel::WorldMap::checkPosBoundary()
{
	if (position.x > posBoundary.x)
	{
		nextPosition.x = posBoundary.x;
	}
	else if (position.x < -posBoundary.x)
	{
		nextPosition.x = -posBoundary.x;
	}

	if (position.z > posBoundary.z)
	{
		nextPosition.z = posBoundary.z;
	}
	else if (position.z < -posBoundary.z)
	{
		nextPosition.z = -posBoundary.z;
	}
}

void Voxel::WorldMap::checkNextPosBoundary()
{
	if (nextPosition.x > posBoundary.x)
	{
		nextPosition.x = posBoundary.x;
	}
	else if (nextPosition.x < -posBoundary.x)
	{
		nextPosition.x = -posBoundary.x;
	}

	if (nextPosition.z > posBoundary.z)
	{
		nextPosition.z = posBoundary.z;
	}
	else if (nextPosition.z < -posBoundary.z)
	{
		nextPosition.z = -posBoundary.z;
	}
}

void Voxel::WorldMap::raycastRegion()
{
	std::cout << "raycast" << std::endl;

	std::cout << "mp = " << prevMouseClickedPos.x << ", " << prevMouseClickedPos.y << "\n";

	auto screenSize = glm::vec2(Application::getInstance().getGLView()->getScreenSize());
	
	// In range of [-1, 1]
	auto scaled = prevMouseClickedPos / screenSize;

	std::cout << "scaled = " << scaled.x << ", " << scaled.y << "\n";

	glm::mat4 proj = Camera::mainCamera->getProjection();
	glm::mat4 view = getViewMatrix();
	glm::mat4 inv = glm::inverse(proj * view * modelMat);
	glm::vec4 screenPos = glm::vec4(scaled.x, scaled.y, 1.0f, 1.0f);
	glm::vec4 worldPos = inv * screenPos;

	std::cout << "WorldPos = " << worldPos.x << ", " << worldPos.y << ", " << worldPos.z << ", " << worldPos.w << "\n";

	glm::vec3 dir = glm::normalize(glm::vec3(worldPos));

	std::cout << "dir = " << dir.x << ", " << dir.y << ", " << dir.z <<  "\n";
	
	glm::vec3 camPos = glm::vec3(glm::translate(mat4(1.0f), position) * (glm::rotate(glm::mat4(1.0f), glm::radians(-rotation.x), glm::vec3(1, 0, 0)) * (glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, zoomZ)) * glm::vec4(0, 0, 0, 1))));

	std::cout << "camPos = " << camPos.x << ", " << camPos.y << ", " << camPos.z << "\n";

	Ray ray(camPos, camPos + (dir * 20000.0f));
	ray.print();

	auto start = Utility::Time::now();

	int regionID = -1;

	for (auto& tri : regionMeshes.at(54)->triangles)
	{
		int result = ray.doesIntersectsTriangle(tri);

		if (result == 1)
		{
			regionID = 54;
			break;
		}
	}

	auto end = Utility::Time::now();

	std::cout << "t = " << Utility::Time::toMicroSecondString(start, end) << "\n";

	if (regionID != -1)
	{
		std::cout << "intersects with 54" << std::endl;
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

void Voxel::WorldMap::print()
{
	std::cout << "[WorldMap] info\n";
	std::cout << "Zoom: " << zoomZ << "\n";
	std::cout << "Model\n";
	std::cout << "p = (" << position.x << ", " << position.y << ", " << position.z << ")\n";
	std::cout << "r = (" << rotation.x << ", " << rotation.y << ")\n";
}
