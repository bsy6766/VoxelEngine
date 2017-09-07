#include "Chunk.h"
#include <ChunkSection.h>
#include <iostream>
#include <ChunkMesh.h>
#include <ChunkUtil.h>
#include <Utility.h>
#include <HeightMap.h>

using namespace Voxel;

Chunk::Chunk()
	: position(0)
	, worldPosition(0.0f)
	, chunkMesh(nullptr)
	, active(false)
	, visible(false)
	, timestamp(0)
{
	chunkMesh = new ChunkMesh();
	generated.store(false);
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

	std::cout << "[Chunk] Deleting chunk at (" << position.x << ", " << position.z << ")..." << std::endl;
}

Chunk* Chunk::create(const int x, const int z)
{
	Chunk* newChunk = new Chunk();
	if (newChunk->init(x, z))
	{
		if (newChunk->generate())
		{
			//std::cout << "[Chunk] Done.\n" << std::endl;
			std::cout << "[Chunk] Creating new chunk at (" << x << ", " << z << ")..." << std::endl;
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
		//std::cout << "[Chunk] Done.\n" << std::endl;
		std::cout << "[Chunk] Creating new empty chunk at (" << x << ", " << z << ")..." << std::endl;
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
	worldPosition.x = 16.0f * (static_cast<float>(x) + 0.5f);
	worldPosition.y = 0;
	worldPosition.z = 16.0f * (static_cast<float>(z) + 0.5f);

	//std::cout << "[Chunk] position: (" << x << ", 0, " << z << "), world position: (" << worldPosition.x << ", " << worldPosition.y << ", " << worldPosition.z << ")" << std::endl;

	// init border. worldPosition works as center position of border
	float borderDistance = (Constant::CHUNK_BORDER_SIZE * 0.5f);
	
	border.min = glm::vec3(worldPosition.x - borderDistance, 0, worldPosition.z - borderDistance);
	border.max = glm::vec3(worldPosition.x + borderDistance, Constant::TOTAL_CHUNK_SECTION_PER_CHUNK * Constant::CHUNK_SECTION_HEIGHT, worldPosition.z + borderDistance);

	//std::cout << "[Chunk] BorderXZ: min(" << border.min.x << ", " << border.min.z << "), max(" << border.max.x << ", " << border.max.z << ")" << std::endl;

	//auto end = Utility::Time::now();
	//std::cout << "Chunk generation elapsed time: " << Utility::Time::toMilliSecondString(start, end) << std::endl;
	return true;
}

bool Voxel::Chunk::generate()
{	
	//std::cout << "[Chunk] Creating " << Constant::TOTAL_CHUNK_SECTION_PER_CHUNK << " ChunkSections..." << std::endl;
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
			float val = HeightMap::getNoise2D(nx, nz, HeightMap::PRESET::DEBUG);

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
			chunkSections.push_back(nullptr);
		}
		else
		{
			//std::cout << "++ chunksection: " << i << std::endl;
			//auto newChucnkSection = ChunkSection::create(position.x, i, position.z, worldPosition);
			auto newChucnkSection = ChunkSection::createWithValues(position.x, i, position.z, worldPosition, elevationMap, temperatureMap, moistureMap);
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

	generated.store(true);

	return true;
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
	if (y >= 0 && y < chunkSections.size())
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
	if (y >= 0 && y < chunkSections.size())
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

void Voxel::Chunk::deleteChunkSectionAtY(const int y)
{
	if (y >= 0 && y < chunkSections.size())
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

void Voxel::Chunk::render()
{
	if (chunkMesh)
	{
		if (chunkMesh->isRenderable())
		{
			// Ready to render
			chunkMesh->bind();
			chunkMesh->render();
		}
		else
		{
			// mesh is not renderable. See if it can load
			if (chunkMesh->isBufferLoadable())
			{
				chunkMesh->loadBuffer();
			}

			// Now ready to render
			chunkMesh->bind();
			chunkMesh->render();
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

bool Voxel::Chunk::isGenerated()
{
	return generated.load();
}

void Voxel::Chunk::updateTimestamp(const double timestamp)
{
	this->timestamp = timestamp;
}
