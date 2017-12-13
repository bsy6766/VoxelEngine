#include "SpriteSheet.h"

// voxel
#include "Application.h"
#include "Utility.h"

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
	if (texture)
	{
		std::cout << "Texture: " << texture->getName() << "\n";
	}
	else
	{
		std::cout << "Texture: nullptr\n";
	}

	std::cout << "Images...\n";

	for (auto& ie : imageEntryMap)
	{
		std::cout << "Name: " << ie.first << "\n";
		std::cout << "Size: (" << (ie.second).width << ", " << (ie.second).height << ")\n";
		std::cout << "Texture coordinate: " << Utility::Log::vec2ToStr((ie.second).uvOrigin) << ", " << Utility::Log::vec2ToStr((ie.second).uvEnd) << std::endl;
	}
}

bool Voxel::SpriteSheet::init(const std::string & dataFileName)
{
	const std::string wd = Application::getInstance().getWorkingDirectory();

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
	
	auto& meta = j.at("meta");

	std::string textureName = meta.at("image");

	texture = Texture2D::createSpriteSheetTexture(textureName, GL_TEXTURE_2D);

	texture->setLocationOnProgram(ProgramManager::PROGRAM_NAME::UI_TEXTURE_SHADER);

	std::string textureFormatStr = meta.at("format");
	const float textureWidth = meta.at("size").at("w");
	const float textureHeight = meta.at("size").at("h");

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
	std::cout << "[SpriteSheetManager] All SpriteSheet info\n";
	std::cout << "[SpriteSheetManager] SpriteSheet count: " << spriteSheetMap.size() << std::endl;

	for (auto& ss : spriteSheetMap)
	{
		if (ss.second)
		{
			std::cout << "Spritesheet: " << ss.first << std::endl;
			if (detail)
			{
				(ss.second)->print();
			}
		}
	}
}
