#ifndef PROGRAM_MANAGER_H
#define PROGRAM_MANAGER_H

#include <unordered_map>
#include <string>

namespace Voxel
{
	class Program;
	class Shader;

	class ProgramManager
	{
		friend class GLView;
	public:
		enum PROGRAM_NAME : unsigned int
		{
			SHADER_LINE = 0,
			SHADER_COLOR,
			SHADER_TEXTURE_COLOR,
			SHADER_TEXT,
			SHADER_MAX_COUNT
		};
	private:
		ProgramManager() = default;
		~ProgramManager();

		// Delete copy, move, assign operators
		ProgramManager(ProgramManager const&) = delete;             // Copy construct
		ProgramManager(ProgramManager&&) = delete;                  // Move construct
		ProgramManager& operator=(ProgramManager const&) = delete;  // Copy assign
		ProgramManager& operator=(ProgramManager &&) = delete;      // Move assign

		std::unordered_map<unsigned int/*id*/, Program*> defaultPrograms;
		std::unordered_map<std::string, Program*> userProgram;

		void initDefaultPrograms();
	public:
		static ProgramManager& getInstance()
		{
			static ProgramManager instance;
			return instance;
		}

		Program* createProgram(const std::string& name, Shader* vertexShader, Shader* fragmentShader);

		Program* getDefaultProgram(PROGRAM_NAME programID);
		Program* getProgram(const std::string& name);

		void useDefaultProgram(PROGRAM_NAME programID);

		void releaseAll();
	};
}

#endif