#ifndef SKYBOX_H
#define SKYBOX_H

#include <GL\glew.h>
#include <glm\glm.hpp>

namespace Voxel
{
	class Skybox
	{
	private:
		enum class FOG_STATE
		{
			IDLE = 0,
			ANIMATING,
		};

		enum class SKYCOLOR_STATE
		{
			IDLE = 0,
			ANIMATING,
		};

		FOG_STATE fogState;
		SKYCOLOR_STATE skycolorState;
	private:
		glm::vec4 skyColor;
		glm::vec4 curSkyColor;

		GLuint vao;
		int indicesSize;

		float fogDistance;
		float curFogDistance;
		float fogAnimationSpeed;
	public:
		Skybox();
		~Skybox();

		void init(const glm::vec4& skyColor, const int renderDistance);

		void update(const float delta);
		void render();

		glm::vec4 getColor();
		float getFogDistance();

		void setFogDistanceByRenderDistance(const int renderDistance, const bool animate);
		void setFogDistance(const float distance, const bool animate);
	};
}

#endif