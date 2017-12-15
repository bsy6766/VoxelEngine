#ifndef SETTING_H
#define SETTING_H

// cpp
#include <string>

// glm
#include <glm\glm.hpp>

namespace Voxel
{
	// foward declaration
	class DataTree;

	/**
	*	@class Setting
	*	@brief Simple class that stores game setting.
	*	
	*	Setting class loads, edits and saves setting to setting file.
	*	If setting file doesn't eixsts, creates default setting file.
	*	If setting file exists, loads setting values from existing file.
	*
	*	Setting class is singleton. Can be accessed from anywhere
	*/
	class Setting
	{
	private:
		// Constructor
		Setting();

		// Destructor
		~Setting();

		// Delete copy, move, assign operators
		Setting(Setting const&) = delete;             // Copy construct
		Setting(Setting&&) = delete;                  // Move construct
		Setting& operator=(Setting const&) = delete;  // Copy assign
		Setting& operator=(Setting &&) = delete;      // Move assign

	private:
		// Game setting data tree
		DataTree* userSetting;

		// modified flag
		bool modified;

		// Video settings
		int windowMode;					// 0 = windowed, 1 = fullscreen, 2 = windowed fullscren
		int monitorIndex;				// 0 is primary.
		glm::ivec2 resolution;
		bool vsync;
		int renderDistance;
		int fieldOfView;
		int blockShadeMode;				// 0 = none, 1 = minimum, 2 = maximum

		// Keybind settings
		// Audio settings

		// Control
		bool autoJump;

		// Set video mode to default setting
		void setVideoModeToDefault();
	
	public:
		static Setting& getInstance()
		{
			static Setting instance;
			return instance;
		}
		
		// ============================ Video setting ==========================
		int getWindowMode();
		glm::ivec2 getResolution();
		bool getVsync();
		int getRenderDistance();
		int getFieldOfView();
		int getBlockShadeMode();
		// =====================================================================

		bool getAutoJumpMode();
		void setAutoJumpMode(const bool mode);

		std::string getString(const std::string& key);
	};

}
#endif