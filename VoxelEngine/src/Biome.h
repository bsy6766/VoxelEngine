#ifndef BIOME_H
#define BIOME_H

#include <string>
#include <unordered_map>
#include <BiomeType.h>
#include <TreeBuilder.h>

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
	private:
		struct TreePair
		{
		public:
			Voxel::Vegitation::Tree tree;
			int weight;

			TreePair() : tree(Voxel::Vegitation::Tree::NONE), weight(0) {}
			TreePair(const Voxel::Vegitation::Tree tree, const int weight) : tree(tree), weight(weight) {}
		};

		struct PlantPair
		{
		public:
			Voxel::Vegitation::Plant plant;
			int weight;

			PlantPair() : plant(Voxel::Vegitation::Plant::NONE), weight(0) {}
			PlantPair(const Voxel::Vegitation::Plant plant, const int weight) : plant(plant), weight(weight) {}
		};

	public:
		// Store types of vegitation that can grow in biome
		static const std::unordered_map<BiomeType, std::vector<TreePair>> biomeTreeWeightMap;
		static const std::unordered_map<BiomeType, std::vector<PlantPair>> biomePlantWeightMap;
		
		float temperature;
		float moisture;

		Voxel::BiomeType type;
		
		// Possible tree and the weight of tree. Higher the weight, higher the chance to spawn
		std::vector<TreePair> trees;
		// Possible plant and the weight of plant.
		std::vector<PlantPair> plants;
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

		void initVegitation(std::mt19937& engine);
		
		// Check if this biome has flower
		bool hasFlower();
		// Check if this biome has tree
		bool hasTree();
		// Check if this biome has grass
		bool hasPlants();

		// Get tree spawn rate (0 ~ 100)
		int getTreeSpawnRate();
		// Get random tree type
		Voxel::Vegitation::Tree getRandomTreeType(std::mt19937& engine);
			

		// print biome data
		void print();
	};
}

#endif