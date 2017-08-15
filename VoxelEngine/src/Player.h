#ifndef PLAYER_H
#define PLAYER_H

#include <glm\glm.hpp>

namespace Voxel
{
	// forward
	class Camera;

	/**
	*	@class Player
	*
	*	Player is a entity that can be controlled by player.
	*	Todo: Make player actually like player
	*	For now, just represents the position
	*/
	class Player
	{
	private:
		// World position of player
		glm::vec3 position;
		// direction player is viewing
		glm::vec3 rotation;

		// Matrix
		glm::mat4 viewMatrix;

		// speed
		float movementSpeed;
		float rotationSpeed;

		// True if player can fly
		bool fly;

		glm::vec3 getMovedDistByKeyInput(const float angleMod, const glm::vec3 axis, float distance);


		void wrapAngle(float& axis);
	public:
		Player();
		~Player();

		Camera* mainCamera;

		// Initialize player. 
		// Todo: read from save data
		void init(const glm::vec3& position);

		glm::vec3 getPosition();
		void setPosition(const glm::vec3& newPosition);

		void addRotationX(const float x);
		void addRotationY(const float y);
		void setRotation(const glm::vec3& newRotation);
		glm::vec3 getRotation();

		// Movement by inputs
		void moveFoward(const float delta);
		void moveBackward(const float delta);
		void moveLeft(const float delta);
		void moveRight(const float delta);
		void moveUp(const float delta);
		void moveDown(const float delta);
		void jump();
		void sneak();

		void setFly(const bool mode);		
		
		void updateViewMatrix();

		glm::mat4 getVP(const glm::mat4& projection);
	};
}

#endif // !PLAYER_H
