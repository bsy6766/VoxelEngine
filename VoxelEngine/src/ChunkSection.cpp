#include "ChunkSection.h"
#include <Block.h>
#include <iostream>
#include <ChunkUtil.h>
#include <Color.h>

using namespace Voxel;

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
	//std::cout << "[ChunkSection] Creating new chunk section at (" << x << ", " << y << ", " << z << ")..." << std::endl;
	if (newChunkSection->init(x, y, z, chunkPosition))
	{
		//std::cout << "[ChunkSection] Done." << std::endl;
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
	worldPosition.y = (static_cast<float>(y) + 0.5f) * static_cast<float>(Constant::CHUNK_SECTION_HEIGHT);
	
	//std::cout << "[ChunkSection] World position (" << worldPosition.x << ", " << worldPosition.y << ", " << worldPosition.z << ")..." << std::endl;

	// Fill vector in order of width(x), length(z) and then height(y)
	//std::cout << "[ChunkSection] Generating blocks..." << std::endl;
	auto color = Color::getRandomColor();
	for (int i = 0; i < Constant::CHUNK_SECTION_HEIGHT; i++)
	{
		for (int j = 0; j < Constant::CHUNK_SECTION_LENGTH; j++)
		{
			for (int k = 0; k < Constant::CHUNK_SECTION_WIDTH; k++)
			{
				auto newBlock = Block::create(glm::ivec3(k, i, j), position);
				if (newBlock)
				{
					blocks.push_back(newBlock);
					// debug
					newBlock->setColor(color);
				}
				else
				{
					return false;
				}
			}
		}
	}

	//auto size = sizeof(Block);
	//std::cout << "size = " << std::endl;

	return true;
}

int Voxel::ChunkSection::XYZToIndex(const int x, const int y, const int z)
{
	return x + (Constant::CHUNK_SECTION_WIDTH * z) + (y * Constant::CHUNK_SECTION_LENGTH * Constant::CHUNK_SECTION_WIDTH);
}

Block * Voxel::ChunkSection::getBlockAt(const int x, const int y, const int z)
{
	unsigned int index = XYZToIndex(x, y, z);
	if (index >= 0 && index < blocks.size())
	{
		return blocks.at(index);
	}
	else
	{
		return nullptr;
	}
}

glm::vec3 Voxel::ChunkSection::getWorldPosition()
{
	return worldPosition;
}