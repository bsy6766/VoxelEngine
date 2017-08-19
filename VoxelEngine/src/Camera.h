#ifndef CAMERA_H
#define CAMERA_H

#include <glm\glm.hpp>
#include <iostream>

using namespace glm;

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

		vec3 position;
		float fovy;
		float fovx;
		float screenWidth;
		float screenHeight;
		float aspect;
		float nears;
		float fars;

		// This is screen space position in camera's point of view. 
		// All the UI objects will follow this position
		vec3 screenSpacePos;

		vec3 angle;

		bool needUpdate;

		mat4 curMatrix;

		Frustum* frustum;
		void initFrustumPlanes();

		void wrapAngle();
		void wrapAngle(float& axis);
	public:
		~Camera();

		static Camera* create(const vec3& position, const float fovy, const float nears, const float fars, const float screenWidth, const float screenHeight);
		
		static Camera* mainCamera;

		// Get projection matrix. This is based on camera setting (fovy, near, far, aspect)
		mat4 getProjection();
		// Get matrix (projection * view). 
		mat4 getMatrix();
		// Get view matrix. This is 'world to view' matrix. 
		// Camera doesn't move but world is. This moves world to view.
		mat4 getView();
		// Get orientation. Basically rotation. 
		// This doesn't mean that camera is rotated. It's where camera want to look. 
		mat4 getOrientation();
		vec3 getPosition();

		void setPosition(const vec3& position);
		void addPosition(const vec3& distance);
		void setAngle(const vec3& angle);
		void addAngle(const vec3& angle);
		float getAngleY();

		void print();

		// Frustum plane
		vec4 nearPlane;
		vec4 farPlane;
		vec4 leftPlane;
		vec4 rightPlane;

		// Frustum normal
		vec2 nearNormal;
		vec2 farNormal;
		vec2 leftNormal;
		vec2 rightNormal;

		void updateFrustum(const glm::vec3& playerPosition, const glm::mat4& playerOrientation);

		void updateFrustumPlane(const vec3& playerPosition, const vec3& playerRotation);
		Frustum* getFrustum();

		glm::vec3 getScreenSpacePos();

		void updateScreenSizeAndAspect(const float screenWidth, const float screenHeight);

		void setFovy(const float fovy);
		float getFovy();
	};
}

#endif