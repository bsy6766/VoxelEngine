#ifndef CAMERA_H
#define CAMERA_H

// glm
#include <glm\glm.hpp>

// cpp
#include <iostream>

namespace Voxel
{
	class Frustum;

	/**
	*	@class Camera
	*	@brief Virtual camera in 3D OpenGL world
	*	
	*	Camera class calculates the MVP matrix.
	*	Camera seems like function in the real camera, but actually it's just concept not real.
	*	Camera itself doesn't move, rotate or scale. World does.
	*	So camera calculates these matrices.
	*	Projection matrix: A matrix for perspective camera that is computed with FOVY, Near, Far, Aspect.
	*	World To View (aka view) matrix: A matrix that moves the world to camera's view. This is necessary because camera doesn't move and make player think like camera move.
	*	Then these matrices are multiplied once more with model matrix, which is object's own transform matrix.
	*/
	class Camera
	{
	private:
		Camera();

		// Data
		glm::vec3 position;

		// Rotation angle
		glm::vec3 angle;

		// fov
		float fovy;
		float fovx;
		
		// resolution
		float screenWidth;
		float screenHeight;

		// Movement speed
		float speed;

		// width / height
		float aspect;

		// near clipping pane
		float nears;
		// far clipping pane
		float fars;

		// This is screen space position in camera's point of view. 
		// All the UI objects will follow this position
		glm::vec3 screenSpacePos;
		
		// Frustum
		Frustum* frustum;

		// Wrap angle if it goes below 0 or over 360
		void wrapAngle();
		void wrapAngle(float& axis);

		// Updates projection. Call this when camera attribute (near, far, fovy, aspect) is chanced
		void updateProjection();
	public:
		~Camera();

		static Camera* create(const glm::vec3& position, const float fovy, const float nears, const float fars, const float screenWidth, const float screenHeight);
		
		static Camera* mainCamera;

		// UI fovy
		static const float UIFovy;

		void initDebugFrustumLines();

		// Get projection matrix. This is based on camera setting (fovy, near, far, aspect)
		glm::mat4 getProjection();
		// Get projection matrix with specific fovy value.
		glm::mat4 getProjection(const float fovy);
		// Get view matrix. This is 'world to view' matrix. 
		// Camera doesn't move but world is. This moves world to view.
		glm::mat4 getViewMat();
		glm::mat4 getWorldMat();
		// Get orientation. Basically rotation. 
		// This doesn't mean that camera is rotated. It's where camera want to look. 
		glm::mat4 getOrientation();
		// Get position of camera
		glm::vec3 getPosition();
		// Set position of camera
		void setPosition(const glm::vec3& position);
		// add distance to current camera position
		void addPosition(const glm::vec3& distance);
		// Set rotation angle
		void setAngle(const glm::vec3& angle);
		// Add angle to current angle
		void addAngle(const glm::vec3& angle);
		// Get angle
		glm::vec3 getAngle();
		// Get angle in y axis
		float getAngleY();
		// Get near
		float getNear() const;
		// Set near
		void setNear(const float near);
		// get far
		float getFar() const;
		// Set far
		void setFar(const float far);

		// set camera speed
		void setSpeed(const float speed);

		// Print camera info
		void print();

		// Get frustum instance
		Frustum* getFrustum();

		// Update screen size and aspect. Call this whenever resolution changes
		void updateScreenSizeAndAspect(const float screenWidth, const float screenHeight);

		// Set vertical fov
		void setFovy(const float fovy);
		// Get fovy
		float getFovy();

		// Get screen space matrix
		glm::mat4 getScreenSpaceMatrix();
	};
}

#endif