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
		// Biome data
		float temperature;
		float moisture;

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
		void findFirstRegionHasPoint(const glm::vec2& point, unsigned int& regionID);
		unsigned int findClosestRegionToPoint(const glm::vec2& point);

		// Get retion
		Region* getRegion(const unsigned int regionID);

		// get region difficulty
		int getRegionDifficulty(const unsigned int regionID);

		// Check if point is in region
		bool isPointInRegion(const unsigned int regionID, const glm::vec2& point);
		bool isPointInRegionNeighbor(const unsigned int regionID, const glm::vec2& point, unsigned int& neighborID);

		// Update world
		void update(const float delta);

		// Renders visible chunk
		void render();
		void renderVoronoi();
	};
}

#endif // !WORLD_H
