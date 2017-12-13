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

		// debug print
		void print();
	};


	/**
	*	@class SpriteSheetManager
	*	@brief Manages all spritesheets
	*/
	class SpriteSheetManager
	{
	private:
		// default constructor
		SpriteSheetManager() = default;

		// destructor
		~SpriteSheetManager();

		// Delete copy, move, assign operators
		SpriteSheetManager(SpriteSheetManager const&) = delete;             // Copy construct
		SpriteSheetManager(SpriteSheetManager&&) = delete;                  // Move construct
		SpriteSheetManager& operator=(SpriteSheetManager const&) = delete;  // Copy assign
		SpriteSheetManager& operator=(SpriteSheetManager &&) = delete;      // Move assign

		// map of sprite sheets with name by key. Since we don't query sprite sheet a lot, string key is fine.
		std::unordered_map<std::string, SpriteSheet*> spriteSheetMap;

	public:
		// Get singleton instance
		static SpriteSheetManager& getInstance()
		{
			static SpriteSheetManager instance;
			return instance;
		}

		/**
		*	Add sprite sheet. Spritesheets are stored in map with key of string equal to file name without extention.
		*	For example, ss1.json file will be stored with string key of 'ss1'.
		*	@param jsonFileName Json file name for sprite sheet.
		*	@return true if successfully adds sprite sheet. Else, false.
		*/
		bool addSpriteSheet(const std::string& jsonFileName);
		
		/**
		*	Remove sprite sheet by key. Key equals to sprite sheet name without .json extention.
		*	@param key String key for spritesheet.
		*	@return true if successfully removes sprite shset. Else, false.
		*/
		bool removeSpriteSheetByKey(const std::string& key);

		/**
		*	Remove sprite sheet by file name.
		*	@param jsonFileName File name of sprite sheet to remove.
		*	@return true if successfully removes sprite shset. Else, false.
		*/
		bool removeSpriteSheetByName(const std::string& jsonFileName);

		/**
		*	Get sprite sheet by key
		*	@param key String key for spritesheet.
		*	@return SpriteSheet instance if sprite sheet exists. Else, nullptr.
		*/
		SpriteSheet* getSpriteSheetByKey(const std::string& key);

		/**
		*	Get sprite sheet by file name
		*	@param key File name of spritesheet.
		*	@return SpriteSheet instance if sprite sheet exists. Else, nullptr.
		*/
		SpriteSheet* getSpriteSheetByName(const std::string& jsonFileName);
		
		// Release all spritesheet with texture
		void releaseAll();

		// debug print
		void print(const bool detail);
	};
}

#endif