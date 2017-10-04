#ifndef PHYSICS_H
#define PHYSICS_H

#include <glm\glm.hpp>
#include <vector>

namespace Voxel
{
	class Player;
	class Block;

	/**
	*	@class Physics
	*	@brief Provides physics related functions
	*/
	class Physics
	{
	public:
		// Gravity. Same as earth.
		static const float Gravity;
		// Gravity modifier.
		static const float GravityModifier;
	public:
		Physics();
		~Physics();

		/**
		*	applyGravity
		*	Applies gravity to player.
		*
		*	@param [in] player A player pointer.
		*	@param [in] delta Game tick.
		*/
		void applyGravity(Player* player, const float delta);

		/**
		*	resolvePlayerAndBlockCollision
		*	Resolves collistion between player and blocks.
		*
		*	@param [in] player A player pointer.
		*	@param [in] collidableBlocks A vector of Blocks that is collidable.
		*/
		void resolvePlayerAndBlockCollision(Player* player, const std::vector<Block*>& collidableBlocks);
	};
}

#endif