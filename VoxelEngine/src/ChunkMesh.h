#ifndef CHUNK_MESH_H
#define CHUNK_MESH_H

#include <vector>
#include <GL\glew.h>
#include <glm\glm.hpp>

namespace Voxel
{
	/**
	*	@class ChunkMesh
	*	@brief Contains vertices data of chunk. Also manages OpenGL objects
	*/
	class ChunkMesh
	{
	private:
		std::vector<float> vertices;
		std::vector<float> colors;
		std::vector<glm::vec3> positions;
		std::vector<unsigned int> indices;

		// Opengl objects
		GLuint vao;	// vertex array object
		GLuint vbo;	// vertex buffer object (cube)
		GLuint cbo;	// color buffer object
		GLuint ibo;	// index buffer object 
	public:
		ChunkMesh();
		~ChunkMesh();

		void initBuffer(const std::vector<float>& vertices, const std::vector<float>& colors, const std::vector<unsigned int>& indices);
		void initOpenGLObjects();
		//void initTest(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);

		void bind();
		void render();
		void unbind();

		unsigned int offset = 0;
		bool down = false;

		int getVerticesSize();
	};
}

#endif