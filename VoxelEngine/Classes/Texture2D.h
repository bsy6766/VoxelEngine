#ifndef TEXTURE_2D_H
#define TEXTURE_2D_H

// gl
#include <GL\glew.h>

// glm
#include <glm\glm.hpp>

// cpp
#include <string>
#include <memory>

// voxel
#include "ProgramManager.h"

namespace Voxel
{
	/**
	*	@class Texture2D
	*	@brief Contains OpenGL Texture data. Only supports PNG
	*/
	class Texture2D
	{
		friend class TextureManager;
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
		// Constructor
		Texture2D();

		// texture number id
		unsigned int id;

		// opengl
		GLuint textureObject;
		GLenum textureTarget;
		GLint textureLocation;

		// size of texture
		int width;
		int height;

		// channel of texture
		int channel;

		// load image file
		unsigned char* loadImage(const std::string& textureFilePath, int& width, int& height, int& channel);
		// flip the image file vertically
		void flipImage(unsigned char* data);

		// generate opengl texture
		void generate2DTexture(const int width, const int height, const int channel, unsigned char* data);

		// initailize 
		bool init(const std::string& textureName, GLenum textureTarget);

		// initialize font texture
		bool initFontTexture(const int width, const int height, GLenum textureTarget);
	public:
		// destructor
		~Texture2D();

		// create
		static Texture2D* create(const std::string& textureName, GLenum textureTarget);
		// create font texture
		static Texture2D* createFontTexture(const std::string& textureName, const int width, const int height, GLenum textureTarget);

		// get size of texture
		glm::ivec2 getTextureSize();

		// set texture's location on shader
		void setLocationOnProgram(ProgramManager::PROGRAM_NAME programName);
		void setLocationOnProgram(const GLint textureLocation);

		// get texture id
		unsigned int getID();

		// activate texture
		void activate(GLenum textureUnit);
		// bind texture
		void bind();

		// print texture info
		void print();
	};

	/**
	*	@class TextureManager
	*	@brief Manages all texture in the game.
	*
	*	All other classes that has texture instance don't have to release. 
	*/
	class TextureManager
	{
	private:
		// default constructor
		TextureManager() = default;

		// destructor. releases all textures
		~TextureManager();

		// static id counter
		static unsigned int idCounter;

		// Delete copy, move, assign operators
		TextureManager(TextureManager const&) = delete;             // Copy construct
		TextureManager(TextureManager&&) = delete;                  // Move construct
		TextureManager& operator=(TextureManager const&) = delete;  // Copy assign
		TextureManager& operator=(TextureManager &&) = delete;      // Move assign

		// stores all texture
		std::unordered_map<std::string, std::shared_ptr<Texture2D>> texturesMap;

		// Remove file extention from file name
		std::string removeFileExtention(std::string fileName);
	public:
		static TextureManager& getInstance()
		{
			static TextureManager instance;
			return instance;
		}

		// check if it has texture by name
		bool hasTexture(const std::string& textureName);
		
		// add texture
		bool addTexture(const std::string& textureName, Texture2D* texture);

		// get texture
		std::shared_ptr<Texture2D> getTexture(const std::string& textureName);

		// print all texture info
		void print();

		// release all textures
		void releaseAll();
	};
}

#endif