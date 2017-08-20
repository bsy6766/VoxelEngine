#ifndef PROGRAM_H
#define PROGRAM_H

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <string>

using namespace glm;

namespace Voxel
{
	class Shader;

	/**
	*	@class Program
	*	@brief OpenGL Program instance
	*/
	class Program
	{
	protected:
		Program() = default;
	private:
		GLuint programObject;

		bool init(Shader* vertexShader, Shader* fragmentShader);

		void checkLinkError();
	public:
		~Program();
		static Program* create(Shader* vertexShader, Shader* fragmentShader);

		GLuint getObject();

		GLint getAttribLocation(const GLchar* attributeName);
		GLint getUniformLocation(const GLchar* uniformName);
		void setUniformMat4(const GLint location, const mat4& mat);
		void setUniformMat4(const std::string& name, const mat4& mat);
		void setUniformBool(const GLint location, const bool boolean);
		void setUniformBool(const std::string& name, const bool boolean);
		void setUniformInt(const GLint location, const int integer);
		void setUniformInt(const std::string& name, const int integer);

		void use(const bool use);
	};
}

#endif PROGRAM_H
