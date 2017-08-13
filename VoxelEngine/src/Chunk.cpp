#include "Chunk.h"
#include <ChunkSection.h>

using namespace Voxel;

// static initialize
const unsigned int Chunk::TOTAL_CHUNK_SECTION_PER_CHUNK = 4;

Chunk::Chunk()
	: position(0)
	, worldPosition(0.0f)
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
	if (newChunk->init(x, z))
	{
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

	int chunkSectionY = -((TOTAL_CHUNK_SECTION_PER_CHUNK / 2) + 1);

	for (int i = 0; i < TOTAL_CHUNK_SECTION_PER_CHUNK; i++)
	{
		auto newChucnkSection = ChunkSection::create(x, chunkSectionY, z, worldPosition);
		if (newChucnkSection)
		{
			chunkSections.push_back(newChucnkSection);
		}
		else
		{
			return false;
		}

		chunkSectionY++;
	}

	return true;
}