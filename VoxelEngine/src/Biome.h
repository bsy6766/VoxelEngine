#ifndef BIOME_H
#define BIOME_H

#include <string>
#include <unordered_map>

namespace Voxel
{
	/**
	*	@class Biome
	*	@brief A class that determines the biome based on temperature and moisture
	*	
	*	Biome determines the climate and enviornment, not terrain.
	*
	*	Referense: Look that the image 'BiomesGraph.png' in Researches folder
	*/
	class Biome
	{
	public:
		// Type of biome
		enum class Type
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
			BORDER,					// Area that can not be explored by player. 
			ERROR,
		};

		// types of vegitation.
		// Todo: Separate some types in to more detaisl
		enum class Vegitation
		{
			NONE = 0,				// No vegitation
			OAK_TREE,
			PINE_TREE,
			SPRUCE_TREE,
			BIRCH_TREE,
			ACACIA_TREE,
			GRASS,
			SHRUB,
			CACTUS,
			FLOWER,
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

	public:
		// Store types of vegitation that can grow in biome
		static const std::unordered_map<Type, std::vector<Vegitation>> vegitationMap;
		// Store types of living entity that can live in biome
		// Store types of structure that can be spawned in biome

		static const float COLD;
		static const float WARM;
		static const float HOT;

		float temperature;
		float moisture;

		Type type;
		
	public:
		Biome();
		~Biome() = default;

		static Type getBiomeType(float temperature, float moisture, float elevation);

		static std::string biomeTypeToString(Type type);
		static std::string biomeTypeToString(Biome biomeType);

		void setType(const float temperature, const float moisture);
		Type getType();

		float getTemperature();
		float getMoisture();
	};
}

#endif