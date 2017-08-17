#include "Font.h"

using namespace Voxel;

Font::Font()
	: size(0)
	, linespace(0)
	, texAtlasWidth(0)
	, texAtlasHeight(0)
{
}

Font::~Font()
{
}

Font* Font::create(const std::string& fontName, const int fontSize)
{
	auto newFont = new Font();
	if (newFont->init(fontName, fontSize))
	{
		return newFont;
	}
	else
	{
		delete newFont;
		return nullptr;
	}
}

bool Voxel::Font::init(const std::string & fontName, const int fontSize)
{
	return false;
}

Glyph Font::getCharGlyph(const char c)
{
	auto find_it = glyphMap.find(c);
	if (find_it == glyphMap.end())
	{
		return Glyph();
	}
	else
	{
		return find_it->second;
	}
}