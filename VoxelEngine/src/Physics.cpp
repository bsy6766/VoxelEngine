#include "Physics.h"
#include <iostream>
#include <Player.h>
#include <Block.h>

using namespace Voxel;

const unsigned int Physics::X_AXIS = 0;
const unsigned int Physics::Z_AXIS = 1;

const float Physics::Gravity = 9.80665f;
const float Physics::GravityModifier = 5.0f;

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

	float fallDistance = Physics::Gravity * (fallDuration + delta) * Physics::GravityModifier * delta;

	if (fallDistance > 5.0f)
	{
		fallDistance = 5.0f;
	}

	std::cout << "fd = " << fallDistance << std::endl;
	std::cout << "ft = " << fallDuration + delta << std::endl;

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

void Voxel::Physics::resolveAutoJump(Player * player, const std::vector<Block*>& collidableBlocks)
{
	// current position
	auto playerPos = player->getPosition();
	// position that player tried to move
	auto playerNextPos = player->getNextPosition();
	
	// Resolving position
	auto resolvingPos = playerNextPos;

	// init player bounding box
	auto pBB = player->getBoundingBox(resolvingPos);

	// First, check if player was on ground and collided with block on the side. And if that block doesn't have any other block above, move player up (auto jump)
	for (auto block : collidableBlocks)
	{
		auto blockBB = block->getBoundingBox();

		if (blockBB.doesIntersectsWith(pBB))
		{
			auto intersectingAABB = getIntersectingBoundingBox(pBB, blockBB);
			auto iSize = intersectingAABB.getSize();

			if (iSize.y == blockBB.getSize().y)
			{
				// block's bounding box y axis is in player's bounding box y axis
				if (player->isOnGround())
				{
					// player isn't in mid airs
					if (pBB.getMin().y == blockBB.getMin().y)
					{
						auto upResolvingPos = resolvingPos;
						upResolvingPos.y += 1.0f;

						auto upPBB = player->getBoundingBox(upResolvingPos);

						bool canAutoJump = true;

						for (auto upBlock : collidableBlocks)
						{
							auto upBlockBB = upBlock->getBoundingBox();
							if (upBlockBB.doesIntersectsWith(upPBB))
							{
								auto iBB = getIntersectingBoundingBox(upPBB, upBlockBB);
								if (iBB.getSize().y > 0)
								{
									canAutoJump = false;
									break;
								}
							}
						}

						if (canAutoJump)
						{
							std::cout << "auto jump. p.y = " << playerPos.y << ", np.y = " << playerNextPos.y << ", sizeY = " << iSize.y << std::endl;
							player->autoJump(iSize.y);
							player->runJumpCooldown();
							//autoJumped = true;
							break;
						}
						else
						{
							// there is a block above current block. can't auto jump
						}
					}
				}
			}
		}
	}
}

bool Voxel::Physics::resolvePlayerXAndBlockCollision(Player * player, glm::vec3& resolvingPos, const glm::vec3& movedDist, const std::vector<Block*>& collidableBlocks)
{
	// Get player boundinb box based on resolving position
	auto pBB = player->getBoundingBox(resolvingPos);

	bool resolved = false;

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
				if (movedDist.x > 0.0f)
				{
					// Moved to east (positive x)
					auto bPos = block->getWorldCoordinate();
					std::cout << "bPos = (" << bPos.x << ", " << bPos.y << ", " << bPos.z << ")" << std::endl;
					std::cout << "playerY = " << player->getPosition().y << std::endl;
					std::cout << "iSize.y = " << intersectingAABB.getSize().y << std::endl;
					resolvingPos.x -= (intersectingAABB.getSize().x);
					resolved = true;
					pBB = player->getBoundingBox(resolvingPos);
					std::cout << "Player moved east (positive x). resolved" << std::endl;
				}
				else if (movedDist.x < 0.0f)
				{
					// Moved to west (negative x)
					auto bPos = block->getWorldCoordinate();
					std::cout << "bPos = (" << bPos.x << ", " << bPos.y << ", " << bPos.z << ")" << std::endl;
					std::cout << "playerY = " << player->getPosition().y << std::endl;
					std::cout << "iSize.y = " << intersectingAABB.getSize().y << std::endl;
					resolvingPos.x += (intersectingAABB.getSize().x);
					resolved = true;
					pBB = player->getBoundingBox(resolvingPos);
					std::cout << "Player moved west (negative x). resolved" << std::endl;
				}
			}
		}
	}

	return resolved;
}

bool Voxel::Physics::resolvePlayerZAndBlockCollision(Player * player, glm::vec3& resolvingPos, const glm::vec3& movedDist, const std::vector<Block*>& collidableBlocks)
{
	// Get player boundinb box based on resolving position
	auto pBB = player->getBoundingBox(resolvingPos);

	bool resolved = false;

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
				if (movedDist.z > 0.0f)
				{
					// Moved to south (positive z)
					resolvingPos.z -= (intersectingAABB.getSize().z);
					resolved = true;
					pBB = player->getBoundingBox(resolvingPos);
					std::cout << "Player moved south (positive z). resolved" << std::endl;
				}
				else if (movedDist.z < 0.0f)
				{
					// Moved to north (negative z)
					resolvingPos.z += (intersectingAABB.getSize().z);
					resolved = true;
					pBB = player->getBoundingBox(resolvingPos);
					std::cout << "Player moved north (negative z). resolved" << std::endl;
				}
			}
		}
	}

	return resolved;
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
					continue;
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
						std::cout << "Player moved up. resolved" << std::endl;
					}
					else if (movedDist.y < 0.0f)
					{
						// moved down
						std::cout << "position y = " << resolvingPos.y << std::endl;
						resolvingPos.y += (intersectingAABB.getSize().y);
						std::cout << "resolving y = " << resolvingPos.y << std::endl;
						resolved = true;
						inMidAir = false;
						pBB = player->getBoundingBox(resolvingPos);
						player->setOnGround(true);
						std::cout << "Player moved down. resolved" << std::endl;
					}
				}
			}
		}
	}


	bool resolvedXZ = false;

	if (inMidAir == false)
	{
		resolvingPos.x = playerPos.x;
		resolvingPos.z = playerPos.z;

		glm::vec3 absMovedDist = glm::abs(movedDist);

		if (absMovedDist.x != 0 || absMovedDist.z != 0)
		{
			if (absMovedDist.x >= absMovedDist.z)
			{
				//std::cout << "0" << std::endl;
				resolvingPos.x = playerNextPos.x;
				bool result = false;
				result = resolvePlayerXAndBlockCollision(player, resolvingPos, movedDist, collidableBlocks);
				if (result) resolvedXZ = true;
				resolvingPos.z = playerNextPos.z;
				result = resolvePlayerZAndBlockCollision(player, resolvingPos, movedDist, collidableBlocks);
				if (result) resolvedXZ = true;
			}
			else
			{
				// resolve z first
				resolvingPos.z = playerNextPos.z;
				bool result = false;
				result = resolvePlayerZAndBlockCollision(player, resolvingPos, movedDist, collidableBlocks);
				if (result) resolvedXZ = true;
				resolvingPos.x = playerNextPos.x;
				result = resolvePlayerXAndBlockCollision(player, resolvingPos, movedDist, collidableBlocks);
				if (result) resolvedXZ = true;
				//std::cout << "1" << std::endl;
			}
		}
		/*
		if (movedDist.x != 0 || movedDist.z != 0)
		{
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
									std::cout << "Player moved east (positive x). resolved" << std::endl;
								}
								else if (movedDist.x < 0.0f)
								{
									// Moved to west (negative x)
									resolvingPos.x += (intersectingAABB.getSize().x + pad);
									resolved = true;
									pBB = player->getBoundingBox(resolvingPos);
									std::cout << "Player moved west (negative x). resolved" << std::endl;
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
									std::cout << "Player moved south (positive z). resolved" << std::endl;
								}
								else if (movedDist.z < 0.0f)
								{
									// Moved to north (negative z)
									resolvingPos.z += (intersectingAABB.getSize().z + pad);
									resolved = true;
									pBB = player->getBoundingBox(resolvingPos);
									std::cout << "Player moved north (negative z). resolved" << std::endl;
								}
							}
						}
					}
				}
			}

		}
	
		*/
	}

	if (inMidAir)
	{
		player->setOnGround(false);
	}

	if (resolved || resolvedXZ)
	{
		player->setNextPosition(resolvingPos);
	}
}
