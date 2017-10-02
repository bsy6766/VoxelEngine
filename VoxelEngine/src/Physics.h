#ifndef PHYSICS_H
#define PHYSICS_H

#include <glm\glm.hpp>
#include <vector>

namespace Voxel
{
	class Player;

	class Physics
	{
	public:
		static const float Gravity;
	public:
		Physics();
		~Physics();

		void applyGravity(Player* player, const float delta);
	};
}

#endif