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
	public:
		enum class Type
		{
			VERTEX = 0,
			GEOMETRY,
			FRAGMENT
		};
	private:
		// OpenGL object
		GLuint shaderObject;

		// Shader type
		Type shaderType;

		// initialize shader. Returns false if fails
		bool init(const std::string& filePath, const Type shaderType);

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
		static Shader* create(const std::string& filePath, const Type shaderType);

		// Get shader object.
		GLuint getObject();
	};
}

#endif