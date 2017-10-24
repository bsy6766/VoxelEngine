#include "Chunk.h"
#include <ChunkSection.h>
#include <iostream>
#include <ChunkMesh.h>
#include <ChunkUtil.h>
#include <Utility.h>
#include <HeightMap.h>
#include <ProgramManager.h>
#include <Program.h>

using namespace Voxel;

Chunk::Chunk()
	: position(0)
	, worldPosition(0.0f)
	, chunkMesh(nullptr)
	, active(false)
	, visible(false)
	, timestamp(0)
	, smoothed(false)
{
	chunkMesh = new ChunkMesh();
	generated.store(false);
}

bool Voxel::Chunk::canGenerate()
{
	if (generated.load() == true)
	{
		std::cout << "Chunk trying to generate again\n";
		return false;
	}

	/*
	if (chunkSections.empty() == false)
	{
		std::cout << "Chunk already has chunksections\n";
		return false;
	}
	*/

	return true;
}

Chunk::~Chunk()
{
	for (auto chunkSection : chunkSections)
	{
		if (chunkSection)
		{
			delete chunkSection;
		}
	}

	chunkSections.clear();

	if (chunkMesh)
	{
		delete chunkMesh;
	}

	//std::cout << "[Chunk] Deleting chunk at (" << position.x << ", " << position.z << ")...\n";
}

Chunk* Chunk::create(const int x, const int z)
{
	Chunk* newChunk = new Chunk();
	if (newChunk->init(x, z))
	{
		if (newChunk->generateSingleSection())
		{
			//std::cout << "[Chunk] Done.\n\n";
			//std::cout << "[Chunk] Creating new chunk at (" << x << ", " << z << ")...\n";
			return newChunk;
		}
	}
	
	// Failed
	delete newChunk;
	return nullptr;
}

Chunk * Voxel::Chunk::createEmpty(const int x, const int z)
{
	Chunk* newChunk = new Chunk();
	if (newChunk->init(x, z))
	{
		//std::cout << "[Chunk] Done.\n\n";
		//std::cout << "[Chunk] Creating new empty chunk at (" << x << ", " << z << ")...\n";
		return newChunk;
	}

	// Failed
	delete newChunk;
	return nullptr;
}

void Voxel::Chunk::unload()
{
	if (chunkMesh)
	{
		delete chunkMesh;
		chunkMesh = nullptr;
	}
}

bool Chunk::init(const int x, const int z)
{
	//auto start = Utility::Time::now();
	position = glm::ivec3(x, 0, z);

	// calculate position. Size of each block is 1.0f. There are total 16 x 16 (256) blocks in XZ space.
	worldPosition.x = Constant::CHUNK_BORDER_SIZE * (static_cast<float>(x) + 0.5f);
	worldPosition.y = 0;
	worldPosition.z = Constant::CHUNK_BORDER_SIZE * (static_cast<float>(z) + 0.5f);

	//std::cout << "[Chunk] position: (" << x << ", 0, " << z << "), world position: (" << worldPosition.x << ", " << worldPosition.y << ", " << worldPosition.z << ")\n";

	// init border. worldPosition works as center position of border
	float borderDistance = (Constant::CHUNK_BORDER_SIZE * 0.5f);
	
	auto min = glm::vec3(worldPosition.x - borderDistance, 0, worldPosition.z - borderDistance);
	auto max = glm::vec3(worldPosition.x + borderDistance, Constant::TOTAL_CHUNK_SECTION_PER_CHUNK * Constant::CHUNK_SECTION_HEIGHT, worldPosition.z + borderDistance);

	auto size = max - min;

	boundingBox.center = min + (size * 0.5f);
	boundingBox.size = size;

	chunkSections = std::vector<ChunkSection*>(Constant::TOTAL_CHUNK_SECTION_PER_CHUNK, nullptr);

	//std::cout << "[Chunk] BorderXZ: min(" << border.min.x << ", " << border.min.z << "), max(" << border.max.x << ", " << border.max.z << ")\n";

	//auto end = Utility::Time::now();
	//std::cout << "Chunk generation elapsed time: " << Utility::Time::toMilliSecondString(start, end) << std::endl;
	return true;
}

bool Voxel::Chunk::generate()
{
	assert(canGenerate());

	// For terrain color variation. Repeating all same color for large amount of area gives bad visual. so use this noise to smoothly mix color
	std::vector<std::vector<float>> colorMap;
	HeightMap::getHeightMapForColor(position, colorMap);

	// Add grass and flower. Grass and flowers are represented as single block, even if they are higher than 1 block (never gets bigger than 1 block wide & len). 

	// Grass doesn't need 

	// Flower noise. Flowers are spawned based on this noise. Spawns flower if value is higher than specific value. If region has multiple type of flower, 
	std::vector<std::vector<float>> flowerMap;



	for (auto chunkSection : chunkSections)
	{
		if (chunkSection != nullptr)
		{
			chunkSection->init(heightMap, colorMap);
		}
	}

	//regionMap.clear();
	//heightMap.clear();

	generated.store(true);

	return true;
}

bool Voxel::Chunk::generateSingleSection()
{
	assert(canGenerate());

	for (int i = 0; i < Constant::TOTAL_CHUNK_SECTION_PER_CHUNK; i++)
	{
		// Temp. All blocks above chunk section y 3 will be air.
		if (i > 1)
		{
			//std::cout << "+ chunksection: " << i << std::endl;
			//chunkSections.push_back(nullptr);
			continue;
		}
		else
		{
			//std::cout << "++ chunksection: " << i << std::endl;
			auto newChucnkSection = ChunkSection::createWithFill(position.x, i, position.z, worldPosition);
			if (newChucnkSection)
			{
				//chunkSections.push_back(newChucnkSection);
				chunkSections.at(i) = newChucnkSection;
			}
			else
			{
				throw std::runtime_error("Failed to create chunk section at (" + std::to_string(position.x) + ", " + std::to_string(i) + ", " + std::to_string(position.z) + ")");
			}
		}
	}

	generated.store(true);
	
	return true;
}

bool Voxel::Chunk::generateWithBiomeTest()
{
	assert(canGenerate());

	//std::cout << "[Chunk] Creating " << Constant::TOTAL_CHUNK_SECTION_PER_CHUNK << " ChunkSections...\n";
	std::vector<std::vector<float>> elevationMap;
	std::vector<std::vector<float>> temperatureMap;
	std::vector<std::vector<float>> moistureMap;

	int maxY = 0;
	int minY = 10000;

	int xStart = position.x;
	int zStart = position.z;
	int xEnd = xStart + Constant::CHUNK_SECTION_WIDTH;
	int zEnd = zStart + Constant::CHUNK_SECTION_LENGTH;

	float nx = static_cast<float>(position.x);
	float nz = static_cast<float>(position.z);
	const float step = 1.0f / Constant::CHUNK_BORDER_SIZE;

	for (int x = xStart; x < xEnd; x++)
	{
		elevationMap.push_back(std::vector<float>());
		temperatureMap.push_back(std::vector<float>());
		moistureMap.push_back(std::vector<float>());

		for (int z = zStart; z < zEnd; z++)
		{
			// Get height 
			float val = HeightMap::getNoise2D(nx, nz, HeightMap::PRESET::PLAIN);

			// The lowest block level is 30. The range of terrain in y axis is 120 (30 
			int y = (static_cast<int>(val * 60.0f) + 30);

			elevationMap.back().push_back(val);

			if (y > maxY)
			{
				maxY = y;
			}
			else if (y < minY)
			{
				minY = y;
			}

			// Get temperature
			temperatureMap.back().push_back(HeightMap::getTemperatureNoise2D(nx, nz));
			moistureMap.back().push_back(HeightMap::getMoistureNosie2D(nx, nz));

			nz += step;
		}
		nx += step;
		nz = static_cast<float>(position.z);
	}

	int heighestChunkSection = (maxY / Constant::CHUNK_SECTION_HEIGHT);
	int lowestChunkSection = ((minY - 5) / Constant::CHUNK_SECTION_HEIGHT);
	if (lowestChunkSection < 0)
	{
		lowestChunkSection = 0;
	}

	/*
	if (maxY >= 255.0f)
	{
		std::cout << "Chunk: " << Utility::Log::vec3ToStr(position) << std::endl;
		std::cout << "maxY = " << maxY << std::endl;
		std::cout << "minY = " << minY << std::endl;
		std::cout << "heighestChunkSection = " << heighestChunkSection << std::endl;
		std::cout << "lowestChunkSection = " << lowestChunkSection << std::endl;
	}
	*/

	/*
	for (int i = 0; i < Constant::TOTAL_CHUNK_SECTION_PER_CHUNK; i++)
	{
		// Temp. All blocks above chunk section y 3 will be air.
		if (i > 3)
		{
			//std::cout << "+ chunksection: " << i << std::endl;
			chunkSections.push_back(nullptr);
		}
		else
		{
			//std::cout << "++ chunksection: " << i << std::endl;
			auto newChucnkSection = ChunkSection::create(position.x, i, position.z, worldPosition);
			//auto newChucnkSection = ChunkSection::createWithHeightMap(position.x, i, position.z, worldPosition, heightMap);
			if (newChucnkSection)
			{
				chunkSections.push_back(newChucnkSection);
			}
			else
			{
				throw std::runtime_error("Failed to create chunk section at (" + std::to_string(position.x) + ", " + std::to_string(i) + ", " + std::to_string(position.z) + ")");
			}
		}
	}
	*/

	for (int i = 0; i < Constant::TOTAL_CHUNK_SECTION_PER_CHUNK; i++)
	{
		// Temp. All blocks above chunk section y 3 will be air.
		if (i > heighestChunkSection || i < lowestChunkSection)
		{
			//std::cout << "+ chunksection: " << i << std::endl;
			//chunkSections.push_back(nullptr);
			continue;
		}
		else
		{
			//std::cout << "++ chunksection: " << i << std::endl;
			//auto newChucnkSection = ChunkSection::create(position.x, i, position.z, worldPosition);
			auto newChucnkSection = ChunkSection::createWithValues(position.x, i, position.z, worldPosition, elevationMap, temperatureMap, moistureMap);
			if (newChucnkSection)
			{
				chunkSections.at(i) = newChucnkSection;
			}
			else
			{
				throw std::runtime_error("Failed to create chunk section at (" + std::to_string(position.x) + ", " + std::to_string(i) + ", " + std::to_string(position.z) + ")");
			}
		}
	}

	generated.store(true);


	return true;
}

bool Voxel::Chunk::generateWithColor(const glm::uvec3 & color)
{
	assert(canGenerate());

	for (int i = 0; i < Constant::TOTAL_CHUNK_SECTION_PER_CHUNK; i++)
	{
		// Temp. All blocks above chunk section y 3 will be air.
		if (i > 1)
		{
			//std::cout << "+ chunksection: " << i << std::endl;
			//chunkSections.push_back(nullptr);
			continue;
		}
		else
		{
			//std::cout << "++ chunksection: " << i << std::endl;
			auto newChucnkSection = ChunkSection::createWithColor(position.x, i, position.z, worldPosition, color);
			if (newChucnkSection)
			{
				chunkSections.at(i) = newChucnkSection;
			}
			else
			{
				throw std::runtime_error("Failed to create chunk section at (" + std::to_string(position.x) + ", " + std::to_string(i) + ", " + std::to_string(position.z) + ")");
			}
		}
	}

	generated.store(true);

	return true;
}

bool Voxel::Chunk::generateWithRegionColor()
{
	assert(canGenerate());
	
	for (int i = 0; i < Constant::TOTAL_CHUNK_SECTION_PER_CHUNK; i++)
	{
		// Temp. All blocks above chunk section y 3 will be air.
		if (i > 1)
		{
			//std::cout << "+ chunksection: " << i << std::endl;
			//chunkSections.push_back(nullptr);
			continue;
		}
		else
		{
			//std::cout << "++ chunksection: " << i << std::endl;
			auto newChucnkSection = ChunkSection::createWithRegionColor(position.x, i, position.z, worldPosition, regionMap);
			if (newChucnkSection)
			{
				chunkSections.at(i) = newChucnkSection;
			}
			else
			{
				throw std::runtime_error("Failed to create chunk section at (" + std::to_string(position.x) + ", " + std::to_string(i) + ", " + std::to_string(position.z) + ")");
			}
		}
	}

	generated.store(true);

	return true;
}

void Voxel::Chunk::preGenerateChunkSections(const int minY, const int maxY)
{
	if (chunkSections.empty())
	{
		for (int i = 0; i < Constant::TOTAL_CHUNK_SECTION_PER_CHUNK; i++)
		{
			// Temp. All blocks above chunk section y 3 will be air.
			if (i > maxY || i < minY)
			{
				//std::cout << "+ chunksection: " << i << std::endl;
				chunkSections.push_back(nullptr);
			}
			else
			{
				//std::cout << "++ chunksection: " << i << std::endl;
				auto newChucnkSection = ChunkSection::createEmpty(position.x, i, position.z, worldPosition);
				if (newChucnkSection)
				{
					chunkSections.push_back(newChucnkSection);
				}
				else
				{
					throw std::runtime_error("Failed to create chunk section at (" + std::to_string(position.x) + ", " + std::to_string(i) + ", " + std::to_string(position.z) + ")");
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < Constant::TOTAL_CHUNK_SECTION_PER_CHUNK; i++)
		{
			// Temp. All blocks above chunk section y 3 will be air.
			if (i > maxY || i < minY)
			{
				continue;
			}
			else
			{
				//std::cout << "++ chunksection: " << i << std::endl;
				auto newChucnkSection = ChunkSection::createEmpty(position.x, i, position.z, worldPosition);
				if (newChucnkSection)
				{
					chunkSections.at(i) = newChucnkSection;
				}
				else
				{
					throw std::runtime_error("Failed to create chunk section at (" + std::to_string(position.x) + ", " + std::to_string(i) + ", " + std::to_string(position.z) + ")");
				}
			}
		}
	}
}

glm::ivec3 Chunk::getPosition()
{
	return position;
}

glm::ivec2 Voxel::Chunk::getCoordinate()
{
	return glm::ivec2(position.x, position.z);
}

glm::vec3 Chunk::getWorldPosition()
{
	return worldPosition;
}

ChunkSection * Voxel::Chunk::getChunkSectionAtY(const int y)
{
	if (y >= 0 && y < static_cast<int>(chunkSections.size()))
	{
		if (chunkSections.at(y) == nullptr)
		{
			return nullptr;
		}
		else
		{
			return chunkSections.at(y);
		}
	}
	else
	{
		return nullptr;
	}
}

void Voxel::Chunk::createChunkSectionAtY(const int y)
{
	if (y < 0)
	{
		return;
	}
	else
	{
		if (y >= 0 && y < static_cast<int>(chunkSections.size()))
		{
			if (chunkSections.at(y) == nullptr)
			{
				auto newChucnkSection = ChunkSection::createEmpty(position.x, y, position.z, worldPosition);
				if (newChucnkSection)
				{
					chunkSections.at(y) = newChucnkSection;
				}
				else
				{
					throw std::runtime_error("Failed to add chunk section at (" + std::to_string(position.x) + ", " + std::to_string(y) + ", " + std::to_string(position.z) + ")");
				}
			}
		}
		else
		{
			return;
		}
	}
}

void Voxel::Chunk::deleteChunkSectionAtY(const int y)
{
	if (y >= 0 && y < static_cast<int>(chunkSections.size()))
	{
		if (chunkSections.at(y))
		{
			delete chunkSections.at(y);
			chunkSections.at(y) = nullptr;
		}
	}
	else
	{
		return;
	}
}

int Voxel::Chunk::findMaxY()
{
	int max = 0;
	for (auto& row : heightMap)
	{
		for (auto& val : row)
		{
			if (val > max)
			{
				max = val;
			}
		}
	}

	return max;
}

void Voxel::Chunk::render()
{
	auto program = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::SHADER_COLOR);

	if (chunkMesh)
	{
		if (chunkMesh->isRenderable())
		{
			// Ready to render
			bool result = chunkMesh->bind();
			if (result)
			{
				chunkMesh->render();
			}
			else
			{
				assert(false);
			}
		}
		else
		{
			// mesh is not renderable. See if it can load
			if (chunkMesh->isBufferLoadable())
			{
				chunkMesh->loadBuffer(program);

				// Now ready to render
				bool result = chunkMesh->bind();
				if (result)
				{
					chunkMesh->render();
				}
				else
				{
					assert(false);
				}
				chunkMesh->render();
			}
			else
			{
				// Not renderable, buffer not ready.
				return;
			}
		}
	}
	// Doens't have chunk mesh. should be error
}

void Voxel::Chunk::setActive(const bool state)
{
	active = state;
}

bool Voxel::Chunk::isActive()
{
	return active;
}

void Voxel::Chunk::setVisibility(const bool visibility)
{
	visible = visibility;
}

bool Voxel::Chunk::isVisible()
{
	return visible;
}

void Voxel::Chunk::releaseMesh()
{
	if (chunkMesh)
	{
		// delete mesh
		chunkMesh->releaseVAO();
	}
}

ChunkMesh * Voxel::Chunk::getMesh()
{
	return chunkMesh;
}

Geometry::AABB Voxel::Chunk::getBoundingBox()
{
	return boundingBox;
}

void Voxel::Chunk::setRegionMap(const unsigned int regionID)
{
	regionMap.clear();
	regionMap.push_back(regionID);
}

void Voxel::Chunk::setRegionMap(const std::vector<unsigned int>& regionIDs)
{
	regionMap.clear();
	regionMap = regionIDs;
}

void Voxel::Chunk::mergeHeightMap(std::vector<std::vector<int>>& plainHeightMap)
{
	const int rowSize = heightMap.size();
	const int colSize = heightMap.front().size();

	assert(rowSize == plainHeightMap.size());
	assert(colSize == plainHeightMap.front().size());

	for (int row = 0; row < rowSize; row++)
	{
		for (int col = 0; col < colSize; col++)
		{
			auto pH = plainHeightMap.at(row).at(col);
			auto h = heightMap.at(row).at(col);
			if (h < pH)
			{
				heightMap.at(row).at(col) = ((h + pH) / 2);
			}
		}
	}
}

bool Voxel::Chunk::isGenerated()
{
	return generated.load();
}

bool Voxel::Chunk::hasMultipleRegion()
{
	return regionMap.size() > 1;
}

int Voxel::Chunk::getQ11()
{
	return heightMap.front().front();
}

int Voxel::Chunk::getQ12()
{
	return heightMap.front().back();
}

int Voxel::Chunk::getQ21()
{
	return heightMap.back().front();
}

int Voxel::Chunk::getQ22()
{
	return heightMap.back().back();
}

bool Voxel::Chunk::isSmoothed()
{
	return smoothed;
}

int Voxel::Chunk::getTopY(const int x, const int z)
{
	return heightMap.at(x).at(z);
}

void Voxel::Chunk::updateTimestamp(const double timestamp)
{
	this->timestamp = timestamp;
}

void Voxel::Chunk::print()
{
	std::cout << "Chunk (" << position.x << ", " << position.z << ") info.\n";
	std::cout << "World position = " << Utility::Log::vec3ToStr(worldPosition) << std::endl;
	std::cout << "Active: " << (active ? "True" : "False") << std::endl;
	std::cout << "Visible: " << (visible ? "True" : "False") << std::endl;
	std::cout << "Generated: " << (generated.load() ? "True" : "False") << std::endl;
	if (regionMap.size() == 1)
	{
		std::cout << "Region: " << regionMap.front() << std::endl;
	}
	else
	{
		int c = 0;
		for (auto i : regionMap)
		{
			std::cout << i << " ";
			c++;
			if (c >= 16)
			{
				std::cout << std::endl;
				c = 0;
			}
		}
	}
}
