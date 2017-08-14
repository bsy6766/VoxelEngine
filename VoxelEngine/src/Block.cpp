#include "Block.h"

#include <Color.h>
#include <glm/gtx/transform.hpp>

using namespace Voxel;

Block::Block()
	: position(0)
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
	worldCoordinate.x += 16.0f * chunkSectionPosition.x;
	worldCoordinate.y += 16.0f * chunkSectionPosition.y;
	worldCoordinate.z += 16.0f * chunkSectionPosition.z;

	// Not sure if I would need local position. TODO: add local position if needed
	
	// Calculate position of block in the world
	this->position = glm::vec3(localCoordinate) + 0.5f;

	// update matrix
	matrix = glm::translate(glm::mat4(1.0f), this->position);

	color = Color::getRandomColor();

	id = BLOCK_ID::GRASS;

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