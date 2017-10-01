#ifndef BIOME_TYPE_H
#define BIOME_TYPE_H

namespace Voxel
{
	// Type of biome
	enum class BiomeType
	{
		NONE = 0,
		OCEAN,					// Filled with water
		TUNDRA,					// Coldest area covered with snow. Hard to find any trees. Several short shrubs or mosses. Possible berries. Rarely snows.
		GRASS_DESERT,			// Desert with some plants. No trees. Random water (oasis). some cactus
		TAIGA,					// Cold area. No snow. Possbile rain. Spruce and pine tree. Tree can grow short to mega sized.
		DESERT,					// Hot and dry area. No water can be found. No plants. few cactus
		WOODS,					// Warm area. few moisture too. Trees can grow but not there aren't many. 
		FOREST,					// Warm area with some moisture. Covered with trees.  
		SWAMP,					// Warm area with high moisture. Water can be found. Oak trees can be found.
		SAVANNA,				// Hot and few moisture. Acacia tree can be found
		SEASONAL_FOREST,		// 
		RAIN_FOREST,			// Jungle. Lots of rain and hot area. Lots of tall trees. Lots of plants below the tree. 
		ERROR,
	};

	enum class Tree
	{
		NONE = 0,
		OAK,
		BIRCH,
		SPRUCE,
		PINE,
		ACACIA,
		JUNGLE,
		MAPLE
	};

	enum class Plant
	{
		NONE = 0,
		LOW_GRASS,
		TALL_GRASS,
		CACTUS,
		SHRUB,
	};

	enum class Flower
	{
		NONE = 0,
		SUNFLOWER,
		ROSE,
		DAISY,
	};

	// Types of living entity for biomes. 
	enum class LivingEntity
	{
		NONE = 0,
	};

	// types of structure that can be spawned
	enum class Structure
	{
		NONE = 0,
	};
}

#endif