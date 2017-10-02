#include "Physics.h"
#include <iostream>
#include <Player.h>

using namespace Voxel;

const float Physics::Gravity = 9.80665f;

Voxel::Physics::Physics()
{
}

Voxel::Physics::~Physics()
{
}

void Voxel::Physics::applyGravity(Player * player, const float delta)
{
	//auto playerPos = player->
}
