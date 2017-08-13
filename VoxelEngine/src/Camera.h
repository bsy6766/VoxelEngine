#ifndef CAMERA_H
#define CAMERA_H

#include <glm\glm.hpp>
#include <iostream>

using namespace glm;

namespace Voxel
{
	class Camera
	{
	private:
		Camera();

		vec3 position;
		float fovy;
		float aspect;
		float nears;
		float fars;

		vec3 angle;

		bool needUpdate;

		mat4 curMatrix;

		void wrapAngle();
		void wrapAngle(float& axis);
	public:
		~Camera();

		static Camera* create(const vec3& position, const float fovy, const float nears, const float fars, const float aspect);
		
		static Camera* mainCamera;

		// Get projection
		mat4 getProjection();
		// Get matrix (projection * view)
		mat4 getMatrix();
		mat4 getView();
		mat4 getOrientation();
		vec3 getPosition();

		void setPosition(const vec3& position);
		void addPosition(const vec3& distance);
		void setAngle(const vec3& angle);
		void addAngle(const vec3& angle);
	};
}

#endif