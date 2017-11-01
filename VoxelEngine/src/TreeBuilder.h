#ifndef TREE_BUILDER_H
#define TREE_BUILDER_H

#include <vector>
#include <Block.h>
#include <Utility.h>

namespace Voxel
{
	class ChunkMap;

	struct TreeData
	{
		int trunkHeight;
	};
	
	class TreeBuilder
	{
	public:
		// Type of tree.
		enum class TreeType
		{
			OAK = 0,
			SPRUCE,
			BIRCH
		};

		// Height of tree
		enum class TrunkHeight
		{
			SMALL = 0,
			MEDIUM,
			LARGE,
			MEGA
		};

		// Color of leaves

		// width of tree
		enum class TrunkWidth
		{
			SMALL = 0,
			MEDIUM,
			LARGE,
			MEGA
		};

	private:
		TreeBuilder() = delete;

		static void addTrunk(ChunkMap* map, std::vector<glm::ivec3>& p, glm::vec3 color, const glm::vec3& colorStep, const int pStart, const int pEnd, const int trunkHeight, const int startY);
		static void addOakLeaves(ChunkMap* map, std::vector<glm::ivec3>& p, const int w, const int h, const int l, const glm::ivec3& pos, std::mt19937& engine);
		static void addOakLeave(ChunkMap* map, const int w, const int h, const int l, const glm::ivec3& pos, std::mt19937& engine);
		static void addOakBranch(ChunkMap* map, std::vector<glm::ivec3>& p, const int branchBaseY);
		
		static void createOakTree(const TreeBuilder::TrunkHeight h, const TreeBuilder::TrunkWidth w, ChunkMap* chunkMap, const glm::ivec2& chunkXZ, const glm::ivec2& treeLocalXZ, const int treeY, std::mt19937& engine);
		static void createOakTree(ChunkMap* chunkMap, const glm::ivec2& chunkXZ, const glm::ivec2& treeLocalXZ, const int treeY, std::mt19937& engine);
	public:
		static void createTree(const TreeBuilder::TreeType type, const TreeBuilder::TrunkHeight h, const TreeBuilder::TrunkWidth w, ChunkMap* chunkMap, const glm::ivec2& chunkXZ, const glm::ivec2& treeLocalXZ, const int treeY, std::mt19937& engine);
		static void createTree(const TreeBuilder::TreeType type, ChunkMap* chunkMap, const glm::ivec2& chunkXZ, const glm::ivec2& treeLocalXZ, const int treeY, std::mt19937& engine);

		~TreeBuilder() = delete;
	};
}

#endif