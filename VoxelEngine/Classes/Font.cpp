#include "Font.h"

// voxel
#include "Application.h"
#include "Utility.h"
#include "Texture2D.h"
#include "Utility.h"
#include "FileSystem.h"
#include "Logger.h"
#include "ErrorCode.h"

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
	, outlineSize(0)
{
	initLibrary();
}

void Voxel::Font::initLibrary()
{
	if (!library)
	{
		if (FT_Init_FreeType(&library))
		{
			library = nullptr;

			throw std::runtime_error(std::to_string(Voxel::Error::Code::ERROR_FAILED_TO_INITIALIZE_FREETYPE2));
		}
	}
}

Font* Font::create(const std::string& fontName, const int fontSize)
{
	auto newFont = new Font();
	if (newFont->init(fontName, fontSize, 0))
	{
		return newFont;
	}
	else
	{
		delete newFont;
		return nullptr;
	}
}

Font * Voxel::Font::createWithOutline(const std::string & fontName, const int fontSize, const int outlineSize)
{
	auto newFont = new Font();
	if (newFont->init(fontName, fontSize, outlineSize))
	{
		return newFont;
	}
	else
	{
		delete newFont;
		return nullptr;
	}
}

bool Voxel::Font::init(const std::string & fontName, const int fontSize, const int outlineSize)
{
	// Load the font with freetype2
	FT_Face face;

	this->outlineSize = outlineSize;

	std::string fontPath = FileSystem::getInstance().getWorkingDirectory() + "/" + DEFAULT_FONT_PATH + fontName;

	if (FT_New_Face(library, fontPath.c_str(), 0, &face))
	{
		throw std::runtime_error(std::to_string(Voxel::Error::Code::ERROR_FAILED_TO_LOAD_FONT_FILE) + "\nInvalid font file: \"" + fontPath + "\"");
	}

	//std::cout << "[Font] Glyph count = " << (face->ascender >> 6) << std::endl;

	// save font size
	size = fontSize;
	// If size is else than min, force to Min value
	if (size < MIN_FONT_SIZE)
	{
		size = MIN_FONT_SIZE;
	}
	
	// Set font size. Larger size = Higher font texture
	FT_Set_Pixel_Sizes(face, 0, size);

	FT_GlyphSlot glyphSlot = face->glyph;

	unsigned int widthSum = 0;
	unsigned int maxHeight = 0;

	// This is a 2 pixel wide extra padding in x, y coordinate in texture to prevent bleeding. So 1 pixels per side.
	unsigned int widthPadding = 1;
	unsigned int heightPadding = 1;

	const int textureSizeLimit = 2048;
	
	auto logger = &Voxel::Logger::getInstance();

	// whitespace to tilde. Iterate all character to find the size of character map
	// this loops find the texture size we are going to make for this font.
	// whitespace doesn't have texture space, so we can ignore, but font always can have different stuffs filled in any char, so we still check for it.
	for (unsigned int i = ' '; i <= '~'; i++)
	{
		if (FT_Load_Char(face, i, FT_LOAD_RENDER))
		{
#if V_DEBUG && V_DEBUG_LOG_CONSOLE
			logger->consoleWarn("[Font] \"" + fontName + " failed to load character: " + std::to_string(static_cast<char>(i)));
#endif
			continue;
		}

		// Adding pad
		widthSum += glyphSlot->bitmap.width + (widthPadding * 2) + (outlineSize * 2);

		unsigned int curHeight = (glyphSlot->bitmap.rows + (heightPadding * 2) + (outlineSize + 2));
		if (maxHeight < curHeight)
		{
			maxHeight = curHeight;
		}
	}

	// Add another padding at the end to prevent bleeding
	//maxHeight += heightPadding;

	int widthDiv = widthSum / 8;
	int heightDiv = maxHeight * 16;
	
	if (widthDiv > textureSizeLimit || heightDiv > textureSizeLimit)
	{
		FT_Done_Face(face);

		// Font texutre size is too big.
		// Todo: Fix this. Idea: Create program that exports font texture to separate image per char and use texture packer to create font atlas...
		assert(false);
		return false;
	}

	int pow2Width = Utility::Math::findNearestPowTwo(widthDiv);
	int pow2Height = Utility::Math::findNearestPowTwo(heightDiv);

	// Todo: don't foce to 256. make algorithm that finds minimum power 2 rentangular shape.
	pow2Width = 256;
	pow2Height = 256;
	
	//int pow2WidthSum = Utility::Math::findNearestPowTwo(widthSum);
	//int pow2MaxHeight = Utility::Math::findNearestPowTwo(maxHeight);

	//texAtlasWidth = static_cast<float>(pow2WidthSum);
	//texAtlasHeight = static_cast<float>(pow2MaxHeight);

	texAtlasWidth = static_cast<float>(pow2Width);
	texAtlasHeight = static_cast<float>(pow2Height);

	std::string textureName = Utility::String::removeFileExtFromFileName(fontName) + "_" + std::to_string(fontSize);

	if (outlineSize != 0)
	{
		textureName += "_Outlined";
	}

	texture = Texture2D::createFontTexture(textureName, pow2Width, pow2Height, GL_TEXTURE_2D);
	texture->setLocationOnProgram(ProgramManager::PROGRAM_NAME::UI_TEXT_SHADER);

	// bind font texture
	texture->bind();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// iterate over char map (whitespace to tilde) again. 
	// x offset. 
	float x = 0;
	// y offset.
	float y = static_cast<float>(heightPadding + outlineSize);

	int whitespaceHeight = 0;

	int customLineSpace = 0;

	for (unsigned int i = ' '; i <= '~'; i++)
	{
		if (FT_Load_Char(face, i, FT_LOAD_RENDER))
		{
#if V_DEBUG && V_DEBUG_LOG_CONSOLE
			logger->consoleWarn("[Font] \"" + fontName + " failed to load character: " + std::to_string(static_cast<char>(i)));
#endif
			continue;
		}

		// create glyph data
		char c = static_cast<char>(i);
		glyphMap.emplace(c, Glyph());

		auto& glyph = glyphMap[c];

		glyph.valid = true;
		glyph.c = c;
		glyph.metrics = glyphSlot->metrics;
		glyph.height = glyphSlot->metrics.height >> 6;
		glyph.width = glyphSlot->metrics.width >> 6;
		glyph.bearingX = glyphSlot->metrics.horiBearingX >> 6;
		glyph.bearingY = glyphSlot->metrics.horiBearingY >> 6;
		//std::cout << c << " " << glyph.height << ", " << glyph.bearingY << std::endl;
		glyph.botY = glyph.height - glyph.bearingY;
		glyph.advance = glyphSlot->metrics.horiAdvance >> 6;

		if (whitespaceHeight < glyph.height)
		{
			whitespaceHeight = glyph.height;
		}

		if ((size + glyph.botY) > customLineSpace)
		{
			customLineSpace = (size + glyph.botY);
		}

		// get next X.
		int nextX = (int)x + glyphSlot->bitmap.width + ((widthPadding + outlineSize) * 2);

		if (nextX > pow2Width)
		{
			// go to next line. Use max height as y offset instead of linespace
			y += static_cast<float>(maxHeight);

			if (y > pow2Height)
			{
#if V_DEBUG && V_DEBUG_LOG_CONSOLE
				logger->consoleError("[Font] \"" + fontName + " failed to load character: " + std::to_string(static_cast<char>(i)));
#endif
				throw std::runtime_error(std::to_string(Voxel::Error::Code::ERROR_FONT_TEXTURE_TOO_SMALL));
				return false;
			}
			x = 0;
		}

		// advance by paddings
		x += static_cast<float>(widthPadding + outlineSize);

		// substitude bitmap buffer to texture
		glTexSubImage2D(GL_TEXTURE_2D, 0, static_cast<int>(x), static_cast<int>(y), glyphSlot->bitmap.width, glyphSlot->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, glyphSlot->bitmap.buffer);

		// Top left of uv. Advance back by outline padding
		glyph.uvTopLeft = glm::vec2((x - static_cast<float>(outlineSize)) / texAtlasWidth, (y - static_cast<float>(outlineSize)) / texAtlasHeight);

		// advance by size of char bitmap width
		x += static_cast<float>(glyphSlot->bitmap.width);

		// Bottom right of uv. advance more 
		glyph.uvBotRight = glm::vec2((x + static_cast<float>(outlineSize)) / texAtlasWidth, ((static_cast<float>(glyphSlot->bitmap.rows) + y + static_cast<float>(outlineSize)) / texAtlasHeight));

		/*
		if (i == 70)
		{
			std::cout << "char = \"" << c << "\"\n";
			std::cout << "uvTopLeft (" << glyph.uvTopLeft.x << " ," << glyph.uvTopLeft.y << ")\n";
			std::cout << "uvBotRight (" << glyph.uvBotRight.x << " ," << glyph.uvBotRight.y << ")\n";
		}
		*/

		// advance by paddings
		x += static_cast<float>(widthPadding + outlineSize);
	}


	//linespace = (face->height >> 6);
	//linespace = customLineSpace;

	glyphMap[' '].height = whitespaceHeight;
	//linespace = whitespaceHeight;
	linespace = face->size->metrics.height >> 6;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	texture->saveToPNG();

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

int Voxel::Font::getOutlineSize()
{
	return outlineSize;
}

Texture2D * Voxel::Font::getTexture()
{
	return texture;
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

bool Voxel::Font::isOutlineEnabled()
{
	return outlineSize != 0;
}

int Voxel::Font::getSize()
{
	return size;
}
