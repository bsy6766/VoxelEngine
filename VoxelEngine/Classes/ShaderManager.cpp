// pch
#include "PreCompiled.h"

// voxel
#include "ShaderManager.h"
#include "Logger.h"
#include "Config.h"

using namespace Voxel;

ShaderManager::~ShaderManager()
{
	releaseAll();
}

Shader * ShaderManager::createShader(const std::string& name, const std::string & filePath, const Shader::Type shaderType)
{
	Shader* newShader = Shader::create(filePath, shaderType);

	if (newShader)
	{
		if (shaderType == Shader::Type::VERTEX)
		{
			addVertexShader(name, newShader);
		}
		else if (shaderType == Shader::Type::GEOMETRY)
		{
			// I don't use geomtry shader yet.
			delete newShader;
			return nullptr;
		}
		else if (shaderType == Shader::Type::FRAGMENT)
		{
			addFragmentShader(name, newShader);
		}
		else
		{
#if V_DEBUG && V_DEBUG_LOG_CONSOLE
			auto logger = &Voxel::Logger::getInstance();

			logger->consoleWarn("[ShaderManager] Trying to create shader type that is invalid. Shader::Type value: " + std::to_string(static_cast<int>(shaderType)));
#endif

			delete newShader;
			return nullptr;
		}

		return newShader;
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

#if V_DEBUG && V_DEBUG_LOG_CONSOLE
		auto logger = &Voxel::Logger::getInstance();

		logger->consoleInfo("[ShaderManager] Successfully added vertex shader: " + name + " with shader object: " + std::to_string(vertexShader->getObject()));
#endif

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

#if V_DEBUG && V_DEBUG_LOG_CONSOLE
		auto logger = &Voxel::Logger::getInstance();

		logger->consoleInfo("[ShaderManager] Successfully added fragment shader: " + name + " with shader object: " + std::to_string(fragmentShader->getObject()));
#endif

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

void Voxel::ShaderManager::releaseAll()
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
