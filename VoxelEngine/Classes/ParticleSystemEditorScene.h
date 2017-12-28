#ifndef PARTICLE_SYSTEM_EDITOR_SCENE_H
#define PARTICLE_SYSTEM_EDITOR_SCENE_H

// voxel
#include "Scene.h"
#include "UI.h"

// cpp
#include <string>
#include <vector>

namespace Voxel
{
	// foward delcaration
	class Cursor;
	class InputHandler;
	class DataTree;

	class ParticleSystemEditorScene : public Scene
	{
	public:
		enum class State
		{
			IDLE = 0,
			FILE_MENU_DROP_DOWN,
			CREATE_NEW_FILE,
			ASK_SAVE_MODIFIED_FILE,
			ASK_FILE_NAME_FOR_MODIFIED_FILE,
			ASK_OVERWRITE,
		};

		enum class MouseState
		{
			IDLE = 0,
			SETTING_EMIT_POS,
		};

		enum class ModifierLabel
		{
			DURATION = 0,
			LIVING_PARTICLES,
			TOTAL_PARTICLES,
			EMISSION_RATE,
			PARTICLE_LIFE_SPAN,
			PARTICLE_LIFE_SPAN_VAR,
			SPEED,
			SPEED_VAR,
			EMIT_POS,
			EMIT_POS_X_VAR,
			EMIT_POS_Y_VAR,
			GRAVITY_X,
			GRAVITY_Y,
			EMIT_ANGLE,
			EMIT_ANGLE_VAR,
			ACCEL_RAD,
			ACCEL_RAD_VAR,
			ACCEL_TAN,
			ACCEL_TAN_VAR,
			START_SIZE,
			START_SIZE_VAR,
			END_SIZE,
			END_SIZE_VAR,
			START_ANGLE,
			START_ANGLE_VAR,
			END_ANGLE,
			END_ANGLE_VAR,
			COLOR_PICKER,
			START_COLOR,
			START_COLOR_VAR,
			END_COLOR,
			END_COLOR_VAR
		};
	private:
		// canvas
		Voxel::UI::Canvas* canvas;

		// Menubar button
		Voxel::UI::Button* fileBtn;
		Voxel::UI::Button* returnToMainMenuBtn;
		Voxel::UI::Button* exitGameBtn;

		// Files drop down
		Voxel::UI::NinePatchImage* fileDropDownBg;
		Voxel::UI::Button* saveBtn;
		Voxel::UI::Button* saveAsBtn;

		// New create window
		Voxel::UI::NinePatchImage* newCreateWindow;
		Voxel::UI::Button* createBtn;
		Voxel::UI::InputField* newFileNameInputField;

		// overwrite window
		Voxel::UI::NinePatchImage* overwriteWindow;
		Voxel::UI::Text* overwritePrompt;

		// fpslabel
		Voxel::UI::Text* fpsLabel;

		// working particle system
		Voxel::UI::ParticleSystem* workingParticleSystem;
		Voxel::DataTree* workingParticleSystemDataTree;
		bool particleSystemModified;

		// particle system modifiers
		Voxel::UI::Node* modifierNode;
		std::vector<Voxel::UI::Text*> modifierLabels;
		std::vector<Voxel::UI::Slider*> modifierSliders;
		Voxel::UI::Node* colorPickerNode;
		Voxel::UI::ColorPicker* colorPicker;
		Voxel::UI::Slider* colorSlider;
		Voxel::UI::Slider* alphaSlider;
		Voxel::UI::Image* startColorPreview;
		Voxel::UI::Image* startColorVarPreview;
		Voxel::UI::Image* endColorPreview;
		Voxel::UI::Image* endColorVarPreview;

		// position rect (Visualize particle spawn area)
		Voxel::UI::Image* emissionAreaImage;
		Voxel::UI::CheckBox* emissionAreaCB;

		// emit pos guid line
		Voxel::UI::Line* emitPosXLine;
		Voxel::UI::Line* emitPosYLine;

		// new file name
		std::string newFileName;

		// state
		State state;
		MouseState mouseState;

		// inputhandler
		InputHandler * input;

		// cursor
		Cursor* cursor;

		// init
		void initUI();
		void initMenuBar();
		void initFileDropDownMenu();
		void initNewCreateWindow();
		void initOverwriteWindow();
		void initModifiers();
		Voxel::UI::Slider* initModifierSlider(const std::string& name, const std::string& initLabel, const std::string& label,  const float min, const float max, const float value, float& y);
		void initSpawnBoundaryImage();
		void initEmitPosLine();

		// update key
		void updateKey();

		// update mouse
		bool updateMouseMove(const float delta);
		bool updateMousePress();
		bool updateMouseRelease();

		// update emission pos
		void updateEmissionPos();

		void createEmptyParticleSystem();
		bool attempToCreateFile();
		void askOverwrite();

		// callback
		void onFPSCount(int fps);
		void onFileButtonClicked(Voxel::UI::Button* sender);
		void onReturnToMainMenuButtonClicked(Voxel::UI::Button* sender);
		void onExitButtonClicked(Voxel::UI::Button* sender);
		void onNewButtonClicked(Voxel::UI::Button* sender);
		void onNewCancelButtonClicked(Voxel::UI::Button* sender);
		void onNewFileNameEdit(Voxel::UI::InputField* sender, const std::string text);
		void onNewCreateButtonClicked(Voxel::UI::Button* sender);
		void onOverwrite(Voxel::UI::Button* sender);
		void onOverwriteCancel(Voxel::UI::Button* sender);
		void onEmissionAreaCheckBoxSelected(Voxel::UI::CheckBox* sender);
		void onEmissionAreaCheckBoxDeselected(Voxel::UI::CheckBox* sender);

		// particle system callback
		void onDurationChange(Voxel::UI::Slider* sender);
		void onTotalParticlesChange(Voxel::UI::Slider* sender);
		void onParticleLifeSpanChange(Voxel::UI::Slider* sender);
		void onParticleLifeSpanVarChange(Voxel::UI::Slider* sender);
		void onSpeedChange(Voxel::UI::Slider* sender);
		void onSpeedVarChange(Voxel::UI::Slider* sender);
		void onPosXVarChange(Voxel::UI::Slider* sender);
		void onPosYVarChange(Voxel::UI::Slider* sender);
		void onGravityXChange(Voxel::UI::Slider* sender);
		void onGravityYChange(Voxel::UI::Slider* sender);
		void onEmitAngleChange(Voxel::UI::Slider* sender);
		void onEmitAngleVarChange(Voxel::UI::Slider* sender);
		void onAccelRadChange(Voxel::UI::Slider* sender);
		void onAccelRadVarChange(Voxel::UI::Slider* sender);
		void onAccelTanChange(Voxel::UI::Slider* sender);
		void onAccelTanVarChange(Voxel::UI::Slider* sender);
		void onStartSizeChange(Voxel::UI::Slider* sender);
		void onStartSizeVarChange(Voxel::UI::Slider* sender);
		void onEndSizeChange(Voxel::UI::Slider* sender);
		void onEndSizeVarChange(Voxel::UI::Slider* sender);
		void onStartAngleChange(Voxel::UI::Slider* sender);
		void onStartAngleVarChange(Voxel::UI::Slider* sender);
		void onEndAngleChange(Voxel::UI::Slider* sender);
		void onEndAngleVarChange(Voxel::UI::Slider* sender);
		void onColorPickerValueChange(Voxel::UI::ColorPicker* sender);
		void onColorSliderValueChange(Voxel::UI::Slider* sender);
		void onAlphaSliderValueChange(Voxel::UI::Slider* sender);
		void onStartColorButtonTriggered(Voxel::UI::Button* sender);
		void onStartColorVarButtonTriggered(Voxel::UI::Button* sender);
		void onEndColorButtonTriggered(Voxel::UI::Button* sender);
		void onEndColorVarButtonTriggered(Voxel::UI::Button* sender);
	public:
		ParticleSystemEditorScene();
		~ParticleSystemEditorScene();

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