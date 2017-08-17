#ifndef FONT_H
#define FONT_H

#include <ft2build.h>
#include <freetype\freetype.h>
#include <freetype\ftglyph.h>
#include <glm\glm.hpp>
#include <unordered_map>

namespace Voxel
{
	/**
	*	@class Glyph
	*	@brief Contains each character's glyph metric and texture data in font.
	*/
	struct Glyph
	{
	public:
		// True if this character's glyph is loaded
		bool valid;
		// Character
		char c;
		// metrics (width, height, advance, etc)
		FT_Glyph_Metrics metrics;
		// Texture coordinates
		glm::vec2 uvTopLeft;
		glm::vec2 uvBotRight;
	};


	/**
	*	@class Font
	*	@brief Loads TTF font using Freetype 2
	*/
	class Font
	{
	private:
		// Freetype library. Reads library.
		static FT_Library library;

		int size;
		int linespace;
		float texAtlasWidth;
		float texAtlasHeight;

		// Glyph map
		std::unordered_map<char, Glyph> glyphMap;

		Font();

		bool init(const std::string& fontName, const int fontSize);
	public:
		static Font* create(const std::string& fontName, const int fontSize);
		~Font();

		Glyph getCharGlyph(const char c);

		int getLineSpace();
	};
}

#endif FONT_H