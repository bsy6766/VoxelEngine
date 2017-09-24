#ifndef TREE_BUILDER_H
#define TREE_BUILDER_H

#include <vector>
#include <Block.h>

namespace Voxel
{
	class ChunkMap;

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

		static void addOakLeave(ChunkMap* map, const int w, const int h, const int l, const glm::ivec3& pos);
		
		static void createOakTree(const TreeBuilder::TrunkHeight h, const TreeBuilder::TrunkWidth w, ChunkMap* chunkMap, const glm::ivec2& chunkXZ, const glm::ivec2& treeLocalXZ, const int treeY);
	public:
		static void createTree(const TreeBuilder::TreeType type, const TreeBuilder::TrunkHeight h, const TreeBuilder::TrunkWidth w, ChunkMap* chunkMap, const glm::ivec2& chunkXZ, const glm::ivec2& treeLocalXZ, const int treeY);

		~TreeBuilder() = delete;
	};
}

#endif