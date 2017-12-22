#ifndef GAME_MENU_H
#define GAME_MENU_H

// voxel
#include "UI.h"

namespace Voxel
{
	// foward declaration
	class GameScene;

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
			OPTIONS_MENU_BG,
			OPTIONS_MENU_LABEL,
			OPTIONS_MENU_BUTTON,
			VIDEOS_MENU_BG,
			VIDEOS_MENU_LABEL,
			VIDEOS_MENU_BUTTON,
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

		// reset game menu. Return to front page. Called when game menu gets closed
		void reset();

		// callbacks
		void onOptionClicked(Voxel::UI::Button* sender);
		void onControlsClicked(Voxel::UI::Button* sender);
		void onExitGameClicked(Voxel::UI::Button* sender);
		void onRetrunToMenuClicked(Voxel::UI::Button* sender);
	public:
		// Constructor
		GameMenu();

		// Destructor
		~GameMenu();

		// initialize all uis
		void init(GameScene* gameScenePtr);
		void initGameMenu(GameScene* gameScenePtr);
		void initOptionMenu(GameScene * gameScenePtr);

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
		void updateMousePress(const glm::vec2& mousePosition, const int button);

		// update mouse release on game menu
		void updateMouseReleased(const glm::vec2& mousePosition, const int button);

		void render();
	};
}

#endif