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
{
}

ChunkMesh::~ChunkMesh()
{
	// Todo: Instead of deleting vertices, freeze chunk and save mesh for future?
	// clear vector
	vertices.clear();
	colors.clear();
	indices.clear();

	// Delte buffers
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &cbo);
	glDeleteBuffers(1, &ibo);
	// Delte array
	glDeleteVertexArrays(1, &vao);
}

void Voxel::ChunkMesh::initBuffer(const std::vector<float>& vertices, const std::vector<float>& colors, const std::vector<unsigned int>& indices)
{
	// Copy vertices
	this->vertices = vertices;
	this->colors = colors;
	this->indices = indices;
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
	// Load cube vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->vertices.size(), &this->vertices.front(), GL_STATIC_DRAW);

	// Get program
	auto program = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::SHADER_COLOR);

	// Enable vertices attrib
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
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
	//glVertexAttribDivisor(colorLoc, 1);

	// 4. IBO
	// Generate indices object
	glGenBuffers(1, &ibo);
	// Bind indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	// Load indices
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * this->indices.size(), &this->indices[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * this->indices.size(), &this->indices.front(), GL_STATIC_DRAW);

	glBindVertexArray(0);
}

/*
void Voxel::ChunkMesh::initTest(const std::vector<float>& vertices, const std::vector<unsigned int>& indices)
{
	this->vertices = vertices;
	this->indices = indices;

	// Generate vertex array object
	glGenVertexArrays(1, &vao);
	// Bind it
	glBindVertexArray(vao);

	// Generate buffer object
	glGenBuffers(1, &vbo);
	// Bind it
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// Load cube vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->vertices) * this->vertices.size(), &this->vertices[0], GL_STATIC_DRAW);

	// Enable vertices attrib
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

	// Generate indices object
	glGenBuffers(1, &ibo);
	// Bind indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	// Load indices
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(this->indices) * this->indices.size(), &this->indices[0], GL_STATIC_DRAW);
	// unbind buffer
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}
*/

void ChunkMesh::bind()
{
	glBindVertexArray(vao);
}

void ChunkMesh::render()
{
	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	//glDrawElements(GL_TRIANGLES, 3 + offset, GL_UNSIGNED_INT, 0);
}

void ChunkMesh::unbind()
{
	glBindVertexArray(0);
}

int Voxel::ChunkMesh::getVerticesSize()
{
	return vertices.size();
}
