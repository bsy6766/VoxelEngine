#ifndef SPRITE_SHEET_H
#define SPRITE_SHEET_H

#include <string>
#include <unordered_map>
#include <json.hpp>
#include <Texture2D.h>
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
		SpriteSheet();

		Texture2D* texture;

		std::unordered_map<std::string/*image file name*/, ImageEntry> imageEntryMap;

		bool init(const std::string& dataFileName);
	public:
		~SpriteSheet();

		static SpriteSheet* create(const std::string& dataFileName);

		bool hasImage(const std::string& imageName);
		
		const ImageEntry* getImageEntry(const std::string& imageName);

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