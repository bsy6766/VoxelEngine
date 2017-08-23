#ifndef CHUNK_MESH_H
#define CHUNK_MESH_H

#include <vector>
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <atomic>

namespace Voxel
{
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
		std::vector<unsigned int> indices;

		int indicesSize;

		// Atomic bool. True if buffers are loaded
		std::atomic<bool> bufferReady;
		// Another atomic bool. True if mesh is ready to render
		std::atomic<bool> bufferLoaded;

		// Opengl objects
		GLuint vao;	// vertex array object
		GLuint vbo;	// vertex buffer object (cube)
		GLuint cbo;	// color buffer object
		GLuint ibo;	// index buffer object 
	public:
		ChunkMesh();
		~ChunkMesh();

		void initBuffer(const std::vector<float>& vertices, const std::vector<float>& colors, const std::vector<unsigned int>& indices);
		void loadBuffer();

		void bind();
		void render();
		void unbind();

		// Release mesh. Delete vao and set bools to false
		void release();

		// True if mesh has all vertices, colors and indices ready to loaded to GPU
		bool hasBufferToLoad();
		// True if mesh loaded buffer to GPU
		bool hasLoaded();
	};
}

#endif