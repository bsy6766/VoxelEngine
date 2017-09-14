#include "Region.h"

#include <Voronoi.h>
#include <iostream>
#include <Utility.h>
#include <Color.h>
#include <HeightMap.h>

using namespace Voxel;

Voxel::Region::Region(Voronoi::Cell * cell)
	: cell(cell)
	, difficulty(-1)
	, randColor(Color::getRandomColor255())
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

glm::vec2 Voxel::Region::getSitePosition()
{
	return cell->getSitePosition();
}

void Voxel::Region::setAsStartingRegion()
{
	difficulty = 0;
}

void Voxel::Region::initBiomeType(const float minT, const float maxT, const float minM, const float maxM)
{
	auto sitePos = cell->getSitePosition();
	auto t = HeightMap::getTemperatureNoise2D(sitePos.x, sitePos.y);

	float shiftedMinT = minT - minT;
	float shiftedMaxT = maxT - minT;

	float newT = shiftedMaxT * t / 2.0f;

	newT += minT;

	auto m = HeightMap::getMoistureNosie2D(sitePos.x, sitePos.y);

	float shiftedMinM = minM - minM;
	float shiftedMaxM = maxM - minM;

	float newM = shiftedMaxM * m / 2.0f;

	newM += minM;

	biomeType.setType(newT, newM);

	if (cell->isValid())
	{
		std::cout << "Setting region #" << cell->getID() << " t: " << newT << ", m: " << newM << std::endl;
		std::cout << "Biome: " << Biome::biomeTypeToString(biomeType.getType()) << std::endl;
	}
}

Biome Voxel::Region::getBiomeType()
{
	return biomeType;
}

void Voxel::Region::initTerrainType()
{
	terrainType.setType(Terrain::Type::PLAIN);
	terrainType.setModifier(Terrain::Modifier::NONE);
}

void Voxel::Region::initTerrainType(Terrain::Type type)
{
	terrainType.setType(type);
	terrainType.setModifier(Terrain::Modifier::MEGA);
}

Terrain Voxel::Region::getTerrainType()
{
	return terrainType;
}

bool Voxel::Region::isPointIsInRegion(const glm::vec2 & point, Voronoi::Cell * cell)
{
	auto& edges = cell->getEdges();

	std::vector<glm::vec2> vertices;
	for (auto e : edges)
	{
		vertices.push_back(e->getStart());
	}

	int nvert = static_cast<int>(vertices.size());

	int i, j, c = 0;

	for (i = 0, j = nvert - 1; i < nvert; j = i++)
	{
		if (((vertices.at(i).y > point.y) != (vertices.at(j).y > point.y)) && (point.x < (vertices.at(j).x - vertices.at(i).x) * (point.y - vertices.at(i).y) / (vertices.at(j).y - vertices.at(i).y) + vertices.at(i).x))
			c = !c;
	}

	return c;
}

bool Voxel::Region::isPointIsInRegion(const glm::vec2 & point)
{
	return isPointIsInRegion(point, this->cell);
}

bool Voxel::Region::isPointIsInRegionNeighbor(const glm::vec2 & point, unsigned int& neighborID)
{
	auto& nc = cell->getNeighbors();
	for (auto neighbor : nc)
	{
		if (isPointIsInRegion(point, neighbor))
		{
			neighborID = neighbor->getID();
			return true;
		}
		else
		{
			continue;
		}
	}

	return false;
}

bool Voxel::Region::isCellValid()
{
	return cell->isValid();
}

unsigned int Voxel::Region::getID()
{
	return cell->getID();
}
