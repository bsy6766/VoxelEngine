#ifndef TERRAIN_TYPE_H
#define TERRAIN_TYPE_H

namespace Voxel
{
	// Default terrains. 
	enum class TerrainType
	{
		NONE = 0,
		PLAIN,			// Flat terrain. No hills and mountains
		HILLS,			// Few hills in flat terrain
		MOUNTAINS,		// Few mountinas
		PLATEAU,
		BORDER,
	};



	// Modifiers gives changes on world generation, gives more randomness to the game
	enum class TerrainModifier
	{
		NONE = 0,		// Defult terrain
		MEDIUM,			// Few variation 
		LARGE,			// Some variation
		MEGA			// Large variation
	};
}

#endif