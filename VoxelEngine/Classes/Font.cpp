#include "Font.h"

// voxel
#include "Application.h"
#include "Utility.h"
#include "Texture2D.h"
#include "Utility.h"
#include "FileSystem.h"
#include "Logger.h"
#include "ErrorCode.h"
#include "BinPacker.h"

using namespace Voxel;

FT_Library Font::library = nullptr;

const int Font::MIN_FONT_SIZE = 10;
const std::string Font::DEFAULT_FONT_PATH = "fonts/";

Font::Font()
	: fontSize(0)
	, linespace(0)
	, textureSize(0)
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

int Voxel::Font::getTextureSizeByLocale(const Voxel::Localization::Tag locale) const
{
	int size = 0;

	switch (locale)
	{
	case Voxel::Localization::Tag::en_US:
		if (outlineSize == 0)
		{
			size = 128;
		}
		else
		{
			size = 256;
		}
		break;
	case Voxel::Localization::Tag::ko_KR:
		size = 2048;
		break;
	default:
		// Bad locale
		assert(false);
		break;
	}

	return size;
}

Font* Font::create(const std::string& fontName, const int fontSize, const Voxel::Localization::Tag locale)
{
	auto newFont = new Font();
	if (newFont->init(fontName, fontSize, 0, locale))
	{
		return newFont;
	}
	else
	{
		delete newFont;
		return nullptr;
	}
}

Font * Voxel::Font::createWithOutline(const std::string & fontName, const int fontSize, const int outlineSize, const Voxel::Localization::Tag locale)
{
	auto newFont = new Font();
	if (newFont->init(fontName, fontSize, outlineSize, locale))
	{
		return newFont;
	}
	else
	{
		delete newFont;
		return nullptr;
	}
}

bool Voxel::Font::init(const std::string & fontName, const int fontSize, const int outlineSize, const Voxel::Localization::Tag locale)
{
	auto start = Utility::Time::now();

	// Load the font with freetype2
	FT_Face face;

	this->fontName = fontName;

	this->outlineSize = outlineSize;

	std::string fontPath = FileSystem::getInstance().getWorkingDirectory() + "/" + DEFAULT_FONT_PATH + fontName;

	if (FT_New_Face(library, fontPath.c_str(), 0, &face))
	{
		throw std::runtime_error(std::to_string(Voxel::Error::Code::ERROR_FAILED_TO_LOAD_FONT_FILE) + "\nInvalid font file: \"" + fontPath + "\"");
	}

	// save font size
	this->fontSize = fontSize;
	// If size is else than min, force to Min value
	if (this->fontSize < MIN_FONT_SIZE)
	{
		this->fontSize = MIN_FONT_SIZE;
	}
	
	// Set font size. Larger size = Higher font texture
	FT_Set_Pixel_Sizes(face, 0, this->fontSize);

	// Todo: Each language need different size of texture
	textureSize = getTextureSizeByLocale(locale);
	
	std::string textureName = Utility::String::removeFileExtFromFileName(fontName) + "_" + std::to_string(fontSize);

	if (outlineSize != 0)
	{
		textureName += "_Outlined";
	}

	texture = Texture2D::createFontTexture(textureName, textureSize, textureSize, GL_TEXTURE_2D);
	texture->setLocationOnProgram(ProgramManager::PROGRAM_NAME::UI_TEXT_SHADER);

	// bind font texture
	texture->bind();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// iterate over char map (whitespace to tilde) again. 

	auto startBinPack = Utility::Time::now();
	switch (locale)
	{
	case Voxel::Localization::Tag::en_US:
		initEnUS(face);
		break;
	case Voxel::Localization::Tag::ko_KR:
		initKoKR(face);
		break;
	default:
		assert(false);
		break;
	}
	auto endBinPack = Utility::Time::now();
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	auto end = Utility::Time::now();

#if V_DEBUG && V_DEBUG_CONSOLE
	Voxel::Logger::getInstance().consoleInfo("[Font] Texture generation took: " + Utility::Time::toMicroSecondString(start, end) + ", bin packing took: " + Utility::Time::toMicroSecondString(startBinPack, endBinPack));
#endif

#if V_DEBUG
	//texture->saveToPNG();
#endif

	//make sure release face.
	FT_Done_Face(face);

	return true;
}

bool Voxel::Font::initDefaultCharacters(FT_Face & face)
{
	return false;
}

bool Voxel::Font::initEnUS(FT_Face& face)
{
	FT_GlyphSlot glyphSlot = face->glyph;

	auto logger = &Voxel::Logger::getInstance();

	int whitespaceHeight = 0;
	
	const int padding = 1;
	const float padF = static_cast<float>(padding);
	const int totalPad = padding + outlineSize;
	const float totalPadF = static_cast<float>(totalPad);
	const float textureSizeF = static_cast<float>(textureSize);

	Voxel::Bin::BinPacker bp;
	bp.init(glm::vec2(textureSizeF, textureSizeF));
	
	// en-US load characters in ascii table.
	for (int unicode = 32/* whitespace */; unicode <= 126/* ~ */; unicode++)
	{
		if (FT_Load_Char(face, unicode, FT_LOAD_RENDER))
		{
#if V_DEBUG && V_DEBUG_LOG_CONSOLE
			//logger->consoleWarn("[Font] \"" + fontName + " failed to load character: " + std::to_string(static_cast<char>(unicode)));
#endif
			continue;
		}

		// create glyph data
		glyphMap.emplace(unicode, Glyph());

		auto& glyph = glyphMap[unicode];

		glyph.valid = true;
		glyph.unicode = unicode;
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

		const float charX = (totalPadF * 2.0f) + static_cast<float>(glyphSlot->bitmap.width);

		const float charY = (totalPadF * 2.0f) + static_cast<float>(glyphSlot->bitmap.rows);

		Voxel::Bin::ItemNode* itemNode = bp.insert(glm::vec2(charX, charY));

		if (itemNode)
		{
			// substitude bitmap buffer to texture

			if (unicode == 32/* whitespace */)
			{
				glyph.uvTopLeft.x = 0.0f;
				glyph.uvTopLeft.y = 0.0f;

				glyph.uvBotRight.x = 0.0f;
				glyph.uvBotRight.y = 0.0f;
			}
			else
			{
				glTexSubImage2D(GL_TEXTURE_2D, 0, static_cast<int>(itemNode->area.origin.x) + totalPad, static_cast<int>(itemNode->area.origin.y) + totalPad, glyphSlot->bitmap.width, glyphSlot->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, glyphSlot->bitmap.buffer);

				glyph.uvTopLeft.x = (itemNode->area.origin.x + padF) / textureSizeF;
				glyph.uvTopLeft.y = ((itemNode->area.origin.y + padF) / textureSizeF);

				glyph.uvBotRight.x = ((itemNode->area.origin.x + itemNode->area.size.x) - padF) / textureSizeF;
				glyph.uvBotRight.y = (((itemNode->area.origin.y + itemNode->area.size.y) - padF) / textureSizeF);
			}
		}
		else
		{
			// todo: remove this on release
			assert(false);
		}
	}
	
	glyphMap[32/* whtiesapce */].height = whitespaceHeight;

	linespace = face->size->metrics.height >> 6;

	return true;
}

bool Voxel::Font::initKoKR(FT_Face & face)
{
	FT_GlyphSlot glyphSlot = face->glyph;

	auto logger = &Voxel::Logger::getInstance();

	int whitespaceHeight = 0;

	const int padding = 1;
	const float padF = static_cast<float>(padding);
	const int totalPad = padding + outlineSize;
	const float totalPadF = static_cast<float>(totalPad);
	const float textureSizeF = static_cast<float>(textureSize);

	Voxel::Bin::BinPacker bp;
	bp.init(glm::vec2(textureSizeF, textureSizeF));

	for (int unicode = 44032; unicode <= 55203; unicode++)
	{
		auto ci = FT_Get_Char_Index(face, unicode);
		if (ci == 0)
		{
			continue;
		}

		if (FT_Load_Char(face, unicode, FT_LOAD_RENDER))
		{
#if V_DEBUG && V_DEBUG_LOG_CONSOLE
			logger->consoleWarn("[Font] \"" + fontName + " failed to load character: " + std::to_string(unicode));
#endif
			continue;
		}

		// create glyph data
		glyphMap.emplace(unicode, Glyph());

		auto& glyph = glyphMap[unicode];

		glyph.valid = true;
		glyph.unicode = unicode;
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

		const float charX = (totalPadF * 2.0f) + static_cast<float>(glyphSlot->bitmap.width);

		const float charY = (totalPadF * 2.0f) + static_cast<float>(glyphSlot->bitmap.rows);

		Voxel::Bin::ItemNode* itemNode = bp.insert(glm::vec2(charX, charY));

		if (itemNode)
		{
			// substitude bitmap buffer to texture
			const glm::vec2 origin = itemNode->area.origin;
			const glm::vec2 size = itemNode->area.size;

			glTexSubImage2D(GL_TEXTURE_2D, 0, static_cast<int>(origin.x) + totalPad, static_cast<int>(origin.y) + totalPad, glyphSlot->bitmap.width, glyphSlot->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, glyphSlot->bitmap.buffer);

			glyph.uvTopLeft.x = (itemNode->area.origin.x + padF) / textureSizeF;
			glyph.uvTopLeft.y = ((itemNode->area.origin.y + padF) / textureSizeF);

			glyph.uvBotRight.x = ((itemNode->area.origin.x + itemNode->area.size.x) - padF) / textureSizeF;
			glyph.uvBotRight.y = (((itemNode->area.origin.y + itemNode->area.size.y) - padF) / textureSizeF);
		}
		else
		{
			// todo: remove this on release
			abort();
		}
	}

	glyphMap[' '].height = whitespaceHeight;

	linespace = face->size->metrics.height >> 6;

	return true;
}

Glyph * Voxel::Font::getGlyph(const int unicode)
{
	auto find_it = glyphMap.find(unicode);
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
	return fontSize;
}
