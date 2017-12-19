// pch
#include "PreCompiled.h"

#include "ChunkSection.h"

// voxel
#include "ChunkUtil.h"
#include "Color.h"
#include "Biome.h"
#include "Application.h"
#include "HeightMap.h"
#include "World.h"
#include "Region.h"

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

ChunkSection* ChunkSection::create(const int x, const int y, const int z, const glm::vec3& chunkPosition, const std::vector<unsigned int>& regionMap, const std::vector<std::vector<int>>& heightMap, const std::vector<std::vector<float>>& colorMap)
{
	ChunkSection* newChunkSection = new ChunkSection();
	//std::cout << "[ChunkSection] Creating new chunk section at (" << x << ", " << y << ", " << z << ")...\n";
	if (newChunkSection->init(x, y, z, chunkPosition, regionMap, heightMap, colorMap))
	{
		//std::cout << "[ChunkSection] Done.\n";
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
	//std::cout << "[ChunkSection] Creating new chunk section at (" << x << ", " << y << ", " << z << ")...\n";
	if (newChunkSection->initEmpty(x, y, z, chunkPosition))
	{
		//std::cout << "[ChunkSection] Done.\n";
		return newChunkSection;
	}
	else
	{
		delete newChunkSection;
		return nullptr;
	}
}

void Voxel::ChunkSection::init(const std::vector<std::vector<int>>& heightMap, const std::vector<std::vector<float>>& colorMap)
{
	int yStart = position.y * Constant::CHUNK_SECTION_HEIGHT;

	const int single = 1;
	const int multiple = Constant::CHUNK_SECTION_WIDTH * Constant::CHUNK_SECTION_LENGTH;

	blocks.resize(Constant::TOTAL_BLOCKS, nullptr);

	float nx = static_cast<float>(position.x);
	float nz = static_cast<float>(position.z);
	const float step = 1.0f / Constant::CHUNK_BORDER_SIZE;

	for (int blockX = 0; blockX < Constant::CHUNK_SECTION_WIDTH; blockX++)
	{
		for (int blockZ = 0; blockZ < Constant::CHUNK_SECTION_WIDTH; blockZ++)
		{
			int localY = 0;
			int heightY = heightMap.at(blockX).at(blockZ);

			nz += step;
			
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
					blocks.at(localBlockXYZToIndex(blockX, localY, blockZ)) = newBlock;

					if ((heightY - blockY) > 2)
					{
						newBlock->setBlockID(Voxel::Block::BLOCK_ID::STONE);
						newBlock->setColorU3(Color::STONE);
					}
					else
					{
						newBlock->setBlockID(Voxel::Block::BLOCK_ID::GRASS);
						newBlock->setColorU3(Color::GRASS);
					}

					glm::vec3 color = newBlock->getColor3();
					glm::vec3 colorMix = Color::colorU3TocolorV3(Color::GRASS_MIX);
					color = glm::mix(color, colorMix, 0.5f) * colorMap.at(blockX).at(blockZ);

					if (blockY > 80)
					{
						int e = blockY - 80;
						float ef = 1.0f - (static_cast<float>(e) / 30.0f);
						color = glm::lerp(glm::vec3(0.6f), color, ef);
					}

					newBlock->setColor(color);

					localY++;

					if (newBlock->getBlockID() != Block::BLOCK_ID::AIR)
					{
						nonAirBlockSize++;
					}

				}
			}
		}

		nx += step;
		nz = static_cast<float>(position.z);
	}

}

bool ChunkSection::init(const int x, const int y, const int z, const glm::vec3& chunkPosition, const std::vector<unsigned int>& regionMap, const std::vector<std::vector<int>>& heightMap, const std::vector<std::vector<float>>& colorMap)
{
	position = glm::ivec3(x, y, z);

	// calculate world position. Only need to calculate Y.
	worldPosition = chunkPosition;
	worldPosition.y = (static_cast<float>(y) + 0.5f) * static_cast<float>(Constant::CHUNK_SECTION_HEIGHT);
	
	//std::cout << "[ChunkSection] World position (" << worldPosition.x << ", " << worldPosition.y << ", " << worldPosition.z << ")...\n";

	// Fill vector in order of width(x), length(z) and then height(y)
	//std::cout << "[ChunkSection] Generating blocks...\n";
	//auto color = Color::getRandomColor();

	blocks.clear();
	blocks.resize(Constant::TOTAL_BLOCKS, nullptr);

	int yStart = y * Constant::CHUNK_SECTION_HEIGHT;
	//auto color = Color::getRandomColor();
	//auto color = Color::GRASS;

	const int single = 1;
	const int multiple = Constant::CHUNK_SECTION_WIDTH * Constant::CHUNK_SECTION_LENGTH;

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
					blocks.at(localBlockXYZToIndex(blockX, localY, blockZ)) = newBlock;

					if (newBlock->getBlockID() != Block::BLOCK_ID::AIR)
					{
						nonAirBlockSize++;
					}

					localY++;

					auto color = newBlock->getColor3() * colorMap.at(blockX).at(blockZ);
					newBlock->setColor(color);
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

bool Voxel::ChunkSection::initEmpty(const int x, const int y, const int z, const glm::vec3 & chunkPosition)
{
	position = glm::ivec3(x, y, z);

	// calculate world position. Only need to calculate Y.
	worldPosition = chunkPosition;
	worldPosition.y = (static_cast<float>(y) + 0.5f) * static_cast<float>(Constant::CHUNK_SECTION_HEIGHT);

	blocks.clear();
	blocks.resize(Constant::TOTAL_BLOCKS, nullptr);

	return true;
}

int Voxel::ChunkSection::localBlockXYZToIndex(const int x, const int y, const int z)
{
	return x + (Constant::CHUNK_SECTION_WIDTH * z) + (y * Constant::CHUNK_SECTION_LENGTH * Constant::CHUNK_SECTION_WIDTH);
}

int Voxel::ChunkSection::localBlockXZToMapIndex(const int x, const int z)
{
	return x + (Constant::CHUNK_SECTION_WIDTH * z);
}

Block * Voxel::ChunkSection::getBlockAt(const int x, const int y, const int z)
{
	unsigned int index = localBlockXYZToIndex(x, y, z);
	if (index >= 0 && index < blocks.size())
	{
		return blocks.at(index);
	}
	else
	{
		return nullptr;
	}
}

void Voxel::ChunkSection::setBlockAt(const glm::ivec3 & localCoordinate, const Block::BLOCK_ID blockID, const bool overwrite)
{
	setBlockAt(localCoordinate.x, localCoordinate.y, localCoordinate.z, blockID, overwrite);
}

void Voxel::ChunkSection::setBlockAt(const glm::ivec3 & localCoordinate, const Block::BLOCK_ID blockID, const glm::uvec3 & color, const bool overwrite)
{
	setBlockAt(localCoordinate.x, localCoordinate.y, localCoordinate.z, blockID, color, overwrite);
}

void Voxel::ChunkSection::setBlockAt(const glm::ivec3 & localCoordinate, const Block::BLOCK_ID blockID, const glm::vec3 & color, const bool overwrite)
{
	setBlockAt(localCoordinate.x, localCoordinate.y, localCoordinate.z, blockID, color, overwrite);
}

void Voxel::ChunkSection::setBlockAt(const int x, const int y, const int z, const Block::BLOCK_ID blockID, const bool overwrite)
{
	//Todo: Specify the color of the block when placing it.
	unsigned int index = localBlockXYZToIndex(x, y, z);
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
			else
			{
				// setting block.
				if (overwrite)
				{
					if (blocks.at(index)->getBlockID() == Block::BLOCK_ID::AIR)
					{
						// cur block is air
						nonAirBlockSize++;
					}

					// overwrite existing block
					blocks.at(index)->setBlockID(blockID);

				}
			}
		}
	}
	else
	{
		return;
	}
}

void Voxel::ChunkSection::setBlockAt(const int x, const int y, const int z, const Block::BLOCK_ID blockID, const glm::uvec3 & color, const bool overwrite)
{
	unsigned int index = localBlockXYZToIndex(x, y, z);
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
				blocks.at(index)->setColorU3(color);

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
			else
			{
				// setting block.
				if (overwrite)
				{
					if (blocks.at(index)->getBlockID() == Block::BLOCK_ID::AIR)
					{
						// cur block is air
						nonAirBlockSize++;
					}

					// overwrite existing block
					blocks.at(index)->setBlockID(blockID);
					blocks.at(index)->setColorU3(color);
				}
			}
		}
	}
	else
	{
		return;
	}
}

void Voxel::ChunkSection::setBlockAt(const int x, const int y, const int z, const Block::BLOCK_ID blockID, const glm::vec3 & color, const bool overwrite)
{
	unsigned int index = localBlockXYZToIndex(x, y, z);
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
				blocks.at(index)->setColor(color);

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
			else
			{
				// setting block.
				if (overwrite)
				{
					if (blocks.at(index)->getBlockID() == Block::BLOCK_ID::AIR)
					{
						// cur block is air
						nonAirBlockSize++;
					}

					// overwrite existing block
					blocks.at(index)->setBlockID(blockID);
					blocks.at(index)->setColor(color);
				}
			}
		}
	}
	else
	{
		return;
	}
}

int Voxel::ChunkSection::getLocalTopY(const int localX, const int localZ)
{
	int maxY = -1;

	for (int i = 0; i < Constant::CHUNK_SECTION_HEIGHT; i++)
	{
		auto index = localBlockXYZToIndex(localX, i, localZ);
		if (blocks.at(index))
		{
			maxY = std::max(maxY, i);
		}
		else
		{
			continue;
		}
	}

	return maxY;
}

glm::vec3 Voxel::ChunkSection::getWorldPosition()
{
	return worldPosition;
}

int Voxel::ChunkSection::getTotalNonAirBlockSize()
{
	return nonAirBlockSize;
}
