#include "Biome.h"
#include <iostream>

using namespace Voxel;

Biome::Type Voxel::Biome::getBiomeType(float moisture, float temperature, float elevation)
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
		Lowest ocean level = 30
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
		// Freezing
		// Always tundra
		return Type::TUNDRA;
	}
	else if (temperature > 0.5f && temperature <= 1.0f)
	{
		// Cold
		/*
		if (moisture <= 0.5f)
		{
		// Cold and none or few moisture.
		// Grass desert
		return Type::GRASS_DESERT;
		}
		else
		{
		}
		*/
		// Taiga
		return Type::TAIGA;
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
				return Type::DESERT;
			}
			else
			{
				// Not so hot as desert. Some plants can live.
				// Grass Desert
				return Type::GRASS_DESERT;
			}
		}
		else if (moisture > 0.5f && moisture <= 1.0f)
		{
			// Warm and some moisture.
			// Woods
			return Type::WOODS;
		}
		else if (moisture > 1.0f && moisture <= 1.5f)
		{
			// Warm and little humid
			// Forest
			return Type::FOREST;
		}
		else if (moisture > 1.5f)
		{
			// Warm and humid
			// Swamp
			return Type::SWAMP;
		}
	}
	else if (temperature > 1.5f)
	{
		// Hot
		if (moisture <= 0.5f)
		{
			// Hot and none or few moisture.
			// Desert
			return Type::DESERT;
		}
		else if (moisture > 0.5f && moisture <= 1.0f)
		{
			// Hot and some moisture.
			// Savanna
			return Type::SAVANNA;
		}
		else if (moisture > 1.0f && moisture <= 1.5f)
		{
			// Hot and little humid
			// Seasonla forest
			return Type::SEASONAL_FOREST;
		}
		else if (moisture > 1.5f)
		{
			// Hot and humid
			// Rain forest
			return Type::RAIN_FOREST;
		}
	}

	// Error. Can't get biome
	std::cout << "[Biome] Error. Wasn't able to find biome with t: " << temperature << ", m: " << moisture << ", e: " << elevation << std::endl;
	return Type::ERROR;
}
