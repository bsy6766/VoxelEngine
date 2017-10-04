#include "Physics.h"
#include <iostream>
#include <Player.h>
#include <Block.h>

using namespace Voxel;

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

void Voxel::Physics::resolvePlayerAndBlockCollision(Player * player, const std::vector<Block*>& collidableBlocks)
{
	auto playerBB = player->getBoundingBox();

	for (auto block : collidableBlocks)
	{
		auto blockBB = block->getBoundingBox();
	}
}
