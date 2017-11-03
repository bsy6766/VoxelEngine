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
	
	/**
	*	@class TreeBuilder
	*	@brief Static class that build trees
	*
	*	Oak Tree
	*	Oak tree is the most common tree that can be found in the game.
	*	Most of the oak tree has SMALL or MEDIUM TrunkHeight and rarely has LARGE TrunkHeight. 
	*	MEGA TrunkHeight oak tree only spawns in special conditions, such as in rare biome.
	*	Average TrunkWidth of oak tree is MEDIUM. It rarely gets SMALL and LARGE.
	*	Leaves are ellipsoid shape and my have extra round leaves.
	*	Trunk color is brown and leaves are slightly dark green.
	*	Leave color can change to lots of different color if biome is Seasonal Forest.
	*
	*	Birch Tree
	*	Birch tree is the second most common tree that can be found in the game.
	*	Most of the birch tree has MEDIUM OR LARGE Trunk Height and rarely has SMALL TrunkHeight.
	*	Unlike oak tree, birch tree can't have LARGE TrunkWidth. Most of them are thin.
	*	Leaves are slightly brighter than oak tree, near to light green than green
	*	Trunk color is gray-ish white with some black in middle
	*	Leave color can change to lots of different color if biome is Seasonal Forest.
	*/
	class TreeBuilder
	{
	public:
		// Type of tree.
		enum class TreeType
		{
			OAK = 0,
			SPRUCE,
			BIRCH,
		};

		// Height of tree
		enum class TrunkHeightType
		{
			SMALL = 0,
			MEDIUM,
			LARGE,
			MEGA,
		};

		// Color of leaves

		// width of tree
		enum class TrunkWidthType
		{
			SMALL = 0,
			MEDIUM,
			LARGE,
			MEGA,
		};

	private:
		TreeBuilder() = delete;

		static void addPosLayer(std::vector<glm::ivec3>& p, const int level);
		/**
		*	Get reandom tree height from tree type and trunk height
		*	@param [in] treeType Type of tree
		*	@param [in] trunkHeight Type of trunk height
		*	@param [in] engine Reference of chunk's local random engine
		*	@return An integer of tree trunk height.
		*/
		static int getRandomTreeTrunkHeight(const TreeBuilder::TreeType& treeType, const TreeBuilder::TrunkHeightType& trunkHeight, std::mt19937& engine);

		/**
		*	Get random leave size
		*	@param [in] treeType Type of tree
		*	@param [in] treeWidth Type of trunk width
		*	@param [out] width Width of leaves
		*	@param [out] height Height of leaves
		*	@param [out] length Length of leaves
		*	@param [in] engine Reference of chunk's local random engine
		*/
		static void getRandomLeavesSize(const TreeBuilder::TreeType& treeType, const TreeBuilder::TrunkWidthType& trunkWidthType, int& width, int& height, int& length, std::mt19937& engine);

		/**
		*	Add oak tree trunk.
		*	vector p's y position is set back to original position.
		*	@param [in] map Pointer to chunk map.
		*	@param [in] p List of points to add tree block in XZ space.
		*	@param [in] color Color of the trunk
		*	@param [in] colorStep Amount of color to add to trunk. Gives gradiant through trunk.
		*	@param [in] pStart Start index of vector p
		*	@param [in] pEnd Last index of vector p
		*	@param [in] trunkHeight Height of the trunk
		*	@param [in] startY First y position to add up trunk.
		*/
		static void addOakTrunk(ChunkMap* map, std::vector<glm::ivec3>& p, glm::vec3 color, const glm::vec3& colorStep, const int pStart, const int pEnd, const int trunkHeight, const int startY);

		/**
		*	Add multiple oak leaves
		*	@param [in] map Pointer to chunk map.
		*	@param [in] w Width of leaves
		*	@param [in] h Height of leaves
		*	@param [in] l Length of leaves
		*	@param [in] pos Center position of leaves
		*/
		static void addOakLeaves(ChunkMap* map, const int w, const int h, const int l, const glm::ivec3& pos, std::mt19937& engine);

		/**
		*	Add single oak leave
		*	@param [in] map Pointer to chunk map.
		*	@param [in] w Width of leaves
		*	@param [in] h Height of leaves
		*	@param [in] l Length of leaves
		*	@param [in] pos Center position of leaves
		*/
		static void addOakLeave(ChunkMap* map, const int w, const int h, const int l, const glm::ivec3& pos, std::mt19937& engine);

		/**
		*	Add branch on oak tree
		*	@param [in] map Pointer to chunk map.
		*	@param [in] p List of points to add tree block in XZ space.
		*	@param [in] branchBaseY First y level of the branch to expand.
		*	@param [in] engine Reference of chunk's local random engine
		*/
		static void addOakBranch(ChunkMap* map, std::vector<glm::ivec3>& p, const int branchBaseY, std::mt19937& engine);

		/**
		*	Add birch tree trunk.
		*	Birch tree has black crack-like marks on trunk
		*	vector p's y position is set back to original position.
		*	@param [in] map Pointer to chunk map.
		*	@param [in] p List of points to add tree block in XZ space.
		*	@param [in] trunkColor Color of the trunk
		*	@param [in] colorStep Amount of color to add to trunk. Gives gradiant through trunk.
		*	@param [in] markColor Color of the marks
		*	@param [in] pStart Start index of vector p
		*	@param [in] pEnd Last index of vector p
		*	@param [in] trunkHeight Height of the trunk
		*	@param [in] startY First y position to add up trunk.
		*	@param [in] engine Reference of chunk's local random engine for random marks
		*/
		static void addBirchTrunk(ChunkMap* map, std::vector<glm::ivec3>& p, glm::vec3 trunkColor, const glm::vec3& colorStep, const glm::vec3& markColor, const int pStart, const int pEnd, const int trunkHeight, const int startY, std::mt19937& engine);

		/**
		*	Add multiple birch leaves
		*	Birch leaves have multiple side leaves compared to oak tree. Also main leave is long ellipsoid.
		*	@param [in] map Pointer to chunk map.
		*	@param [in] w Width of leaves
		*	@param [in] h Height of leaves
		*	@param [in] l Length of leaves
		*	@param [in] pos Center position of leaves
		*/
		static void addBirchLeaves(ChunkMap* map, const int w, const int h, const int l, const glm::ivec3& pos, std::mt19937& engine);

		/**
		*	Add single birch leave
		*	@param [in] map Pointer to chunk map.
		*	@param [in] w Width of leaves
		*	@param [in] h Height of leaves
		*	@param [in] l Length of leaves
		*	@param [in] pos Center position of leaves
		*/
		static void addBirchLeave(ChunkMap* map, const int w, const int h, const int l, const glm::ivec3& pos, std::mt19937& engine);

		/**
		*	Create oak tree
		*	@param [in] chunkMap Pointer to chunk map.
		*	@param [in] chunkXZ Position of chunk to add tree.
		*	@param [in] treeLocalPos Tree's local position in chunk.
		*	@param [in] engine Reference of chunk's local random engine
		*/
		static void createOakTree(ChunkMap* chunkMap, const glm::ivec2& chunkXZ, const glm::ivec3& treeLocalPos, std::mt19937& engine);

		/**
		*	Create oak tree
		*	@param [in] chunkMap Pointer to chunk map.
		*	@param [in] h Trunk height of tree. 
		*	@param [in] w Trunk width of tree.
		*	@param [in] chunkXZ Position of chunk to add tree.
		*	@param [in] treeLocalPos Tree's local position in chunk.
		*	@param [in] engine Reference of chunk's local random engine
		*/
		static void createOakTree(const TreeBuilder::TrunkHeightType h, const TreeBuilder::TrunkWidthType w, ChunkMap* chunkMap, const glm::ivec2& chunkXZ, const glm::ivec3& treeLocalPos, std::mt19937& engine);

		/**
		*	Create birch tree
		*	@param [in] chunkMap Pointer to chunk map.
		*	@param [in] chunkXZ Position of chunk to add tree.
		*	@param [in] treeLocalPos Tree's local position in chunk.
		*	@param [in] engine Reference of chunk's local random engine
		*/
		static void createBirchTree(ChunkMap* chunkMap, const glm::ivec2& chunkXZ, const glm::ivec3& treeLocalPos, std::mt19937& engine);

		/**
		*	Create birch tree
		*	@param [in] chunkMap Pointer to chunk map.
		*	@param [in] h Trunk height of tree.
		*	@param [in] w Trunk width of tree.
		*	@param [in] chunkXZ Position of chunk to add tree.
		*	@param [in] treeLocalPos Tree's local position in chunk.
		*	@param [in] engine Reference of chunk's local random engine
		*/
		static void createBirchTree(const TreeBuilder::TrunkHeightType h, const TreeBuilder::TrunkWidthType w, ChunkMap* chunkMap, const glm::ivec2& chunkXZ, const glm::ivec3& treeLocalPos, std::mt19937& engine);

	public:
		/**
		*	Creates tree.
		*	@param [in] type Type of tree to create
		*	@param [in] chunkMap Pointer to chunk map.
		*	@param [in] chunkXZ Position of chunk to add tree.
		*	@param [in] treeLocalPos Tree's local position in chunk.
		*	@param [in] engine Reference of chunk's local random engine
		*/
		static void createTree(const TreeBuilder::TreeType type, ChunkMap* chunkMap, const glm::ivec2& chunkXZ, const glm::ivec3& treeLocalPos, std::mt19937& engine);

		/**
		*	Creates tree.
		*	@param [in] type Type of tree to create
		*	@param [in] h Trunk height of tree. 
		*	@param [in] w Trunk width of tree.
		*	@param [in] chunkMap Pointer to chunk map.
		*	@param [in] chunkXZ Position of chunk to add tree.
		*	@param [in] treeLocalPos Tree's local position in chunk.
		*	@param [in] engine Reference of chunk's local random engine
		*/
		static void createTree(const TreeBuilder::TreeType type, const TreeBuilder::TrunkHeightType h, const TreeBuilder::TrunkWidthType w, ChunkMap* chunkMap, const glm::ivec2& chunkXZ, const glm::ivec3& treeLocalPos, std::mt19937& engine);

		~TreeBuilder() = delete;
	};
}

#endif