#ifndef REGION_H
#define REGION_H

#include <vector>
#include <string>
#include <Biome.h>
#include <Physics.h>

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
		Biome::Type biomeType;

		// AABB
		AABB boundingBox;

		void initBoundingBox();
	public:
		Region() = delete;
		Region(Voronoi::Cell* cell);
		~Region();

		void setDifficulty(const int difficulty);
		void setDifficulty(const float totalDistance, const int pathSize, const float maxTotalDistance, const int maxPathSize, const float minBound, const float maxBound);
		int getDifficulty();

		void setAsStartingRegion();
	};
}

#endif // !REGION_H
