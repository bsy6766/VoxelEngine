#include "ChunkMesh.h"

#include <ProgramManager.h>
#include <Program.h>
#include <InputHandler.h>
#include <glm/gtx/transform.hpp>
#include <Utility.h>

using namespace Voxel;

ChunkMesh::ChunkMesh()
	: vao(0)
	, vbo(0)
	, cbo(0)
	, nbo(0)
	, ibo(0)
	, indicesSize(0)
{
	renderable.store(false);
	loadable.store(false);
}

ChunkMesh::~ChunkMesh()
{
	// clear vectors
	vertices.clear();
	colors.clear();
	normals.clear();
	indices.clear();

	// Delte vao
	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
	}
}

void Voxel::ChunkMesh::initBuffer(const std::vector<float>& vertices, const std::vector<float>& colors, const std::vector<float>& normals, const std::vector<unsigned int>& indices)
{
	// clear vectors
	this->vertices.clear();
	this->colors.clear();
	this->indices.clear();

	// Copy vertices
	this->vertices = vertices;
	this->colors = colors;
	this->indices = indices;
	this->normals = normals;
	this->indicesSize = indices.size();

	loadable.store(true);

	if (vao != 0)
	{
		markAsUpdated();
	}
}

void Voxel::ChunkMesh::loadBuffer()
{
	if (vao != 0)
	{
		glDeleteVertexArrays(1, &vao);
	}

	//auto start = Utility::Time::now();
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices.front(), GL_STATIC_DRAW);

	// Get program
	auto program = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::SHADER_COLOR);

	// Enable vertices attrib
	GLint vertLoc = program->getAttribLocation("vert");
	GLint colorLoc = program->getAttribLocation("color");
	GLint normalLoc = program->getAttribLocation("normal");

	// vert
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// 3. CBO
	// Generate buffer boejct
	glGenBuffers(1, &cbo);
	// Bind it
	glBindBuffer(GL_ARRAY_BUFFER, cbo);
	// load color data
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * colors.size(), &colors.front(), GL_STATIC_DRAW);

	// color
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

	// 4. NBO
	// Generate buffer boejct
	glGenBuffers(1, &nbo);
	// Bind it
	glBindBuffer(GL_ARRAY_BUFFER, nbo);
	// load normal vector data
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normals.size(), &normals.front(), GL_STATIC_DRAW);

	// normal
	glEnableVertexAttribArray(normalLoc);
	glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// 5. IBO
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
	glDeleteBuffers(1, &nbo);
	glDeleteBuffers(1, &ibo);

	vbo = 0;
	cbo = 0;
	nbo = 0;
	ibo = 0;

	this->vertices.clear();
	this->colors.clear();
	this->normals.clear();
	this->indices.clear();

	renderable.store(true);
	loadable.store(false);

	//auto end = Utility::Time::now();
	//std::cout << "Loading buffer Elapsed time: " << Utility::Time::toMilliSecondString(start, end) << std::endl;
}

void Voxel::ChunkMesh::markAsUpdated()
{
	renderable.store(false);
}

void ChunkMesh::bind()
{
	glBindVertexArray(vao);
}

void ChunkMesh::render()
{
	glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);
	//glDrawElements(GL_TRIANGLES, 3 + offset, GL_UNSIGNED_INT, 0);
}

void ChunkMesh::unbind()
{
	glBindVertexArray(0);
}

void Voxel::ChunkMesh::releaseVAO()
{
	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
	}

	vao = 0;
	vbo = 0;
	cbo = 0;
	nbo = 0;
	ibo = 0;
}

void Voxel::ChunkMesh::clearBuffers()
{
	vertices.clear();
	colors.clear();
	normals.clear();
	indices.clear();
	indicesSize = 0;

}

bool Voxel::ChunkMesh::isRenderable()
{
	return renderable.load();
}

bool Voxel::ChunkMesh::isBufferLoadable()
{
	// Check if mesh has buffer.
	return loadable.load();
}
