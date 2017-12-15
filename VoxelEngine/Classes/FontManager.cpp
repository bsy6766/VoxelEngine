#include "FontManager.h"

// voxel
#include "Font.h"

// cpp
#include <iostream>

using namespace Voxel;

int FontManager::idCounter = 0;

FontManager::~FontManager()
{
	clear();
	Font::closeFreetype();
}


int FontManager::addFont(const std::string& fontName, const int fontSize, int outline)
{
	Font* newFont = nullptr; 

	if (outline < 0)
	{
		outline = 0;
	}

	if (outline == 0)
	{
		newFont = Font::create(fontName, fontSize);
	}
	else
	{
		newFont = Font::createWithOutline(fontName, fontSize, outline);
	}

	if (newFont)
	{
		FontManager::idCounter++;
		fonts.emplace(FontManager::idCounter, newFont);
		std::cout << "[FontManager] Adding new font \"" + fontName + "\" with size: " << fontSize << std::endl;
		return FontManager::idCounter;
	}
	else
	{
		delete newFont;
		newFont = nullptr;
		return -1;
	}
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

void Voxel::FontManager::clear()
{
	Font* arial = nullptr;

	for (auto font : fonts)
	{
		if (font.first == 0)
		{
			// Don't delete arial
			arial = font.second;
			continue;
		}
		else
		{
			if (font.second)
			{
				delete font.second;
			}
		}
	}

	fonts.clear();

	fonts.emplace(0, arial);

	FontManager::idCounter = 0;
}
