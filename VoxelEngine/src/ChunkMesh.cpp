#include "ChunkMesh.h"

#include <ProgramManager.h>
#include <Program.h>
#include <InputHandler.h>
#include <glm/gtx/transform.hpp>

using namespace Voxel;

ChunkMesh::ChunkMesh()
	: vao(0)
	, vbo(0)
	, cbo(0)
	, ibo(0)
	, tbo(0)
{
}

ChunkMesh::~ChunkMesh()
{
	// Todo: Instead of deleting verticies, freeze chunk and save mesh for future?
	// clear vector
	verticies.clear();
	colors.clear();
	indicies.clear();

	// Delte buffers
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &cbo);
	glDeleteBuffers(1, &ibo);
	// Delte array
	glDeleteVertexArrays(1, &vao);
}

void Voxel::ChunkMesh::initBuffer(const std::vector<float>& verticies, const std::vector<float>& colors, const std::vector<unsigned int>& indicies)
{
	// Copy verticies
	this->verticies = verticies;
	this->colors = colors;
	this->indicies = indicies;
}

void Voxel::ChunkMesh::initMatrix(const std::vector<glm::vec3>& blockPositions)
{
	positions = blockPositions;
	for (auto pos : positions)
	{
		pos *= 10.0f;
	}
	for (auto pos : blockPositions)
	{
		this->transforms.push_back(glm::translate(mat4(1.0f), pos));
	}
}

void Voxel::ChunkMesh::initOpenGLObjects()
{
	// 1. VAO
	// Generate vertex array object
	glGenVertexArrays(1, &vao);
	// Bind it
	glBindVertexArray(vao);

	// 2. VBO
	// Generate buffer object
	glGenBuffers(1, &vbo);
	// Bind it
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Load cube verticies
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->verticies.size(), &this->verticies.front(), GL_STATIC_DRAW);

	// Get program
	auto program = ProgramManager::getInstance().getDefaultProgram();

	// Enable verticies attrib
	GLint vertLoc = program->getAttribLocation("vert");
	GLint colorLoc = program->getAttribLocation("color");
	//GLint transformLoc = program->getAttribLocation("modelTransform");

	// vert
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// 3. CBO
	// Generate buffer boejct
	glGenBuffers(1, &cbo);
	// Bind it
	glBindBuffer(GL_ARRAY_BUFFER, cbo);
	// load color data
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->colors.size(), &this->colors.front(), GL_STATIC_DRAW);

	// color
	glEnableVertexAttribArray(colorLoc);
	//glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	//glVertexAttribDivisor(colorLoc, 1);

	// 4. TBO
	// Generate buffer object
	/*
	glGenBuffers(1, &tbo);
	glBindBuffer(GL_ARRAY_BUFFER, tbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * this->transforms.size(), &this->transforms.front(), GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * this->positions.size(), &this->positions.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(transformLoc);
	glVertexAttribPointer(transformLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glVertexAttribDivisor(transformLoc, 1);
	*/

	// 4. IBO
	// Generate indicies object
	glGenBuffers(1, &ibo);
	// Bind indicies
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	// Load indicies
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * this->indicies.size(), &this->indicies[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * this->indicies.size(), &this->indicies.front(), GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void Voxel::ChunkMesh::initTest(const std::vector<float>& verticies, const std::vector<unsigned int>& indicies)
{
	this->verticies = verticies;
	this->indicies = indicies;

	// Generate vertex array object
	glGenVertexArrays(1, &vao);
	// Bind it
	glBindVertexArray(vao);

	// Generate buffer object
	glGenBuffers(1, &vbo);
	// Bind it
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// Load cube verticies
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->verticies) * this->verticies.size(), &this->verticies[0], GL_STATIC_DRAW);

	// Enable verticies attrib
	auto program = ProgramManager::getInstance().getDefaultProgram();
	GLint vertLoc = program->getAttribLocation("vert");
	GLint colorLoc = program->getAttribLocation("color");
	// vert
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
	// color
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));
	// unbind buffer
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Generate indicies object
	glGenBuffers(1, &ibo);
	// Bind indicies
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	// Load indicies
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(this->indicies) * this->indicies.size(), &this->indicies[0], GL_STATIC_DRAW);
	// unbind buffer
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void ChunkMesh::bind()
{
	glBindVertexArray(vao);
}

void ChunkMesh::render()
{
	if (InputHandler::getInstance().getMouseDown(GLFW_MOUSE_BUTTON_RIGHT))
	{
		down = true;
	}
	
	if (down && InputHandler::getInstance().getMouseUp(GLFW_MOUSE_BUTTON_RIGHT))
	{
		down = false;
		offset += 3;
	}

	glDrawElements(GL_TRIANGLES, this->indicies.size(), GL_UNSIGNED_INT, 0);
	//glDrawElements(GL_TRIANGLES, 3 + offset, GL_UNSIGNED_INT, 0);
}

void ChunkMesh::unbind()
{
	glBindVertexArray(0);
}