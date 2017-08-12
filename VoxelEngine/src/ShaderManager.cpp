#include "ShaderManager.h"
#include "Shader.h"

ShaderManager::~ShaderManager()
{
	for (auto it : vertexShaders)
	{
		if (it.second != nullptr)
		{
			delete it.second;
		}
	}

	vertexShaders.clear();

	for (auto it : fragmentShaders)
	{
		if (it.second != nullptr)
		{
			delete it.second;
		}
	}

	fragmentShaders.clear();
}

Shader * ShaderManager::createShader(const std::string& name, const std::string & filePath, GLenum shaderType)
{
	if (shaderType == GL_VERTEX_SHADER)
	{
		Shader* newShader = Shader::create(filePath, shaderType);
		if (newShader)
		{
			if (addVertexShader(name, newShader))
			{
				return newShader;
			}
			else
			{
				return nullptr;
			}
		}
	}
	else if (shaderType == GL_FRAGMENT_SHADER)
	{
		Shader* newShader = Shader::create(filePath, shaderType);
		if (newShader)
		{
			if (addFragmentShader(name, newShader))
			{
				return newShader;
			}
			else
			{
				return nullptr;
			}
		}
	}
	else
	{
		return nullptr;
	}
}

bool ShaderManager::addVertexShader(const std::string & name, Shader * vertexShader)
{
	auto find_it = vertexShaders.find(name);
	if (find_it == vertexShaders.end())
	{
		vertexShaders.emplace(name, vertexShader);
		return true;
	}
	else
	{
		return false;
	}
}

bool ShaderManager::addFragmentShader(const std::string & name, Shader * fragmentShader)
{
	auto find_it = fragmentShaders.find(name);
	if (find_it == fragmentShaders.end())
	{
		fragmentShaders.emplace(name, fragmentShader);
		return true;
	}
	else
	{
		return false;
	}
}

Shader * ShaderManager::getVertexShader(const std::string & name)
{
	auto find_it = vertexShaders.find(name);
	if (find_it == vertexShaders.end())
	{
		return nullptr;
	}
	else
	{
		return find_it->second;
	}
}

Shader * ShaderManager::getFragmentShader(const std::string & name)
{
	auto find_it = fragmentShaders.find(name);
	if (find_it == fragmentShaders.end())
	{
		return nullptr;
	}
	else
	{
		return find_it->second;
	}
}
