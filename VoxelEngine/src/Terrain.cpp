#include "Terrain.h"
#include <Utility.h>

using namespace Voxel;


const std::unordered_map<Voxel::BiomeType, std::vector<Voxel::TerrainType>> Terrain::biomeTerrainMap = 
{
	{ Voxel::BiomeType::WOODS,{ Voxel::TerrainType::PLAIN, Voxel::TerrainType::HILLS } },
	{ Voxel::BiomeType::TAIGA,{ Voxel::TerrainType::PLAIN, Voxel::TerrainType::HILLS } },
	{ Voxel::BiomeType::FOREST,{ Voxel::TerrainType::PLAIN, Voxel::TerrainType::HILLS, Voxel::TerrainType::MOUNTAINS } },
	{ Voxel::BiomeType::TAIGA_FOREST,{ Voxel::TerrainType::PLAIN, Voxel::TerrainType::HILLS, Voxel::TerrainType::MOUNTAINS } },
};

/*
const std::unordered_map<Voxel::TerrainType, std::vector<Voxel::TerrainModifier>> Terrain::TerrainModifierMap = 
{
	{ Voxel::TerrainType::PLAIN,{ Voxel::TerrainModifier::NONE } },
	{ Voxel::TerrainType::HILLS,{ Voxel::TerrainModifier::MEDIUM, Voxel::TerrainModifier::LARGE } },
	{ Voxel::TerrainType::MOUNTAINS,{ Voxel::TerrainModifier::LARGE, Voxel::TerrainModifier::MEGA } },
};

*/

Voxel::Terrain::Terrain()
	: type(Voxel::TerrainType::NONE)
	//, modifier(Modifier::NONE)
{}

std::string Voxel::Terrain::terrainTypeToString(Voxel::TerrainType terrain, Voxel::TerrainModifier modifier)
{
	std::string terrainStr;

	switch (terrain)
	{
	case Voxel::TerrainType::PLAIN:
		terrainStr = "PLAIN";
		break;
	case Voxel::TerrainType::HILLS:
		terrainStr = "HILLS";
		break;
	case Voxel::TerrainType::MOUNTAINS:
		terrainStr = "MOUNTAINS";
		break;
	default:
		return "ERROR";
		break;
	}

	/*
	switch (modifier)
	{
	case Voxel::Voxel::TerrainModifier::NONE:
		break;
	case Voxel::Voxel::TerrainModifier::MEDIUM:
		terrainStr += " M";
		break;
	case Voxel::Voxel::TerrainModifier::LARGE:
		terrainStr += " L";
		break;
	case Voxel::Voxel::TerrainModifier::MEGA:
		terrainStr += " MEGA";
		break;
	default:
		return "ERROR";
		break;
	}

	*/
	return terrainStr;
}

std::string Voxel::Terrain::terrainTypeToString(Terrain terrainType)
{
	return terrainTypeToString(terrainType.type, terrainType.modifier);
}

void Voxel::Terrain::setTypeByBiome(Voxel::BiomeType biomeType, std::mt19937& engine)
{
	auto find_it = Terrain::biomeTerrainMap.find(biomeType);
	if (find_it == Terrain::biomeTerrainMap.end())
	{
		throw std::runtime_error("Biome type doesn't exists: " + Biome::biomeTypeToString(biomeType));
	}
	else
	{
		auto& terrainTypeList = find_it->second;
		
		int randIndex = std::uniform_int_distribution<>(0, static_cast<int>(terrainTypeList.size() - 1))(engine);

		setType(terrainTypeList.at(randIndex));

		// For now, no modifier
		modifier = Voxel::TerrainModifier::NONE;
	}
}

void Voxel::Terrain::setType(Voxel::TerrainType type)
{
	this->type = type;
}

Voxel::TerrainType Voxel::Terrain::getType() const
{
	return this->type;
}

void Voxel::Terrain::print()
{
	std::cout << "Terrain: " << Terrain::terrainTypeToString(type, modifier) << std::endl;
	std::cout << "\n";
}

/*
void Voxel::Terrain::setModifier(Modifier modifier)
{
	this->modifier = modifier;
}

Voxel::Voxel::TerrainModifier Voxel::Terrain::getModifier()
{
	return this->modifier;
}

*/