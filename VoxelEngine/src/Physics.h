#ifndef PHYSICS_H
#define PHYSICS_H

#include <glm\glm.hpp>
#include <vector>
#include <Geometry.h>

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
	private:
		// for collision resolution. We divide step to x axis and z axis
		static const unsigned int X_AXIS;
		static const unsigned int Z_AXIS;
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
		*	getIntersectingBoundingBox
		*	Returns intersecting bounding box between two bounding boxes
		*
		*	@param [in] A First bounding box.
		*	@param [in] B Second bounding box.
		*	@return A Geometry::AABB of intersection between bounding box A and B.
		*/
		Geometry::AABB getIntersectingBoundingBox(const Geometry::AABB& A, const Geometry::AABB& B);

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