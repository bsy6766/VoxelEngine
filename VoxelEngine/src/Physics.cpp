#include "Physics.h"
#include <iostream>
#include <Player.h>
#include <Block.h>

using namespace Voxel;

const unsigned int Physics::X_AXIS = 0;
const unsigned int Physics::Z_AXIS = 1;

const float Physics::Gravity = 9.80665f;
const float Physics::GravityModifier = 0.75f;

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
	
	auto pos = player->getNextPosition();

	float fallDuration = player->getFallDuration();

	float fallDistance = Physics::Gravity * (fallDuration + delta) * Physics::GravityModifier;

	if (fallDistance > 6.0f)
	{
		fallDistance = 6.0f;
	}

	//std::cout << "fd = " << fallDistance << std::endl;
	//std::cout << "ft = " << fallDuration + delta << std::endl;

	pos.y -= fallDistance;

	//std::cout << "pos.y = " << pos.y << std::endl;

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
	auto resolvingPos = playerNextPos;

	bool resolved = false;
	const float pad = 0;

	// resolve y.

	// init player bounding box
	auto pBB = player->getBoundingBox(resolvingPos);

	bool autoJumped = false;
	// First, check if player was on ground and collided with block on the side. And if that block doesn't have any other block above, move player up (auto jump)
	for (auto block : collidableBlocks)
	{
		auto blockBB = block->getBoundingBox();

		if (blockBB.doesIntersectsWith(pBB))
		{
			auto intersectingAABB = getIntersectingBoundingBox(pBB, blockBB);
			float sizeY = intersectingAABB.getSize().y;

			if (sizeY == blockBB.getSize().y)
			{
				// block's bounding box y axis is in player's bounding box y axis
				if (player->isOnGround())
				{
					// player isn't in mid air
					if (pBB.getMin().y == blockBB.getMin().y)
					{
						// player and block is on same level
						auto bPos = block->getWorldCoordinate();
						bool result = false;
						for (auto upBlock : collidableBlocks)
						{
							auto ubPos = block->getWorldCoordinate();
							if (glm::abs(bPos.y - ubPos.y) == 1)
							{
								// There is a block above current block
								result = true;
								break;
							}
						}

						if (result)
						{
							// there is a block above current block. can't auto jump
						}
						else
						{
							// There is no other block above current block. 
							player->jumpInstant(sizeY);
							autoJumped = true;
							break;
						}
					}
				}
			}
		}
	}

	if (autoJumped)
	{
		resolvingPos = playerPos;
		playerPos = player->getPosition();
		playerNextPos = player->getNextPosition();
	}
	else
	{
		resolvingPos.y = playerNextPos.y;
	}
	pBB = player->getBoundingBox(resolvingPos);

	// Now iterate blocks again and resolve y again
	bool inMidAir = true;

	for (auto block : collidableBlocks)
	{
		auto blockBB = block->getBoundingBox();

		if (blockBB.doesIntersectsWith(pBB))
		{
			auto intersectingAABB = getIntersectingBoundingBox(pBB, blockBB);
			float sizeY = intersectingAABB.getSize().y;
			if (sizeY == 0.0f)
			{
				// block and player isn't intersecting in y axis. They might touching. Check y.
				if (pBB.getMin().y == blockBB.getMax().y)
				{
					// touching
					inMidAir = false;
				}
				else
				{
					continue;
				}
			}
			else
			{
				// block and player is intersecting in y axis
				if (sizeY == blockBB.getSize().y)
				{
					// block is completly inside of player's bounding box in y axis. skip					
					continue;
				}
				else
				{
					// player and block is intersecting partialy.
					if (movedDist.y > 0.0f)
					{
						// moved up
						resolvingPos.y -= (intersectingAABB.getSize().y);
						resolved = true;
						pBB = player->getBoundingBox(resolvingPos);
					}
					else if (movedDist.y < 0.0f)
					{
						// moved down
						resolvingPos.y += (intersectingAABB.getSize().y);
						resolved = true;
						inMidAir = false;
						pBB = player->getBoundingBox(resolvingPos);
						player->setOnGround(true);
					}
				}
			}
		}
	}
	
	// resolve for x and z axis
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

				// intersecting AABB muse not be zero in all axis
				if (!intersectingAABB.isZero(false))
				{
					if (i == Physics::X_AXIS)
					{
						if (movedDist.x > 0.0f)
						{
							// Moved to east (positive x)
							resolvingPos.x -= (intersectingAABB.getSize().x + pad);
							resolved = true;
							pBB = player->getBoundingBox(resolvingPos);
						}
						else if (movedDist.x < 0.0f)
						{
							// Moved to west (negative x)
							resolvingPos.x += (intersectingAABB.getSize().x + pad);
							resolved = true;
							pBB = player->getBoundingBox(resolvingPos);
						}
					}
					else if (i == Physics::Z_AXIS)
					{
						if (movedDist.z > 0.0f)
						{
							// Moved to south (positive z)
							resolvingPos.z -= (intersectingAABB.getSize().z + pad);
							resolved = true;
							pBB = player->getBoundingBox(resolvingPos);
						}
						else if (movedDist.z < 0.0f)
						{
							// Moved to north (negative z)
							resolvingPos.z += (intersectingAABB.getSize().z + pad);
							resolved = true;
							pBB = player->getBoundingBox(resolvingPos);
						}
					}
				}
			}
		}
	}

	

	if (inMidAir)
	{
		player->setOnGround(false);
	}

	if (resolved)
	{
		player->setNextPosition(resolvingPos);
	}
}
