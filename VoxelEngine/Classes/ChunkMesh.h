#ifndef CHUNK_MESH_H
#define CHUNK_MESH_H

// cpp
#include <vector>
#include <atomic>

// gl
#include <GL\glew.h>

// glm
#include <glm\glm.hpp>

namespace Voxel
{
	class Program;

	/**
	*	@class ChunkMesh
	*	@brief Contains vertices data of chunk. Also manages OpenGL objects
	*/
	class ChunkMesh
	{
	private:
		// temporary vertices vectors. This gets cleared once data is loaded
		std::vector<float> vertices;
		std::vector<float> colors;
		std::vector<float> normals;
		std::vector<unsigned int> indices;

		int indicesSize;

		std::atomic<bool> renderable;
		std::atomic<bool> loadable;
		
		// Opengl objects
		GLuint vao;	// vertex array object
		GLuint vbo;	// vertex buffer object (cube)
		GLuint cbo;	// color buffer object
		GLuint nbo;
		GLuint ibo;	// index buffer object 

		// Mark this mesh as updated and need to update buffer.
		void markAsUpdated();
	public:
		ChunkMesh();
		~ChunkMesh();

		void initBuffer(const std::vector<float>& vertices, const std::vector<float>& colors, const std::vector<float>& normals, const std::vector<unsigned int>& indices);
		void loadBuffer(Program* program);

		bool bind();
		void render();
		void unbind();

		// Release mesh. Delete vao and set bools to false
		void releaseVAO();
		// Clear all buffers(vertices, colors, indices)
		void clearBuffers();
		
		// Check if it mesh is renderable. True if vao is not 0 or has buffer to load
		bool isRenderable();
		// Check if mesh buffer can be loaded to gPU
		bool isBufferLoadable();
	};
}

#endif