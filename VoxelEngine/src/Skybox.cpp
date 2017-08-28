#include "Skybox.h"
#include <Cube.h>
#include <vector>
#include <ProgramManager.h>
#include <Program.h>
#include <ChunkUtil.h>
#include <Utility.h>
#include <iostream>

using namespace Voxel;

Skybox::Skybox()
	: vao(0)
	, indicesSize(0)
	, fogState(FOG_STATE::IDLE)
	, skycolorState(SKYCOLOR_STATE::IDLE)
	, fogDistance(0)
	, curFogDistance(0)
	, fogAnimationSpeed(0.25f)
{
}

Skybox::~Skybox()
{
	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
	}
}

void Voxel::Skybox::init(const glm::vec4 & skyColor, const int renderDistance)
{
	// 3 times than render distance. making sure it renders everthing.
	auto range = renderDistance * 4;
	std::vector<float> vertices = Cube::getVertices(static_cast<float>(range * 16));
	
	std::vector<float> colors;
	auto len = vertices.size();
	for (unsigned int i = 0; i < len; i += 3)
	{
		colors.push_back(skyColor.r);
		colors.push_back(skyColor.g);
		colors.push_back(skyColor.b);
		colors.push_back(skyColor.a);
	}

	this->skyColor = this->curSkyColor = skyColor;
	setFogDistanceByRenderDistance(renderDistance, false);

	std::cout << "[Skybox] Fog distance = " << fogDistance << std::endl;
	std::cout << "[Skybox] Sky color = " << this->skyColor.r << ", " << this->skyColor.g << ", " << this->skyColor.b << std::endl;

	std::vector<unsigned int> indices = Cube::getIndices(Cube::Face::ALL, 0);

	indicesSize = indices.size();
	// 1. VAO
	// Generate vertex array object
	glGenVertexArrays(1, &vao);
	// Bind it
	glBindVertexArray(vao);

	// 2. VBO
	GLuint vbo;
	// Generate buffer object
	glGenBuffers(1, &vbo);
	// Bind it
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Load cube vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices.front(), GL_STATIC_DRAW);

	// Get program
	auto program = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::SHADER_COLOR);

	// Enable vertices attrib
	GLint vertLoc = program->getAttribLocation("vert");
	GLint colorLoc = program->getAttribLocation("color");

	// vert
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// 3. CBO
	GLuint cbo;
	// Generate buffer boejct
	glGenBuffers(1, &cbo);
	// Bind it
	glBindBuffer(GL_ARRAY_BUFFER, cbo);
	// load color data
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * colors.size(), &colors.front(), GL_STATIC_DRAW);

	// color
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

	// 4. IBO
	GLuint ibo;
	// Generate indices object
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

void Voxel::Skybox::update(const float delta)
{
	if (fogState == FOG_STATE::ANIMATING)
	{
		curFogDistance = Utility::Math::lerp(curFogDistance, fogDistance, delta * fogAnimationSpeed);
		if (abs(curFogDistance - fogDistance) < 2.0f)
		{
			fogState = FOG_STATE::IDLE;
			fogDistance = curFogDistance;
		}
	}

	if (skycolorState == SKYCOLOR_STATE::ANIMATING)
	{

	}
}

void Voxel::Skybox::render()
{
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);
}

glm::vec4 Voxel::Skybox::getColor()
{
	return skyColor;
}

float Voxel::Skybox::getFogDistance()
{
	if (fogState == FOG_STATE::IDLE)
	{
		return fogDistance;
	}
	else
	{
		return curFogDistance;
	}
}

void Voxel::Skybox::setFogDistanceByRenderDistance(const int renderDistance, const bool animate)
{
	setFogDistance((static_cast<float>(renderDistance - 1) - 0.5f) * Constant::CHUNK_BORDER_SIZE, animate);
}

void Voxel::Skybox::setFogDistance(const float distance, const bool animate)
{
	this->curFogDistance = this->fogDistance;
	this->fogDistance = distance;

	if (animate)
	{
		fogState = FOG_STATE::ANIMATING;
	}
	else
	{
		fogState = FOG_STATE::IDLE;
	}
}
