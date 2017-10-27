#include "SpriteSheet.h"

#include <Application.h>
#include <fstream>

using namespace Voxel;
using json = nlohmann::json;

Voxel::SpriteSheet::SpriteSheet()
	: texture(nullptr)
{
}

Voxel::SpriteSheet::~SpriteSheet()
{
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

	texture = Texture2D::create(textureName, GL_TEXTURE_2D);

	texture->setLocationOnProgram(ProgramManager::PROGRAM_NAME::SHADER_TEXTURE_COLOR);

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
		
		newEntry.uvOrigin.x = ((newEntry.position.x + newEntry.width) / textureWidth);
		newEntry.uvOrigin.y = ((newEntry.position.y + newEntry.height) / textureHeight);

		newEntry.uvEnd.x = (newEntry.position.x / textureWidth);
		newEntry.uvEnd.y = (newEntry.position.y / textureHeight);

		imageEntryMap.emplace(imageName, newEntry);
	}

	return true;
}




Voxel::SpriteSheetManager::~SpriteSheetManager()
{
	releaseAll();
}

std::string Voxel::SpriteSheetManager::trimFileExtention(const std::string & fileName)
{
	return fileName.substr(0, fileName.find_last_of('.'));
}

bool Voxel::SpriteSheetManager::addSpriteSheet(const std::string & jsonFileName)
{
	auto fileName = trimFileExtention(jsonFileName);
	
	auto find_it = spriteSheetMap.find(fileName);

	if (find_it == spriteSheetMap.end())
	{
		auto ss = SpriteSheet::create(jsonFileName);
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

SpriteSheet * Voxel::SpriteSheetManager::getSpriteSheet(const std::string & jsonFileName)
{
	auto find_it = spriteSheetMap.find(trimFileExtention(jsonFileName));
	if (find_it == spriteSheetMap.end())
	{
		return nullptr;
	}
	else
	{
		return find_it->second;
	}
}

bool Voxel::SpriteSheetManager::hasSpriteSheet(const std::string & name)
{
	return this->spriteSheetMap.find(name) != this->spriteSheetMap.end();
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
