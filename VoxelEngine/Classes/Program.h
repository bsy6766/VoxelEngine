#ifndef PROGRAM_H
#define PROGRAM_H

// gl
#include <GL\glew.h>
#include <glm\glm.hpp>

// cpp
#include <string>
#include <unordered_map>

using namespace glm;

namespace Voxel
{
	class Shader;

	/**
	*	Union for uniform values
	*/
	struct UniformValue
	{
		GLenum type;
		union Value
		{
			bool boolValue;
			int intValue;
			float floatValue;
			float v2Value[2];
			float v3Value[3];
			float v4Value[4];
			float mat4Value[16];
		} value;
	};

	/**
	*	@class Program
	*	@brief OpenGL Program instance
	*/
	class Program
	{
	private:
		// Constructor
		Program();

		// OpenGL object
		GLuint programObject;

		// Last bounded opengl program object
		static GLuint lastProgramObject;

		// Uniform location by name
		std::unordered_map<std::string, GLint> uniformLocations;

		// Uniform locations by location id
		std::unordered_map<GLint, UniformValue> uniformValues;

		// Initialize program
		bool init(Shader* vertexShader, Shader* fragmentShader);

		// Initialize uniforms
		void findAllUniforms();

		// Check if link failed
		void checkLinkError();

	public:
		// Destructor
		~Program();

		/**
		*	Creates opengl program
		*	@param vertexShader Vertex shader to link
		*	@param fragmetnShader Fragment shader to link
		*	@return Program instance if program is successfully created. Else, nullptr;
		*/
		static Program* create(Shader* vertexShader, Shader* fragmentShader);

		// Get program object
		GLuint getObject() const;

		// Get attribute location by name
		GLint getAttribLocation(const GLchar* attributeName);
		GLint getUniformLocation(const GLchar* uniformName);

		void setUniformBool(const GLint location, const bool boolean);
		void setUniformBool(const std::string& name, const bool boolean);

		void setUniformInt(const GLint location, const int integer);
		void setUniformInt(const std::string& name, const int integer);

		void setUniformFloat(const GLint location, const float val);
		void setUniformFloat(const std::string& name, const float val);

		void setUniformVec2(const GLint location, const glm::vec2& val);
		void setUniformVec2(const std::string& name, const glm::vec2& val);

		void setUniformVec3(const GLint location, const glm::vec3& val);
		void setUniformVec3(const std::string& name, const glm::vec3& val);

		void setUniformVec4(const GLint location, const glm::vec4& val);
		void setUniformVec4(const std::string& name, const glm::vec4& val);

		void setUniformMat4(const GLint location, const mat4& mat);
		void setUniformMat4(const std::string& name, const mat4& mat);

		void use(const bool use);
	};
}

#endif PROGRAM_H
