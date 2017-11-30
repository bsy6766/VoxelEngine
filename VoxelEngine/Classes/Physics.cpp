#include "Physics.h"

// cpp
#include <iostream>

// glm
#include <glm/gtx/compatibility.hpp>

// voxel
#include "Player.h"
#include "Block.h"

using namespace Voxel;

const unsigned int Physics::X_AXIS = 0;
const unsigned int Physics::Z_AXIS = 1;

const float Physics::Gravity = 9.80665f;
const float Physics::GravityModifier = 3.0f;

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
	if (player->isJumping()) return;
	
	auto pos = player->getNextPosition();

	float fallDuration = player->getFallDuration();

	float fallDistance = Physics::Gravity * (fallDuration + delta) * Physics::GravityModifier * delta;

	if (fallDistance > 5.0f)
	{
		fallDistance = 5.0f;
	}

	//std::cout << "fd = " << fallDistance << std::endl;
	//std::cout << "ft = " << fallDuration + delta << std::endl;

	pos.y -= fallDistance;

	//std::cout << "Applying gravity: " << fallDistance << "\n";

	//std::cout << "pos.y = " << pos.y << std::endl;

	//player->setPosition(pos, true);
	player->setNextPosition(pos);

	player->setFallDuration(fallDuration + delta);
	player->setFallDistance(fallDistance + player->getFallDistance());
}

Shape::AABB Voxel::Physics::getIntersectingBoundingBox(const Shape::AABB & A, const Shape::AABB & B)
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

	return Shape::AABB(iMin + (iSize * 0.5f), iSize);
}

bool Voxel::Physics::resolveAutoJump(Player * player, const std::vector<Block*>& collidableBlocks)
{
	if (collidableBlocks.empty()) return false;

	// position that player tried to move
	auto playerNextPos = player->getNextPosition();
	
	// Resolving position
	auto resolvingPos = playerNextPos;

	// init player bounding box
	auto pBB = player->getBoundingBox(resolvingPos);

	// First, check if player was on ground and collided with block on the side. And if that block doesn't have any other block above, move player up (auto jump)
	for (auto block : collidableBlocks)
	{
		// Get block Bounding box
		auto blockBB = block->getBoundingBox();

		// Check intersection
		if (blockBB.doesIntersectsWith(pBB))
		{
			// Get intersecting Bounding box
			auto intersectingAABB = getIntersectingBoundingBox(pBB, blockBB);
			// Get size
			auto iSize = intersectingAABB.getSize();

			if (iSize.y == blockBB.getSize().y)
			{
				// block's bounding box y axis is in player's bounding box y axis
				if (pBB.getMin().y == blockBB.getMin().y)
				{
					// player is at same level with colliding block
					auto upResolvingPos = resolvingPos;
					upResolvingPos.y += 1.0f;

					// get bounding box with 1 block higher
					auto upPBB = player->getBoundingBox(upResolvingPos);

					bool canAutoJump = true;

					// Iterate blocks again and check again if it collides anything
					for (auto upBlock : collidableBlocks)
					{
						// Get bounding box
						auto upBlockBB = upBlock->getBoundingBox();
						if (upBlockBB.doesIntersectsWith(upPBB))
						{
							// Intersects
							auto iBB = getIntersectingBoundingBox(upPBB, upBlockBB);
							if (iBB.getSize().y > 0)
							{
								// Can't intersect
								canAutoJump = false;
								break;
							}
						}
					}

					if (canAutoJump)
					{
						std::cout << "auto jump. p.y = " << player->getPosition().y << ", np.y = " << playerNextPos.y << ", sizeY = " << iSize.y << std::endl;
						player->autoJump(iSize.y);

						//resolvingPos.y += 1.0f;

						//player->autoJump(resolvingPos);
						player->setOnGround(true);
						player->setJumpState(Player::JumpState::IDLE);
						//autoJumped = true;
 						return true;
					}
					else
					{
						// there is a block above current block. can't auto jump
					}
				}
			}
		}
	}

	return false;
}

bool Voxel::Physics::resolvePlayerXAndBlockCollision(Player * player, glm::vec3& resolvingPos, const glm::vec3& movedDist, const std::vector<Block*>& collidableBlocks)
{
	if (collidableBlocks.empty()) return false;

	// Get player boundinb box based on resolving position
	auto pBB = player->getBoundingBox(resolvingPos);

	bool resolved = false;

	float pad = 0.0f;

	// iterate blocks and resolve
	for (auto block : collidableBlocks)
	{
		auto blockBB = block->getBoundingBox();

		if (blockBB.doesIntersectsWith(pBB))
		{
			auto intersectingAABB = getIntersectingBoundingBox(pBB, blockBB);

			// intersecting AABB muse not be zero in all axis
			if (!intersectingAABB.isAnyZero())
			{
				//auto bPos = block->getWorldCoordinate();
				//std::cout << "bPos = (" << bPos.x << ", " << bPos.y << ", " << bPos.z << ")\n";
				//std::cout << "playerY = " << player->getPosition().y << std::endl;
				//std::cout << "iSize.y = " << intersectingAABB.getSize().y << std::endl;
				//std::cout << "Resolving pos = (" << resolvingPos.x << ", " << resolvingPos.y << ", " << resolvingPos.z << ")\n";

				if (movedDist.x > 0.0f)
				{
					// Moved to east (positive x)
					resolvingPos.x -= (intersectingAABB.getSize().x + pad);
					resolved = true;
					pBB = player->getBoundingBox(resolvingPos);
					//std::cout << "Player moved east (positive x). resolved\n";
				}
				else if (movedDist.x < 0.0f)
				{
					// Moved to west (negative x)
					resolvingPos.x += (intersectingAABB.getSize().x + pad);
					resolved = true;
					pBB = player->getBoundingBox(resolvingPos);
					//std::cout << "Player moved west (negative x). resolved\n";
				}
				//std::cout << "Resolved pos = (" << resolvingPos.x << ", " << resolvingPos.y << ", " << resolvingPos.z << ")\n";
			}
		}
	}

	return resolved;
}

bool Voxel::Physics::resolvePlayerZAndBlockCollision(Player * player, glm::vec3& resolvingPos, const glm::vec3& movedDist, const std::vector<Block*>& collidableBlocks)
{
	if (collidableBlocks.empty()) return false;

	// Get player boundinb box based on resolving position
	auto pBB = player->getBoundingBox(resolvingPos);

	bool resolved = false;

	float pad = 0.0f;

	// iterate blocks and resolve
	for (auto block : collidableBlocks)
	{
		auto blockBB = block->getBoundingBox();

		if (blockBB.doesIntersectsWith(pBB))
		{
			auto intersectingAABB = getIntersectingBoundingBox(pBB, blockBB);

			// intersecting AABB muse not be zero in all axis
			if (!intersectingAABB.isAnyZero())
			{
				//auto bPos = block->getWorldCoordinate();
				//std::cout << "bPos = (" << bPos.x << ", " << bPos.y << ", " << bPos.z << ")\n";
				//std::cout << "playerY = " << player->getPosition().y << std::endl;
				//std::cout << "iSize.y = " << intersectingAABB.getSize().y << std::endl;
				//std::cout << "Resolving pos = (" << resolvingPos.x << ", " << resolvingPos.y << ", " << resolvingPos.z << ")\n";

				if (movedDist.z > 0.0f)
				{
					// Moved to south (positive z)
					resolvingPos.z -= (intersectingAABB.getSize().z + pad);
					resolved = true;
					pBB = player->getBoundingBox(resolvingPos);
					//std::cout << "Player moved south (positive z). resolved\n";
				}
				else if (movedDist.z < 0.0f)
				{
					// Moved to north (negative z)
					resolvingPos.z += (intersectingAABB.getSize().z + pad);
					resolved = true;
					pBB = player->getBoundingBox(resolvingPos);
					//std::cout << "Player moved north (negative z). resolved\n";
				}
				//std::cout << "Resolved pos = (" << resolvingPos.x << ", " << resolvingPos.y << ", " << resolvingPos.z << ")\n";
			}
		}
	}

	return resolved;
}

/*
void Voxel::Physics::resolvePlayerAndBlockCollision(Player * player, const std::vector<Block*>& collidableBlocks)
{
	if (collidableBlocks.empty()) return;

	// current position
	auto playerPos = player->getPosition();
	// position that player tried to move
	auto playerNextPos = player->getNextPosition();

	// get moved distance
	auto movedDist = playerNextPos - playerPos;

	// Resolving position
	auto resolvingPos = glm::vec3(playerPos.x, playerNextPos.y, playerPos.z);

	bool resolved = false;
	
	// init player bounding box
	auto pBB = player->getBoundingBox(resolvingPos);
	
	// Now iterate blocks again and resolve y again
	bool inMidAir = true;

	// Check if player is jumping
	if (!player->isJumping())
	{
		// not jumping
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
						if (!intersectingAABB.isZero(false))
						{
							// player and block is intersecting partialy.
							if (movedDist.y > 0.0f)
							{
								// moved up
								resolvingPos.y -= (intersectingAABB.getSize().y);
								resolved = true;
								pBB = player->getBoundingBox(resolvingPos);
								std::cout << "Player moved up. resolved\n";
							}
							else if (movedDist.y < 0.0f)
							{
								if (pBB.getMin().y == blockBB.getMax().y)
								{
								}
								// moved down


								std::cout << "position y = " << resolvingPos.y << std::endl;
								resolvingPos.y += (intersectingAABB.getSize().y);
								std::cout << "resolving y = " << resolvingPos.y << std::endl;
								resolved = true;
								inMidAir = false;
								pBB = player->getBoundingBox(resolvingPos);
								player->setOnGround(true);
								std::cout << "Player moved down. resolved\n";
							}
						}
					}
				}
			}
		}
	}

	bool resolvedXZ = false;

	if (true)
	{
		resolvingPos.x = playerPos.x;
		resolvingPos.z = playerPos.z;

		glm::vec3 absMovedDist = glm::abs(movedDist);

		if (absMovedDist.x != 0 || absMovedDist.z != 0)
		{
			if (absMovedDist.x >= absMovedDist.z)
			{
				//std::cout << "0\n";
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
				//std::cout << "1\n";
			}
		}
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
*/

void Voxel::Physics::resolvePlayerAndBlockCollisionInXZAxis(Player * player, const std::vector<Block*>& collidableBlocks)
{
	if (collidableBlocks.empty()) return;

	// current position
	auto playerPos = player->getPosition();
	// position that player tried to move
	auto playerNextPos = player->getNextPosition();

	// get moved distance
	auto movedDist = playerNextPos - playerPos;

	// Resolving position
	auto resolvingPos = playerPos;

	bool resolved = false;

	glm::vec3 absMovedDist = glm::abs(movedDist);

	if (absMovedDist.x != 0 || absMovedDist.z != 0)
	{
		if (absMovedDist.x >= absMovedDist.z)
		{
			// resolve x first
			//std::cout << "0\n";

			bool result = false;

			resolvingPos.x = playerNextPos.x;

			result = resolvePlayerXAndBlockCollision(player, resolvingPos, movedDist, collidableBlocks);
			
			if (result) 
				resolved = true;

			resolvingPos.z = playerNextPos.z;

			result = resolvePlayerZAndBlockCollision(player, resolvingPos, movedDist, collidableBlocks);
			
			if (result) 
				resolved = true;
		}
		else
		{
			// resolve z first
			//std::cout << "1\n";
			bool result = false;

			resolvingPos.z = playerNextPos.z;

			result = resolvePlayerZAndBlockCollision(player, resolvingPos, movedDist, collidableBlocks);

			if (result) 
				resolved = true;

			resolvingPos.x = playerNextPos.x;

			result = resolvePlayerXAndBlockCollision(player, resolvingPos, movedDist, collidableBlocks);
			
			if (result) 
				resolved = true;
		}
	}

	if (resolved)
	{
		//std::cout << "XZ resolved from (" << playerNextPos.x << ", " << playerNextPos.y << ", " << playerNextPos.z << ") to (" << resolvingPos.x << ", " << playerNextPos.y << ", " << resolvingPos.z << ")\n";
		player->setResolvedNextPosition(glm::vec3(resolvingPos.x, playerNextPos.y, resolvingPos.z));
	}
}

void Voxel::Physics::resolvePlayerBottomCollision(Player * player, const std::vector<Block*>& collidableBlocks)
{
	if (collidableBlocks.empty()) return;

	// current position
	auto playerPos = player->getPosition();
	// position that player tried to move
	auto playerNextPos = player->getNextPosition();

	// get moved distance
	auto movedDist = playerNextPos - playerPos;

	if (movedDist.y >= 0.0f)
	{
		// player didn't move downward. 
		//std::cout << "Player didn't move downward" << std::endl;
		return;
	}

	// Resolving position
	auto resolvingPos = playerNextPos;

	// init player bounding box
	auto pBB = player->getBoundingBox(resolvingPos);

	for (auto block : collidableBlocks)
	{
		auto blockBB = block->getBoundingBox();

		// Check intersection
		if (blockBB.doesIntersectsWith(pBB))
		{
			// intersects with block
			auto intersectingAABB = getIntersectingBoundingBox(pBB, blockBB);

			// get y size
			float sizeY = intersectingAABB.getSize().y;// block i

			//if (!intersectingAABB.isZero(false))
			if(sizeY > 0)
			{
				// player and block is intersecting
				//std::cout << "playerPos y = " << playerPos.y << std::endl;
				//std::cout << "playerNextPos y = " << playerNextPos.y << std::endl;
				//std::cout << "r position y = " << resolvingPos.y << std::endl;
				resolvingPos.y += (intersectingAABB.getSize().y);
				//std::cout << "resolving y = " << resolvingPos.y << std::endl;

				//pBB = player->getBoundingBox(resolvingPos);
 				//player->setOnGround(true);
				//player->setJumpState(Player::JumpState::IDLE);

				player->setResolvedNextPosition(resolvingPos);

				//std::cout << "Player moved down. resolved\n";

				return;
			}
		}
	}

 	//player->setOnGround(false);
	//player->setAsFalling();
}

void Voxel::Physics::resolvePlayerTopCollision(Player * player, const std::vector<Block*>& collidableBlocks)
{
	if (collidableBlocks.empty()) return;

	// current position
	auto playerPos = player->getPosition();
	// position that player tried to move
	auto playerNextPos = player->getNextPosition();

	// get moved distance
	auto movedDistY = playerNextPos.y - playerPos.y;

	if (movedDistY <= 0.0f)
	{
		// player didn't move upward;
		//std::cout << "Player didn't move upward" << std::endl;
		return;
	}

	// Resolving position
	auto resolvingPos = playerNextPos;

	// init player bounding box
	auto pBB = player->getBoundingBox(resolvingPos);

	for (auto block : collidableBlocks)
	{
		auto blockBB = block->getBoundingBox();

		// Check intersection
		if (blockBB.doesIntersectsWith(pBB))
		{
			// intersects with block
			auto intersectingAABB = getIntersectingBoundingBox(pBB, blockBB);

			// get y size
			float sizeY = intersectingAABB.getSize().y;// block i

			//if (!intersectingAABB.isZero(false))
			if (sizeY > 0)
			{
				// Player bumped on block. Cancel jump.
				playerJumpForce.y = 0.0f;
				player->setJumpState(Player::JumpState::FALLING);
				// player and block is intersecting
				//std::cout << "position y = " << playerPos.y << std::endl;
				//std::cout << "r position y = " << resolvingPos.y << std::endl;
				resolvingPos.y -= (intersectingAABB.getSize().y);
				//std::cout << "resolving y = " << resolvingPos.y << std::endl;
				
				player->setResolvedNextPosition(resolvingPos);

				//std::cout << "Player moved up. resolved\n";

				return;
			}
		}
	}
}

void Voxel::Physics::checkIfPlayerIsFalling(Player * player, const std::vector<Block*>& collidableBlocks)
{
	if (collidableBlocks.empty()) return;

	// Get next player pos because it's all resolved at this point
	auto playerPos = player->getPosition();
	//std::cout << "-- playerPos: (" << playerPos.x << ", " << playerPos.y << ", " << playerPos.z << ")\n";
	//playerPos.y = player->getNextPosition().y;
	//std::cout << "-- pos: (" << playerPos.x << ", " << playerPos.y << ", " << playerPos.z << ")\n";


	// init player bounding box
	auto pBB = player->getBoundingBox(playerPos);

	for (auto block : collidableBlocks)
	{
		auto blockBB = block->getBoundingBox();

		if (blockBB.getMax().y >= pBB.getMin().y)
		{
			//std::cout << "block max y: " << blockBB.getMax().y << ", player min y " << pBB.getMin().y << std::endl;
			auto resolvingPos = playerPos;
			resolvingPos.y -= 0.1f;

			auto resolvedPlayerBB = player->getBoundingBox(resolvingPos);

			if (resolvedPlayerBB.doesIntersectsWith(blockBB))
			{
				// player and block is intersecting
				//std::cout << "Player is on ground" << std::endl;
				player->setOnGround(true);
				player->setJumpState(Player::JumpState::IDLE);
				return;
			}
		}
	}

	player->setOnGround(false);
	/*
	if (playerJumpForce.y > 0.0f)
	{
		player->setJumpState(Player::JumpState::JUMPING);
	}
	else
	{
		player->setJumpState(Player::JumpState::FALLING);
	}
	*/

	if (player->isJumpStateIdle())
	{
		player->setJumpState(Voxel::Player::JumpState::FALLING);
	}

	//std::cout << "Player is NOT on ground" << std::endl;
}

bool Voxel::Physics::checkCollisionWithBlocks(const Shape::AABB & boundingBox, const std::vector<Block*>& nearByBlocks)
{
	for (auto block : nearByBlocks)
	{
		auto blockBB = block->getBoundingBox();

		// Check intersection
		if (blockBB.doesIntersectsWith(boundingBox))
		{
			return true;
		}
	}

	return false;
}

bool Voxel::Physics::checkSphereCollisionWithBlocks(const Shape::Sphere& sphere, const std::vector<Block*>& nearByBlocks)
{
	for (auto block : nearByBlocks)
	{
		auto blockBB = block->getBoundingBox();

		// Check intersection
		if (blockBB.doesIntersectsWith(sphere))
		{
			return true;
		}
	}

	return false;
}

void Voxel::Physics::applyJumpForceToPlayer(const glm::vec3 & force)
{
	if (force.y >= 0.0f)
	{
		playerJumpForce = force;
	}
}

bool Voxel::Physics::updatePlayerJumpForce(Player * player, const float delta)
{
	if (player)
	{
		if (playerJumpForce.x != 0 || playerJumpForce.y != 0 || playerJumpForce.z != 0)
		{
			//std::cout << "Player JF = (" << playerJumpForce.x << ", " << playerJumpForce.y << ", " << playerJumpForce.z << ")" << std::endl;

			auto nextForce = glm::lerp(playerJumpForce, glm::vec3(0), 10.0f * delta);

			//std::cout << "nextForce = (" << nextForce.x << ", " << nextForce.y << ", " << nextForce.z << ")" << std::endl;

			auto forceDelta = playerJumpForce - nextForce;

			//std::cout << "forceDelta = (" << forceDelta.x << ", " << forceDelta.y << ", " << forceDelta.z << ")" << std::endl;

			if (nextForce.x <= 0.025f)
			{
				forceDelta.x = playerJumpForce.x;
				nextForce.x = 0.0f;
			}

			if (nextForce.y <= 0.025f)
			{
				forceDelta.y = playerJumpForce.y;
				nextForce.y = 0.0f;
			}

			if (nextForce.z <= 0.025f)
			{
				forceDelta.z = playerJumpForce.z;
				nextForce.z = 0.0f;
			}

			playerJumpForce = nextForce;

			auto pNextPos = player->getNextPosition();
			pNextPos += forceDelta;
			//player->addCameraY(-forceDelta.y * 0.5f);

			if (playerJumpForce.y <= 0.0f)
			{
				std::cout << "Jump finished\n";
				player->setJumpState(Player::JumpState::FALLING);
			}

			player->setNextPosition(pNextPos);
			
			return true;
		}
	}

	return false;
}