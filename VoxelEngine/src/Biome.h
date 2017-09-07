#ifndef BIOME_H
#define BIOME_H

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
		enum class Type
		{
			OCEAN,
			TUNDRA,
			GRASS_DESERT,
			TAIGA,
			DESERT,
			WOODS,
			FOREST,
			SWAMP,
			SAVANNA,
			SEASONAL_FOREST,
			RAIN_FOREST,
			ERROR,
		};
	private:
		Biome() = delete;
		~Biome() = delete;

	public:
		static Type getBiomeType(float moisture, float temperature, float elevation);
	};
}

#endif