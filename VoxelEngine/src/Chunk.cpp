#include "Chunk.h"
#include <ChunkSection.h>
#include <iostream>
#include <ChunkUtil.h>

using namespace Voxel;

Chunk::Chunk()
	: position(0)
	, worldPosition(0.0f)
	, active(false)
{}

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
}

Chunk* Chunk::create(const int x, const int z)
{
	Chunk* newChunk = new Chunk();
	//std::cout << "[Chunk] Creating new chunk at (" << x << ", " << z << ")..." << std::endl;
	if (newChunk->init(x, z))
	{
		//std::cout << "[Chunk] Done.\n" << std::endl;
		return newChunk;
	}
	else
	{
		delete newChunk;
		return nullptr;
	}
}

bool Chunk::init(const int x, const int z)
{
	position = glm::ivec3(x, 0, z);

	// calculate position. Size of each block is 1.0f. There are total 16 x 16 (256) blocks in XZ space.
	worldPosition.x = 16.0f * (static_cast<float>(x) + 0.5f);
	worldPosition.y = 0;
	worldPosition.z = 16.0f * (static_cast<float>(z) + 0.5f);

	//std::cout << "[Chunk] position: (" << x << ", 0, " << z << "), world position: (" << worldPosition.x << ", " << worldPosition.y << ", " << worldPosition.z << ")" << std::endl;
	//std::cout << "[Chunk] Creating " << Constant::TOTAL_CHUNK_SECTION_PER_CHUNK << " ChunkSections..." << std::endl;

	for (int i = 0; i < Constant::TOTAL_CHUNK_SECTION_PER_CHUNK; i++)
	{
		// Temp. All blocks above chunk section y 3 will be air.
		if (i > 4)
		{
			chunkSections.push_back(nullptr);
		}
		else
		{
			auto newChucnkSection = ChunkSection::create(x, i, z, worldPosition);
			if (newChucnkSection)
			{
				chunkSections.push_back(newChucnkSection);
			}
			else
			{
				return false;
			}
		}
	}

	// init border. worldPosition works as center position of border
	float borderDistance = (Constant::CHUNK_BORDER_SIZE * 0.5f) - 0.05f;
	
	border.min = glm::vec3(worldPosition.x - borderDistance, 0, worldPosition.z - borderDistance);
	border.max = glm::vec3(worldPosition.x + borderDistance, 0, worldPosition.z + borderDistance);

	//std::cout << "[Chunk] BorderXZ: min(" << border.min.x << ", " << border.min.z << "), max(" << border.max.x << ", " << border.max.z << ")" << std::endl;

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
	for (auto chunkSection : chunkSections)
	{
		if (chunkSection)
		{
			chunkSection->render();
		}
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

void Voxel::Chunk::setAllVisibility(const bool visibility)
{
	for (auto chunkSection : chunkSections)
	{
		if (chunkSection)
		{
			chunkSection->setVisibility(visibility);
		}
	}
}

void Voxel::Chunk::releaseAllMeshes()
{
	for (auto cs : chunkSections)
	{
		if (cs)
		{
			cs->releaseMesh();
		}
	}
}

bool Voxel::Chunk::hasChunkSectionNeedMesh()
{
	for (auto chunkSection : chunkSections)
	{
		if (chunkSection)
		{
			if (chunkSection->hasMesh())
			{
				continue;
			}
			else
			{
				return true;
			}
		}
		else
		{
			continue;
		}
	}

	return false;
}
