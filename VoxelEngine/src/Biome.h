#ifndef BIOME_H
#define BIOME_H

#include <string>
#include <unordered_map>
#include <BiomeType.h>

namespace Voxel
{
	/**
	*	@class Biome
	*	@brief A class that determines the biome based on temperature and moisture
	*	
	*	Biome determines the climate and enviornment, not terrain.
	*
	*	Referense: Look that the image 'BiomesGraph.png' in Researches folder
	*/
	class Biome
	{
	public:
		// Store types of vegitation that can grow in biome
		static const std::unordered_map<BiomeType, std::vector<Voxel::Vegitation::Tree>> biomeTreeMap;
		// Store types of living entity that can live in biome
		// Store types of structure that can be spawned in biome
		
		float temperature;
		float moisture;

		Voxel::BiomeType type;
		
		// Possible tree and the weight of tree. Higher the weight, higher the chance to spawn
		std::unordered_map<Voxel::Vegitation::Tree, int> trees;
		// Possible plant and the weight of plant.
		std::unordered_map<Voxel::Vegitation::Plant, int> plants;
		// Possible flowers and the weight of flowers.
		std::unordered_map<Voxel::Vegitation::Flower, int> flowers;
	public:
		Biome();
		~Biome() = default;

		static Voxel::BiomeType getBiomeType(float temperature, float moisture, float elevation);

		static std::string biomeTypeToString(Voxel::BiomeType type);
		static std::string biomeTypeToString(Biome biomeType);

		void setType(const float temperature, const float moisture);
		Voxel::BiomeType getType();

		float getTemperature();
		float getMoisture();

		// Check if this biome has flower
		bool hasFlower();
		// Check if this biome has tree
		bool hasTree();
		// Check if this biome has grass
		bool hasGrass();
	};
}

#endif