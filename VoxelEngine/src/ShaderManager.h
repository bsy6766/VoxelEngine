#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include <unordered_map>
#include <string>

#include <GL\glew.h>

class Shader;

class ShaderManager
{
private:
	ShaderManager() = default;
	~ShaderManager();

	// Delete copy, move, assign operators
	ShaderManager(ShaderManager const&) = delete;             // Copy construct
	ShaderManager(ShaderManager&&) = delete;                  // Move construct
	ShaderManager& operator=(ShaderManager const&) = delete;  // Copy assign
	ShaderManager& operator=(ShaderManager &&) = delete;      // Move assign

	std::unordered_map<std::string, Shader*> vertexShaders;
	std::unordered_map<std::string, Shader*> fragmentShaders;
public:
	static ShaderManager& getInstance()
	{
		static ShaderManager instance;
		return instance;
	}

	Shader* createShader(const std::string& name, const std::string& filePath, GLenum shaderType);

	bool addVertexShader(const std::string& name, Shader* vertexShader);
	bool addFragmentShader(const std::string& name, Shader* fragmentShader);

	Shader* getVertexShader(const std::string& name);
	Shader* getFragmentShader(const std::string& name);
};

#endif