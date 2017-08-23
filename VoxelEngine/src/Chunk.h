#ifndef CHUNK_H
#define CHUNK_H

#include <vector>
#include <glm\glm.hpp>

namespace Voxel
{
	class ChunkSection;

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

		// Border
		ChunkBorder border;

		// True if chunk is loaded on chunk loader. Active state tells you that this chunk is in the player's render distance.
		bool active;

		// Timestamp. If chunk hasn't been activated for long time, it gets removed from map.
		double timestamp;

		// Initialize
		bool init(int x, int z);
	public:
		~Chunk();

		static Chunk* create(const int x, const int z);
		
		// Checks if other chunk is next to it including diagonal
		//bool isAdjacent(Chunk* other);

		// Get position of chunk in coordinate
		glm::ivec3 getPosition();
		// Get chunk position in world (center of chunk)
		glm::vec3 getWorldPosition();

		// Get chunkSection by Y (chunk section's y level not world pos)
		ChunkSection* getChunkSectionByY(int y);
		
		void render();

		void setActive(const bool state);
		bool isActive();

		// Set visibility to all chunk sections
		void setAllVisibility(const bool visibility);

		// Release all chunk section's mesh
		void releaseAllMeshes();

		// Check if there is chunk section needs new mesh
		bool hasChunkSectionNeedMesh();
	};

}
#endif