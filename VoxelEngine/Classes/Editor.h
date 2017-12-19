#ifndef EDITOR_H
#define EDITOR_H

// voxel
#include "Scene.h"
#include "UI.h"
#include "Cube.h"

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
			CLICKED_RIGHT_BUTTON,		// Rotate model
			CLICKED_MIDDLE_BUTTON,		// pan model in x, z axis
		};

		enum class EditState
		{
			NONE = 0,
			ADD_BLOCK,
			REMOVE_BLOCK,
		};

		enum class SchematicState
		{
			NONE = 0,
			NEW,
			OPENED,
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
		Voxel::UI::Slider* dimensionXSlider;
		Voxel::UI::Slider* dimensionYSlider;
		Voxel::UI::Slider* dimensionZSlider;
		Voxel::UI::Text* dimensionLabel;
		Voxel::UI::Text* dimensionXLabel;
		Voxel::UI::Text* dimensionYLabel;
		Voxel::UI::Text* dimensionZLabel;

		// overwrite window
		Voxel::UI::NinePatchImage* overwriteWindow;
		Voxel::UI::Text* overwritePrompt;

		// inspector
		Voxel::UI::Image* inspectorWindow;
		Voxel::UI::Text* inspectorDimensionLablel;
		Voxel::UI::Text* inspectorCursorBlockPos;

		bool menuBarDropDowned;

		// inputhandler
		InputHandler* input;

		// cursor
		Cursor* cursor;

		// mouse state
		MouseState mouseState;

		// edit state
		EditState editState;

		std::string newFileName;

		// Schematic
		glm::ivec3 dimension;
		Schematic* schematic;
		SchematicState schematicState;
		bool schematicModified;

		// FPS label
		Voxel::UI::Text* fpsLabel;

		// floor
		GLuint floorVao;
		glm::vec3 floorPosition;
		float floorAngleX;
		float floorAngleY;
		glm::vec4 floorColor;
		glm::mat4 floorModelMat;
		glm::vec2 floorSize;
		
		// block face indicator
		GLuint faceIndicatorVao;
		Cube::Face faceIndicatorCubeFace;
		glm::ivec3 curFaceIndicatorPos;
		bool faceIndicatorVisibility;
		glm::mat4 faceIndicatorModelMat;
		glm::vec4 faceIndicatorColor;

		// origin line
		GLuint axisLineVao;

		// cam pos. For smooth zoom
		glm::vec3 camPosTarget;
		glm::vec3 defaultCamPos;
		
		// init
		void initEditor();
		void initFloor();
		void initFaceIndicator();
		void initAxisGuide();
		void initUI();
		void initMenuBar();
		void initFileDropDownMenu();
		void initNewCreateWindow();
		void initOverwriteWindow();
		void initInspector();

		// release/clean
		void releaseFloor();
		void releaseAxisGuide();

		// update key
		void updateKey();
		// update mouse
		bool updateMouseMove(const float delta);
		bool updateMousePress();
		bool updateMouseRelease();
		void updateMouseScroll();

		// zoom
		int zoomLevel;
		void zoomIn();
		void zoomOut();

		// ray
		// Raycast floor and get intersecting point
		bool raycastFloor(glm::vec3& intersectingPoint);

		void updateFloorModelMat();
		void updateFaceIndicatorModelMat();
		glm::ivec3 intersectingFloorPointToCoordinate(const glm::vec3& intersectingPoint);

		// file
		bool attempToCreateFile();
		void askOverwrite();

		// callback
		void onFPSCount(int fps);
		void onFileButtonClicked();
		void onEditButtonClicked();
		void onReturnToMainMenuButtonClicked();
		void onExitButtonClicked();
		void onNewButtonClicked();
		void onNewCreateButtonClicked();
		void onNewCancelButtonClicked();
		void onNewFileNameEdit(const std::string text);
		void onNewFileNameEditSubmitted(const std::string text);
		void onDimensionXSliderMove(float value);
		void onDimensionYSliderMove(float value);
		void onDimensionZSliderMove(float value);
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