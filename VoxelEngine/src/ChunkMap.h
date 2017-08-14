#ifndef CHUNK_MAP_H
#define CHUNK_MAP_H

#include <glm\glm.hpp>
#include <unordered_map>
#include <unordered_set>

namespace Voxel
{
	// forward
	class Chunk;
	class Block;

	// Hash for glm ivec2
	struct KeyFuncs
	{
		size_t operator()(const glm::ivec2& k)const
		{
			return std::hash<int>()(k.x) ^ std::hash<int>()(k.y);
		}

		bool operator()(const glm::ivec2& a, const glm::ivec2& b)const
		{
			return a.x == b.x && a.y == b.y;
		}
	};

	typedef std::unordered_map<glm::ivec2, Chunk*, KeyFuncs, KeyFuncs> ChunkUnorderedMap;

	/**
	*	@class ChunkMap
	*	@brief Simple wrapper or unordered_map with Chunk with some functionalities.
	*
	*	Chunk map stores all the chuncks that are ever generated.
	*	It keep tracks if chunk has been explored and generated or not.
	*	Chunk map doesn't update chunk. It only keeps the data.
	*/
	class ChunkMap
	{
	private:
		ChunkUnorderedMap map;

		// Chunk LUT. This stores chunk position (x, y. Not world pos) that has been ever generated
		std::unordered_set<glm::ivec2, KeyFuncs, KeyFuncs> chunkLUT;
		// Active Chunk LUT. This stores chunk position that is currently loaded in game
		std::unordered_set<glm::ivec2, KeyFuncs, KeyFuncs> activeChunkLUT;

	public:
		ChunkMap() = default;
		~ChunkMap();

		void init();

		// Clears all the chunk in the map
		void clear();

		ChunkUnorderedMap& getMapRef();

		bool hasChunkAtXZ(int x, int z);
		Chunk* getChunkAtXZ(int x, int z);

		Block* getBlockAtWorldXYZ(int x, int y, int z);

		// Attemp to laod chunk in map. If map already has chunk, 
		bool attempChunkLoad(int x, int z);

		void render();
	};
}


#endif