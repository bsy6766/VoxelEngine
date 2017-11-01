#include "WorldMap.h"

#include <World.h>
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

	// Initialize 
}

void Voxel::WorldMap::releaseMesh()
{
}