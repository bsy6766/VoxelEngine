#include "ProgramManager.h"
#include "Program.h"
#include <ShaderManager.h>

using namespace Voxel;

ProgramManager::~ProgramManager()
{
	releaseAll();
}

Program * ProgramManager::createProgram(const std::string& name, Shader * vertexShader, Shader * fragmentShader)
{
	Program* newProgram = Program::create(vertexShader, fragmentShader);

	if (newProgram)
	{
		auto find_it = userProgram.find(name);
		if (find_it == userProgram.end())
		{
			userProgram.emplace(name, newProgram);
			return newProgram;
		}
		else
		{
			return nullptr;
		}
	}
	else
	{
		return nullptr;
	}
}

void Voxel::ProgramManager::initDefaultPrograms()
{
	auto voxelShaderColorVert = ShaderManager::getInstance().createShader("voxelShaderColor", "shaders/voxelShaderColor.vert", GL_VERTEX_SHADER);
	auto voxelShaderColorFrag = ShaderManager::getInstance().createShader("voxelShaderColor", "shaders/voxelShaderColor.frag", GL_FRAGMENT_SHADER);
	auto voxelShaderColorProgram = Program::create(voxelShaderColorVert, voxelShaderColorFrag);
	defaultPrograms.emplace(SHADER_COLOR, voxelShaderColorProgram);

	auto voxelShaderTextureColorVert = ShaderManager::getInstance().createShader("voxelShaderTextureColor", "shaders/voxelShaderTextureColor.vert", GL_VERTEX_SHADER);
	auto voxelShaderTextureColorFrag = ShaderManager::getInstance().createShader("voxelShaderTextureColor", "shaders/voxelShaderTextureColor.frag", GL_FRAGMENT_SHADER);
	auto voxelShaderTextureColorProgram = Program::create(voxelShaderTextureColorVert, voxelShaderTextureColorFrag);
	defaultPrograms.emplace(SHADER_TEXTURE_COLOR, voxelShaderTextureColorProgram);

}

Program * Voxel::ProgramManager::getDefaultProgram(PROGRAM programID)
{
	if (programID >= 0 && programID < SHADER_MAX_COUNT)
	{
		return defaultPrograms[programID];
	}
	else
	{
		return nullptr;
	}
}

Program * Voxel::ProgramManager::getProgram(const std::string & name)
{
	auto find_it = userProgram.find(name);
	if (find_it == userProgram.end())
	{
		return nullptr;
	}
	else
	{
		return find_it->second;
	}
}

void Voxel::ProgramManager::useDefaultProgram(PROGRAM programID)
{
	auto find_it = defaultPrograms.find(programID);
	if (find_it != defaultPrograms.end())
	{
		defaultPrograms[programID]->use(true);
	}
	else
	{
		throw std::runtime_error("Bad program use");
	}
}

void ProgramManager::releaseAll()
{
	for (auto it : defaultPrograms)
	{
		if (it.second)
		{
			//second.release();
			delete it.second;
		}
	}

	defaultPrograms.clear();

	for (auto it : userProgram)
	{
		if (it.second)
		{
			//second.release();
			delete it.second;
		}
	}

	userProgram.clear();
}