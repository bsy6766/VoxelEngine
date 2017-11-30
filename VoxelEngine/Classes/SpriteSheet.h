#ifndef SPRITE_SHEET_H
#define SPRITE_SHEET_H

// cpp
#include <string>
#include <unordered_map>

// json
#include <json.hpp>

// voxel
#include "Texture2D.h"

// glm
#include <glm\glm.hpp>

namespace Voxel
{
	/**
	*	@struct ImageEntry
	*	@breif Contains frame data for image in sprite sheet
	*/
	struct ImageEntry
	{
	public:
		glm::vec2 position;
		float width;
		float height;
		glm::vec2 uvOrigin;
		glm::vec2 uvEnd;

		ImageEntry() : position(0), width(0), height(0), uvOrigin(0), uvEnd(0) {}
	};

	/**
	*	@class SpriteSheet
	*	@brief Reads texture packer's json data file and creates 
	*/
	class SpriteSheet
	{
	private:
		// Constructor
		SpriteSheet();

		// sprite sheet texture
		Texture2D* texture;

		// image entries. Contains each sprite's data
		std::unordered_map<std::string/*image file name*/, ImageEntry> imageEntryMap;

		// initailize 
		bool init(const std::string& dataFileName);
	public:
		// destructor
		~SpriteSheet();

		// create sprite sheet
		static SpriteSheet* create(const std::string& dataFileName);

		// check if sprite sheet has specific image(sprite)
		bool hasImage(const std::string& imageName);
		
		// get specific image entry
		const ImageEntry* getImageEntry(const std::string& imageName);

		// get texture
		Texture2D* getTexture();
	};


	/**
	*	@class SpriteSheetManager
	*	@brief Manages all spritesheets
	*/
	class SpriteSheetManager
	{
	private:
		SpriteSheetManager() = default;
		~SpriteSheetManager();

		// Delete copy, move, assign operators
		SpriteSheetManager(SpriteSheetManager const&) = delete;             // Copy construct
		SpriteSheetManager(SpriteSheetManager&&) = delete;                  // Move construct
		SpriteSheetManager& operator=(SpriteSheetManager const&) = delete;  // Copy assign
		SpriteSheetManager& operator=(SpriteSheetManager &&) = delete;      // Move assign

		std::unordered_map<std::string, SpriteSheet*> spriteSheetMap;

		std::string trimFileExtention(const std::string& fileName);
	public:
		static SpriteSheetManager& getInstance()
		{
			static SpriteSheetManager instance;
			return instance;
		}

		bool addSpriteSheet(const std::string& jsonFileName);
		SpriteSheet* getSpriteSheet(const std::string& jsonFileName);
		
		void releaseAll();
	};
}

#endif