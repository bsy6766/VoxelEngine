#include "Texture2D.h"
#include <stb_image.h>
#include <Application.h>
#include <ProgramManager.h>
#include <Program.h>

using namespace Voxel;

const std::string Texture2D::DEFAULT_TEXTURE_PATH = "textures/";

Texture2D::Texture2D()
	: textureObject(0)
	, textureLocation(0)
	, width(0)
	, height(0)
	, channel(0)
{
}

Texture2D::~Texture2D()
{
	glDeleteTextures(1, &textureObject);
}

Texture2D * Voxel::Texture2D::create(const std::string & textureFilePath, GLenum textureTarget)
{
	auto newTexture = new Texture2D();
	if (newTexture->init(textureFilePath, textureTarget))
	{
		return newTexture;
	}
	else
	{
		delete newTexture;
		return nullptr;
	}
}

Texture2D * Voxel::Texture2D::createFontTexturx(const int width, const int height)
{
	auto newTexture = new Texture2D();
	if (newTexture->initFontTexture(width, height))
	{
		return newTexture;
	}
	else
	{
		delete newTexture;
		return nullptr;
	}
}

glm::ivec2 Voxel::Texture2D::getTextureSize()
{
	return glm::ivec2(width, height);
}

void Voxel::Texture2D::activate(GLenum textureUnit)
{
	glActiveTexture(textureUnit);
}

void Voxel::Texture2D::bind()
{
	glBindTexture(textureTarget, textureObject);
	glUniform1i(textureLocation, 0);
}

bool Voxel::Texture2D::init(const std::string & textureFilePath, GLenum textureTarget)
{
	unsigned char* data = loadImage(textureFilePath, this->width, this->height, this->channel);

	if (width == 0 || height == 0 || channel == 0)
	{
		throw std::runtime_error("Bad texture file");
	}

	this->textureTarget = textureTarget;
	this->textureLocation = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM::SHADER_TEXTURE_COLOR)->getUniformLocation("tex");

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

bool Voxel::Texture2D::initFontTexture(const int width, const int height)
{
	//allocate blank texture.
	glGenTextures(1, &this->textureObject);
	glBindTexture(GL_TEXTURE_2D, this->textureObject);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//Generate empty texture.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

	return true;
}

unsigned char * Voxel::Texture2D::loadImage(const std::string& textureFilePath, int & width, int & height, int & channel)
{
	auto filePath = Application::getInstance().getWorkingDirectory() + "/" + Texture2D::DEFAULT_TEXTURE_PATH + textureFilePath;
	
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
	flipImage(data);

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
		std::cout << "[Texture2D] Created texture size of (" << width << ", " << height << ") with RGBA channel" << std::endl;
		break;
	case Channel::NONE:
	default:
		break;
	}

	glBindTexture(textureTarget, 0);
}