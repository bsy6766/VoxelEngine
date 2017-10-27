#ifndef TEXTURE_2D_H
#define TEXTURE_2D_H

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <string>
#include <ProgramManager.h>
#include <memory>

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
		Texture2D();

		unsigned int id;

		GLuint textureObject;
		GLenum textureTarget;
		GLint textureLocation;
		
		int width;
		int height;

		int channel;

		unsigned char* loadImage(const std::string& textureFilePath, int& width, int& height, int& channel);
		void flipImage(unsigned char* data);

		void generate2DTexture(const int width, const int height, const int channel, unsigned char* data);

		bool init(const std::string& textureName, GLenum textureTarget);
		bool initFontTexture(const int width, const int height, GLenum textureTarget);
	public:
		~Texture2D();

		static Texture2D* create(const std::string& textureName, GLenum textureTarget);
		static Texture2D* createFontTexture(const std::string& textureName, const int width, const int height, GLenum textureTarget);

		glm::ivec2 getTextureSize();

		void setLocationOnProgram(ProgramManager::PROGRAM_NAME programName);

		void activate(GLenum textureUnit);
		void bind();

		unsigned int getID();

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
		TextureManager() = default;
		~TextureManager();

		static unsigned int idCounter;

		// Delete copy, move, assign operators
		TextureManager(TextureManager const&) = delete;             // Copy construct
		TextureManager(TextureManager&&) = delete;                  // Move construct
		TextureManager& operator=(TextureManager const&) = delete;  // Copy assign
		TextureManager& operator=(TextureManager &&) = delete;      // Move assign

		std::unordered_map<std::string, std::shared_ptr<Texture2D>> texturesMap;

		std::string removeFileExtention(std::string fileName);
	public:
		static TextureManager& getInstance()
		{
			static TextureManager instance;
			return instance;
		}

		bool hasTexture(const std::string& textureName);
		
		bool addTexture(const std::string& textureName, Texture2D* texture);

		std::shared_ptr<Texture2D> getTexture(const std::string& textureName);

		void print();
		void releaseAll();
	};
}

#endif