#ifndef CHUNK_H
#define CHUNK_H

#include <vector>
#include <glm\glm.hpp>

namespace Voxel
{
	class ChunkSection;

	/**
	*	@class Chunk
	*	@brief A chunk of data that contains 16 x 16 x 16 blocks.
	*/
	class Chunk
	{
	public:
		const static unsigned int TOTAL_CHUNK_SECTION_PER_CHUNK;
	private:
		Chunk();

		// Chunk position in the world
		glm::ivec3 position;

		// World position of chunk. Ignore y. Y is handled in ChunkSection
		glm::vec3 worldPosition;

		// ChunkSections
		std::vector<ChunkSection*> chunkSections;

		bool init(int x, int z);
	public:
		~Chunk();

		static Chunk* create(const int x, const int z);
	};

}
#endif