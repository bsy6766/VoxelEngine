#include "SpriteSheet.h"

// voxel
#include "Application.h"
#include "Utility.h"
#include "FileSystem.h"
#include "Logger.h"
#include "Config.h"

// cpp
#include <fstream>

using namespace Voxel;
using json = nlohmann::json;

Voxel::SpriteSheet::SpriteSheet()
	: texture(nullptr)
{
}

Voxel::SpriteSheet::~SpriteSheet()
{
	if (texture)
	{
		TextureManager::getInstance().removeTexture(texture->getName());
	}

	imageEntryMap.clear();
}

SpriteSheet * Voxel::SpriteSheet::create(const std::string & dataFileName)
{
	auto newSS = new SpriteSheet();
	if (newSS->init(dataFileName))
	{
		return newSS;
	}

	delete newSS;
	return nullptr;
}

bool Voxel::SpriteSheet::hasImage(const std::string & imageName)
{
	return imageEntryMap.find(imageName) != imageEntryMap.end();
}

const ImageEntry* Voxel::SpriteSheet::getImageEntry(const std::string & imageName)
{
	if (hasImage(imageName))
	{
		return &imageEntryMap[imageName];
	}
	else
	{
		return nullptr;
	}
}

Texture2D * Voxel::SpriteSheet::getTexture()
{
	return texture;
}

void Voxel::SpriteSheet::print()
{
#if V_DEBUG && V_DEBUG_LOG_CONSOLE
	auto logger = &Voxel::Logger::getInstance();

	logger->consoleInfo("[SpriteSheet] Info");

	if (texture)
	{
		logger->consoleInfo("[SpriteSheet] Texture name: " + texture->getName());
	}
	else
	{
		logger->consoleWarn("[SpriteSheet] Texture doesn't eixsts.");
	}

	logger->consoleInfo("[SpriteSheet] " + std::to_string(imageEntryMap.size()) + " images in spritesheet");

	for (auto& ie : imageEntryMap)
	{
		logger->consoleInfo("[ImageEntry] Name: " + ie.first);
		logger->consoleInfo("[ImageEntry] Width: " + std::to_string((ie.second).width));
		logger->consoleInfo("[ImageEntry] Height: " + std::to_string((ie.second).height));
		logger->consoleInfo("[ImageEntry] Height: " + std::to_string((ie.second).height));
		logger->consoleInfo("[ImageEntry] uvOrigin: (" + std::to_string((ie.second).uvOrigin.x) + ", " + std::to_string((ie.second).uvOrigin.y) + ")");
		logger->consoleInfo("[ImageEntry] uvOrigin: (" + std::to_string((ie.second).uvEnd.x) + ", " + std::to_string((ie.second).uvEnd.y) + ")");
	}
#endif
}

bool Voxel::SpriteSheet::init(const std::string & dataFileName)
{
	const std::string wd = FileSystem::getInstance().getWorkingDirectory();

	const std::string path = wd + "/spritesheets/";

	std::ifstream i(path + dataFileName);

	json j;

	if (i.is_open())
	{
		i >> j;
	}
	else
	{
		return false;
	}
	
	auto& textureInfo = j.at("texture");

	std::string textureName = textureInfo.at("path");

	texture = Texture2D::createSpriteSheetTexture(textureName, GL_TEXTURE_2D);

	if (!texture)
	{
		return false;
	}

	texture->setLocationOnProgram(ProgramManager::PROGRAM_NAME::UI_TEXTURE_SHADER);

	auto textureSize = texture->getTextureSize();

	const float textureWidth = static_cast<float>(textureSize.x);
	const float textureHeight = static_cast<float>(textureSize.y);

	auto& frames = j.at("frames");

	for (auto& e : frames)
	{
		// Assume all images aren't roated and pivot is center.
		std::string imageName = e.at("filename");

		auto& frame = e.at("frame");

		ImageEntry newEntry;
		newEntry.position.x = frame.at("x");
		newEntry.position.y = frame.at("y");
		newEntry.width = frame.at("w");
		newEntry.height = frame.at("h");
		
		/*
		newEntry.uvOrigin.x = ((newEntry.position.x + newEntry.width) / textureWidth);
		newEntry.uvOrigin.y = ((newEntry.position.y + newEntry.height) / textureHeight);

		newEntry.uvEnd.x = (newEntry.position.x / textureWidth);
		newEntry.uvEnd.y = (newEntry.position.y / textureHeight);
		*/

		// Texture's y is flipped when it's read by stb_image. So we assign y as flipped

		newEntry.uvOrigin.x = (newEntry.position.x / textureWidth);
		newEntry.uvOrigin.y = ((newEntry.position.y + newEntry.height) / textureHeight);

		newEntry.uvEnd.x = ((newEntry.position.x + newEntry.width) / textureWidth);
		newEntry.uvEnd.y = (newEntry.position.y / textureHeight);

		imageEntryMap.emplace(imageName, newEntry);
	}

	return true;
}




Voxel::SpriteSheetManager::~SpriteSheetManager()
{
	// Sprite sheets are released in Application manually in cleanUp();
}

bool Voxel::SpriteSheetManager::addSpriteSheet(const std::string & jsonFileName)
{
	std::string fileName;
	std::string ext = "";

	Utility::String::fileNameToNameAndExt(jsonFileName, fileName, ext);

	if (ext.empty())
	{
		ext = ".json";
	}
	
	auto find_it = spriteSheetMap.find(fileName);

	if (find_it == spriteSheetMap.end())
	{
		auto ss = SpriteSheet::create(fileName + ext);
		if (ss)
		{
			spriteSheetMap.emplace(fileName, ss);
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool Voxel::SpriteSheetManager::removeSpriteSheetByKey(const std::string & key)
{
	auto find_it = spriteSheetMap.find(key);

	if (find_it == spriteSheetMap.end())
	{
		return false;
	}
	else
	{
		if (find_it->second)
		{
			delete (find_it->second);
		}

		spriteSheetMap.erase(find_it);

		return true;
	}
}

bool Voxel::SpriteSheetManager::removeSpriteSheetByName(const std::string & jsonFileName)
{
	return removeSpriteSheetByKey(Utility::String::removeFileExtFromFileName(jsonFileName));
}

SpriteSheet * Voxel::SpriteSheetManager::getSpriteSheetByKey(const std::string & key)
{
	auto find_it = spriteSheetMap.find(key);

	if (find_it == spriteSheetMap.end())
	{
		return nullptr;
	}
	else
	{
		return find_it->second;
	}
}

SpriteSheet * Voxel::SpriteSheetManager::getSpriteSheetByName(const std::string & jsonFileName)
{
	return getSpriteSheetByKey(Utility::String::removeFileExtFromFileName(jsonFileName));
}

void Voxel::SpriteSheetManager::releaseAll()
{
	for (auto& e : spriteSheetMap)
	{
		if (e.second)
		{
			delete e.second;
		}
	}

	spriteSheetMap.clear();
}

void Voxel::SpriteSheetManager::print(const bool detail)
{
#if V_DEBUG && V_DEBUG_LOG_CONSOLE
	auto logger = &Voxel::Logger::getInstance();
	
	logger->consoleInfo("[SpriteSheetManager] All SpriteSheet informations");
	logger->consoleInfo("[SpriteSheetManager] Total spritesheets: " + std::to_string(spriteSheetMap.size()));

	for (auto& ss : spriteSheetMap)
	{
		if (ss.second)
		{
			logger->consoleInfo("[SpriteSheetManager] Key: " + ss.first);

			if (detail)
			{
				(ss.second)->print();
			}
		}
	}
#endif
}
