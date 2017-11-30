#ifndef TERRAIN_H
#define TERRAIN_H

// cpp
#include <string>
#include <unordered_map>
#include <random>

// voxel
#include "Biome.h"
#include "TerrainType.h"

namespace Voxel
{
	/**
	*	@class Terrain
	*	@brief Contains terrain data.
	*
	*	Terrain data tells you how terrain will be generated in world generation.
	*	This is different than biome. Biome decides climate, vegitaion, living entities, etc.
	*	
	*	To decide which terrain to use, you first need biome type. Each biome has possible terrains that can be generated.
	*	The reasion is to prevent bad world generation such as desert with mountians. 
	*
	*	Once type is made, Modifier gives more randomness to the game.
	*	Modifier adds extra features to the terrain generation. For example, LARGE type Hills will have lots of hills between while NONE has few here and there.
	*/
	class Terrain
	{
	public:
		Voxel::TerrainType type;
		Voxel::TerrainModifier modifier;
	public:
		Terrain();
		~Terrain() = default;

		// Stores possible terrains for each biome
		static const std::unordered_map<Voxel::BiomeType, std::vector<Voxel::TerrainType>> biomeTerrainMap;
		// Stores possible terrain modifer for terrain type
		//static const std::unordered_map<Type, std::vector<Modifier>> TerrainModifierMap;

		// Get terrain type to string
		static std::string terrainTypeToString(Voxel::TerrainType terrain, Voxel::TerrainModifier modifier);
		static std::string terrainTypeToString(Terrain terrainType);

		// getter
		void setTypeByBiome(Voxel::BiomeType biomeType, std::mt19937& engine);
		void setType(Voxel::TerrainType type);
		Voxel::TerrainType getType() const;

		// setter
		//void setModifier(Modifier modifier);
		//Modifier getModifier();

		void print();
	};
}

#endif // !TERRAIN_H