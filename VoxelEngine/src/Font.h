#ifndef FONT_H
#define FONT_H

#include <ft2build.h>
#include <freetype\freetype.h>
#include <freetype\ftglyph.h>
#include <glm\glm.hpp>
#include <unordered_map>
#include <string>

namespace Voxel
{
	class Texture2D;

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
	public:
		const static int MIN_FONT_SIZE;
		const static std::string DEFAULT_FONT_PATH;
	private:
		// Freetype library. Reads library.
		static FT_Library library;

		// Font attributes
		int size;
		int linespace;

		// Font texture
		Texture2D* texture;
		float texAtlasWidth;
		float texAtlasHeight;


		// Glyph map
		std::unordered_map<char, Glyph> glyphMap;

		Font();

		bool init(const std::string& fontName, const int fontSize);
		void initLibrary();
	public:
		static Font* create(const std::string& fontName, const int fontSize);
		~Font();

		Glyph getCharGlyph(const char c);

		int getLineSpace();

		static void closeFreetype();
	};
}

#endif FONT_H