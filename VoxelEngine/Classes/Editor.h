#ifndef EDITOR_H
#define EDITOR_H

// voxel
#include "Scene.h"
#include "UI.h"

// cpp
#include <array>

//gl
#include <GL\glew.h>

namespace Voxel
{
	// foward delcaration
	class Cursor;
	class InputHandler;
	class Schematic;

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

		enum class MouseState
		{
			IDLE = 0,					// doing nothing
			CLICKED_RIGHT_BUTTON,
			
		};
	private:
		// canvas
		Voxel::UI::Canvas* canvas;
		// File button
		Voxel::UI::Button* fileBtn;
		Voxel::UI::Button* editBtn;
		Voxel::UI::Button* returnToMainMenuBtn;
		Voxel::UI::Button* exitGameBtn;
		// Files drop down
		Voxel::UI::NinePatchImage* fileDropDownBg;
		// New create window
		Voxel::UI::NinePatchImage* newCreateWindow;
		Voxel::UI::Button* createBtn;
		Voxel::UI::InputField* newFileNameInputField;

		bool menuBarDropDowned;

		// inputhandler
		InputHandler* input;

		// cursor
		Cursor* cursor;

		// mouse state
		MouseState mouseState;

		std::string newFileName;

		// Schematic
		Schematic* schematic;

		// floor
		GLuint floorVao;
		float floorAngle;
		glm::vec4 floorColor;
		glm::mat4 floorModelMat;

		// origin line
		GLuint originLineVao;

		// block face indicator
		GLuint faceIndicatorVao;

		// init
		void initEditor();
		void initUI();

		// update mouse
		bool updateMouseMove(const float delta);
		bool updateMousePress();
		bool updateMouseRelease();
		void updateMouseScroll();

		// callback
		void onFileButtonClicked();
		void onEditButtonClicked();
		void onReturnToMainMenuButtonClicked();
		void onExitButtonClicked();
		void onNewButtonClicked();
		void onNewCreateButtonClicked();
		void onNewCancelButtonClicked();
		void onNewFileNameEdit(const std::string text);
		void onNewFileNameEditSubmitted(const std::string text);
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