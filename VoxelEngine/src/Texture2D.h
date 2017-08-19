#ifndef TEXTURE_2D_H
#define TEXTURE_2D_H

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <string>

namespace Voxel
{
	/**
	*	@class Texture2D
	*	@brief Contains OpenGL Texture data. Only supports PNG
	*/
	class Texture2D
	{
	public:
		enum class Channel : int
		{
			NONE = 0,
			GRAYSCALE = 1,
			GRAYSCALE_ALPHA = 2,
			RGB = 3,
			RGBA = 4
		};

		const static std::string DEFAULT_TEXTURE_PATH;
	private:
		Texture2D();

		GLuint textureObject;
		GLenum textureTarget;
		GLint textureLocation;

		int width;
		int height;

		int channel;

		unsigned char* loadImage(const std::string& textureFilePath, int& width, int& height, int& channel);
		void flipImage(unsigned char* data);

		void generate2DTexture(const int width, const int height, const int channel, unsigned char* data);

		bool init(const std::string& textureFilePath, GLenum textureTarget);
	public:
		~Texture2D();

		static Texture2D* create(const std::string& textureFilePath, GLenum textureTarget);

		glm::ivec2 getTextureSize();

		void bind(GLenum textureUnit);
	};
}

#endif