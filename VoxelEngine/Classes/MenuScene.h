#ifndef MENU_SCENE_H
#define MENU_SCENE_H

// voxel
#include "Scene.h"
#include "UI.h"

// cpp
#include <array>

namespace Voxel
{
	// foward declaration
	namespace UI
	{
		class Cursor;
	}
	class InputHandler;

	class MenuScene : public Scene
	{
	private:
		enum ButtonIndex : unsigned int
		{
			PLAY = 0,
#if V_DEBUG && V_DEBUG_EDITOR
			EDITOR,
#endif
			OPTIONS,
			CREDITS,
			EXIT_GAME,
			MAX_BUTTON_COUNT
		};
	private:
		// UI
		Voxel::UI::Canvas* canvas;
		Voxel::UI::Image* buttonBg;
		// buttons
		std::array<Voxel::UI::Button*, MAX_BUTTON_COUNT> buttons;

		// Hover
		int curHoveringButtonIndex;

		// cursor
		UI::Cursor* cursor;
		glm::vec2 prevCursorPos;

		// input handler instance
		InputHandler* input;

		// exiting flag. All inputs are ignored if this is true. Becomes true on onExit
		bool exiting;

		// temp
		Voxel::UI::Image* bg;

		/**
		*	Updates keyboard input. May interupt and skip update during debug mode.
		*/
		void updateKeyboardInput();

		/**
		*	Updates mouse movement input. 
		*/
		void updateMouseMoveInput();

		/**
		*	Updates mouse click input
		*/
		void updateMouseClickInput();
		
		/**
		*	Updates mouse scroll input.
		*/
		void updateMouseScrollInput();

		/**
		*	Updates controller input
		*	@param delta Elapsed time on current frame.
		*/
		void updateControllerInput(const float delta);
		
		// callbacks
		void onPlayClicked();
		void onExitGameClicked();
	public:
		// Constructor
		MenuScene();

		// Destructor
		~MenuScene();

		// overrides
		void init() override;
		void onEnter() override;
		void onEnterFinished() override;
		void onExit() override;
		void update(const float delta) override;
		void render() override;
	};
}

#endif