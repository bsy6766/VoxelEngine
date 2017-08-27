#include "Setting.h"
#include <iostream>

using namespace Voxel;

Setting::Setting()
{
	gameSetting = DataTree::create("data/setting");

	windowMode = gameSetting->getInt("videoSetting.window.mode");
	int w = gameSetting->getInt("videoSetting.window.resolution.width");
	int h = gameSetting->getInt("videoSetting.window.resolution.height");
	resolution = glm::ivec2(w, h);
	vsync = gameSetting->getBool("videoSetting.vsync");
	renderDistance = gameSetting->getInt("videoSetting.renderDistance");
	fieldOfView = gameSetting->getInt("videoSetting.fieldOfView");

	std::cout << "[Setting] Window mode = " << windowMode << std::endl;
	std::cout << "[Setting] Resolution = (" << resolution.x << ", " << resolution.y << ")" << std::endl;
	std::cout << "[Setting] Vsnyc = " << vsync << std::endl;
	std::cout << "[Setting] Render distance = " << renderDistance << std::endl;
	std::cout << "[Setting] Field of view = " << fieldOfView << std::endl;
}

Setting::~Setting()
{
	if (gameSetting)
	{
		delete gameSetting;
	}
}

int Voxel::Setting::getWindowMode()
{
	return windowMode;
}

glm::ivec2 Voxel::Setting::getResolution()
{
	return resolution;
}

bool Voxel::Setting::getVsync()
{
	return vsync;
}

int Voxel::Setting::getRenderDistance()
{
	return renderDistance;
}

int Voxel::Setting::getFieldOfView()
{
	return fieldOfView;
}
