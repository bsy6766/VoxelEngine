#include "World.h"

#include <Region.h>
#include <iostream>
#include <Utility.h>

using namespace Voxel;

World::World()
	: currentRegion(nullptr)
	, vd(nullptr)
	, gridWidth(0)
	, gridLength(0)
	, minTemperature(0)
	, maxTemperature(0)
	, minMoisture(0)
	, maxMoisture(0)
{
}

World::~World()
{
	for (auto& e : regions)
	{
		if (e.second)
		{
			delete e.second;
		}
	}

	regions.clear();

	if (vd)
	{
		delete vd;
	}
}


void Voxel::World::init(const int gridWidth, const int gridLength)
{
	this->gridWidth = gridWidth;
	this->gridLength = gridLength;

	initVoronoi();
	initRegions();
	initRegionDifficulty();
	initRegionBiome();
	initRegionTerrain();
	initVoronoiDebug();
}

void Voxel::World::rebuildWorldMap()
{
	rebuildVoronoi();
	rebuildRegions();
	initVoronoiDebug();
}

Region * Voxel::World::getCurrentRegion()
{
	return currentRegion;
}

Voronoi::Diagram * Voxel::World::getVoronoi()
{
	return vd;
}

bool Voxel::World::findRegionWithAABB(const AABB & boundingBox, unsigned int & regionID) const
{
	std::vector<glm::vec2> vertices;

	auto min = boundingBox.getMin();
	auto max = boundingBox.getMax();

	vertices.push_back(glm::vec2(min.x, min.z));
	vertices.push_back(glm::vec2(min.x, max.z));
	vertices.push_back(glm::vec2(max.x, min.z));
	vertices.push_back(glm::vec2(max.x, max.z));
	

	for (auto& e : regions)
	{
		auto region = e.second;

		if (region->isCellValid())
		{
			bool result = true;

			for (auto& v : vertices)
			{
				if (!region->isPointIsInRegion(v))
				{
					result = false;
					break;
				}
			}

			if (result)
			{
				regionID = e.first;
				return true;
			}
			else
			{
				continue;
			}
		}
	}

	return false;
}

void Voxel::World::findAllRegionsWithAABB(const AABB & boundingBox, std::vector<unsigned int>& regionIDs)
{
	std::vector<glm::vec2> vertices;

	auto min = boundingBox.getMin();
	auto max = boundingBox.getMax();

	vertices.push_back(glm::vec2(min.x, min.z));
	vertices.push_back(glm::vec2(min.x, max.z));
	vertices.push_back(glm::vec2(max.x, min.z));
	vertices.push_back(glm::vec2(max.x, max.z));


	for (auto& e : regions)
	{
		auto region = e.second;

		// Only check valid cells because invalid cells doesn't have edge data
		if (region->isCellValid())
		{
			for (auto& v : vertices)
			{
				if (region->isPointIsInRegion(v))
				{
					regionIDs.push_back(e.first);
					break;
				}
			}
		}
	}
}

bool Voxel::World::findFirstRegionHasPoint(const glm::vec2 & point, unsigned int & regionID)
{
	for (auto& e : regions)
	{
		auto region = e.second;

		if (region->isCellValid())
		{
			if (region->isPointIsInRegion(point))
			{
				regionID = e.first;
				return true;
			}
		}
	}

	return false;
}

unsigned int Voxel::World::findClosestRegionToPoint(const glm::vec2 & point)
{
	if (vd->isPointInBoundary(point))
	{
		float dist = std::numeric_limits<float>::max();
		unsigned int regionID = -1;

		for (auto& e : regions)
		{
			auto region = e.second;

			auto pos = region->getSitePosition();

			float d = glm::abs(glm::distance(point, pos));

			if (d < dist)
			{
				dist = d;
				regionID = e.first;
			}
		}

		return regionID;
	}
	else
	{
		return -1;
	}
}

Region * Voxel::World::getRegion(const unsigned int regionID)
{
	auto find_it = regions.find(regionID);
	if (find_it == regions.end())
	{
		return nullptr;
	}
	else
	{
		return find_it->second;
	}
}

int Voxel::World::getRegionDifficulty(const unsigned int regionID)
{
	auto find_it = regions.find(regionID);
	if (find_it == regions.end())
	{
		return -1;
	}
	else
	{
		return (find_it->second)->getDifficulty();
	}
}

bool Voxel::World::isPointInRegion(const unsigned int regionID, const glm::vec2& point)
{
	auto region = getRegion(regionID);
	if (region)
	{
		return region->isPointIsInRegion(point);
	}
	else
	{
		return false;
	}
}

bool Voxel::World::isPointInRegionNeighbor(const unsigned int regionID, const glm::vec2 & point, unsigned int& neighborID)
{
	auto region = getRegion(regionID);
	if (region)
	{
		return region->isPointIsInRegionNeighbor(point, neighborID);
	}
	else
	{
		return false;
	}
}

void Voxel::World::setTemperature(float min, float max)
{
	min = glm::clamp(min, 0.0f, 2.0f);
	max = glm::clamp(max, 0.0f, 2.0f);

	minTemperature = min;
	maxTemperature = max;
}

void Voxel::World::setMoisture(float min, float max)
{
	min = glm::clamp(min, 0.0f, 2.0f);
	max = glm::clamp(max, 0.0f, 2.0f);

	minMoisture = min;
	maxMoisture = max;
}

void Voxel::World::update(const float delta)
{

}

bool Voxel::World::updatePlayerPos(const glm::vec3 & playerPos)
{
	// Check if player moved to new region
	auto pos = glm::vec2(playerPos.x, playerPos.z);
	if (currentRegion->isPointIsInRegion(pos))
	{
		return false;
	}
	else
	{
		// Player is not in current region. Check neighbor
		unsigned int regionID = -1;
		if (currentRegion->isPointIsInRegionNeighbor(pos, regionID))
		{
			std::cout << "Player moved to neighbor region #" << regionID << " from " << currentRegion->getID() << std::endl;
			Region* r = getRegion(regionID);
			if (r == nullptr)
			{
				std::cout << "Region is nullptr" << std::endl;
				return false;
			}
			else
			{
				currentRegion = r;
				return true;
			}
		}
		else
		{
			// Player is not in neighbor regions. Find
			for (auto& e : regions)
			{
				auto region = e.second;

				if (region->isPointIsInRegion(pos))
				{
					std::cout << "Player moved to region #" << e.first << " from " << currentRegion->getID() << std::endl;
					currentRegion = region;
					return true;
				}
			}

			std::cout << "Can not find region at player position " << Utility::Log::vec3ToStr(playerPos) << std::endl;
			return false;
		}
	}

	return false;
}

void Voxel::World::initVoronoi()
{
	// Generate random grid
	std::vector<std::vector<int>> grid;

	const int EMPTY = 0;
	const int MARKED = 1;
	const int OMITTED = 2;
	const int BORDER = 3;

	// Fill with M (1)
	for (int i = 0; i < gridWidth; ++i)
	{
		grid.push_back(std::vector<int>());
		for (int j = 0; j < gridLength; ++j)
		{
			grid.back().push_back(MARKED);
		}
	}

	// set edge of grid as border (2)
	for (auto& i : grid.front())
	{
		i = BORDER;
	}

	for (auto& i : grid.back())
	{
		i = BORDER;
	}

	for (int i = 0; i < gridWidth; ++i)
	{
		grid.at(i).front() = BORDER;
		grid.at(i).back() = BORDER;
	}

	// print grid
	for (auto i : grid)
	{
		for (auto j : i)
		{
			std::string str;
			switch (j)
			{
			case EMPTY:
				str = "0";
				break;
			case MARKED:
				str = "M";
				break;
			case OMITTED:
				str = "X";
				break;
			case BORDER:
				str = "B";
				break;
			default:
				continue;
				break;
			}
			std::cout << str << " ";
		}
		std::cout << std::endl;
	}

	//based on grid, generate random points

	int xPos = grid.size() / 2;
	int zPos = grid.front().size() / 2;

	const int interval = 1000;
	const int intervalHalf = interval / 2;

	glm::ivec2 pos = (glm::ivec2(xPos, zPos) * interval) - intervalHalf;

	// For marked 
	const int pad = interval / 10;
	const int randMax = (interval - (pad * 2)) / 2;
	const int randMin = randMax * -1;

	// For omiited cell. More controlled
	const int omittedPad = pad * 3;
	const int omittedRandMax = (interval - (omittedPad * 2)) / 2;
	const int omittedRandMin = omittedRandMax * -1;

	std::vector<Voronoi::Site> points;

	for (auto x : grid)
	{
		for (auto z : x)
		{
			glm::vec2 randPos;
			Voronoi::Site::Type type;
			switch (z)
			{
			case MARKED:
			{
				int randX = Utility::Random::randomInt(randMin, randMax);
				int randZ = Utility::Random::randomInt(randMin, randMax);

				randPos = glm::ivec2(randX, randZ);

				type = Voronoi::Site::Type::MARKED;
				//std::cout << "Marked" << std::endl;
			}
			break;
			case OMITTED:
			{
				//int randX = Utility::Random::randomInt(omittedRandMin, omittedRandMax);
				//int randZ = Utility::Random::randomInt(omittedRandMin, omittedRandMax);

				//randPos = glm::ivec2(randX, randZ);

				int randX = Utility::Random::randomInt(randMin, randMax);
				int randZ = Utility::Random::randomInt(randMin, randMax);

				randPos = glm::ivec2(randX, randZ);

				type = Voronoi::Site::Type::OMITTED;
				//std::cout << "Omitted" << std::endl;
			}
			break;
			case BORDER:
			{
				randPos = glm::vec2(0);
				type = Voronoi::Site::Type::BORDER;
				//std::cout << "Border" << std::endl;
			}
			break;
			case EMPTY:
			default:
				type = Voronoi::Site::Type::NONE;
				continue;
				break;
			}

			randPos += pos;
			//std::cout << "RandPoint = " << Utility::Log::vec2ToStr(randPos) << std::endl;

			points.push_back(Voronoi::Site(randPos, type));

			pos.y/*z*/ -= interval;
		}

		pos.y/*z*/ = (zPos * interval) - intervalHalf;
		pos.x -= interval;
	}

	vd = new Voronoi::Diagram();
	vd->construct(points);

	const float minBound = static_cast<float>(xPos * interval * -1);
	const float maxBound = static_cast<float>(xPos * interval);

	vd->buildCells(minBound, maxBound);
	vd->buildGraph(gridWidth, gridLength);
	vd->randomizeCells(gridWidth, gridLength);
}

void Voxel::World::initVoronoiDebug()
{
	if (vd)
	{
		vd->initDebugDiagram();
	}
}

void Voxel::World::initRegions()
{
	auto& cells = vd->getCells();

	// Iterate through cells and create region.
	unsigned int startingRegionID = -1;

	int xMin = (gridWidth / 2) - (gridWidth / 10);
	int xMax = (gridWidth / 2);
	int zMin = (gridLength / 2) - (gridLength / 10);
	int zMax = (gridLength / 2);

	std::vector<unsigned int> candidates;

	for (int x = xMin; x <= xMax; x++)
	{
		for (int z = zMin; z <= zMax; z++)
		{
			candidates.push_back((x * gridWidth) + z);
		}
	}

	int randIndex = Utility::Random::randomInt(0, candidates.size() - 1);

	startingRegionID = candidates.at(randIndex);

	for (auto e : cells)
	{
		auto cell = e.second;

		auto cellID = cell->getID();
		auto cellSitePos = cell->getSitePosition();

		Region* newRegion = new Region(cell);
		cell->setRegion(newRegion);

		regions.emplace(cellID, newRegion);
	}

	// Pick random starting point. 

	auto find_it = regions.find(startingRegionID);
	if (find_it != regions.end())
	{
		currentRegion = find_it->second;
		currentRegion->setAsStartingRegion();
	}
	else
	{
		throw std::runtime_error("Failed to create starting zome");
	}
}

void Voxel::World::initRegionDifficulty()
{
	// from staring region, calculate distance to all region from starting region
	std::vector<float> dist;
	std::vector<unsigned int> prevPath;

	unsigned int startingRegionID = currentRegion->getID();

	vd->findShortestPathFromSrc(startingRegionID, dist, prevPath);

	// Based on dist and prevPath, calculate difficulty.
	// Difficulty is defined with number of path point and total distance from starting region
	// Therefore, furthur player explore, harder the game is.

	float maxTotalDist = 0;
	int maxPathSize = 0;

	struct pair
	{
		float dist;
		int size;
	};

	auto& cells = vd->getCells();
	std::vector<pair> pairs(cells.size(), { 0, 0 });

	for (auto e : cells)
	{
		auto cell = e.second;

		if (cell->isValid())
		{
			auto cellID = cell->getID();
			float d = dist.at(cellID);
			if (d > maxTotalDist)
			{
				maxTotalDist = d;
			}

			pairs.at(cellID).dist = d;

			// Build path. 
			int pathSize = 0;
			unsigned int curID = cellID;
			while (curID != startingRegionID)
			{
				curID = prevPath.at(curID);
				pathSize++;
			}

			if (pathSize > maxPathSize)
			{
				maxPathSize = pathSize;
			}

			pairs.at(cellID).size = pathSize;
		}
	}

	for (auto e : cells)
	{
		auto cell = e.second;

		if (cell->isValid())
		{
			auto cellID = cell->getID();
			auto region = cell->getRegion();

			region->setDifficulty(pairs.at(cellID).dist, pairs.at(cellID).size, maxTotalDist, maxPathSize, vd->getMinBound(), vd->getMaxBound());
		}
	}
}

void Voxel::World::initRegionBiome()
{
	for (auto& r : regions)
	{
		auto region = r.second;

		region->initBiomeType(minTemperature, maxTemperature, minMoisture, maxMoisture);
	}
}

void Voxel::World::initRegionTerrain()
{
	for (auto& r : regions)
	{
		auto region = r.second;

		if (region->isCellValid())
		{
			region->initTerrainType();
		}
		else
		{
			region->initTerrainType(Terrain::Type::MOUNTAINS);
		}
	}
}

void Voxel::World::rebuildVoronoi()
{
	if (vd)
	{
		delete vd;
	}

	initVoronoi();
}

void Voxel::World::rebuildRegions()
{
	currentRegion = nullptr;
	for (auto e : regions)
	{
		if ((e.second) != nullptr)
		{
			delete e.second;
		}
	}

	regions.clear();

	initRegions();
}

void Voxel::World::render()
{
}

void Voxel::World::renderVoronoi()
{
	vd->render(true, false, true, false);
}