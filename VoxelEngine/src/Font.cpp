#include "Font.h"
#include <Application.h>
#include <Utility.h>
#include <Texture2D.h>

using namespace Voxel;

FT_Library Font::library = nullptr;

const int Font::MIN_FONT_SIZE = 10;
const std::string Font::DEFAULT_FONT_PATH = "fonts/";

Font::Font()
	: size(0)
	, linespace(0)
	, texAtlasWidth(0)
	, texAtlasHeight(0)
	, texture(nullptr)
{
	initLibrary();
}

Font::~Font()
{
	if (texture)
	{
		delete texture;
	}
}

void Voxel::Font::initLibrary()
{
	if (!library)
	{
		if (FT_Init_FreeType(&library))
		{
			library = nullptr;
			throw std::runtime_error("Failed to initialize Freetype2");
		}
	}
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
	// Load the font with freetype2
	FT_Face face;

	std::string fontPath = Application::getInstance().getWorkingDirectory() + "/" + DEFAULT_FONT_PATH + fontName;
	if (FT_New_Face(library, fontPath.c_str(), 0, &face))
	{
		throw std::runtime_error("Freetype2 failed to load font at \"" + fontPath + "\"");
	}

	// save font size
	size = fontSize;
	// If size is else than min, force to Min value
	if (size < MIN_FONT_SIZE)
	{
		size = MIN_FONT_SIZE;
	}
	
	// Set font size. Larger size = Higher font texture
	FT_Set_Pixel_Sizes(face, 0, size);

	linespace = (face->height >> 6);

	FT_GlyphSlot glyphSlot = face->glyph;

	unsigned int widthSum = 0;
	unsigned int maxHeight = 0;

	int widthPadding = 2;

	// whitespace to tilde. Iterate all character to find the size of character map
	for (size_t i = ' '; i < '~'; i++)
	{
		if (FT_Load_Char(face, i, FT_LOAD_RENDER))
		{
			std::cout << "[Font] Failed to load character" << static_cast<char>(i) << std::endl;
			continue;
		}

		// Adding pad
		widthSum += glyphSlot->bitmap.width + widthPadding;

		if (maxHeight < glyphSlot->bitmap.rows)
		{
			maxHeight = glyphSlot->bitmap.rows;
		}
	}

	texAtlasWidth = static_cast<float>(widthSum);
	texAtlasHeight = static_cast<float>(maxHeight);

	texture = Texture2D::createFontTexturx(widthSum, maxHeight);

	// bind font texture
	texture->bind();

	// iterate over char map (whitespace to tilde) again. 
	// x offset
	float x = 0;
	for (size_t i = ' '; i <= '~'; i++)
	{
		if (FT_Load_Char(face, i, FT_LOAD_RENDER))
		{
			std::cout << "[Font] Failed to load character" << static_cast<char>(i) << std::endl;
			continue;
		}

		// substitude bitmap buffer to texture
		glTexSubImage2D(GL_TEXTURE_2D, 0, static_cast<int>(x), 0, glyphSlot->bitmap.width, glyphSlot->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, glyphSlot->bitmap.buffer);

		// create glyph data
		char c = static_cast<char>(i);
		glyphMap.emplace(c, Glyph());

		auto& glyph = glyphMap[c];

		glyph.valid = true;
		glyph.c = c;
		glyph.metrics = glyphSlot->metrics;

		glyph.uvTopLeft = glm::vec2(x / texAtlasWidth, 0);

		x += glyphSlot->bitmap.width;

		glyph.uvBotRight = glm::vec2(x / texAtlasWidth, (glyphSlot->bitmap.rows / texAtlasHeight));

		x += widthPadding;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//make sure release face.
	FT_Done_Face(face);

	return true;
}

Glyph* Font::getCharGlyph(const char c)
{
	auto find_it = glyphMap.find(c);
	if (find_it == glyphMap.end())
	{
		return nullptr;
	}
	else
	{
		return &(find_it->second);
	}
}

void Voxel::Font::closeFreetype()
{
	if(Font::library)
	{
		FT_Done_FreeType(library);
	}

	Font::library = nullptr;
}

void Voxel::Font::activateTexture(unsigned int textureUnit)
{
	texture->activate(textureUnit);
}

void Voxel::Font::bind()
{
	texture->bind();
}