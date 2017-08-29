#ifndef SETTING_H
#define SETTING_H

#include <DataTree.h>
#include <glm\glm.hpp>

namespace Voxel
{
	class Setting
	{
	private:
		Setting();
		~Setting();

		// Delete copy, move, assign operators
		Setting(Setting const&) = delete;             // Copy construct
		Setting(Setting&&) = delete;                  // Move construct
		Setting& operator=(Setting const&) = delete;  // Copy assign
		Setting& operator=(Setting &&) = delete;      // Move assign
	private:
		DataTree* gameSetting;

		// Video settings
		int windowMode;
		glm::ivec2 resolution;
		bool vsync;
		int renderDistance;
		int fieldOfView;
		bool blockShadeMode;

		// Keybind settings
		// Audio settings
	public:
		static Setting& getInstance()
		{
			static Setting instance;
			return instance;
		}

		int getWindowMode();
		glm::ivec2 getResolution();
		bool getVsync();
		int getRenderDistance();
		int getFieldOfView();
		bool getBlockShadeMode();
	};

}
#endif