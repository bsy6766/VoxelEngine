#ifndef SKYBOX_H
#define SKYBOX_H

#include <GL\glew.h>
#include <glm\glm.hpp>

namespace Voxel
{
	class Skybox
	{
	private:
		glm::vec3 skyColor;

		GLuint vao;
		int indicesSize;
	public:
		Skybox();
		~Skybox();

		void init(const glm::vec4& skyColor, const int renderDistance);

		void update(const float delta);
		void render();

		glm::vec3 getColor();
	};
}

#endif