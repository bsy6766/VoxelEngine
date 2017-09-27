#ifndef PLAYER_H
#define PLAYER_H

#include <glm\glm.hpp>
#include <GL\glew.h>
#include <Cube.h>

namespace Voxel
{
	// forward
	class Camera;
	class Block;
	class Program;

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
		// player's rotation angle in degree
		glm::vec3 rotation;
		// direction vector of player
		glm::vec3 direction;
		// Range in blocks that player can reach from position
		float rayRange;

		// Matrix
		glm::mat4 viewMatrix;
		glm::mat4 dirMatrix;

		// speed
		float movementSpeed;
		float rotationSpeed;

		// True if player can fly
		bool fly;

		// True if player moved or rotated this frame
		bool moved;
		bool rotated;

		// Block that player is looking at
		Block* lookingBlock;
		Cube::Face lookingFace;

		glm::vec3 getMovedDistByKeyInput(const float angleMod, const glm::vec3 axis, float distance);

		void wrapAngle(float& axis);
		void wrapAngle();
		void wrapAngleX();

		// temp
		GLuint yLineVao;
		GLuint rayVao;
	public:
		Player();
		~Player();

		Camera* mainCamera;

		// Initialize player. 
		// Todo: read from save data
		void init(const glm::vec3& position);

		// Debug
		void initYLine();
		void initRayLine();

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
		void updateDirMatrix();
		void updateDirection();

		void update();
		void renderDebugLines(Program* lineProgram);

		//glm::mat4 getVP(const glm::mat4& projection);
		glm::mat4 getViewMatrix();
		//glm::mat4 getDirVP(const glm::mat4& projection);
		glm::mat4 getOrientation();
		glm::mat4 getTranslationMat();
		glm::mat4 getDirMatrix();
		glm::mat4 getBillboardMatrix();

		// Check if player moved this frame
		bool didMoveThisFrame();
		// Check if player rotated this frame
		bool didRotateThisFrame();

		// Set player movementspeed
		void setMovementSpeed(float speed);
		float getMovementSpeed();

		// Get direction of player
		glm::vec3 getDirection();
		// Get ray range of player
		float getRange();
		// Get end point of player's raycast
		glm::vec3 getRayEnd();


		void setLookingBlock(Block* block, const Cube::Face& face);
		// Check if player is looking at block
		bool isLookingAtBlock();
		Block* getLookingBlock();
		Cube::Face getLookingFace();
	};
}

#endif // !PLAYER_H
