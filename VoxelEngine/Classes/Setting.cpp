// pch
#include "PreCompiled.h"

#include "Setting.h"

// voxel
#include "DataTree.h"
#include "FileSystem.h"
#include "Application.h"
#include "Logger.h"
#include "GLView.h"

using namespace Voxel;

Setting::Setting()
	: modified(false)
	// Video setting
	, windowMode(1)
	, monitorIndex(0)
	, resolution(0)
	, vsync(false)
	, renderDistance(0)
	, fieldOfView(0)
	, blockShadeMode(0)
	, localizationTag(Voxel::Localization::Tag::en_US)
{
	// Initialize setting

	// get filesystem
	auto fs = &Voxel::FileSystem::getInstance();

	// get logger
	auto logger = &Voxel::Logger::getInstance();

	auto userSettingFilePath = fs->getUserDirectory() + "\\user.txt";

	// Check if user setting file exists
	if (fs->doesPathExists(userSettingFilePath))
	{
		// user file exists. read from it.
		logger->info("[System] User setting file found");

		// Create data tree file
		userSetting = DataTree::create(fs->getUserDirectory() + "\\user.txt");

		// load setting

		// localization
		localizationTag = Localization::toTag(userSetting->getString("localization"));
		windowMode = userSetting->getInt("videoSetting.window.mode");
		monitorIndex = userSetting->getInt("videoSetting.monitorIndex");
		int w = userSetting->getInt("videoSetting.window.resolution.width");
		int h = userSetting->getInt("videoSetting.window.resolution.height");
		resolution = glm::ivec2(w, h);
		vsync = userSetting->getBool("videoSetting.vsync");
		renderDistance = userSetting->getInt("videoSetting.renderDistance");
		fieldOfView = userSetting->getInt("videoSetting.fieldOfView");
		blockShadeMode = userSetting->getInt("videoSetting.blockShade");
	}
	else
	{
		// user file doesn't exists. set values to default
		logger->info("[System] User setting file not found. New setting created with default");

		setLocalizationToDefault();
		setVideoModeToDefault();

		// Create data tree file
		userSetting = DataTree::create(userSettingFilePath);

		// set setting
		userSetting->setString("localization", Voxel::Localization::toString(localizationTag));
		userSetting->setInt("videoSetting.window.mode", windowMode);
		userSetting->setInt("videoSetting.monitorIndex", monitorIndex);
		userSetting->setInt("videoSetting.window.resolution.width", resolution.x);
		userSetting->setInt("videoSetting.window.resolution.height", resolution.y);
		userSetting->setBool("videoSetting.vsync", vsync);
		userSetting->setInt("videoSetting.renderDistance", renderDistance);
		userSetting->setInt("videoSetting.fieldOfView", fieldOfView);
		userSetting->setInt("videoSetting.blockShade", blockShadeMode);

		// save
		userSetting->save(userSettingFilePath);
	}

	logger->info("[System] Localization: " + Voxel::Localization::toString(localizationTag));

	if (windowMode == 0)
	{
		logger->info("[System] Window mode: Windowed");
	}
	else if (windowMode == 1)
	{
		logger->info("[System] Window mode: Fullscreen");
	}
	else if (windowMode == 2)
	{
		logger->info("[System] Window mode: Windowed fullscreen");
	}

	logger->info("[System] Resolution: " + std::to_string(resolution.x) + " x " + std::to_string(resolution.y));
	logger->info("[System] Vertical sync: " + std::string(vsync ? "1" : "0"));

	if (!fs->doesPathExists(userSettingFilePath))
	{
		logger->error("[System] Failed to save user settings");
	}

	autoJump = userSetting->getBool("control.autoJump");
}

Setting::~Setting()
{
	if (userSetting)
	{
		delete userSetting;
	}
}

void Voxel::Setting::setLocalizationToDefault()
{
	localizationTag = Voxel::Localization::Tag::en_US;
}

void Voxel::Setting::setVideoModeToDefault()
{
	// fullscreen
	windowMode = 1;
	// primary monitor
	monitorIndex = 0;
	// Set to primary's monitors highest resolution
	resolution = Application::getInstance().getGLView()->getMonitorInfo(monitorIndex)->getVideoModes().back().getResolution();
	// Disable vsync
	vsync = false;
	// default render distance
	renderDistance = 8;
	// default fov
	fieldOfView = 70;
	// default shade mode
	blockShadeMode = 2;
}

int Voxel::Setting::getWindowMode() const
{
	return windowMode;
}

int Voxel::Setting::getMonitorIndex() const
{
	return monitorIndex;
}

glm::ivec2 Voxel::Setting::getResolution() const
{
	return resolution;
}

bool Voxel::Setting::getVsync() const
{
	return vsync;
}

int Voxel::Setting::getRenderDistance() const
{
	return renderDistance;
}

int Voxel::Setting::getFieldOfView() const
{
	return fieldOfView;
}

int Voxel::Setting::getBlockShadeMode() const
{
	return blockShadeMode;
}

bool Voxel::Setting::getAutoJumpMode() const
{
	return autoJump;
}

void Voxel::Setting::setAutoJumpMode(const bool mode)
{
	autoJump = mode;
}

std::string Voxel::Setting::getString(const std::string & key)
{
	return userSetting->getString(key);
}

Localization::Tag Voxel::Setting::getLocalizationTag() const
{
	return localizationTag;
}
