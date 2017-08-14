#include "ProgramManager.h"
#include "Program.h"

using namespace Voxel;

ProgramManager::~ProgramManager()
{
	releaseAll();
}

Program * ProgramManager::createProgram(const std::string & name, Shader * vertexShader, Shader * fragmentShader)
{
	Program* newProgram = Program::create(vertexShader, fragmentShader);

	if (newProgram)
	{
		auto find_it = programs.find(name);
		if (find_it == programs.end())
		{
			programs.emplace(name, newProgram);
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

Program * Voxel::ProgramManager::getDefaultProgram()
{
	return programs["defaultProgram"];
}

void ProgramManager::releaseAll()
{
	for (auto it : programs)
	{
		if (it.second)
		{
			//second.release();
			delete it.second;
		}
	}

	programs.clear();
}