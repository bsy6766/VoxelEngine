#ifndef BIOME_TYPE_H
#define BIOME_TYPE_H

#include <string>

namespace Voxel
{
	// Type of biome
	enum class BiomeType
	{
		NONE = 0,
		OCEAN,					// Filled with water
		/**
		*	Tundra
		*	Very cold area with very few vegitation.
		*	
		*	Weather
		*	Now rain. Clear most of the time
		* 
		*	Vegitation
		*	Tree: Very low chance of Spruce and Pine tree.
		*	Grass: Few Low Grasses.
		*	Flower: //Todo: Add winter flower
		*/
		TUNDRA,
		GRASS_DESERT,			// Cold and warm area. Covered with sand. No rain, but possible oasis and cactus
		DESERT,					// Warm and hot area. Covered with sand. No rain. Very few vegiation. 
		/**
		*	Icy Taiga
		*	Very similar to Taiga but more cold and less vegitation. Some ices can be found.
		*	
		*	Weather
		*	Can rain
		*
		*	Vegitation
		*	Tree: Very low chance of Spruce and Pine tree
		*/
		ICY_TAIGA,
		/**
		*	Taiga
		*	Cold area with some vegitation
		*
		*	Weather
		*	Can rain.
		*
		*	Vegitation
		*	Tree: Low to medium chance of Spruce and Pine tree.
		*	Grass: Low Grasses, Ferns(?), Some berries grows here
		*/
		TAIGA,
		WOODS,					// Warm area. few moisture too. Few tree grows. Some grass and flowers.
		SAVANNA,				// Hot area. Few moisture. Few tree, grass, flowers grows. 
		TAIGA_FOREST,			// Cold area. Basically Taiga but with more vegitation. 
		FOREST,					// Warm area with some moisture. Fully covered with trees. 
		SEASONAL_FOREST,		// Basically forest but with different variation of trees. 
		SWAMP,					// Warm area with high moisture. Water can be found. Oak trees can be found.
		RAIN_FOREST,			// Jungle. Lots of rain and hot area. Lots of tall trees. Lots of plants below the tree. 
		ERROR,
	};

	namespace Vegitation
	{
		enum class Tree
		{
			NONE = 0,
			OAK,
			OAK_BIRCH,
			BIRCH,
			SPRUCE,
			PINE,
			SPRUCE_PINE,
			ACACIA,
			JUNGLE,
			MAPLE
		};
		
		/**
		*	@enum Plant
		*
		*	Plants are small vegitation that takes up to 1 block size
		*/
		enum class Plant
		{
			NONE = 0,
			SHORT_GRASS,
			TALL_GRASS,
			FERN,
			KORU,
			CAT_TAIL,
			CACTUS,
			SHRUB,
		};

		/**
		*	@enum Flower
		*	
		*	Flowers naturally grows through out the world. Different types of flower can be found on different biomes.
		*	Once player founds flower field, they can collect all the flowers nearby by holding the key.
		*	
		*	Flowers are main source of pigment. Once they are looted, it autoamtically converts to pigment.
		*	For example, rose become red pigment and sunflower becomes yellow pigment.
		*	Pigments are main source to unlock different colors of block. 
		*/
		enum class Flower
		{
			NONE = 0,
			ROSE,			// Red
			DAISY,			// White
		};
	}

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