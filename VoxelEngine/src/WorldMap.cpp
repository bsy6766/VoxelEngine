#include "WorldMap.h"

#include <World.h>
#include <Region.h>
#include <UI.h>

using namespace Voxel;

Voxel::WorldMap::WorldMap()
	: vao(0)
	, uiCanvas(nullptr)
	, compass(nullptr)
	, cameraIcon(nullptr)
	, worldName(nullptr)
{}

Voxel::WorldMap::~WorldMap()
{
	releaseMesh();
}

void Voxel::WorldMap::init(World * world)
{
	// Initailize world map

	// Initialize background

	// Initialize mesh for each region
	unsigned int gridSize = world->getGridSize();

	for (unsigned int i = 0; i < gridSize; i++)
	{
		Region* region = world->getRegion(i);
		if (region)
		{
			if (region->isCellValid())
			{
				std::vector<float> edgePoints;
				region->getVoronoiEdgePoints(edgePoints);
			}
		}
		// Else, continue
	}
}

void Voxel::WorldMap::releaseMesh()
{
}