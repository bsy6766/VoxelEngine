#include "Block.h"

#include <Color.h>
#include <glm/gtx/transform.hpp>
#include <ChunkUtil.h>
#include <Utility.h>

using namespace Voxel;

Block::Block()
	: worldPosition(0)
	//, localPosition(0)
	, localCoordinate(0)
	, worldCoordinate(0)
	, color(Color::WHITE)
	, matrix(1.0f)
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
	localCoordinate = position;
	// World coordinate
	worldCoordinate = localCoordinate;
	worldCoordinate.x += Constant::CHUNK_SECTION_WIDTH * chunkSectionPosition.x;
	worldCoordinate.y += Constant::CHUNK_SECTION_HEIGHT * chunkSectionPosition.y;
	worldCoordinate.z += Constant::CHUNK_SECTION_LENGTH * chunkSectionPosition.z;

	// Not sure if I would need local position. TODO: add local position if needed
	this->localPosition = glm::vec3(localCoordinate) + 0.5f;

	// Calculate world position of block in the world
	this->worldPosition = localPosition;
	this->worldPosition.x += (static_cast<float>(chunkSectionPosition.x) * Constant::CHUNK_SECTION_WIDTH);
	this->worldPosition.y += (static_cast<float>(chunkSectionPosition.y) * Constant::CHUNK_SECTION_HEIGHT);
	this->worldPosition.z += (static_cast<float>(chunkSectionPosition.z) * Constant::CHUNK_SECTION_LENGTH);

	// update matrix
	matrix = glm::translate(glm::mat4(1.0f), this->worldPosition);

	color = Color::getRandomColor();

	auto rand = Utility::Random::randomInt100();
	if (rand <= 100)
	{
		id = BLOCK_ID::GRASS;
	}
	else
	{
		id = BLOCK_ID::AIR;
	}

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