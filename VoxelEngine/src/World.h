#ifndef WORLD_H
#define WORLD_H

#include <unordered_map>
#include <Voronoi.h>

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

		// Update world
		void update(const float delta);

		// Renders visible chunk
		void render();
		void renderVoronoi();
	};
}

#endif // !WORLD_H
