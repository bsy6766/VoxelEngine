#ifndef PROGRAM_MANAGER_H
#define PROGRAM_MANAGER_H

// cpp
#include <unordered_map>
#include <string>

// glm
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
			SKYBOX_SHADER,
			UI_TEXTURE_SHADER,
			UI_TEXT_SHADER,
			UI_COLOR_PICKER_SHADER,
			UI_PARTICLE_SYSTEM_SHADER,
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
		*	Update "projMat" matrix on selected program
		*	@param projMat New projection matrix to set
		*/
		void updateProjMat(const glm::mat4& projMat);

		/**
		*	Update "projMat" matrix on all program
		*	@param projMat New projection matrix to set
		*/
		void updateUIProjMat(const glm::mat4& uiProjMat);
		
		/**
		*	Release all programs
		*/
		void releaseAll();
	};
}

#endif