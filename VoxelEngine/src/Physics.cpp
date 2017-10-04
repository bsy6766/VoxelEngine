#include "Physics.h"
#include <iostream>
#include <Player.h>
#include <Block.h>

using namespace Voxel;

const unsigned int Physics::X_AXIS = 0;
const unsigned int Physics::Z_AXIS = 1;

const float Physics::Gravity = 9.80665f;
const float Physics::GravityModifier = 0.1f;

Voxel::Physics::Physics()
{
}

Voxel::Physics::~Physics()
{
}

void Voxel::Physics::applyGravity(Player * player, const float delta)
{
	if (player->isFlying()) return;
	if (player->isOnGround()) return; 
	
	auto pos = player->getPosition();

	float fallDuration = player->getFallDuration();

	float fallDistance = Physics::Gravity * (fallDuration + delta) * Physics::GravityModifier;

	if (fallDistance > 6.0f)
	{
		fallDistance = 6.0f;
	}


	std::cout << "fd = " << fallDistance << std::endl;
	std::cout << "ft = " << fallDuration + delta << std::endl;

	pos.y -= fallDistance;

	std::cout << "pos.y = " << pos.y << std::endl;

	player->setPosition(pos, true);

	player->setFallDuration(fallDuration + delta);
	player->setFallDistance(fallDistance + player->getFallDistance());
}

Geometry::AABB Voxel::Physics::getIntersectingBoundingBox(const Geometry::AABB & A, const Geometry::AABB & B)
{
	// Get min and max
	auto aMin = A.getMin();
	auto aMax = A.getMax();

	auto bMin = B.getMin();
	auto bMax = B.getMax();

	// Get intersection origin
	auto iMin = glm::vec3(0);
	iMin.x = (aMin.x > bMin.x) ? aMin.x : bMin.x;
	iMin.y = (aMin.y > bMin.y) ? aMin.y : bMin.y;
	iMin.z = (aMin.z > bMin.z) ? aMin.z : bMin.z;

	// get Size
	auto iSize = glm::vec3(0);

	if (aMax.x < bMax.x)
	{
		iSize.x = aMax.x - iMin.x;
	}
	else
	{
		iSize.x = bMax.x - iMin.x;
	}

	if (aMax.y < bMax.y)
	{
		iSize.y = aMax.y - iMin.y;
	}
	else
	{
		iSize.y = bMax.y - iMin.y;
	}

	if (aMax.z < bMax.z)
	{
		iSize.z = aMax.z - iMin.z;
	}
	else
	{
		iSize.z = bMax.z - iMin.z;
	}

	return Geometry::AABB(iMin + (iSize * 0.5f), iSize);
}

void Voxel::Physics::resolvePlayerAndBlockCollision(Player * player, const std::vector<Block*>& collidableBlocks)
{
	// current position
	auto playerPos = player->getPosition();
	// position that player tried to move
	auto playerNextPos = player->getNextPosition();

	// get moved distance
	auto movedDist = playerNextPos - playerPos;

	// Resolving position
	auto resolvingPos = playerPos;

	bool resolved = false;

	// resolve for x and z axis first
	for (int i = 0; i < 2; i++)
	{
		if (i == Physics::X_AXIS)
		{
			resolvingPos.x = playerNextPos.x;
		}
		else if(i == Physics::Z_AXIS)
		{ 
			resolvingPos.z = playerNextPos.z;
		}

		// Get player boundinb box based on resolving position
		auto pBB = player->getBoundingBox(resolvingPos);

		// iterate blocks and resolve
		for (auto block : collidableBlocks)
		{
			auto blockBB = block->getBoundingBox();

			if (blockBB.doesIntersectsWith(pBB))
			{
				auto intersectingAABB = getIntersectingBoundingBox(pBB, blockBB);

				// Check if any axis of intersecting AABB is zero.
				if (!intersectingAABB.isZero(false))
				{
					// All axis in intersecting AABB is not zero
					if (i == Physics::X_AXIS)
					{
						if (movedDist.x > 0.0f)
						{
							// Moved to east (positive x)
							resolvingPos.x -= intersectingAABB.getSize().x;
							resolved = true;
						}
						else if (movedDist.x < 0.0f)
						{
							// Moved to west (negative x)
							resolvingPos.x += intersectingAABB.getSize().x;
							resolved = true;
						}
					}
					else if (i == Physics::Z_AXIS)
					{

					}

				}
			}
		}
	}

	if (resolved)
	{
		player->setNextPosition(resolvingPos);
	}

	player->applyNextPosition();
}
