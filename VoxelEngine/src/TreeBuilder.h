#ifndef TREE_BUILDER_H
#define TREE_BUILDER_H

#include <vector>
#include <Block.h>

namespace Voxel
{
	class ChunkMap;

	class Tree
	{
		friend class TreeBuilder;
	public:
		// Type of tree.
		enum class Type
		{
			OAK = 0,
			SPRUCE
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
		Tree();

		Type type;
		TrunkHeight height;
		TrunkWidth width;
		
		bool init(Type type, TrunkHeight height, TrunkWidth width);

		void buildRoot();
		void buildTrunk();

		unsigned int xyzToIndex(const int x, const int y, const int z);
	public:
		~Tree();
	};

	class TreeBuilder
	{
	private:
		TreeBuilder() = delete;
	public:
		static Tree* createOakTree(const Tree::TrunkHeight h, const Tree::TrunkWidth w);
		static void createTree(const Tree::Type type, const Tree::TrunkHeight h, const Tree::TrunkWidth w, ChunkMap* chunkMap, const glm::ivec2& chunkXZ, const glm::ivec2& treeLocalPos, const int topY);

		~TreeBuilder() = delete;
	};
}

#endif