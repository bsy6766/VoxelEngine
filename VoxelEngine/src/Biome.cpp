#include "Biome.h"
#include <iostream>

using namespace Voxel;

const std::unordered_map<BiomeType, std::vector<Biome::TreePair>> Biome::biomeTreeMap =
{
	{BiomeType::WOODS, {{Voxel::Vegitation::Tree::OAK, 100}}},
	{BiomeType::FOREST, {{Voxel::Vegitation::Tree::OAK, 50}, {Voxel::Vegitation::Tree::BIRCH, 50}}},
	{BiomeType::TAIGA, {{Voxel::Vegitation::Tree::SPRUCE, 100}}},
	{BiomeType::TAIGA_FOREST, {{Voxel::Vegitation::Tree::SPRUCE, 100}}}
};

Voxel::Biome::Biome()
	: temperature(0)
	, moisture(0)
	, type(Voxel::BiomeType::NONE)
{}

Voxel::BiomeType Voxel::Biome::getBiomeType(float temperature, float moisture, float elevation)
{
	// can't be negative
	if (moisture < 0) moisture = 0;
	if (temperature < 0) temperature = 0;
	if (elevation < 0) elevation = 0;

	// Max value for both moistuere and temperature is 2.0f
	if (moisture > 2.0f) moisture = 2.0f;
	if (temperature > 2.0f) temperature = 2.0f;
	if (elevation > 2.0f) elevation = 2.0f;

	/*
		Lowest ocean level = 30		// Todo: lowest sea level is 33 now.
		Sea level = 60
		Highest block above sea level = 150
	
		elevation : 2.0(max) = block level : 150 (max)
		0 : 2.0f = 30 (Lowest ocean level) : 150

		Shift right side by -30

		0 : 2.0f = 0 : 120

		Then..
		0.0f : 2.0f = 0 : 120 (30 : 150)
		0.5f : 2.0f = 30 : 120  (60 : 150)
		2.0f : 2.0f = 120 : 120 (150 : 150)

		if elevation is less than 0.5f, it means it's ocean
	*/

	/*
	if (elevation <= 0.5f)
	{
		// Ocean
		return Type::OCEAN;
	}
	else
	{
		
	}
	*/

	// Above sea level
	if (temperature <= 0.5f)
	{
		// Freezing.
		if (moisture <= 0.5f)
		{
			// Freezing and low moisture. Tundra.
			// Snowy biome with very few vegitation. 
			return Voxel::BiomeType::TUNDRA;
		}
		else
		{
			// Frezzing but has some moisture. Some plants and trees can grow in this biome
			return Voxel::BiomeType::ICY_TAIGA;
		}
	}
	else if (temperature > 0.5f && temperature <= 1.0f)
	{
		// Cold
		if (moisture <= 0.5f)
		{
			return Voxel::BiomeType::GRASS_DESERT;
		}
		else if (moisture > 0.5f && moisture <= 1.0f)
		{
			return Voxel::BiomeType::TAIGA;
		}
		else
		{
			return Voxel::BiomeType::TAIGA_FOREST;
		}
	}
	else if (temperature > 1.0f && temperature <= 1.5f)
	{
		// Warm
		if (moisture <= 0.5f)
		{
			// Warm and none or few moisture.
			if (temperature < 1.1f)
			{
				// Still hot
				// Desert
				return Voxel::BiomeType::DESERT;
			}
			else
			{
				// Not so hot as desert. Some plants can live.
				// Grass Desert
				return Voxel::BiomeType::GRASS_DESERT;
			}
		}
		else if (moisture > 0.5f && moisture <= 1.0f)
		{
			// Warm and some moisture.
			// Woods
			return Voxel::BiomeType::WOODS;
		}
		else if (moisture > 1.0f && moisture <= 1.5f)
		{
			// Warm and little humid
			// Forest
			return Voxel::BiomeType::FOREST;
		}
		else if (moisture > 1.5f)
		{
			// Warm and humid
			// Swamp
			return Voxel::BiomeType::SWAMP;
		}
	}
	else if (temperature > 1.5f)
	{
		// Hot
		if (moisture <= 0.5f)
		{
			// Hot and none or few moisture.
			// Desert
			return Voxel::BiomeType::DESERT;
		}
		else if (moisture > 0.5f && moisture <= 1.0f)
		{
			// Hot and some moisture.
			// Savanna
			return Voxel::BiomeType::SAVANNA;
		}
		else if (moisture > 1.0f && moisture <= 1.5f)
		{
			// Hot and little humid
			// Seasonla forest
			return Voxel::BiomeType::SEASONAL_FOREST;
		}
		else if (moisture > 1.5f)
		{
			// Hot and humid
			// Rain forest
			return Voxel::BiomeType::RAIN_FOREST;
		}
	}

	// Error. Can't get biome
	std::cout << "[Biome] Error. Wasn't able to find biome with t: " << temperature << ", m: " << moisture << ", e: " << elevation << std::endl;
	return Voxel::BiomeType::ERROR;
}

std::string Voxel::Biome::biomeTypeToString(Voxel::BiomeType type)
{
	switch (type)
	{
	case Voxel::BiomeType::OCEAN:
		return "OCEAN";
		break;
	case Voxel::BiomeType::TUNDRA:
		return "TUNDRA";
		break;
	case Voxel::BiomeType::GRASS_DESERT:
		return "GRASS DESERT";
		break;
	case Voxel::BiomeType::TAIGA:
		return "TAIGA";
		break;
	case Voxel::BiomeType::ICY_TAIGA:
		return "ICY_TAIGA";
		break;
	case Voxel::BiomeType::DESERT:
		return "DESERT";
		break;
	case Voxel::BiomeType::WOODS:
		return "WOODS";
		break;
	case Voxel::BiomeType::FOREST:
		return "FOREST";
		break;
	case Voxel::BiomeType::TAIGA_FOREST:
		return "TAIGA_FOREST";
		break;
	case Voxel::BiomeType::SWAMP:
		return "SWAMP";
		break;
	case Voxel::BiomeType::SAVANNA:
		return "SAVANNA";
		break;
	case Voxel::BiomeType::SEASONAL_FOREST:
		return "SEASONAL FOREST";
		break;
	case Voxel::BiomeType::RAIN_FOREST:
		return "RAIN FOREST";
		break;
	case Voxel::BiomeType::ERROR:
	default:
		break;
	}

	return "ERROR";
}

std::string Voxel::Biome::biomeTypeToString(Biome biomeType)
{
	return biomeTypeToString(biomeType.type);
}

void Voxel::Biome::setType(const float temperature, const float moisture)
{
	this->temperature = temperature;
	this->moisture = moisture;

	this->type = getBiomeType(temperature, moisture, 0);
}

::Voxel::BiomeType Voxel::Biome::getType()
{
	return this->type;
}

float Voxel::Biome::getTemperature()
{
	return temperature;
}

float Voxel::Biome::getMoisture()
{
	return moisture;
}

void Voxel::Biome::initVegitation()
{
	// init vegitation. 
	// when initialize vegitation with weight, use base weight and randomly modify the weight.
}

bool Voxel::Biome::hasTree()
{
	return Biome::biomeTreeMap.find(this->type) != Biome::biomeTreeMap.end();
}
