#include "Terrain.h"

using namespace Voxel;


const std::unordered_map<Biome::Type, std::vector<Terrain::Type>> Terrain::biomeTerrainMap = 
{
	{ Biome::Type::WOODS,{ Terrain::Type::PLAIN, Terrain::Type::HILLS } },
	{ Biome::Type::TAIGA,{ Terrain::Type::PLAIN, Terrain::Type::HILLS } },
	{ Biome::Type::FOREST,{ Terrain::Type::PLAIN, Terrain::Type::HILLS, Terrain::Type::MOUNTAINS } },
};

const std::unordered_map<Terrain::Type, std::vector<Terrain::Modifier>> Terrain::TerrainModifierMap = 
{
	{ Terrain::Type::PLAIN,{ Terrain::Modifier::NONE } },
	{ Terrain::Type::HILLS,{ Terrain::Modifier::MEDIUM, Terrain::Modifier::LARGE } },
	{ Terrain::Type::MOUNTAINS,{ Terrain::Modifier::LARGE, Terrain::Modifier::MEGA } },
};


Voxel::Terrain::Terrain()
	: type(Type::NONE)
	, modifier(Modifier::NONE)
{}

std::string Voxel::Terrain::terrainTypeToString(Voxel::Terrain::Type terrain, Voxel::Terrain::Modifier modifier)
{
	std::string terrainStr;

	switch (terrain)
	{
	case Voxel::Terrain::Type::PLAIN:
		terrainStr = "PLAIN";
		break;
	case Voxel::Terrain::Type::HILLS:
		terrainStr = "HILLS";
		break;
	case Voxel::Terrain::Type::MOUNTAINS:
		terrainStr = "MOUNTAINS";
		break;
	default:
		return "ERROR";
		break;
	}

	switch (modifier)
	{
	case Voxel::Terrain::Modifier::NONE:
		break;
	case Voxel::Terrain::Modifier::MEDIUM:
		terrainStr += " M";
		break;
	case Voxel::Terrain::Modifier::LARGE:
		terrainStr += " L";
		break;
	case Voxel::Terrain::Modifier::MEGA:
		terrainStr += " MEGA";
		break;
	default:
		return "ERROR";
		break;
	}

	return terrainStr;
}

std::string Voxel::Terrain::terrainTypeToString(Terrain terrainType)
{
	return terrainTypeToString(terrainType.type, terrainType.modifier);
}

void Voxel::Terrain::setType(Type type)
{
	this->type = type;
}

Voxel::Terrain::Type Voxel::Terrain::getType()
{
	return this->type;
}

void Voxel::Terrain::setModifier(Modifier modifier)
{
	this->modifier = modifier;
}

Voxel::Terrain::Modifier Voxel::Terrain::getModifier()
{
	return this->modifier;
}
