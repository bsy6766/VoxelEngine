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
		std::vector<float> normals;
		std::vector<unsigned int> indices;

		int indicesSize;

		// Atomic bool. True if buffers are ready to get loaded to GPU
		std::atomic<bool> bufferReady;
		// Another atomic bool. True if mesh is ready to render
		std::atomic<bool> bufferLoaded;

		// Opengl objects
		GLuint vao;	// vertex array object
		GLuint vbo;	// vertex buffer object (cube)
		GLuint cbo;	// color buffer object
		GLuint nbo;
		GLuint ibo;	// index buffer object 
	public:
		ChunkMesh();
		~ChunkMesh();

		void initBuffer(const std::vector<float>& vertices, const std::vector<float>& colors, const std::vector<float>& normals, const std::vector<unsigned int>& indices);
		void loadBuffer();

		void bind();
		void render();
		void unbind();

		// Release mesh. Delete vao and set bools to false
		void releaseVAO();
		// Clear all buffers(vertices, colors, indices)
		void clearBuffers();

		// True if mesh has all vertices, colors and indices ready to loaded to GPU
		bool hasBufferToLoad();
		// True if mesh loaded buffer to GPU
		bool hasLoaded();
	};
}

#endif