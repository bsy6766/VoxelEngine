#include "Block.h"

#include <Color.h>
#include <glm/gtx/transform.hpp>
#include <ChunkUtil.h>
#include <Utility.h>

using namespace Voxel;

Block::Block()
	:  worldCoordinate(0)
	//, worldPosition(0)
	//, localPosition(0)
	//, localCoordinate(0)
	//, color(Color::WHITE)
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

Block::BLOCK_ID Voxel::Block::getBlockID()
{
	return this->id;
}

void Voxel::Block::setBlockID(const BLOCK_ID blockID)
{
	this->id = blockID;
	setColorU3(Color::getColorU3FromBlockID(blockID));
}

AABB Voxel::Block::getAABB()
{
	return AABB(this->getWorldPosition(), glm::vec3(1.0f));
}
