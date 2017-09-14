#ifndef TERRAIN_H
#define TERRAIN_H

#include <string>
#include <unordered_map>

#include <Biome.h>

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
		// Default terrains. 
		enum class Type
		{
			NONE = 0,
			PLAIN,			// Flat terrain. No hills and mountains
			HILLS,			// Few hills in flat terrain
			MOUNTAINS,		// 
		};

		// Modifiers gives changes on world generation, gives more randomness to the game
		enum class Modifier
		{
			NONE = 0,		// Defult terrain
			MEDIUM,			// Few variation 
			LARGE,			// Some variation
			MEGA			// Large variation
		};

		Type type;
		Modifier modifier;
	public:
		Terrain();
		~Terrain() = default;

		// Stores possible terrains for each biome
		static const std::unordered_map<Biome::Type, std::vector<Type>> biomeTerrainMap;
		// Stores possible terrain modifer for terrain type
		static const std::unordered_map<Type, std::vector<Modifier>> TerrainModifierMap;

		// Get terrain type to string
		static std::string terrainTypeToString(Type terrain, Modifier modifier);
		static std::string terrainTypeToString(Terrain terrainType);

		// getter
		void setType(Type type);
		Type getType() const;

		// setter
		void setModifier(Modifier modifier);
		Modifier getModifier();
	};
}

#endif // !TERRAIN_H