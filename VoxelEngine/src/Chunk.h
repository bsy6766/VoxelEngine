#ifndef CHUNK_H
#define CHUNK_H

#include <vector>
#include <glm\glm.hpp>

namespace Voxel
{
	class ChunkSection;
	class ChunkMesh;

	struct ChunkBorder
	{
		// In world position
		glm::vec3 min;
		glm::vec3 max;
	};

	/**
	*	@class Chunk
	*	@brief A chunk of data that contains 16 x 16 x 16 blocks.
	*/
	class Chunk
	{
		friend class ChunkMeshGenerator;
		friend class ChunkLoader;
	public:
	private:
		Chunk();

		// Chunk position in the world
		glm::ivec3 position;

		// World position of chunk. Ignore y. Y is handled in ChunkSection
		glm::vec3 worldPosition;

		// ChunkSections
		std::vector<ChunkSection*> chunkSections;

		// Mesh. Contains mesh data and OpenGL objects
		ChunkMesh* chunkMesh;

		// Border
		ChunkBorder border;

		// active state. Only active chunk can be queried
		bool active;

		bool init(int x, int z);
	public:
		~Chunk();

		static Chunk* create(const int x, const int z);

		// Unloads chunk. Delete chunk mesh. Stops everything that is in this chunk
		void unload();

		// Checks if other chunk is next to it including diagonal
		//bool isAdjacent(Chunk* other);

		// Get position
		glm::ivec3 getPosition();

		// Get chunkSection by Y (chunk section's y level not world pos)
		ChunkSection* getChunkSectionByY(int y);

		void render();

		void setActive(const bool state);
		bool isActive();
	};

}
#endif