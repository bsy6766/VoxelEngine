#ifndef SHADER_H
#define SHADER_H

// gl
#include <GL\glew.h>

// cpp
#include <string>

namespace Voxel
{
	/**
	*	@class Shader
	*	@brief Loads GLSL file and create OpenGL shader
	*/
	class Shader
	{
	private:
		// OpenGL object
		GLuint shaderObject;

		// initialize shader. Returns false if fails
		bool init(const std::string& filePath, GLenum shaderType);

		// Check compile error. Throws runtime error if fails.
		void checkCompileError();
	public:
		// Constructor
		Shader();

		// Destructor
		~Shader();

		/**
		*	Create opengl shader
		*	@param filePath Shader file path
		*	@param shaderType Type of shader (Vertex, Geometry, Fragment)
		*	@return Shader instance if successfully created shader object. Else, false.
		*/
		static Shader* create(const std::string& filePath, GLenum shaderType);

		// Get shader object.
		GLuint getObject();
	};
}

#endif