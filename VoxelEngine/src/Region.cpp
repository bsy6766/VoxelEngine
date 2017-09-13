#include "Region.h"

#include <Voronoi.h>
#include <iostream>
#include <Utility.h>

using namespace Voxel;

Voxel::Region::Region(Voronoi::Cell * cell)
	: cell(cell)
	, difficulty(-1)
{
	initBoundingBox();
}

Region::~Region()
{
}

void Voxel::Region::initBoundingBox()
{
	if (cell->isValid())
	{
		auto& edges = cell->getEdges();
		auto id = cell->getID();

		float minX = std::numeric_limits<float>::max();
		float maxX = std::numeric_limits<float>::min();

		float shift = maxX * 0.5f;

		float minZ = minX;
		float maxZ = maxX;

		for (auto e : edges)
		{
			glm::vec2 e0 = e->getStart() + shift;

			if (e0.x < minX)
			{
				minX = e0.x;
			}

			if (e0.x > maxX)
			{
				maxX = e0.x;
			}

			if (e0.y < minZ)
			{
				minZ = e0.y;
			}

			if (e0.y > maxZ)
			{
				maxZ = e0.y;
			}
		}

		maxX -= shift;
		minX -= shift;

		maxZ -= shift;
		minZ -= shift;

		float sizeX = maxX - minX;
		float sizeZ = maxZ - minZ;

		float centerX = minX + (sizeX * 0.5f);
		float centerZ = minZ + (sizeZ * 0.5f);

		boundingBox.center = glm::vec3(centerX, 0, centerZ);
		boundingBox.size = glm::vec3(sizeX, 0, sizeZ);

		//std::cout << "Creating region #" << cell->getID() << " bb. center: " << Utility::Log::vec3ToStr(boundingBox.center) << ", size: " << Utility::Log::vec3ToStr(boundingBox.size) << std::endl;
	}
}

void Voxel::Region::setDifficulty(const int difficulty)
{
	if (difficulty < 0)
	{
		return;
	}

	this->difficulty = difficulty;
}

void Voxel::Region::setDifficulty(const float totalDistance, const int pathSize, const float maxTotalDistance, const int maxPathSize, const float minBound, const float maxBound)
{
	// 50% from total distance and 50% from path size.
	// 
	
	float psf = static_cast<float>(pathSize);
	float maxPsf = static_cast<float>(maxPathSize);

	float pathSizeFactor = psf / maxPsf;

	float totalDistanceFactor = totalDistance / maxTotalDistance;

	const float pathSizeRatio = 0.65f;
	const float totalDistanceRatio = 1.0f - pathSizeRatio;

	float value = ((pathSizeFactor * pathSizeRatio) + (totalDistanceFactor * totalDistanceRatio)) * 5.0f;
	//std::cout << "Setting difficulty in Region #" << cell->getID() << " with td: " << totalDistance << ", maxTd: " << maxTotalDistance << ", ps: " << pathSize << ", maxPs: " << maxPathSize << std::endl;
	//std::cout << "Value = " << value << std::endl;
	
	difficulty = static_cast<int>(glm::round(value));
	//std::cout << "difficulty = " << difficulty << std::endl;
}

int Voxel::Region::getDifficulty()
{
	return difficulty;
}

void Voxel::Region::setAsStartingRegion()
{
	difficulty = 0;
}
