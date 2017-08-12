#include "ProgramManager.h"
#include "Program.h"

using namespace Voxel;

ProgramManager::~ProgramManager()
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

