#include "Terrain.h"

using namespace Voxel;

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
