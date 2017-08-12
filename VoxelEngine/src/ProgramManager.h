#ifndef PROGRAM_MANAGER_H
#define PROGRAM_MANAGER_H

#include <unordered_map>
#include <string>

class Program;
class Shader;

class ProgramManager
{
private:
	ProgramManager() = default;
	~ProgramManager();

	// Delete copy, move, assign operators
	ProgramManager(ProgramManager const&) = delete;             // Copy construct
	ProgramManager(ProgramManager&&) = delete;                  // Move construct
	ProgramManager& operator=(ProgramManager const&) = delete;  // Copy assign
	ProgramManager& operator=(ProgramManager &&) = delete;      // Move assign

	std::unordered_map<std::string, Program*> programs;
public:
	static ProgramManager& getInstance()
	{
		static ProgramManager instance;
		return instance;
	}

	Program* createProgram(const std::string& name, Shader* vertexShader, Shader* fragmentShader);
};

#endif