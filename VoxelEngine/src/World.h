#ifndef WORLD_H
#define WORLD_H

#include <unordered_map>
#include <Voronoi.h>
#include <Physics.h>

namespace Voxel
{
	class Region;

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

		// Voronoi diagram
		Voronoi::Diagram* vd;

		// Voronoi 
		void initVoronoi();
		void rebuildVoronoi();

		// Voronoi debug
		void initVoronoiDebug(); // Must call after building region

		// regions
		void initRegions();
		void rebuildRegions();
		void initRegionDifficulty();
		void initRegionBiome();
		void initRegionTerrain();
	public:
		World();
		~World();

		// Initialize world
		void init(const int gridWidth, const int gridLength);
		void rebuildWorldMap();

		// Getters
		Region* getCurrentRegion();
		Voronoi::Diagram* getVoronoi();

		// Find region that contains the bounding box and return the region(Cell) id. If success, returns true. Else, false.
		bool findRegionWithAABB(const AABB& boundingBox, unsigned int& regionID) const;
		void findAllRegionsWithAABB(const AABB& boundingBox, std::vector<unsigned int>& regionIDs);
		bool findFirstRegionHasPoint(const glm::vec2& point, unsigned int& regionID);
		unsigned int findClosestRegionToPoint(const glm::vec2& point);

		// Get retion
		Region* getRegion(const unsigned int regionID);

		// get region difficulty
		int getRegionDifficulty(const unsigned int regionID);

		// Check if point is in region
		bool isPointInRegion(const unsigned int regionID, const glm::vec2& point);
		bool isPointInRegionNeighbor(const unsigned int regionID, const glm::vec2& point, unsigned int& neighborID);

		// set biome theme
		void setTemperature(float min, float max);
		void setMoisture(float min, float max);

		// Update world
		void update(const float delta);
		// Returns true if player moved to new region
		bool updatePlayerPos(const glm::vec3& playerPos);

		// Renders visible chunk
		void render();
		void renderVoronoi();
	};
}

#endif // !WORLD_H
