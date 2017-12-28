// pch
#include "PreCompiled.h"

#include "Texture2D.h"

// voxel
#include "Application.h"
#include "ProgramManager.h"
#include "Program.h"
#include "Utility.h"
#include "FileSystem.h"
#include "Logger.h"
#include "Config.h"

using namespace Voxel;

const std::string Texture2D::DEFAULT_TEXTURE_PATH = "textures/";

Texture2D::Texture2D()
	: textureObject(0)
	, textureLocation(-1)
	, width(0)
	, height(0)
	, channel(0)
{}

Texture2D::~Texture2D()
{
	glDeleteTextures(1, &textureObject);
}

Texture2D * Voxel::Texture2D::create(const std::string & textureName, GLenum textureTarget)
{
	auto& tm = TextureManager::getInstance();

	std::string rawName = Utility::String::removeFileExtFromFileName(textureName);

	if (tm.hasTexture(rawName))
	{
		return tm.getTexture(rawName).get();
	}
	else
	{
		auto newTexture = new Texture2D();
		if (newTexture->init(textureName, textureTarget))
		{
			newTexture->name = rawName;
			tm.addTexture(rawName, newTexture);
			return newTexture;
		}
		else
		{
			delete newTexture;
			return nullptr;
		}
	}
}

Texture2D * Voxel::Texture2D::createSpriteSheetTexture(const std::string & textureName, GLenum textureTarget)
{
	auto& tm = TextureManager::getInstance();

	std::string rawName = Utility::String::removeFileExtFromFileName(textureName);

	if (tm.hasTexture(rawName))
	{
		return tm.getTexture(rawName).get();
	}
	else
	{
		auto newTexture = new Texture2D();
		if (newTexture->initUISpriteSheetTexture(textureName, textureTarget))
		{
			newTexture->name = rawName;
			tm.addTexture(textureName, newTexture);
			return newTexture;
		}
		else
		{
			delete newTexture;
			return nullptr;
		}
	}
}

Texture2D * Voxel::Texture2D::createFontTexture(const std::string& textureName, const int width, const int height, GLenum textureTarget)
{
	auto& tm = TextureManager::getInstance();
	
	if (tm.hasTexture(textureName))
	{
		return tm.getTexture(textureName).get();
	}
	else
	{
		auto newTexture = new Texture2D();
		if (newTexture->initFontTexture(width, height, textureTarget))
		{
			newTexture->name = textureName;
			tm.addTexture(textureName, newTexture);
			return newTexture;
		}
		else
		{
			delete newTexture;
			return nullptr;
		}
	}
}

glm::ivec2 Voxel::Texture2D::getTextureSize()
{
	return glm::ivec2(width, height);
}

void Voxel::Texture2D::setLocationOnProgram(ProgramManager::PROGRAM_NAME programName)
{
	//ProgramManager::getInstance().getDefaultProgram(programName)->use(true);
	setLocationOnProgram(ProgramManager::getInstance().getProgram(programName)->getUniformLocation("tex"));
}

void Voxel::Texture2D::setLocationOnProgram(const GLint textureLocation)
{
	this->textureLocation = textureLocation;
}

std::string Voxel::Texture2D::getName() const
{
	return name;
}

bool Voxel::Texture2D::saveToPNG()
{
	auto start = Utility::Time::now();

#if V_DEBUG && V_DEBUG_CONSOLE
	auto logger = &Voxel::Logger::getInstance();
	logger->consoleInfo("[Texture2D] Saving to png...");
#endif

	// bind
	glBindTexture(this->textureTarget, this->textureObject);

	// buffer
	uint8_t *pixels;

	// to channel enum
	Channel channelEnum = static_cast<Channel>(channel);

	// Gl get tex image reads left to right, top to bottom
	switch (channelEnum)
	{
	case Voxel::Texture2D::Channel::RGB:
		pixels = new uint8_t[width * height * 3];
		glGetTexImage(textureTarget, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
		break;
	case Voxel::Texture2D::Channel::RGBA:
		pixels = new uint8_t[width * height * 4];
		glGetTexImage(textureTarget, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		break;
	case Voxel::Texture2D::Channel::GRAYSCALE:
	case Voxel::Texture2D::Channel::GRAYSCALE_ALPHA:
	default:
		// This engine doesn't supports grayscale
		glBindTexture(this->textureTarget, 0);
		return false;
		break;
	}

	// open file
	FILE* file = fopen((Voxel::FileSystem::getInstance().getWorkingDirectory() + "/FontTextureDump/" + name + ".png").c_str(), "wb");

	// check file
	if (!file)
	{
		// Failed to open file
#if V_DEBUG && V_DEBUG_CONSOLE
		logger->consoleInfo("[Texture2D] Failed to open file: " + (Voxel::FileSystem::getInstance().getWorkingDirectory() + "/" + name + ".png"));
#endif
		// delete pixels
		delete[] pixels;
		// unbind
		glBindTexture(this->textureTarget, 0);

		return false;
	}

	// create png struct
	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png)
	{
		// failed
		// delete pixels
		delete[] pixels;
		// close file
		fclose(file);
		// unbind
		glBindTexture(this->textureTarget, 0);

		return false;
	}

	// create png info
	png_infop info = png_create_info_struct(png);
	if (!info)
	{
		// failed
		// delete pixels
		delete[] pixels;
		// close file
		fclose(file);
		// unbind
		glBindTexture(this->textureTarget, 0);

		return false;
	}

	if (setjmp(png_jmpbuf(png)))
	{
		// delete pixels
		delete[] pixels;
		// close file
		fclose(file);
		// unbind
		glBindTexture(this->textureTarget, 0);
		// destroy png struct and info
		png_destroy_write_struct(&png, &info);

		return false;
	}

	png_init_io(png, file);

	if (channelEnum == Channel::RGB)
	{
		png_set_IHDR(png, info, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	}
	else if (channelEnum == Channel::RGBA)
	{
		png_set_IHDR(png, info, width, height, 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	}

	png_write_info(png, info);

	png_bytep *row_pointers = (png_bytep *)malloc(height * sizeof(png_bytep));

	if (row_pointers == nullptr)
	{
		// failed to malloc
		// delete pixels
		delete[] pixels;
		// close file
		fclose(file);
		// unbind
		glBindTexture(this->textureTarget, 0);
		// destroy png struct and info
		png_destroy_write_struct(&png, &info);

		return false;
	}

	// Convert pixel to row pointer
	if (channelEnum == Channel::RGB)
	{
		for (int i = 0; i < height; i++)
		{
			row_pointers[i] = (png_bytep)pixels + (i * width * 3);
		}
	}
	else if (channelEnum == Channel::RGBA)
	{
		for (int i = 0; i < height; i++)
		{
			row_pointers[i] = (png_bytep)pixels + (i * width * 4);
		}
	}

	// write png
	png_write_image(png, row_pointers);

	// free
	free(row_pointers);
	row_pointers = nullptr;

	// end png
	png_write_end(png, info);

	// release
	png_destroy_write_struct(&png, &info);
	
	// release pixels
	delete[] pixels;

	// close file
	fclose(file);

	// unbind texture
	glBindTexture(this->textureTarget, 0);

	auto end = Utility::Time::now();

#if V_DEBUG && V_DEBUG_CONSOLE
	logger->consoleInfo("[Texture2D] Texture saved (" + Utility::Time::toMilliSecondString(start, end) + "): " + (Voxel::FileSystem::getInstance().getWorkingDirectory() + "/" + name + ".png"));
#endif

	// success!
	return true;
}

void Voxel::Texture2D::activate(GLenum textureUnit)
{
	glActiveTexture(textureUnit);
}

void Voxel::Texture2D::bind()
{
	glBindTexture(textureTarget, textureObject);
}

void Voxel::Texture2D::enableTexLoc()
{
	if (textureLocation != -1)
	{
		glUniform1i(textureLocation, 0);
	}
}

void Voxel::Texture2D::print()
{
#if V_DEBUG && V_DEBUG_LOG_CONSOLE
	auto logger = &Voxel::Logger::getInstance();

	logger->consoleInfo("[Texture] Info");
	logger->consoleInfo("[Texture] Name: " + name);
	logger->consoleInfo("[Texture] Texture object: " + std::to_string(textureObject));
	logger->consoleInfo("[Texture] Texture target: " + std::to_string(textureTarget));
	logger->consoleInfo("[Texture] Texture location: " + std::to_string(textureLocation));
	logger->consoleInfo("[Texture] Width: " + std::to_string(width));
	logger->consoleInfo("[Texture] Height: " + std::to_string(height));
	logger->consoleInfo("[Texture] Channel: " + std::to_string(channel));
#endif
}

bool Voxel::Texture2D::init(const std::string & textureName, GLenum textureTarget)
{
	unsigned char* data = loadImage(textureName, this->width, this->height, this->channel);

	if (width == 0 || height == 0 || channel == 0)
	{
		throw std::runtime_error("Bad texture file");
	}

	this->textureTarget = textureTarget;

	if (data)
	{
		generate2DTexture(width, height, channel, data);

		stbi_image_free(data);

		return true;
	}
	else
	{
		return false;
	}
}

bool Voxel::Texture2D::initUISpriteSheetTexture(const std::string & textureName, GLenum textureTarget)
{
	unsigned char* data = loadImage(textureName, this->width, this->height, this->channel);

	if (width == 0 || height == 0 || channel == 0)
	{
		throw std::runtime_error("Bad texture file");
	}

	this->textureTarget = textureTarget;

	if (data)
	{
		generate2DUISpriteSheetTexture(width, height, channel, data);

		stbi_image_free(data);

		return true;
	}
	else
	{
		return false;
	}
}

bool Voxel::Texture2D::initFontTexture(const int width, const int height, GLenum textureTarget)
{
	//allocate blank texture.
	glGenTextures(1, &this->textureObject);
	glBindTexture(GL_TEXTURE_2D, this->textureObject);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//Generate empty texture.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

	this->textureTarget = textureTarget;
	this->width = width;
	this->height = height;
	this->channel = 3;

	return true;
}

unsigned char * Voxel::Texture2D::loadImage(const std::string& textureFilePath, int & width, int & height, int & channel)
{
	auto filePath = FileSystem::getInstance().getWorkingDirectory() + "/" + Texture2D::DEFAULT_TEXTURE_PATH + textureFilePath;
	
	FILE* file = nullptr;
	auto err = fopen_s(&file, filePath.c_str(), "rb");
	if (err != 0)
	{
		throw std::runtime_error("Bad texture filepath");
	}

	/*
		stbi_load_from_file
		stb_image reads from top-left most in the image.
		pixel data has y scanelines of x pixels
		But glTexImage2D reads pixal data from bottom left. See initTexture()
	*/
	unsigned char* data = stbi_load_from_file(file, &width, &height, &channel, 0);
	// So.. we filp image 
	//flipImage(data);

	fclose(file);

	return data;
}

void Voxel::Texture2D::flipImage(unsigned char * data)
{
	unsigned long rowSize = channel * width;
	unsigned char* rowBuffer = new unsigned char[rowSize];
	unsigned halfRows = height / 2;

	for (unsigned rowIdx = 0; rowIdx < halfRows; ++rowIdx) {
		unsigned char* row = data + ((rowIdx * width + 0) * channel);
		unsigned char* oppositeRow = data + (((height - rowIdx - 1) * width + 0) * channel);

		std::memcpy(rowBuffer, row, rowSize);
		std::memcpy(row, oppositeRow, rowSize);
		std::memcpy(oppositeRow, rowBuffer, rowSize);
	}

	delete[] rowBuffer;
}

void Voxel::Texture2D::generate2DTexture(const int width, const int height, const int channel, unsigned char * data)
{
	glGenTextures(1, &textureObject);
	glBindTexture(textureTarget, textureObject);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	switch (static_cast<Channel>(channel))
	{
	case Channel::GRAYSCALE:
		glTexImage2D(textureTarget, 0, GL_RGBA8, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
		break;
	case Channel::GRAYSCALE_ALPHA:
		glTexImage2D(textureTarget, 0, GL_RGBA8, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data);
		break;
	case Channel::RGB:
		// JPEG
		glTexImage2D(textureTarget, 0, GL_RGBA8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		break;
	case Channel::RGBA:
		// PNG
		glTexImage2D(textureTarget, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		break;
	case Channel::NONE:
	default:
		break;
	}

	glBindTexture(textureTarget, 0);
}

void Voxel::Texture2D::generate2DUISpriteSheetTexture(const int width, const int height, const int channel, unsigned char * data)
{
	glGenTextures(1, &textureObject);
	glBindTexture(textureTarget, textureObject);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	switch (static_cast<Channel>(channel))
	{
	case Channel::GRAYSCALE:
		glTexImage2D(textureTarget, 0, GL_RGBA8, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
		break;
	case Channel::GRAYSCALE_ALPHA:
		glTexImage2D(textureTarget, 0, GL_RGBA8, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data);
		break;
	case Channel::RGB:
		// JPEG
		glTexImage2D(textureTarget, 0, GL_RGBA8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		break;
	case Channel::RGBA:
		// PNG
		glTexImage2D(textureTarget, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		break;
	case Channel::NONE:
	default:
		break;
	}

	glBindTexture(textureTarget, 0);
}




Voxel::TextureManager::~TextureManager()
{
	releaseAll();
}

bool Voxel::TextureManager::hasTexture(const std::string & textureName)
{
	return texturesMap.find(Utility::String::removeFileExtFromFileName(textureName)) != texturesMap.end();
}

bool Voxel::TextureManager::addTexture(const std::string & textureName, Texture2D * texture)
{
	std::string rawName = Utility::String::removeFileExtFromFileName(textureName);

	if (hasTexture(rawName))
	{
		return false;
	}
	else
	{
		texturesMap.emplace(rawName, std::shared_ptr<Texture2D>(texture));
		return true;
	}
}

bool Voxel::TextureManager::removeTexture(const std::string & textureName)
{
	std::string rawName = Utility::String::removeFileExtFromFileName(textureName);

	if (hasTexture(rawName))
	{
		texturesMap.erase(rawName);
		return true;
	}
	else
	{
		return false;
	}
}

std::shared_ptr<Texture2D> Voxel::TextureManager::getTexture(const std::string & textureName)
{
	std::string rawName = Utility::String::removeFileExtFromFileName(textureName);

	if (hasTexture(rawName))
	{
		return texturesMap[rawName];
	}
	else
	{
		return nullptr;
	}
}

void Voxel::TextureManager::print()
{
#if V_DEBUG && V_DEBUG_LOG_CONSOLE
	auto logger = &Voxel::Logger::getInstance();

	logger->consoleInfo("[TextureManager] All texture informations");
	logger->consoleInfo("[TextureManager] Total textures: " + std::to_string(texturesMap.size()));

	for (auto& e : texturesMap)
	{
		if (e.second)
		{
			logger->consoleInfo("[TextureManager] Key: " + e.first + ", RefCount: " + std::to_string(e.second.use_count()));
			e.second->print();
		}
	}
#endif
}

void Voxel::TextureManager::releaseAll()
{
	texturesMap.clear();
}
