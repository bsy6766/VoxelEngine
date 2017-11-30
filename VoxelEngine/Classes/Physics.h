#ifndef PHYSICS_H
#define PHYSICS_H

// glm
#include <glm\glm.hpp>

// cpp
#include <vector>

// voxel
#include "Shape.h"

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

		glm::vec3 playerJumpForce;
	public:
		// Gravity. Same as earth.
		static const float Gravity;
		// Gravity modifier.
		static const float GravityModifier;
		
		// Player jump distance (1 block by default)
		static const float PlayerJumpDistance;
		// 

		bool resolvePlayerXAndBlockCollision(Player* player, glm::vec3& resolvingPos, const glm::vec3& movedDist, const std::vector<Block*>& collidableBlocks);
		bool resolvePlayerZAndBlockCollision(Player* player, glm::vec3& resolvingPos, const glm::vec3& movedDist, const std::vector<Block*>& collidableBlocks);
	public:
		Physics();
		~Physics();

		/**
		*	
		*/
		void applyJumpForceToPlayer(const glm::vec3& force);

		/**
		*	Applies gravity to player.
		*
		*	@param [in] player A player pointer.
		*	@param [in] delta Game tick.
		*/
		void applyGravity(Player* player, const float delta);

		/**
		*	Returns intersecting bounding box between two bounding boxes
		*
		*	@param [in] A First bounding box.
		*	@param [in] B Second bounding box.
		*	@return A Geometry::AABB of intersection between bounding box A and B.
		*/
		Shape::AABB getIntersectingBoundingBox(const Shape::AABB& A, const Shape::AABB& B);

		/**
		*	Resolves auto jump feature
		*	Auto jump is a feature that player can automatically move up the block without jumping
		*	
		*	@param [in] player A player pointer.
		*	@param [in] collidableBlocks A vector of Blocks that is collidable.
		*/
		bool resolveAutoJump(Player* player, const std::vector<Block*>& collidableBlocks);

		/**
		*	Resolves collistion between player and blocks.
		*
		*	@param [in] player A player pointer.
		*	@param [in] collidableBlocks A vector of Blocks that is collidable.
		*/
		void resolvePlayerAndBlockCollision(Player* player, const std::vector<Block*>& collidableBlocks);

		void resolvePlayerAndBlockCollisionInXZAxis(Player* player, const std::vector<Block*>& collidableBlocks);

		void resolvePlayerBottomCollision(Player* player, const std::vector<Block*>& collidableBlocks);

		// Resolves collision between blocks above player. Only resolves in Y axis. Only called while jupming.
		void resolvePlayerTopCollision(Player* player, const std::vector<Block*>& collidableBlocks);

		void checkIfPlayerIsFalling(Player* player, const std::vector<Block*>& collidableBlocks);

		bool checkCollisionWithBlocks(const Shape::AABB& boundingBox, const std::vector<Block*>& nearByBlocks);
		bool checkSphereCollisionWithBlocks(const Shape::Sphere& sphere, const std::vector<Block*>& nearByBlocks);

		bool updatePlayerJumpForce(Player* player, const float delta);
	};
}

#endif