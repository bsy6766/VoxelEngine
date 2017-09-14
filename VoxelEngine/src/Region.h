#ifndef REGION_H
#define REGION_H

#include <vector>
#include <string>
#include <Physics.h>
#include <Biome.h>

namespace Voxel
{
	// forward declaration
	namespace Voronoi
	{
		class Cell;
	}

	/**
	*	@class Region
	*	@brief A region defined by voronoi cell. Contains region data
	*/
	class Region
	{
	private:
		// Cell data
		Voronoi::Cell* cell;

		// The difficulty of region.
		int difficulty;

		// Name of region
		std::string name;

		// Main biome 
		float temperature;
		float moisture;

		// Terrain type
		Biome::Terrain terrainType;

		// AABB
		AABB boundingBox;

		void initBoundingBox();
	public:
		Region() = delete;
		Region(Voronoi::Cell* cell);
		~Region();

		// For debug. region random color
		glm::uvec3 randColor;

		void setDifficulty(const int difficulty);
		void setDifficulty(const float totalDistance, const int pathSize, const float maxTotalDistance, const int maxPathSize, const float minBound, const float maxBound);
		int getDifficulty();

		glm::vec2 getSitePosition();

		void setAsStartingRegion();

		// init temperature and moisture = biome
		void initTemperatureAndMoisture(const float minT, const float maxT, const float minM, const float maxM);
		// init terrain type
		void initTerrainType();
		Biome::Terrain getTerrainType();

		// Check if point is in cell's edges (polygon)
		bool isPointIsInRegion(const glm::vec2& point, Voronoi::Cell* cell);
		bool isPointIsInRegion(const glm::vec2& point);
		bool isPointIsInRegionNeighbor(const glm::vec2& point, unsigned int& neighborID);

		// Cehck if region's cell is valid
		bool isCellValid();

		// Get temperature and moisture
		float getTemperature();
		float getMoisture();

		// Get cell id
		unsigned int getID();
	};
}

#endif // !REGION_H
