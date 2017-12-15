#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H

// cpp
#include <string>
#include <unordered_map>

namespace Voxel
{
	class Font;
	/**
	*	@class FontManager
	*	@brief Manages Font instances. Has Ariel.ttf (size 50) as default font.
	*/
	class FontManager
	{
	private:
		// Constructor
		FontManager() = default;

		// Font id counter
		static int idCounter;

		// Fonts
		std::unordered_map<int, Font*> fonts;

	public:
		// Constructor
		~FontManager();

		/**
		*  FontManager instance getter
		*/
		static FontManager& getInstance() {
			static FontManager instance;
			return instance;
		}

		//singleton
		FontManager(FontManager const&) = delete;             // Copy construct
		FontManager(FontManager&&) = delete;                  // Move construct
		FontManager& operator=(FontManager const&) = delete;  // Copy assign
		FontManager& operator=(FontManager &&) = delete;      // Move assign

		// Add font. Returns integer font ID or -1 if fails to load font.
		// Id starts from 1. 0 is used by default font
		int addFont(const std::string& fontName, const int fontSize, int outlineSize = 0);

		// Get font by id
		Font* getFont(const int id);

		// Clear all fonts
		void clear();
	};
}

#endif