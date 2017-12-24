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
	class Cursor;
	class InputHandler;

	class MenuScene : public Scene
	{
	private:
		enum ButtonIndex : unsigned int
		{
			PLAY = 0,
#if V_DEBUG
#if V_DEBUG_EDITOR
			EDITOR,
#endif
#if V_DEBUG_PARTICLE_SYSTEM_EDITOR
			PARTICLE_SYSTEM_EDITOR,
#endif
#if V_DEBUG_UI_TEST
			UI_TEST,
#endif
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
		Cursor* cursor;

		// input handler instance
		InputHandler* input;
		
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
		void onPlayClicked(Voxel::UI::Button* sender);
		void onExitGameClicked(Voxel::UI::Button* sender);
#if V_DEBUG
#if V_DEBUG_EDITOR
		void onEditorClicked(Voxel::UI::Button* sender);
#endif
#if V_DEBUG_PARTICLE_SYSTEM_EDITOR
		void onParticleSystemEditorClicked(Voxel::UI::Button* sender);
#endif
#if V_DEBUG_UI_TEST
		void onUITestClicked(Voxel::UI::Button* sender);
#endif
#endif
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
		void onExitFinished() override;
		void update(const float delta) override;
		void render() override;
		void release() override;
	};
}

#endif