#ifndef SHADER_H
#define SHADER_H

#include <GL\glew.h>
#include <string>

namespace Voxel
{
	/**
	*	@class Shader
	*	@brief Loads GLSL file and create OpenGL shader
	*/
	class Shader
	{
	protected:
		Shader() = default;
	private:
		// OpenGL object
		GLuint shaderObject;

		// initialize shader. Returns false if fails
		bool init(const std::string& filePath, GLenum shaderType);

		void checkCompileError();
	public:
		~Shader();

		static Shader* create(const std::string& filePath, GLenum shaderType);

		GLuint getObject();
	};
}

#endif