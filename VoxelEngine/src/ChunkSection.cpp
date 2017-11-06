#include "ChunkSection.h"
#include <iostream>
#include <ChunkUtil.h>
#include <Color.h>
#include <Biome.h>
#include <Application.h>
#include <HeightMap.h>
#include <Game.h>
#include <World.h>
#include <Region.h>
#include <glm/gtx/compatibility.hpp>

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

ChunkSection * Voxel::ChunkSection::createWithFill(const int x, const int y, const int z, const glm::vec3 & chunkPosition)
{
	ChunkSection* newChunkSection = new ChunkSection();
	//std::cout << "[ChunkSection] Creating new chunk section at (" << x << ", " << y << ", " << z << ")...\n";
	if (newChunkSection->initWithFill(x, y, z, chunkPosition))
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

ChunkSection * Voxel::ChunkSection::createWithHeightMap(const int x, const int y, const int z, const glm::vec3 & chunkPosition, const std::vector<std::vector<float>>& heightMap)
{
	ChunkSection* newChunkSection = new ChunkSection();
	//std::cout << "[ChunkSection] Creating new chunk section at (" << x << ", " << y << ", " << z << ")...\n";
	if (newChunkSection->initWithHeightMap(x, y, z, chunkPosition, heightMap))
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

ChunkSection * Voxel::ChunkSection::createWithValues(const int x, const int y, const int z, const glm::vec3 & chunkPosition, const std::vector<std::vector<float>>& eMap, const std::vector<std::vector<float>>& tMap, const std::vector<std::vector<float>>& mMap)
{
	ChunkSection* newChunkSection = new ChunkSection();
	//std::cout << "[ChunkSection] Creating new chunk section at (" << x << ", " << y << ", " << z << ")...\n";
	if (newChunkSection->initWithValues(x, y, z, chunkPosition, eMap, tMap, mMap))
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

ChunkSection * Voxel::ChunkSection::createWithColor(const int x, const int y, const int z, const glm::vec3 & chunkPosition, const glm::uvec3 & color)
{
	ChunkSection* newChunkSection = new ChunkSection();
	//std::cout << "[ChunkSection] Creating new chunk section at (" << x << ", " << y << ", " << z << ")...\n";
	if (newChunkSection->initWithColor(x, y, z, chunkPosition, color))
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

ChunkSection * Voxel::ChunkSection::createWithRegionColor(const int x, const int y, const int z, const glm::vec3 & chunkPosition, const std::vector<unsigned int>& blockRegion)
{
	ChunkSection* newChunkSection = new ChunkSection();
	//std::cout << "[ChunkSection] Creating new chunk section at (" << x << ", " << y << ", " << z << ")...\n";
	if (newChunkSection->initWithRegionColor(x, y, z, chunkPosition, blockRegion))
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

bool Voxel::ChunkSection::initWithFill(const int x, const int y, const int z, const glm::vec3 & chunkPosition)
{
	position = glm::ivec3(x, y, z);

	// calculate world position. Only need to calculate Y.
	worldPosition = chunkPosition;
	worldPosition.y = (static_cast<float>(y) + 0.5f) * static_cast<float>(Constant::CHUNK_SECTION_HEIGHT);

	//std::cout << "[ChunkSection] World position (" << worldPosition.x << ", " << worldPosition.y << ", " << worldPosition.z << ")...\n";

	// Fill vector in order of width(x), length(z) and then height(y)
	//std::cout << "[ChunkSection] Generating blocks...\n";
	//auto color = Color::getRandomColor();

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
					//newBlock->setColor(color);
				}
				else
				{
					return false;
				}
			}
		}
	}

	//auto size = sizeof(Block);
	//std::cout << "size = \n";

	return true;
}

bool Voxel::ChunkSection::initWithHeightMap(const int x, const int y, const int z, const glm::vec3 & chunkPosition, const std::vector<std::vector<float>>& heightMap)
{
	position = glm::ivec3(x, y, z);

	// calculate world position. Only need to calculate Y.
	worldPosition = chunkPosition;
	worldPosition.y = (static_cast<float>(y) + 0.5f) * static_cast<float>(Constant::CHUNK_SECTION_HEIGHT);

	blocks.clear();
	blocks.resize(Constant::TOTAL_BLOCKS, nullptr);

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
			float e = heightMap.at(blockX).at(blockZ);
			int heightY = static_cast<int>(e * 60.0f) + 30;

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

bool Voxel::ChunkSection::initWithValues(const int x, const int y, const int z, const glm::vec3 & chunkPosition, const std::vector<std::vector<float>>& eMap, const std::vector<std::vector<float>>& tMap, const std::vector<std::vector<float>>& mMap)
{
	position = glm::ivec3(x, y, z);

	// calculate world position. Only need to calculate Y.
	worldPosition = chunkPosition;
	worldPosition.y = (static_cast<float>(y) + 0.5f) * static_cast<float>(Constant::CHUNK_SECTION_HEIGHT);

	blocks.clear();
	blocks.resize(Constant::TOTAL_BLOCKS, nullptr);

	int yStart = y * Constant::CHUNK_SECTION_HEIGHT;
	//auto color = Color::getRandomColor();
	//auto color = Color::GRASS;

	for (int blockX = 0; blockX < Constant::CHUNK_SECTION_WIDTH; blockX++)
	{
		for (int blockZ = 0; blockZ < Constant::CHUNK_SECTION_WIDTH; blockZ++)
		{
			int localY = 0;
			float e = eMap.at(blockX).at(blockZ);
			int heightY = static_cast<int>(e * 60.0f) + 30;

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

					auto biome = Biome::getBiomeType(tMap.at(blockX).at(blockZ), mMap.at(blockX).at(blockZ), e);
					if (biome != Voxel::BiomeType::ERROR)
					{
						switch (biome)
						{
						case Voxel::BiomeType::OCEAN:
							newBlock->setColorU3(Color::OCEAN);
							break;
						case Voxel::BiomeType::TUNDRA:
							newBlock->setColorU3(Color::TUNDRA);
							break;
						case Voxel::BiomeType::GRASS_DESERT:
							newBlock->setColorU3(Color::GRASS_DESERT);
							break;
						case Voxel::BiomeType::TAIGA:
							newBlock->setColorU3(Color::TAIGA);
							break;
						case Voxel::BiomeType::DESERT:
							newBlock->setColorU3(Color::DESERT);
							break;
						case Voxel::BiomeType::WOODS:
							newBlock->setColorU3(Color::WOODS);
							break;
						case Voxel::BiomeType::FOREST:
							newBlock->setColorU3(Color::FOREST);
							break;
						case Voxel::BiomeType::SWAMP:
							newBlock->setColorU3(Color::SWAMP);
							break;
						case Voxel::BiomeType::SAVANNA:
							newBlock->setColorU3(Color::SAVANNA);
							break;
						case Voxel::BiomeType::SEASONAL_FOREST:
							newBlock->setColorU3(Color::SEASONAL_FOREST);
							break;
						case Voxel::BiomeType::RAIN_FOREST:
							newBlock->setColorU3(Color::RAIN_FOREST);
							break;
						default:
							newBlock->setColorRGB(28, 192, 11);
							break;
						}
					}
					else
					{
						throw std::runtime_error("Biome type is error");
					}

					//newBlock->setColor(color);
					blocks.at(localBlockXYZToIndex(blockX, localY, blockZ)) = newBlock;

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

bool Voxel::ChunkSection::initWithColor(const int x, const int y, const int z, const glm::vec3 & chunkPosition, const glm::uvec3 & color)
{
	position = glm::ivec3(x, y, z);

	// calculate world position. Only need to calculate Y.
	worldPosition = chunkPosition;
	worldPosition.y = (static_cast<float>(y) + 0.5f) * static_cast<float>(Constant::CHUNK_SECTION_HEIGHT);

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

					newBlock->setColorU3(color);
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

bool Voxel::ChunkSection::initWithRegionColor(const int x, const int y, const int z, const glm::vec3 & chunkPosition, const std::vector<unsigned int>& blockRegion)
{
	position = glm::ivec3(x, y, z);

	// calculate world position. Only need to calculate Y.
	worldPosition = chunkPosition;
	worldPosition.y = (static_cast<float>(y) + 0.5f) * static_cast<float>(Constant::CHUNK_SECTION_HEIGHT);

	const int single = 1;
	const int multiple = Constant::CHUNK_SECTION_WIDTH * Constant::CHUNK_SECTION_LENGTH;

	glm::uvec3 color;

	auto size = blockRegion.size();

	if (size == single)
	{
		if (blockRegion.front() == -1)
		{
			color = glm::uvec3(255);
		}
		else
		{
			color = Application::getInstance().getGame()->getWorld()->getRegion(blockRegion.front())->randColor;
		}
	}

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

					if (size == single)
					{
						newBlock->setColorU3(color);

					}
					else if (size == multiple)
					{
						auto regionId = blockRegion.at(localBlockXZToMapIndex(k, j));
						if (regionId == -1)
						{
							newBlock->setColorRGB(255, 255, 255);
							//throw std::runtime_error("Region ID is invalid for this block. Fix this");
						}
						else
						{
							color = Application::getInstance().getGame()->getWorld()->getRegion(regionId)->randColor;
							newBlock->setColorU3(color);
						}
					}
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

glm::vec3 Voxel::ChunkSection::getWorldPosition()
{
	return worldPosition;
}

int Voxel::ChunkSection::getTotalNonAirBlockSize()
{
	return nonAirBlockSize;
}
