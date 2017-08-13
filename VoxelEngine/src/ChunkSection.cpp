#include "ChunkSection.h"
#include <Block.h>

using namespace Voxel;

const unsigned int ChunkSection::TOTAL_BLOCKS = 4096;
const float ChunkSection::CHUNK_SECTION_HEIGHT = 16.0f;
const float ChunkSection::CHUNK_SECTION_WIDTH = 16.0f;
const float ChunkSection::CHUNK_SECTION_LENGTH = 16.0f;

ChunkSection::ChunkSection()
	: position(0)
	, worldPosition(0.0f)
{}

ChunkSection::~ChunkSection()
{
	for (auto block : blocks)
	{
		if (block)
		{
			delete block;
		}
	}

	blocks.clear();
}

ChunkSection* ChunkSection::create(const int x, const int y, const int z, const glm::vec3& chunkPosition)
{
	ChunkSection* newChunkSection = new ChunkSection();
	if (newChunkSection->init(x, y, z, chunkPosition))
	{
		return newChunkSection;
	}
	else
	{
		delete newChunkSection;
		return nullptr;
	}
}

bool ChunkSection::init(const int x, const int y, const int z, const glm::vec3& chunkPosition)
{
	position = glm::ivec3(x, y, z);

	// calculate world position. Only need to calculate Y.
	worldPosition = chunkPosition;
	worldPosition.y = (static_cast<float>(y) - 0.5f) * CHUNK_SECTION_HEIGHT;

	// Fill vector in order of width(x), length(z) and then height(y)
	for (int i = 0; i > -CHUNK_SECTION_HEIGHT; i--)
	{
		for (int j = 0; j < CHUNK_SECTION_LENGTH; j++)
		{
			for (int k = 0; k < CHUNK_SECTION_WIDTH; k++)
			{
				auto newBlock = Block::create(glm::ivec3(k, i, j), position);
				if (newBlock)
				{
					blocks.push_back(newBlock);
				}
				else
				{
					return false;
				}
			}
		}
	}

	return true;
}