#ifndef TERRAIN_H
#define TERRAIN_H

#include <string>

namespace Voxel
{
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

		static std::string terrainTypeToString(Type terrain, Modifier modifier);
		static std::string terrainTypeToString(Terrain terrainType);

		// setter and getter
		void setType(Type type);
		Type getType();

		void setModifier(Modifier modifier);
		Modifier getModifier();
	};
}

#endif // !TERRAIN_H