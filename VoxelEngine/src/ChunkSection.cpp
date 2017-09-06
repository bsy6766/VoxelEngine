#include "ChunkSection.h"
#include <iostream>
#include <ChunkUtil.h>
#include <Color.h>

using namespace Voxel;

ChunkSection::ChunkSection()
	: position(0)
	, worldPosition(0.0f)
	, nonAirBlockSize(0)
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

ChunkSection * Voxel::ChunkSection::createEmpty(const int x, const int y, const int z, const glm::vec3 & chunkPosition)
{
	ChunkSection* newChunkSection = new ChunkSection();
	//std::cout << "[ChunkSection] Creating new chunk section at (" << x << ", " << y << ", " << z << ")..." << std::endl;
	if (newChunkSection->initEmpty(x, y, z, chunkPosition))
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

ChunkSection * Voxel::ChunkSection::createWithHeightMap(const int x, const int y, const int z, const glm::vec3 & chunkPosition, const std::vector<std::vector<int>>& heightMap)
{
	ChunkSection* newChunkSection = new ChunkSection();
	//std::cout << "[ChunkSection] Creating new chunk section at (" << x << ", " << y << ", " << z << ")..." << std::endl;
	if (newChunkSection->initWithHeightMap(x, y, z, chunkPosition, heightMap))
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
					if (newBlock->getBlockID() != Block::BLOCK_ID::AIR)
					{
						nonAirBlockSize++;
					}
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

bool Voxel::ChunkSection::initEmpty(const int x, const int y, const int z, const glm::vec3 & chunkPosition)
{
	position = glm::ivec3(x, y, z);

	// calculate world position. Only need to calculate Y.
	worldPosition = chunkPosition;
	worldPosition.y = (static_cast<float>(y) + 0.5f) * static_cast<float>(Constant::CHUNK_SECTION_HEIGHT);

	blocks.clear();
	blocks.resize(4096, nullptr);

	return true;
}

bool Voxel::ChunkSection::initWithHeightMap(const int x, const int y, const int z, const glm::vec3 & chunkPosition, const std::vector<std::vector<int>>& heightMap)
{
	position = glm::ivec3(x, y, z);

	// calculate world position. Only need to calculate Y.
	worldPosition = chunkPosition;
	worldPosition.y = (static_cast<float>(y) + 0.5f) * static_cast<float>(Constant::CHUNK_SECTION_HEIGHT);

	blocks.clear();
	blocks.resize(4096, nullptr);

	/*
	// Log: Day 19. Attempted to create only 5 blocks below the hieght map.
	//		Turns out to terrible decision. Image where mountain has edge longer than 5 blocks high. There will be empty holes.
	//		Mybe I can iterate all 4096 blocks and check if it's visible. Else, delete the block.
	//		Or, check height map nearby and see if near block is higher than me. It so, delete.

	int chunkYStart = y * Constant::CHUNK_SECTION_HEIGHT;
	//auto color = Color::getRandomColor();
	auto color = Color::GRASS;

	for (int blockX = 0; blockX < Constant::CHUNK_SECTION_WIDTH; blockX++)
	{
		for (int blockZ = 0; blockZ < Constant::CHUNK_SECTION_WIDTH; blockZ++)
		{
			int heightY = heightMap.at(blockX).at(blockZ);
			int heightChunkY = heightY / Constant::CHUNK_SECTION_HEIGHT;
			bool heighest = (heightChunkY == y);

			int localY = 0;

			int localHeight = heightY % Constant::CHUNK_SECTION_HEIGHT;

			int yEnd = 0;

			if (heighest)
			{
				localY = localHeight - 4;
				if (localY < 0) localY = 0;
				yEnd = localHeight;
			}
			else
			{
				bool below = ((heightChunkY - 1) == y);
				if (below)
				{
					if (localHeight < 4)
					{
						localY = localHeight - 4;
						if (localY < 0) localY += Constant::CHUNK_SECTION_HEIGHT;
						yEnd = Constant::CHUNK_SECTION_HEIGHT - 1;
					}
					else
					{
						continue;
					}
				}
				// else, Don't have to fill
				else
				{
					continue;
				}
			}

			for (int blockY = localY; blockY <= yEnd; blockY++)
			{
				auto newBlock = Block::create(glm::ivec3(blockX, blockY, blockZ), position);
				newBlock->setColor(color);
				blocks.at(XYZToIndex(blockX, blockY, blockZ)) = newBlock;
			}
		}
	}

	*/

	int yStart = y * Constant::CHUNK_SECTION_HEIGHT;
	//auto color = Color::getRandomColor();
	//auto color = Color::GRASS;

	for (int blockX = 0; blockX < Constant::CHUNK_SECTION_WIDTH; blockX++)
	{
		for (int blockZ = 0; blockZ < Constant::CHUNK_SECTION_WIDTH; blockZ++)
		{
			int localY = 0;
			int heightY = heightMap.at(blockX).at(blockZ);

			if (yStart <= heightY)
			{
				int yEnd = yStart + Constant::CHUNK_SECTION_HEIGHT - 1;
				if (yEnd > heightY)
				{
					yEnd = heightY;
				}

				for (int blockY = yStart; blockY <= yEnd; blockY++)
				{
					auto newBlock = Block::create(glm::ivec3(blockX, localY, blockZ), position);
					//newBlock->setColor(color);
					blocks.at(XYZToIndex(blockX, localY, blockZ)) = newBlock;

					if (newBlock->getBlockID() != Block::BLOCK_ID::AIR)
					{
						nonAirBlockSize++;
					}

					localY++;
				}
			}
			else
			{
				continue;
			}
		}
	}

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

void Voxel::ChunkSection::setBlockAt(const glm::ivec3 & localCoordinate, const Block::BLOCK_ID blockID)
{
	setBlockAt(localCoordinate.x, localCoordinate.y, localCoordinate.z, blockID);
}

void Voxel::ChunkSection::setBlockAt(const int x, const int y, const int z, const Block::BLOCK_ID blockID)
{
	unsigned int index = XYZToIndex(x, y, z);
	if (index >= 0 && index < blocks.size())
	{
		if (blocks.at(index) == nullptr)
		{
			// Block doesn't exists
			if (blockID != Block::BLOCK_ID::AIR)
			{
				// Block isn't air
				blocks.at(index) = Block::create(glm::ivec3(x, y, z), position);
				blocks.at(index)->setBlockID(blockID);

				nonAirBlockSize++;
			}
			// Else, block is air. do nothing
		}
		else
		{
			// Block already exists
			if (blockID == Block::BLOCK_ID::AIR)
			{
				// Remove block
				delete blocks.at(index);
				blocks.at(index) = nullptr;

				nonAirBlockSize--;
			}
			// Else, can't modify block.
		}
	}
	else
	{
		return;
	}
}

glm::vec3 Voxel::ChunkSection::getWorldPosition()
{
	return worldPosition;
}

int Voxel::ChunkSection::getTotalNonAirBlockSize()
{
	return nonAirBlockSize;
}
