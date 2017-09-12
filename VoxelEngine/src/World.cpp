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
	initVoronoiDebug();
}

void Voxel::World::rebuildWorldMap()
{
	rebuildVoronoi();
	rebuildRegions();
	initVoronoiDebug();
}

void Voxel::World::update(const float delta)
{
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
	//float minDistFromCenter = std::numeric_limits<float>::max();
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

		/*
		auto d = glm::abs(glm::distance(glm::vec2(0, 0), cellSitePos));
		if (d < minDistFromCenter)
		{
		minDistFromCenter = d;
		startingRegionID = cellID;
		}
		*/

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

	// from staring region, calculate distance to all region from starting region
	std::vector<float> dist;
	std::vector<unsigned int> prevPath;
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