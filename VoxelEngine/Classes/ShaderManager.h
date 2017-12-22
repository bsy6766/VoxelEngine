#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

// cpp
#include <unordered_map>
#include <string>

// gl
#include <GL\glew.h>

namespace Voxel
{
	class Shader;

	/**
	*	@class ShaderManager
	*	@brief Manage all shaders in application.
	*
	*	Simply manages all shaders in application. Stores shaders by string name.
	*	Can create shader directly from ShaderManager or can add existing shader.
	*
	*	Once shader is linked to program and have no usage anymore,
	*	it's good to release and detroy shader object.
	*/
	class ShaderManager
	{
	private:
		// Constructor
		ShaderManager() = default;

		// Destructor. Releases all shaders
		~ShaderManager();

		// Delete copy, move, assign operators
		ShaderManager(ShaderManager const&) = delete;             // Copy construct
		ShaderManager(ShaderManager&&) = delete;                  // Move construct
		ShaderManager& operator=(ShaderManager const&) = delete;  // Copy assign
		ShaderManager& operator=(ShaderManager &&) = delete;      // Move assign

		// Vertex shader stored by name
		std::unordered_map<std::string, Shader*> vertexShaders;

		// Fragment shader stored by name
		std::unordered_map<std::string, Shader*> fragmentShaders;
	public:
		// Get instance.
		static ShaderManager& getInstance()
		{
			static ShaderManager instance;
			return instance;
		}

		/**
		*	Create shader.
		*	@param name Unique name of shader. If ShaderManager has shader with same name, rejects.
		*	@param filePath Shader file path.
		*	@param shaderType Type of shader
		*/
		Shader* createShader(const std::string& name, const std::string& filePath, GLenum shaderType);

		/**
		*	Add vertex shader
		*	@param name Name of shader.
		*	@param vertexShader Shader instance to add.
		*	@return true if successfully adds shader. Else, false.
		*/
		bool addVertexShader(const std::string& name, Shader* vertexShader);

		/**
		*	Add fragment shader
		*	@param name Name of shader.
		*	@param fragmentShader Shader instance to add.
		*	@return true if successfully adds shader. Else, false.
		*/
		bool addFragmentShader(const std::string& name, Shader* fragmentShader);

		/**
		*	Get vertex shader
		*	@param name Name of shader to get.
		*	@return Shader instance if shader exists. Else, false.
		*/
		Shader* getVertexShader(const std::string& name);

		/**
		*	Get fragment shader
		*	@param name Name of shader to get.
		*	@return Shader instance if shader exists. Else, false.
		*/
		Shader* getFragmentShader(const std::string& name);

		/**
		*	Release all shaders.
		*	This is called by ShaderManager's destructor.
		*/
		void releaseAll();
	};
}

#endif