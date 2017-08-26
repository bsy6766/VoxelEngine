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
	//int randY = Utility::Random::randomInt(2, 5);
	for (int i = 0; i < Constant::TOTAL_CHUNK_SECTION_PER_CHUNK; i++)
	{
		// Temp. All blocks above chunk section y 3 will be air.
		if (i > 3)
		{
			chunkSections.push_back(nullptr);
		}
		else
		{
			auto newChucnkSection = ChunkSection::create(position.x, i, position.z, worldPosition);
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

/*
bool Voxel::Chunk::isAdjacent(Chunk * other)
{
	auto otherPos = other->position;

	auto diff = otherPos - position;

	return true;
}
*/

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

bool Voxel::Chunk::isPointInBorder(const glm::vec3 & point)
{
	// For now, only check x, z
	return (border.min.x < point.x && point.x < border.max.x) && (border.min.z < point.z && point.z < border.max.z);
}

void Voxel::Chunk::updateTimestamp(const double timestamp)
{
	this->timestamp = timestamp;
}
