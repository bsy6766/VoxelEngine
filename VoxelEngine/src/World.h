#ifndef WORLD_H
#define WORLD_H

#include <glm\glm.hpp>
#include <GL\glew.h>
#include <GLFW\glfw3.h>

//temp
#include <ChunkMesh.h>

namespace Voxel
{
	// Foward
	class Program;
	class ChunkMap;
	class ChunkLoader;
	class ChunkMeshGenerator;
	class InputHandler;

	/**
	*	@class World
	*	@brief A world where everything is in. Terrain, player, weather, skybox, entity, items and all the stuffs that are rendered or even those doesen't.
	*
	*	For now, World instance is managed by Application class.
	*	There will be only one world at a time. There is no plan for zone phase shift or whatnot.
	*	Just giant single world where (theoretically) infinitely generates.
	*/
	class World
	{
	private:

		// temp
		GLuint vao;
		GLuint vbo;
		GLuint ibo;
		Program* program;
		glm::vec3 angle = glm::vec3(0);
		glm::mat4 tempRotation = glm::mat4(1.0f);
		glm::mat4 tempTralsnate = glm::mat4(1.0f);
		void initDebugCube();
		void initTestChunk();

		// Chunks
		ChunkMap* chunkMap;
		ChunkLoader* chunkLoader;
		ChunkMeshGenerator* chunkMeshGenerator;

		// temp
		ChunkMesh* chunkMesh;
	public:
		World();
		~World();

		InputHandler* input;
		float cameraMovementSpeed;

		float prevX, prevY;

		// Updates (tick) the world.
		void update(const float delta);
		void render(const float delta);
	};
}

#endif