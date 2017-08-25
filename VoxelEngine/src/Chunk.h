#ifndef CHUNK_H
#define CHUNK_H

#include <vector>
#include <glm\glm.hpp>
#include <atomic>

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
		// visible state. True if chunk is visible to player
		bool visible;
		// Loaded state. True if chunk is loaded(generated). Else, false. If false, worker thread will generate chunk
		std::atomic<bool> generated;

		// Timestamp. If chunk hasn't been activated for long time, it gets removed from map.
		double timestamp;

		// Initialize
		bool init(int x, int z);
	public:
		~Chunk();

		static Chunk* create(const int x, const int z);
		static Chunk* createEmpty(const int x, const int z);

		// generate terrains
		bool generate();

		// Unloads chunk. Delete chunk mesh. Stops everything that is in this chunk
		void unload();

		// Checks if other chunk is next to it including diagonal
		//bool isAdjacent(Chunk* other);

		// Get position of chunk in coordinate
		glm::ivec3 getPosition();
		glm::ivec2 getCoordinate();
		// Get chunk position in world (center of chunk)
		glm::vec3 getWorldPosition();

		// Get chunkSection by Y (chunk section's y level not world pos)
		ChunkSection* getChunkSectionByY(int y);

		void render();

		void setActive(const bool state);
		bool isActive();

		void setVisibility(const bool visibility);
		bool isVisible();

		void releaseMesh();
		ChunkMesh* getMesh();

		bool isGenerated();

		bool isPointInBorder(const glm::vec3& point);
		void updateTimestamp(const double timestamp);
	};

}
#endif