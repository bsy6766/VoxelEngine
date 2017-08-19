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

	//linespace = (face->height >> 6);

	FT_GlyphSlot glyphSlot = face->glyph;

	unsigned int widthSum = 0;
	unsigned int maxHeight = 0;

	int widthPadding = 2;
	int heightPadding = 2;

	const int textureSizeLimit = 2048;
	
	// whitespace to tilde. Iterate all character to find the size of character map
	// this loops find the texture size we are going to make for this font.
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

	linespace = maxHeight + heightPadding;

	int widthDiv = widthSum / 8;
	int heightDiv = linespace * 16;
	
	if (widthDiv > textureSizeLimit || heightDiv > textureSizeLimit)
	{
		FT_Done_Face(face);
		std::cout << "[Font] Font size is too big to generate font texture" << std::endl;
		return false;
	}

	int pow2Width = Utility::Math::findNearestPowTwo(widthDiv);
	int pow2Height = Utility::Math::findNearestPowTwo(heightDiv);

	// Todo: don't foce to 1024. make algorithm that finds minimum power 2 rentangular shape.
	pow2Width = 1024;
	pow2Height = 1024;

	std::cout << "[Font] Generating font texture sized (" << pow2Width << ", " << pow2Height << ")" << std::endl;

	//int pow2WidthSum = Utility::Math::findNearestPowTwo(widthSum);
	//int pow2MaxHeight = Utility::Math::findNearestPowTwo(maxHeight);

	//texAtlasWidth = static_cast<float>(pow2WidthSum);
	//texAtlasHeight = static_cast<float>(pow2MaxHeight);

	texAtlasWidth = static_cast<float>(pow2Width);
	texAtlasHeight = static_cast<float>(pow2Height);

	texture = Texture2D::createFontTexture(pow2Width, pow2Height, GL_TEXTURE_2D);
	texture->setLocationOnProgram(ProgramManager::PROGRAM_NAME::SHADER_TEXT);

	// bind font texture
	texture->bind();

	// iterate over char map (whitespace to tilde) again. 
	// x offset
	float x = 2;
	float y = 2;
	for (size_t i = ' '; i <= '~'; i++)
	{
		if (FT_Load_Char(face, i, FT_LOAD_RENDER))
		{
			std::cout << "[Font] Failed to load character" << static_cast<char>(i) << std::endl;
			continue;
		}

		// create glyph data
		char c = static_cast<char>(i);
		glyphMap.emplace(c, Glyph());

		auto& glyph = glyphMap[c];

		glyph.valid = true;
		glyph.c = c;
		glyph.metrics = glyphSlot->metrics;

		int botRightX = (int)x + glyphSlot->bitmap.width;
		int xWidthPad = botRightX + widthPadding;

		if (xWidthPad > pow2Width)
		{
			// go to next line
			y += static_cast<float>(linespace);

			if (y > pow2Height)
			{
				std::cout << "[Font] Error. Font size is too big to fit in texture" << std::endl;
				return false;
			}
			x = 2;
		}

		// substitude bitmap buffer to texture
		glTexSubImage2D(GL_TEXTURE_2D, 0, static_cast<int>(x), static_cast<int>(y), glyphSlot->bitmap.width, glyphSlot->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, glyphSlot->bitmap.buffer);


		glyph.uvTopLeft = glm::vec2(x / texAtlasWidth, y / texAtlasHeight);

		x += glyphSlot->bitmap.width;

		glyph.uvBotRight = glm::vec2(x / texAtlasWidth, ((static_cast<float>(glyphSlot->bitmap.rows) + y) / texAtlasHeight));

		//std::cout << "char = \"" << c << "\"" << std::endl;
		//std::cout << "uvTopLeft (" << glyph.uvTopLeft.x << " ," << glyph.uvTopLeft.y << ")" << std::endl;
		//std::cout << "uvBotRight (" << glyph.uvBotRight.x << " ," << glyph.uvBotRight.y << ")" << std::endl;

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

int Voxel::Font::getLineSpace()
{
	return linespace;
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