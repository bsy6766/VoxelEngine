#ifndef FONT_H
#define FONT_H

// freetype
#include <ft2build.h>
#include <freetype\freetype.h>
#include <freetype\ftglyph.h>

// glm
#include <glm\glm.hpp>

// cpp
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
		// Width of character
		int width;
		// Height of character
		int height;
		// The upper height of character from pen position. i.e. F's bearingY equals to height while g's bearingY is small than height because character goes under base line
		int bearingY;
		// the width of character from pen position.
		int bearingX;
		// The lower height of character from pen position
		int botY;
		// Horizontal advance of character. Use this to calculate next character's pen position
		int advance;
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
		// constants
		const static int MIN_FONT_SIZE;
		const static std::string DEFAULT_FONT_PATH;
	private:
		// Freetype library. Reads library.
		static FT_Library library;

		// size of font
		int size;

		// font's space between lines
		int linespace;

		// Font texture
		Texture2D* texture;

		// Texture size
		float texAtlasWidth;
		float texAtlasHeight;

		// outline size in pixel
		int outlineSize;

		// Glyph map
		std::unordered_map<char, Glyph> glyphMap;

		// constructor
		Font();

		// initialize font
		bool init(const std::string& fontName, const int fontSize, const int outlineSize);

		// init FreeType
		void initLibrary();
	public:
		// creates font
		static Font* create(const std::string& fontName, const int fontSize);

		// creates font with outline
		static Font* createWithOutline(const std::string& fontName, const int fontSize, const int outlineSize);

		// destructor
		~Font();

		// Get glyph for specific char
		Glyph* getCharGlyph(const char c);

		// Get line space
		int getLineSpace();

		// Get outline size in pixel
		int getOutlineSize();

		// Get texture
		Texture2D* getTexture();

		// Close free type library
		static void closeFreetype();

		// activate font texture
		void activateTexture(unsigned int/*glenum*/ textureUnit);

		// bind font texture
		void bind();

		// Simply compares the size of outline. If 0, no outline supported
		bool isOutlineEnabled();

		// get font size in pixel
		int getSize();
	};
}

#endif FONT_H