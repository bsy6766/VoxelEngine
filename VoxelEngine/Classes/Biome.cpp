// pch
#include "PreCompiled.h"

#include "Biome.h"

using namespace Voxel;

const std::unordered_map<BiomeType, std::vector<Biome::TreePair>> Biome::biomeTreeWeightMap =
{
	{ BiomeType::WOODS,{ { Voxel::Vegitation::Tree::OAK_BIRCH, 1 },{ Voxel::Vegitation::Tree::OAK, 1 },{ Voxel::Vegitation::Tree::BIRCH, 1 } } },
	{ BiomeType::FOREST,{ { Voxel::Vegitation::Tree::OAK_BIRCH, 1 },{ Voxel::Vegitation::Tree::OAK, 1 },{ Voxel::Vegitation::Tree::BIRCH, 1 } } },
	{ BiomeType::TAIGA,{ { Voxel::Vegitation::Tree::SPRUCE_PINE, 1 },{ Voxel::Vegitation::Tree::SPRUCE, 1 },{ Voxel::Vegitation::Tree::PINE, 1 } } },
	{ BiomeType::TAIGA_FOREST,{ { Voxel::Vegitation::Tree::SPRUCE_PINE, 1 },{ Voxel::Vegitation::Tree::SPRUCE, 1 },{ Voxel::Vegitation::Tree::PINE, 1 } } },
};

const std::unordered_map<BiomeType, std::vector<Biome::PlantPair>> Biome::biomePlantWeightMap = 
{
	{ BiomeType::WOODS,{ { Voxel::Vegitation::Plant::SHORT_GRASS, 1 } } },
	{ BiomeType::FOREST,{ { Voxel::Vegitation::Plant::SHORT_GRASS, 1 } } },
	{ BiomeType::TAIGA,{ { Voxel::Vegitation::Plant::FERN, 1 } } },
	{ BiomeType::TAIGA_FOREST,{ { Voxel::Vegitation::Plant::FERN, 1 } } },
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
	return Voxel::BiomeType::ERROR_BIOME;
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
	case Voxel::BiomeType::ERROR_BIOME:
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

void Voxel::Biome::initVegitation(std::mt19937& engine)
{
	/*
		Initialize vegitation.
		Each biome has different vegitations based on the type.
		Each vegitation has own weight value that used in random generation.
		Higher weight will have higher chance to spawn. 
		This applies to all type of vegitation.
	*/

	// init plants
	auto findPlant = Biome::biomePlantWeightMap.find(type);

	if (findPlant != Biome::biomePlantWeightMap.end())
	{
		auto& plantWeights = findPlant->second;

		const int size = static_cast<int>(plantWeights.size());

		if (size > 0)
		{
			PlantPair plantPair;

			if (size == 1)
			{
				plantPair = (findPlant->second).front();
			}
			else
			{
				std::vector<int> weights;
				for (auto& pair : findPlant->second)
				{
					weights.push_back(pair.weight);
				}

				std::discrete_distribution<> dist(weights.begin(), weights.end());

				int rand = dist(engine);
				if (rand >= size)
				{
					rand = size - 1;
				}

				plantPair = (findPlant->second).at(rand);
			}

			switch (plantPair.plant)
			{
			default:
				plants.push_back(plantPair);
				break;
			}
		}
	}

	// Init tree
	auto findTree = Biome::biomeTreeWeightMap.find(type);

	if (findTree != Biome::biomeTreeWeightMap.end())
	{
		auto& treeWeights = findTree->second;

		const int size = static_cast<int>(treeWeights.size());

		if (size > 0)
		{
			TreePair treePair;

			if (size == 1)
			{
				treePair = (findTree->second).front();
			}
			else
			{
				std::vector<int> weights;
				for (auto& pair : findTree->second)
				{
					weights.push_back(pair.weight);
				}

				std::discrete_distribution<> dist(weights.begin(), weights.end());

				int rand = dist(engine);
				if (rand >= size)
				{
					rand = size - 1;
				}

				treePair = (findTree->second).at(rand);
			}

			switch (treePair.tree)
			{
			case Voxel::Vegitation::Tree::OAK_BIRCH:
			{
				int mixRand = std::uniform_int_distribution<>(20, 80)(engine);
				trees.push_back(TreePair(Voxel::Vegitation::Tree::OAK, mixRand));
				trees.push_back(TreePair(Voxel::Vegitation::Tree::BIRCH, 100 - mixRand));
			}
				break;
			case Voxel::Vegitation::Tree::SPRUCE_PINE:
			{
				int mixRand = std::uniform_int_distribution<>(20, 80)(engine);
				trees.push_back(TreePair(Voxel::Vegitation::Tree::SPRUCE, mixRand));
				trees.push_back(TreePair(Voxel::Vegitation::Tree::PINE, 100 - mixRand));
			}
				break;
			default:
				trees.push_back(treePair);
				break;
			}
		}
	}
}

bool Voxel::Biome::hasTree()
{
	return trees.empty() == false;
}

bool Voxel::Biome::hasPlants()
{
	return plants.empty() == false;
}

int Voxel::Biome::getTreeSpawnRate()
{
	int chance = 0;

	switch (type)
	{
	case Voxel::BiomeType::TUNDRA:
		chance = 1;
		break;
	case Voxel::BiomeType::ICY_TAIGA:
		chance = 5;
		break;
	case Voxel::BiomeType::TAIGA:
		chance = 20;
		break;
	case Voxel::BiomeType::WOODS:
		chance = 10;
		break;
	case Voxel::BiomeType::SAVANNA:
		chance = 10;
		break;
	case Voxel::BiomeType::TAIGA_FOREST:
		chance = 70;
		break;
	case Voxel::BiomeType::FOREST:
		chance = 70;
		break;
	case Voxel::BiomeType::SEASONAL_FOREST:
		chance = 70;
		break;
	case Voxel::BiomeType::SWAMP:
		chance = 10;
		break;
	case Voxel::BiomeType::RAIN_FOREST:
		chance = 85;
		break;
	case Voxel::BiomeType::GRASS_DESERT:
	case Voxel::BiomeType::DESERT:
	case Voxel::BiomeType::OCEAN:
	case Voxel::BiomeType::NONE:
	case Voxel::BiomeType::ERROR_BIOME:
	default:
		break;
	}

	return chance;
}

Voxel::Vegitation::Tree Voxel::Biome::getRandomTreeType(std::mt19937 & engine)
{
	/*
	switch (std::uniform_int_distribution<>(0, 3)(engine))
	{
	case 1:
		return Voxel::Vegitation::Tree::BIRCH;
	case 2:
		return Voxel::Vegitation::Tree::SPRUCE;
	case 3:
		return Voxel::Vegitation::Tree::PINE;
	case 0:
	default:
		return Voxel::Vegitation::Tree::OAK;
	}
	*/

	if (hasTree())
	{
		int size = static_cast<int>(trees.size());

		if (size == 1)
		{
			return trees.front().tree;
		}
		else
		{
			std::vector<int> weights;
			for (auto& pair : trees)
			{
				weights.push_back(pair.weight);
			}

			std::discrete_distribution<> dist(weights.begin(), weights.end());
			
			int rand = dist(engine);

			if (rand >= size)
			{
				rand = size - 1;
			}

			return trees.at(rand).tree;
		}
	}
	else
	{
		return Voxel::Vegitation::Tree::NONE;
	}
}

void Voxel::Biome::print()
{
	std::cout << "Biome info\n";
	std::cout << "Type: " << Biome::biomeTypeToString(type) << "\tM: " << moisture << ", T: " << temperature << "\n";
	std::cout << "Vegitations\n";

	std::cout << "Plants: ";
	if (plants.empty())
	{
		std::cout << "None\n";
	}
	else
	{
		std::cout << "\n";
		for (auto& e : plants)
		{
			std::cout << Voxel::Vegitation::plantToString(e.plant) << " (" << e.weight << ")\n";
		}
	}

	std::cout << "Trees: ";
	if (trees.empty())
	{
		std::cout << "None\n";
	}
	else
	{
		std::cout << "\n";
		for (auto& e : trees)
		{
			std::cout << Voxel::Vegitation::treeToString(e.tree) << " (" << e.weight << ")\n";
		}
	}
	std::cout << "\n";
}
