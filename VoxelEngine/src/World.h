#ifndef WORLD_H
#define WORLD_H

#include <unordered_map>
#include <Voronoi.h>
#include <random>

namespace Voxel
{
	class Region;
	class Program;

	/**
	*	@class World
	*	
	*	Contains region data built by voronoi diagram
	*/
	class World
	{
	private:
		// Biome theme. All regions will have different temperature and moisture based on these values
		float minTemperature;
		float maxTemperature;
		float minMoisture;
		float maxMoisture;

		// Current region that player is at
		Region* currentRegion;

		// All regions
		std::unordered_map<unsigned int, Region*> regions;

		// Grid size
		int gridWidth;
		int gridLength;

		// number id
		unsigned int id;

		// World's unique seed
		std::string seed;

		// Voronoi diagram
		Voronoi::Diagram* vd;

		// render mode
		bool renderVoronoiMode;

		// Voronoi 
		void initVoronoi(std::mt19937& engine);
		void rebuildVoronoi(std::mt19937& engine);

		// Voronoi debug
		void initVoronoiDebug(); // Must call after building region

		// regions
		void initRegions(std::mt19937& engine);
		void rebuildRegions(std::mt19937& engine);
		void initRegionDifficulty();
		void initRegionBiomeAndTerrain();
		void initRegionBiome(std::mt19937& engine);
		void initRegionTerrain(std::mt19937& engine);

		// debug
		void printRegionBiomeAndTerrain();
	public:
		World();
		~World();

		// Initialize world
		void init(const int gridWidth, const int gridLength, const unsigned int id, const std::string& globalSeed);
		void rebuildWorldMap();

		// Getters
		Region* getCurrentRegion();
		Voronoi::Diagram* getVoronoi();
		 
		// Find region that contains the bounding box and return the region(Cell) id. If success, returns true. Else, false.
		/*
		bool findRegionWithAABB(const AABB& boundingBox, unsigned int& regionID) const;
		void findAllRegionsWithAABB(const AABB& boundingBox, std::vector<unsigned int>& regionIDs);
		bool findFirstRegionHasPoint(const glm::vec2& point, unsigned int& regionID);
		*/

		unsigned int findClosestRegionToPoint(const glm::vec2& point);
		unsigned int findRegionHasPoint(const glm::vec2& point);
		bool isPointInBoundary(const glm::vec2& point);

		// Get retion
		Region* getRegion(const unsigned int regionID);

		// get region difficulty
		int getRegionDifficulty(const unsigned int regionID);

		// Check if point is in region
		/*
		bool isPointInRegion(const unsigned int regionID, const glm::vec2& point);
		bool isPointInRegionNeighbor(const unsigned int regionID, const glm::vec2& point, unsigned int& neighborID);
		*/

		// set biome theme
		void setTemperature(float min, float max);
		void setMoisture(float min, float max);

		// Update world
		void update(const float delta);
		// Returns true if player moved to new region
		bool updatePlayerPos(const glm::vec3& playerPos);

		// mode setter
		void setRenderVoronoiMode(const bool mode);

		// Get world seed
		std::string getSeed();

		// Get grid size
		unsigned int getGridSize();

		// Renders visible chunk
		void render();
		void renderVoronoi(Program* program);
	};
}

#endif // !WORLD_H
