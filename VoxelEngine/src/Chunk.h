#ifndef CHUNK_H
#define CHUNK_H

#include <vector>
#include <glm\glm.hpp>
#include <atomic>
#include <Physics.h>

namespace Voxel
{
	class ChunkSection;
	class ChunkMesh;

	/**
	*	@class Chunk
	*	@brief A chunk of data that contains 16 x 16 x 16 blocks.
	*/
	class Chunk
	{
		friend class ChunkMeshGenerator;
		friend class ChunkLoader;
	private:
		Chunk();

		// Chunk position in the world. Ignore y. Y is handled in ChunkSection
		glm::ivec3 position;

		// World position of chunk. Ignore y. Y is handled in ChunkSection
		glm::vec3 worldPosition;

		// ChunkSections
		std::vector<ChunkSection*> chunkSections;

		// Mesh. Contains mesh data and OpenGL objects
		ChunkMesh* chunkMesh;

		// Border & range
		AABB boundingBox;

		// region data for each block
		std::vector<unsigned int> regionMap;

		// active state. Only active chunk can be queried and gets updated
		bool active;

		// visible state. True if chunk is visible to player
		bool visible;

		// Loaded state. True if chunk is loaded(generated). Else, false. If false, worker thread will generate chunk
		std::atomic<bool> generated;

		// Timestamp. If chunk hasn't been activated for long time, it gets removed from map.
		double timestamp;

		// Initialize
		bool init(int x, int z);

		// Check before generate
		bool canGenerate();
	public:
		~Chunk();

		// Create chunk and initailize
		static Chunk* create(const int x, const int z);
		// Create empty chunk
		static Chunk* createEmpty(const int x, const int z);

		// Generates chunk.
		bool generate(const std::vector<std::vector<int>>& heightMap, const int minChunkSectionY, const int maxChunkSectionY);
		// Generates chunk. Default generation. Fills single chunk section with grass block
		bool generateSingleSection();
		// Generates chunk. Uses noise to generate biome.
		bool generateWithBiomeTest();
		// Generates chunk. Same as default generation but with color
		bool generateWithColor(const glm::uvec3& color);
		// Generates chunk with region color. This is for to test if block has correct region data
		bool generateWithRegionColor();

		// Unloads chunk. Delete chunk mesh. Stops everything that is in this chunk
		void unload();
		
		// Get position of chunk in coordinate
		glm::ivec3 getPosition();
		// Get position in x and z axis
		glm::ivec2 getCoordinate();

		// Get chunk position in world (center of chunk)
		glm::vec3 getWorldPosition();

		// Get chunkSection by Y (chunk section's y level not world pos)
		ChunkSection* getChunkSectionAtY(const int y);
		void createChunkSectionAtY(const int y);
		void deleteChunkSectionAtY(const int y);

		// Render chunk
		void render();

		// Set active state
		void setActive(const bool state);
		// Get active state
		bool isActive();

		// SEt chunk's visibility
		void setVisibility(const bool visibility);
		// Get visibility
		bool isVisible();

		// Release chunk mesh and delete vao
		void releaseMesh();
		
		// Get chunk's mesh
		ChunkMesh* getMesh();

		// Get bounding Box
		AABB getBoundingBox();

		// Set all block region data
		void setRegionMap(const unsigned int regionID);
		// Set block region 
		void setRegionMap(const std::vector<unsigned int>& regionIDs);

		// Check if chunk is generated by terrain generator, etc
		bool isGenerated();

		// Update chunk's last activated timestamp
		void updateTimestamp(const double timestamp);
	};

}
#endif