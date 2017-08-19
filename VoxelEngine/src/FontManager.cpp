#include "FontManager.h"

#include <Font.h>
#include <iostream>

using namespace Voxel;

int FontManager::idCounter = 0;

const std::string FontManager::DEFAULT_FONT_NAME = "Ariel.ttf";

FontManager::FontManager()
{
	initDefaultFont();
}

FontManager::~FontManager()
{
	clear();
	Font::closeFreetype();
}

void FontManager::initDefaultFont()
{
	auto arial = Font::create("Arial.ttf", 50);
	if (arial)
	{
		std::cout << "[FontManager] Adding default font \"Arial.ttf\" with id #0" << std::endl;
		fonts.emplace(0, arial);
	}
	else
	{
		throw std::runtime_error("Failed to load default font \"Arial.ttf\"");
	}
}

int FontManager::addFont(const std::string& fontName, const int fontSize)
{
	Font* newFont = Font::create(fontName, fontSize);
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
