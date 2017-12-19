// pch
#include "PreCompiled.h"

#include "Block.h"

// voxel
#include "Color.h"
#include "ChunkUtil.h"
#include "Utility.h"

using namespace Voxel;

Block::Block()
	:  worldCoordinate(0)
	, r(0)
	, g(0)
	, b(0)
{}

Block::~Block()
{}

Block * Voxel::Block::create(const glm::ivec3& position, const glm::ivec3& chunkSectionPosition)
{
	Block* newBlock = new Block();
	if (newBlock->init(position, chunkSectionPosition))
	{
		return newBlock;
	}
	else
	{
		delete newBlock;
		return nullptr;
	}
}

bool Voxel::Block::init(const glm::ivec3& position, const glm::ivec3& chunkSectionPosition)
{
	// Local coordinate
	//localCoordinate = position;
	// World coordinate
	//worldCoordinate = localCoordinate;
	worldCoordinate = position;
	worldCoordinate.x += Constant::CHUNK_SECTION_WIDTH * chunkSectionPosition.x;
	worldCoordinate.y += Constant::CHUNK_SECTION_HEIGHT * chunkSectionPosition.y;
	worldCoordinate.z += Constant::CHUNK_SECTION_LENGTH * chunkSectionPosition.z;

	// Not sure if I would need local position. TODO: add local position if needed
	//this->localPosition = glm::vec3(localCoordinate) + 0.5f;

	// Calculate world position of block in the world
	//this->worldPosition = localPosition;
	//this->worldPosition = glm::vec3(position) + 0.5f;
	//this->worldPosition.x += (static_cast<float>(chunkSectionPosition.x) * Constant::CHUNK_SECTION_WIDTH);
	//this->worldPosition.y += (static_cast<float>(chunkSectionPosition.y) * Constant::CHUNK_SECTION_HEIGHT);
	//this->worldPosition.z += (static_cast<float>(chunkSectionPosition.z) * Constant::CHUNK_SECTION_LENGTH);

	id = BLOCK_ID::GRASS;

	auto color = Color::GRASS;
	r = color.r;
	g = color.g;
	b = color.b;

	return true;
}

bool Voxel::Block::isTransparent()
{
	if (id == BLOCK_ID::AIR)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Voxel::Block::isEmpty()
{
	if (id == BLOCK_ID::AIR)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Voxel::Block::isCollidable()
{
	if (id == BLOCK_ID::AIR)
	{
		return false;
	}
	else
	{
		// for now, everything is collidable.
		// todo, give exceptions (like flower, lowgrass, etc)
		return true;
	}
}

bool Voxel::Block::isSolid()
{
	return true;
}

void Voxel::Block::setColor(const glm::vec3 & color)
{
	r = static_cast<unsigned char>(color.r * 255.0f);
	g = static_cast<unsigned char>(color.g * 255.0f);
	b = static_cast<unsigned char>(color.b * 255.0f);
}

void Voxel::Block::setColorRGB(const unsigned char r, const unsigned char g, const unsigned char b)
{
	this->r = r;
	this->g = g;
	this->b = b;
}

void Voxel::Block::setColorU3(const glm::uvec3 & color)
{
	this->r = static_cast<unsigned char>(color.r);
	this->g = static_cast<unsigned char>(color.g);
	this->b = static_cast<unsigned char>(color.b);
}

glm::vec3 Voxel::Block::getColor3()
{
	return glm::vec3(static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f, static_cast<float>(b) / 255.0f);
}

glm::vec4 Voxel::Block::getColor4()
{
	return glm::vec4(getColor3(), 1.0f);
}

glm::ivec3 Voxel::Block::getWorldCoordinate()
{
	return worldCoordinate;
}

glm::vec3 Voxel::Block::getWorldPosition()
{
	return glm::vec3(worldCoordinate) + 0.5f;
}

glm::ivec3 Voxel::Block::getLocalCoordinate()
{
	int x = worldCoordinate.x;
	int y = worldCoordinate.y;
	int z = worldCoordinate.z;

	int localX = x % Constant::CHUNK_SECTION_WIDTH;

	if (localX < 0)
	{
		localX += Constant::CHUNK_SECTION_WIDTH;
	}
	else if (localX >= Constant::CHUNK_SECTION_WIDTH)
	{
		localX -= Constant::CHUNK_SECTION_WIDTH;
	}

	int localY = y % Constant::CHUNK_SECTION_HEIGHT;

	if (localY < 0)
	{
		localY += Constant::CHUNK_SECTION_HEIGHT;
	}
	else if (localY >= Constant::CHUNK_SECTION_HEIGHT)
	{
		localY -= Constant::CHUNK_SECTION_HEIGHT;
	}

	int localZ = z % Constant::CHUNK_SECTION_LENGTH;

	if (localZ < 0)
	{
		localZ += Constant::CHUNK_SECTION_LENGTH;
	}
	else if (localZ >= Constant::CHUNK_SECTION_LENGTH)
	{
		localZ -= Constant::CHUNK_SECTION_LENGTH;
	}

	return glm::ivec3(localX, localY, localZ);
}

glm::vec3 Voxel::Block::getLocalPosition()
{
	return glm::vec3(getLocalCoordinate()) + 0.5f;
}

glm::vec3 Voxel::Block::getMeshPosition()
{
	int x = worldCoordinate.x;
	int y = worldCoordinate.y;
	int z = worldCoordinate.z;

	int localX = x % Constant::CHUNK_SECTION_WIDTH;

	if (localX < 0)
	{
		localX += Constant::CHUNK_SECTION_WIDTH;
	}
	else if (localX >= Constant::CHUNK_SECTION_WIDTH)
	{
		localX -= Constant::CHUNK_SECTION_WIDTH;
	}
	
	int localZ = z % Constant::CHUNK_SECTION_LENGTH;

	if (localZ < 0)
	{
		localZ += Constant::CHUNK_SECTION_LENGTH;
	}
	else if (localZ >= Constant::CHUNK_SECTION_LENGTH)
	{
		localZ -= Constant::CHUNK_SECTION_LENGTH;
	}

	return glm::vec3(glm::ivec3(localX, worldCoordinate.y, localZ)) + 0.5f;
}

Block::BLOCK_ID Voxel::Block::getBlockID()
{
	return id;
}

void Voxel::Block::setBlockID(const BLOCK_ID blockID)
{
	this->id = blockID;
	//setColorU3(Color::getColorU3FromBlockID(blockID));
}

Shape::AABB Voxel::Block::getBoundingBox()
{
	return Shape::AABB(this->getWorldPosition(), glm::vec3(1.0f));
}




Voxel::PlantBlock::PlantBlock()
	: Block()
{
}

PlantBlock * Voxel::PlantBlock::create(const glm::ivec3 & position, const glm::ivec3 & chunkSectionPosition)
{
	PlantBlock* newPlantBlock = new PlantBlock();
	if (newPlantBlock->init(position, chunkSectionPosition))
	{
		return newPlantBlock;
	}
	else
	{
		delete newPlantBlock;
		return nullptr;
	}
}

bool Voxel::PlantBlock::isSolid()
{
	return false;
}
