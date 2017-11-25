#ifndef PLAYER_H
#define PLAYER_H

#include <Config.h>
#include <glm\glm.hpp>
#include <GL\glew.h>
#include <Cube.h>
#include <Shape.h>

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
	public:
		static const float MaxCameraDistanceX;
		static const float EyeHeight;
	public:
		enum class ViewMode
		{
			FIRST_PERSON_VIEW = 0,
			THIRD_PERSON_VIEW
		};

		enum class JumpState
		{
			IDLE = 0,
			JUMPING,
			FALLING
		};
	private:
		// World position of player
		glm::vec3 position;
		// For collision resolution
		glm::vec3 nextPosition;
		// player's rotation angle in degree
		glm::vec3 rotation;
		// For smooth rotation
		glm::vec3 rotationTarget;
		// direction vector of player
		glm::vec3 direction;

		// Physics
		glm::vec2 xzVelocity;
		glm::vec2 xzAcceleration;
		glm::vec2 xzElapsedTime;


		// Range in blocks that player can reach from position
		float rayRange;

		// view mode
		ViewMode viewMode;

		// jump state
		JumpState jumpState;

		// Matrix
		glm::mat4 viewMatrix;
		glm::mat4 dirMatrix;

		// speed
		float movementSpeed;
		float rotationSpeed;

		// player's camera
		float cameraY;
		float cameraDistanceZ;
		float cameraDistanceTargetZ;
		bool cameraColliding;

		// True if player can fly
		bool fly;

		// True if player moved or rotated this frame
		bool moved;
		bool rotated;

		// The amount of duration that player fell
		float fallDuration;
		// The amount of distant that player fell. Higher the number, higher the fall damage.
		float fallDistance;
		// True if player is standing on block. Else, it's falling
		bool onGround;

		// Block that player is looking at
		Block* lookingBlock;
		Cube::Face lookingFace;

		glm::vec3 getMovedDistByKeyInput(const float angleMod, const glm::vec3& axis, float distance);

		void wrapAngle(float& axis);
		void wrapAngle();
		void wrapAngleX();
		void wrapNextAngleX();
	public:
		Player();
		~Player();

		Camera* mainCamera;

		// Initialize player. 
		// Todo: read from save data
		void init();

		// Debug
		void initYLine();
		void initRayLine();
		void initBoundingBoxLine();

		glm::vec3 getPosition();
		void setPosition(const glm::vec3& newPosition, const bool smoothMovement);
		void setNextPosition(const glm::vec3& nextPosition);
		void setResolvedNextPosition(const glm::vec3& resolvedNextPosition);
		void addCameraY(const float y);
		void applyNextPosition();
		glm::vec3 getNextPosition();
		glm::vec3 getEyePosition();
		glm::vec3 getNextEyePosition();

		void addRotationX(const float x);
		void addRotationY(const float y);
		void setRotation(const glm::vec3& newRotation, const bool smoothRotation);
		glm::vec3 getRotation();

		// Movement by inputs
		void moveFoward(const float delta);
		void stopFoward(const float delta);
		void moveBackward(const float delta);
		void stopBackward(const float delta);
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

		void update(const float delta);
		void updateMovement(const float delta);
		void updateCameraDistanceZ(const float delta);
		void renderDebugLines(Program* lineProgram);

		//glm::mat4 getVP(const glm::mat4& projection);
		// Get view matrix. View matrix transfroms world and model to player's view
		glm::mat4 getViewMatrix();
		// Get world matrix. Uses player's position to get world's matrix. World moves, not player's view.
		glm::mat4 getWorldMatrix() const;
		// Get camera y matrix
		glm::mat4 getCameraYMatrix() const;
		glm::mat4 getPosMatrix() const;
		glm::mat4 getFrustumViewMatrix();
		//glm::mat4 getDirVP(const glm::mat4& projection);
		//glm::mat4 getOrientation();
		glm::mat4 getTranslationMat();
		glm::mat4 getTranslationXZMat();
		glm::mat4 getSkyboxMat(const bool addPosition) const;
		glm::mat4 getDirMatrix();
		glm::mat4 getBillboardMatrix();

		// Check if player moved this frame
		bool didMoveThisFrame();
		// Check if player rotated this frame
		bool didRotateThisFrame();

		float getCameraDistanceZ();
		float getMaxCameraDistanceZ();
		float getCameraY();

		// Set player movementspeed
		void setMovementSpeed(float speed);
		float getMovementSpeed();

		// Add fall tick
		float getFallDistance();
		void setFallDistance(const float dist);
		float getFallDuration();
		void setFallDuration(float time);
		bool isOnGround();
		void setOnGround(const bool onGround);

		bool isFlying();

		// Get direction of player
		glm::vec3 getDirection();
		glm::vec3 getNextDirection();
		// Get ray range of player
		float getRange();
		// Get end point of player's raycast
		glm::vec3 getRayEnd();

		void setViewMode(const int mode);
		ViewMode getViewMode();

		void zoomInCamera();
		void zoomOutCamera();

		void setLookingBlock(Block* block, const Cube::Face& face);
		// Check if player is looking at block
		bool isLookingAtBlock();
		Block* getLookingBlock();
		Cube::Face getLookingFace();

		Shape::AABB getBoundingBox();
		Shape::AABB getBoundingBox(const glm::vec3& position);

		void autoJump(const float y);
		void autoJump(const glm::vec3& resolvedPosition);
				
		/**
		*	Checks if player is jumping
		*/
		bool isJumping();
		bool isFalling();
		bool isJumpStateIdle();

		void setJumpState(const JumpState jumpState);
		JumpState getJumpState() const;

		void setResolvedCameraDistanceZ(const float dist);
		void setCameraColliding(const bool value);

		bool isOnFPViewMode();
		bool isOnTPViewMode();

		// For debug
#if V_DEBUG
#if V_DEBUG_PLAYER_BOUNDING_BOX
		GLuint boundingBoxVao;
#endif
#if V_DEBUG_PLAYER_BOUNDING_BOX
		GLuint yLineVao;
		GLuint rayVao;
#endif
#endif
	};
}

#endif // !PLAYER_H
