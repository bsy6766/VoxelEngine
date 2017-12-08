#ifndef GAME_MENU_H
#define GAME_MENU_H

// voxel
#include "UI.h"

namespace Voxel
{
	class GameMenu
	{
	private:
		enum class STATE
		{
			CLOSED = 0,			// game menu is closed
			OPENED,				// game menu is opened on front page

		};

		enum Z_ORDER
		{
			NONE = 0,
			GAME_MENU_BG,
			GAME_MENU_LABEL,
			GAME_MENU_BUTTON,
		};
	private:
		// UI canvas
		Voxel::UI::Canvas* canvas;
		
		// Game menu
		Voxel::UI::NinePatchImage* gameMenu;
		Voxel::UI::NinePatchImage* optionsMenu;
		Voxel::UI::NinePatchImage* controlsMenu;

		// state
		STATE menuState;

		// initialize all uis
		void init();

		// reset game menu. Return to front page. Called when game menu gets closed
		void reset();

		// callbacks
		void onOptionClicked();
		void onControlsClicked();
		void onExitGameClicked();
	public:
		// Constructor
		GameMenu();

		// Destructor
		~GameMenu();

		// Opens game menu
		void open();

		// Closes game menu
		void close();

		// Check if game menu is opened
		bool isOpened() const;

		// Check if game menu is closed
		bool isClosed() const;

		// update mouse move on game menu
		void updateMouseMove(const glm::vec2& mousePosition);

		// update mouse click on game menu
		void updateMouseClick(const glm::vec2& mousePosition, const int button);

		// update mouse release on game menu
		void updateMouseReleased(const glm::vec2& mousePosition, const int button);

		void render();
	};
}

#endif