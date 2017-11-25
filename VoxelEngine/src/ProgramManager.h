#ifndef PROGRAM_MANAGER_H
#define PROGRAM_MANAGER_H

#include <unordered_map>
#include <string>
#include <glm\glm.hpp>

namespace Voxel
{
	// Forward declaration
	class Program;
	class Shader;

	/**
	*	@class ProgramManager
	*	@brief Manages OpenGL programs. Singleton class
	*/
	class ProgramManager
	{
		friend class GLView;
	public:
		enum PROGRAM_NAME : unsigned int
		{
			LINE_SHADER = 0,
			BLOCK_SHADER,
			POLYGON_SHADER,
			POLYGON_SIDE_SHADER,
			TEXTURE_SHADER,
			TEXT_SHADER,
			SKYBOX_SHADER,
			SHADER_MAX_COUNT
		};
	private:
		// Default constructor
		ProgramManager() = default;

		// Destructor. Releases all programs
		~ProgramManager();

		// Delete copy, move, assign operators
		ProgramManager(ProgramManager const&) = delete;             // Copy construct
		ProgramManager(ProgramManager&&) = delete;                  // Move construct
		ProgramManager& operator=(ProgramManager const&) = delete;  // Copy assign
		ProgramManager& operator=(ProgramManager &&) = delete;      // Move assign

		// map of all programs
		std::unordered_map<PROGRAM_NAME, Program*> programs;

		// Initialize all the shader program that is used
		void initPrograms();
	public:
		// Get instance. 
		static ProgramManager& getInstance()
		{
			static ProgramManager instance;
			return instance;
		}
		
		/**
		*	Get program by id
		*	@param programID An program name enum to get
		*	@return program instance if exists. Else, nullptr;
		*/
		Program* getProgram(const PROGRAM_NAME programID);

		/**
		*	Finds the program by id and uses it
		*	@param programID An program name enum to use
		*/
		void useDefaultProgram(const PROGRAM_NAME programID);

		/**
		*	Update "projMat" matrix on all program
		*	@param projMat New projection matrix to set
		*/
		void updateProjMat(const glm::mat4& projMat);

		/**
		*	Release all programs
		*/
		void releaseAll();
	};
}

#endif