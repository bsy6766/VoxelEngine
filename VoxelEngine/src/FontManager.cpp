#include "FontManager.h"

#include <Font.h>

using namespace Voxel;

const std::string FontManager::DEFAULT_FONT_NAME = "Ariel.ttf";
const std::string FontManager::DEFAULT_FONT_PATH = "fonts/";

FontManager::FontManager()
{
	initDefaultFont();
}

FontManager::~FontManager()
{
	for (auto font : fonts)
	{
		if (font.second)
		{
			delete font.second;
		}
	}

	fonts.clear();
}

void FontManager::initDefaultFont()
{

}

int FontManager::addFont(const std::string& fontPath)
{
	return -1;
}

Font* FontManager::getFont(const int id)
{
	if (id < 0)
	{
		return nullptr;
	}
	else if (id == 0)
	{
		// return default font
	}
	else
	{
		auto find_it = fonts.find(id);
		if (find_it == fonts.end())
		{
			return nullptr;
		}
		else
		{
			return find_it->second;
		}
	}
	return nullptr;
}