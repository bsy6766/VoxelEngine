#include "Chunk.h"
#include <ChunkSection.h>
#include <iostream>
#include <ChunkMesh.h>
#include <ChunkUtil.h>
#include <Utility.h>

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
}

Chunk* Chunk::create(const int x, const int z)
{
	Chunk* newChunk = new Chunk();
	//std::cout << "[Chunk] Creating new chunk at (" << x << ", " << z << ")..." << std::endl;
	if (newChunk->init(x, z))
	{
		if (newChunk->generate())
		{
			//std::cout << "[Chunk] Done.\n" << std::endl;
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
	std::vector<std::vector<int>> heightMap;
	int maxY = 0;
	int minY = 10000;

	int xStart = position.x;
	int zStart = position.z;
	int xEnd = xStart + Constant::CHUNK_SECTION_WIDTH;
	int zEnd = zStart + Constant::CHUNK_SECTION_LENGTH;

	// Frequency. Zoom factor of height map. Higher the value, compact heightmap.
	float freq = 0.03f;

	float nx = static_cast<float>(position.x);
	float nz = static_cast<float>(position.z);
	const float step = 1.0f / Constant::CHUNK_BORDER_SIZE;

	float octave1 = 1.0f;
	float octave2 = 1.0f;
	float octave3 = 1.0f;
	float octave4 = 0.7f;
	float octave5 = 0.3f;
	float octave6 = 0.0f;

	float octave1Mul = 1.0f;
	float octave2Mul = 2.0f;
	float octave3Mul = 4.0f;
	float octave4Mul = 8.0f;
	float octave5Mul = 16.0f;
	float octave6Mul = 32.0f;

	float redistribution = 3.0f;

	for (int x = xStart; x < xEnd; x++)
	{
		heightMap.push_back(std::vector<int>());
		for (int z = zStart; z < zEnd; z++)
		{
			// Noise returns value -1 to 1.
			// Summing 6 noise will result in range of -6 to 6, but because of octave, it varies.
			float val = octave1 * Utility::SimplexNoise::noise(glm::vec2(((octave1Mul * nx) + 0.5f) * freq, ((octave1Mul * nz) + 0.5f) * freq))
					+ octave2 * Utility::SimplexNoise::noise(glm::vec2(((octave2Mul * nx) + 0.5f) * freq, ((octave2Mul * nz) + 0.5f) * freq))
					+ octave3 * Utility::SimplexNoise::noise(glm::vec2(((octave3Mul * nx) + 0.5f) * freq, ((octave3Mul * nz) + 0.5f) * freq))
					+ octave4 * Utility::SimplexNoise::noise(glm::vec2(((octave4Mul * nx) + 0.5f) * freq, ((octave4Mul * nz) + 0.5f) * freq))
					+ octave5 * Utility::SimplexNoise::noise(glm::vec2(((octave5Mul * nx) + 0.5f) * freq, ((octave5Mul * nz) + 0.5f) * freq))
					+ octave6 * Utility::SimplexNoise::noise(glm::vec2(((octave6Mul * nx) + 0.5f) * freq, ((octave6Mul * nz) + 0.5f) * freq));

			// So we devide by sum of octaves
			val /= (octave1 + octave2 + octave3 + octave4 + octave5 + octave6);

			// Now val should be in range of -1 ~ 1. Add 1 to make it in range of 0 ~ 2
			val += 1.0f;
			
			// Just in case if val is still 0, make it 0
			if (val < 0)
			{
				std::cout << "Val = " << val << std::endl;
				val = 0;
			}

			val = powf(val, redistribution);
			//std::cout << "val = " << val << std::endl;
			
			// Noise returns in range of -1 ~ 1. Make it 0~1
			//val = (val + 1.0f) * 0.5f;

			// boost it
			int y = (static_cast<int>(val * 20.0f) + 60);

			heightMap.back().push_back(y);

			if (y > maxY)
			{
				maxY = y;
			}
			else if (y < minY)
			{
				minY = y;
			}

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

	//std::cout << "Chunk: " << Utility::Log::vec3ToStr(position) << std::endl;
	//std::cout << "maxY = " << maxY << std::endl;
	//std::cout << "minY = " << minY << std::endl;
	//std::cout << "heighestChunkSection = " << heighestChunkSection << std::endl;
	//std::cout << "lowestChunkSection = " << lowestChunkSection << std::endl;

	//int randY = Utility::Random::randomInt(2, 5);
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
			auto newChucnkSection = ChunkSection::createWithHeightMap(position.x, i, position.z, worldPosition, heightMap);
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

ChunkSection * Voxel::Chunk::getChunkSectionByY(int y)
{
	if (y >= 0 && y < chunkSections.size())
	{
		return chunkSections.at(y);
	}
	else
	{
		return nullptr;
	}
}

void Voxel::Chunk::render()
{
	if (chunkMesh)
	{
		if (chunkMesh->hasBufferToLoad())
		{
			// Mesh has buffer to load
			if (!chunkMesh->hasLoaded())
			{
				chunkMesh->loadBuffer();
			}

			// Mesh already loaded buffer to gpu. render.
			chunkMesh->bind();
			chunkMesh->render();
		}
		// Else, mesh is not loaded. Don't render
	}
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
