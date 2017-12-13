#ifndef EDITOR_H
#define EDITOR_H

// voxel
#include "Scene.h"
#include "UI.h"

// cpp
#include <array>

namespace Voxel
{
	// foward delcaration
	class Cursor;
	class InputHandler;

	/**
	*	@class Editor
	*	@brief A tool to build objects. Objects can be saved and loaded.
	*
	*	Editor can open, edit and save voxel object.
	*	Voxel objects are saved in schematic format. 
	*	@see Schematic class for detail file format.
	*
	*	Editor is will be only available for debug purpose. Won't be incldued in final release.
	*	However, schematic will be used.
	*/
	class Editor : public Scene
	{
	private:
		enum MenuBarIndex : unsigned int
		{
			FILE,
			EDIT,
			MAX_MENU_BAR_BUTTON,
		};

		enum FileDropDownIndex : unsigned int
		{
			NEW,
			OPEN,
			SAVE,
			SAVE_AS,
			RETURN_TO_MAIN_MENU,
			MAX_FILE_DROP_DOWN_BUTTON,
		};
	private:
		// canvas
		Voxel::UI::Canvas* canvas;
		// Menu bar buttons
		std::array<Voxel::UI::Button*, MAX_MENU_BAR_BUTTON> menuBarButtons;
		// Files drop down
		Voxel::UI::Image* fileDropDownBg;
		// Files drop down buttons
		std::array<Voxel::UI::Button*, MAX_FILE_DROP_DOWN_BUTTON> fileDropDownButtons;

		// inputhandler
		InputHandler* input;

		void updateMouseMove();
		void updateMouseClick();
		void updateMouseScroll();
	public:
		Editor();
		~Editor();

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