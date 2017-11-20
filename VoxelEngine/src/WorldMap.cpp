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
#include <glm/gtx/rotate_vector.hpp>
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
	, curSelectY(0)
	, selectYTarget(0)
	, modelMat(glm::mat4(1.0f))
	, sitePosition(0)
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

void Voxel::RegionMesh::update(const float delta)
{
	if (curSelectY != selectYTarget)
	{
		curSelectY = glm::lerp(curSelectY, selectYTarget, 10.0f * delta);

		if (glm::abs(curSelectY - selectYTarget) < 0.01f)
		{
			curSelectY = selectYTarget;
		}

		modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, curSelectY, 0.0f));
	}
}

void Voxel::RegionMesh::setSitePosition(const glm::vec2 & sitePosition)
{
	this->sitePosition.x = sitePosition.x;
	this->sitePosition.y = 0.0f;
	this->sitePosition.z = sitePosition.y;
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

void Voxel::RegionMesh::select()
{
	selectYTarget = 2.5f;
}

void Voxel::RegionMesh::unSelect()
{
	selectYTarget = 0.0f;
}

void Voxel::RegionMesh::renderPolygon(const glm::mat4& worldModelMat)
{
	if (fillVao)
	{
		RegionMesh::polygonProgram->use(true);
		RegionMesh::polygonProgram->setUniformMat4("modelMat", worldModelMat * modelMat);
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
		RegionMesh::sideProgram->setUniformMat4("modelMat", worldModelMat * modelMat);
		RegionMesh::sideProgram->setUniformVec4("color", sideColor);

		glBindVertexArray(sideVao);
		glDrawElements(GL_TRIANGLES, sideSize, GL_UNSIGNED_INT, 0);
	}

}





Voxel::WorldMap::WorldMap()
	: vao(0)
	, uiCanvas(nullptr)
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
	, prevMouseMoved(0)
	, hoveringRegionID(-1)
	, selectedRegionID(-1)
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

void Voxel::WorldMap::initUI()
{
}


void Voxel::WorldMap::buildMesh(World * world)
{
	// If there is a world map already, clear it.
	clear();

	// Initialize background

	// Initialize mesh for each region
	unsigned int gridSize = world->getGridSize();

	regionMeshes.resize(gridSize, nullptr);

	const float yBot = -10.0f;
	const float yTop = 0.0f;
	const float yTopSide = yTop;

	const float scale = 0.05f;

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
					edge *= scale;
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
				newMesh->sideColor = glm::vec4(randColor * 0.8f, 1.0f);
				newMesh->setSitePosition(region->getSitePosition() * scale);
				
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

					newMesh->triangles.push_back(std::move(Shape::Triangle(glm::vec3(triangles.at(j).x, yTop, triangles.at(j).y), glm::vec3(triangles.at(j + 1).x, yTop, triangles.at(j + 1).y), glm::vec3(triangles.at(j + 2).x, yTop, triangles.at(j + 2).y), glm::vec3(0.0f, 1.0f, 0.0f))));
					newMesh->triangles.back().checkOrientation();

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
					sideVertices.push_back(yTopSide);
					sideVertices.push_back(edgePoints.at(j).y);

					sideVertices.push_back(edgePoints.at(j + 1).x);
					sideVertices.push_back(yBot);
					sideVertices.push_back(edgePoints.at(j + 1).y);

					sideVertices.push_back(edgePoints.at(j + 1).x);
					sideVertices.push_back(yTopSide);
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
				sideVertices.push_back(yTopSide);
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

		//updateViewMatrix();
		updateModelMatrix();
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

	for (auto rm : regionMeshes)
	{
		if (rm)
		{
			rm->update(delta);
		}
	}
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
	viewMatrix = glm::rotate(viewMatrix, glm::radians(35.0f), vec3(1, 0, 0));
	//viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation.x), vec3(1, 0, 0));
	//viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation.y), vec3(0, 1, 0));

	return  viewMatrix;// *glm::translate(glm::mat4(1.0f), -position);
}

void Voxel::WorldMap::updateModelMatrix() 
{
	//glm::mat4 transX = glm::translate(glm::mat4(1.0f), glm::vec3(-position.x, 0, 0));
	//auto rotPos = glm::rotate(-position, glm::radians(rotation.x), glm::vec3(1, 0, 0));
	
	//modelMat = glm::translate(glm::mat4(1.0f), -position);
	//modelMat = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), vec3(1, 0, 0));
	//modelMat = glm::rotate(modelMat, glm::radians(rotation.x), vec3(1, 0, 0));
	//modelMat = glm::translate(modelMat, -position);
	//modelMat = glm::translate(glm::mat4(1.0f), rotPos);
	//modelMat = glm::rotate(modelMat, glm::radians(rotation.y), vec3(0, 1, 0));
	//modelMat = glm::translate(modelMat, -position);

	//glm::mat4 transMat = glm::translate(glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), vec3(1, 0, 0)), -position);
	//glm::mat4 rotMat = glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), vec3(1, 0, 0)), glm::radians(rotation.y), vec3(0, 1, 0));

	//modelMat = modelMat * rotMat;
	
	//glm::mat4 transMat = glm::translate(glm::mat4(1.0f), -position);
	//glm::mat4 rotMat = glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), vec3(1, 0, 0)), glm::radians(rotation.y), vec3(0, 1, 0));
	glm::mat4 rotMatX = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), vec3(1, 0, 0));
	glm::mat4 rotMatY = glm::rotate(rotMatX, glm::radians(rotation.y), vec3(0, 1, 0));
	glm::mat4 transMat = glm::translate(rotMatY, -position);
	//glm::mat4 transMat = glm::translate(glm::mat4(1.0f), rotPos);
	//glm::mat4 transMat = glm::translate(transX, glm::vec3(0, rotPos.y, rotPos.z));
	//glm::mat4 rotMat = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), vec3(0, 1, 0));

	//modelMat = transMat * rotMat;
	//modelMat = transMat * rotMatY;
	//modelMat = rotMatY * transMat;
	modelMat = transMat;
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
			if (glm::abs(glm::distance(prevMouseClickedPos, mousePos) <= 3.0f))
			{
				// Raycast
				raycastRegion(prevMouseClickedPos, true);
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

void Voxel::WorldMap::updateMouseMove(const glm::vec2 & delta, const glm::vec2& mousePos)
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

		/*
		if (nextRotation.x > 30.0f)
		{
			nextRotation.x = 30.0f;
		}
		else if (nextRotation.x < -20.0f)
		{
			nextRotation.x = -20.0f;
		}
		*/

		nextRotation.y += (delta.x * 10.0f);
	}

	if (prevMouseMoved != mousePos)
	{
		if (state == State::IDLE)
		{
			raycastRegion(mousePos, false);
		}
		prevMouseMoved = mousePos;
	}
}

void Voxel::WorldMap::resetPosAndRot()
{
	position = glm::vec3(0.0f);
	nextPosition = glm::vec3(0.0f);

	zoomZ = 500.0f;
	zoomZTarget = 500.0f;

	rotation = glm::vec2(0.0f);
	nextRotation = glm::vec2(0.0f);

	posBoundary = glm::vec3(300.0f, 0.0f, 250.0f);
}

void Voxel::WorldMap::zoomIn()
{
	if (zoomZTarget > 200.0f)
	{
		zoomZTarget -= 50.0f;

		posBoundary.x -= 10.0f;
		posBoundary.z -= 5.0f;
	}
}

void Voxel::WorldMap::zoomOut()
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

	//glClear(GL_DEPTH_BUFFER_BIT);

	for (auto regionMesh : regionMeshes)
	{
		if (regionMesh)
		{
			regionMesh->renderPolygon(modelMat);
		}
	}
}

void Voxel::WorldMap::renderRay()
{
	if (rayVao)
	{
		glBindVertexArray(rayVao);

		glDrawArrays(GL_LINES, 0, 2);
	}
}

void Voxel::WorldMap::renderCenterLine()
{
	if (centerVao)
	{
		glBindVertexArray(centerVao);

		glDrawArrays(GL_LINES, 0, 2);
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

RegionMesh * Voxel::WorldMap::getRegionMesh(const unsigned int regionMeshIndex)
{
	try
	{
		RegionMesh* rm = regionMeshes.at(regionMeshIndex);
		return rm;
	}
	catch (...)
	{
		std::cout << "Error! Failed to get region mesh at " << regionMeshIndex << std::endl;
		std::cout << "selected region mesh: " << selectedRegionID << std::endl;
		std::cout << "hovered region mesh: " << hoveringRegionID << std::endl;

		return nullptr;
	}
}

void Voxel::WorldMap::raycastRegion(const glm::vec2& cursorPos, const bool select)
{
	//std::cout << "raycast" << std::endl;

	//std::cout << "mp = " << prevMouseClickedPos.x << ", " << prevMouseClickedPos.y << "\n";

	auto screenSize = glm::vec2(Application::getInstance().getGLView()->getScreenSize());

	glm::mat4 proj = Camera::mainCamera->getProjection();
	glm::mat4 view = getViewMatrix();
	
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	//std::cout << "viewport = " << viewport[0] << ", " << viewport[1] << ", " << viewport[2] << ", " << viewport[3] << "\n";
	
	auto openglXY = cursorPos + (screenSize * 0.5f);
	//std::cout << "openglXY = " << openglXY.x << ", " << openglXY.y << "\n";

	auto near = glm::unProject(glm::vec3(openglXY.x, openglXY.y, 0.0f), view * modelMat, proj, glm::vec4(0, 0, 1920, 1080));

	auto far = glm::unProject(glm::vec3(openglXY.x, openglXY.y, 1.0f), view * modelMat, proj, glm::vec4(0, 0, 1920, 1080));

	//std::cout << "near = " << Utility::Log::vec3ToStr(near) << "\n";
	//std::cout << "far = " << Utility::Log::vec3ToStr(far) << "\n";
	
	Ray ray(near, far);
	//ray.print();

	//initDebugMousePickRayLine(ray);

	//auto start = Utility::Time::now();

	int regionID = -1;
	 
	bool found = false;
	for (unsigned int i = 0; i < regionMeshes.size(); i++)
	{
		if (regionMeshes.at(i))
		{
			for (auto& tri : regionMeshes.at(i)->triangles)
			{
				float t = 0;
				int result = ray.doesIntersectsTriangle(tri, glm::vec3(0, 1, 0));

				if (result == 1)
				{
					regionID = i;
					found = true;
					break;
				}
			}
		}

		if (found)
		{
			break;
		}
	}

	//auto end = Utility::Time::now();

	//std::cout << "t = " << Utility::Time::toMicroSecondString(start, end) << "\n";

	if (select)
	{
		// Need to select
		if (regionID == -1)
		{
			// Clicked out side of the map. Unselect
			if (selectedRegionID == -1)
			{
				// Nothing was selected. do nothing
				//std::cout << "do nothing" << std::endl;
				return;
			}
			else
			{
				//std::cout << "unselect " << selectedRegionID << std::endl;
				// Unselect region
				getRegionMesh(selectedRegionID)->unSelect();
				// reset 
				selectedRegionID = -1;
			}
		}
		else
		{
			// New region selected
			if (selectedRegionID == -1)
			{
				// There wasn't any region selected before.
				selectedRegionID = regionID;
				//std::cout << "select " << selectedRegionID << std::endl;
				// select
				getRegionMesh(selectedRegionID)->select();
			}
			else
			{
				// There was region already selected
				if (selectedRegionID == regionID)
				{
					//std::cout << "unselect " << selectedRegionID << std::endl;
					/*
					{
						// same, unselect
						getRegionMesh(selectedRegionID)->unSelect();
						// reset 
						selectedRegionID = -1;
					}
					*/
					// Instead of unselecting, move this region to center on screen
					nextPosition = getRegionMesh(selectedRegionID)->sitePosition;
					state = State::PAN;
				}
				else
				{
					//std::cout << "unselect " << selectedRegionID << std::endl;
					// Not same, unselect
					getRegionMesh(selectedRegionID)->unSelect();
					// assign new
					selectedRegionID = regionID;
					//std::cout << "select " << selectedRegionID << std::endl;
					// select
					getRegionMesh(selectedRegionID)->select();
				}
			}
		}
	}
	else
	{
		// hovering
		if (regionID == -1)
		{
			// Hovered outside of the map
			//std::cout << "no intersection" << std::endl;
			if (hoveringRegionID == -1)
			{
				// was hovering nothing. return.
				return;
			}
			else
			{
				// Was hovering region.
				if (hoveringRegionID == selectedRegionID)
				{
					// Was hovering selected region. do nothing
					return;
				}
				else
				{
					// Was hovering region but wasn't selected. unselect
					//std::cout << "unselect " << curHoveringRegionID << std::endl;
					getRegionMesh(hoveringRegionID)->unSelect();
					// reset
					hoveringRegionID = -1;
				}
			}
		}
		else
		{
			// New region hovered.
			// regionID != -1

			//std::cout << "intersects with " << regionID << std::endl;
			if (hoveringRegionID == -1)
			{
				// wasn't hovering region. assign
				hoveringRegionID = regionID;
				if (hoveringRegionID == selectedRegionID)
				{
					// trying to hover selected regionID. do nothing
					return;
				}
				else
				{
					// Not hovering over selected region. select
					getRegionMesh(hoveringRegionID)->select();
				}
			}
			else
			{
				// Was hovering other region
				// hoveringRegionId != -1

				if (regionID == hoveringRegionID)
				{
					// Hovering same region. Do nothing
					return;
				}
				else
				{
					// Hovering different region
					if (regionID == selectedRegionID)
					{
						// hovering selected region. unselect currently hovering region
						getRegionMesh(hoveringRegionID)->unSelect();
						// assign
						hoveringRegionID = regionID;
					}
					else
					{
						if (hoveringRegionID == selectedRegionID)
						{
							// Hovering can't unselect selected region
						}
						else
						{
							// Not hovering selected region. unselect
							getRegionMesh(hoveringRegionID)->unSelect();
						}
						// assign
						hoveringRegionID = regionID;
						// select
						getRegionMesh(hoveringRegionID)->select();
					}
				}
			}
		}
	}
}

void Voxel::WorldMap::releaseMesh()
{
	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
	}

	if (rayVao)
	{
		glDeleteVertexArrays(1, &rayVao);
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

void Voxel::WorldMap::initDebugCenterLine()
{
	GLfloat verts[] = {
		0, 1000, 0, 1, 0, 0, 1,
		0, -1000, 0, 1, 0, 0, 1,
	};

	// Generate vertex array object
	glGenVertexArrays(1, &centerVao);
	// Bind it
	glBindVertexArray(centerVao);

	// Generate buffer object
	GLuint rayVbo;
	glGenBuffers(1, &rayVbo);
	// Bind it
	glBindBuffer(GL_ARRAY_BUFFER, rayVbo);

	// Load cube vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	// Enable vertices attrib
	auto program = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::LINE_SHADER);
	GLint vertLoc = program->getAttribLocation("vert");
	GLint colorLoc = program->getAttribLocation("color");

	// vert
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), nullptr);

	// color
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));

	glBindVertexArray(0);

	glDeleteBuffers(1, &rayVbo);
}

void Voxel::WorldMap::initDebugMousePickRayLine(const Ray & ray)
{
	if (rayVao)
	{
		glDeleteVertexArrays(1, &rayVao);
	}

	auto start = ray.getStart();
	auto end = ray.getEnd();

	GLfloat verts[] = {
		start.x, start.y, start.z, 1, 0, 0, 1,
		end.x, end.y, end.z, 1, 0, 0, 1,
	};	

	// Generate vertex array object
	glGenVertexArrays(1, &rayVao);
	// Bind it
	glBindVertexArray(rayVao);

	// Generate buffer object
	GLuint rayVbo;
	glGenBuffers(1, &rayVbo);
	// Bind it
	glBindBuffer(GL_ARRAY_BUFFER, rayVbo);

	// Load cube vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	// Enable vertices attrib
	auto program = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::LINE_SHADER);
	GLint vertLoc = program->getAttribLocation("vert");
	GLint colorLoc = program->getAttribLocation("color");

	// vert
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), nullptr);

	// color
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));

	glBindVertexArray(0);

	glDeleteBuffers(1, &rayVbo);
}

void Voxel::WorldMap::print()
{
	std::cout << "[WorldMap] info\n";
	std::cout << "Zoom: " << zoomZ << "\n";
	std::cout << "Model\n";
	std::cout << "p = (" << position.x << ", " << position.y << ", " << position.z << ")\n";
	std::cout << "r = (" << rotation.x << ", " << rotation.y << ")\n";
}
