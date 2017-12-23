#ifndef UI_TEST_SCENE_H
#define UI_TEST_SCENE_H

#include <iostream>

// voxel
#include "Scene.h"
#include "UI.h"

namespace Voxel
{
	class Cursor;
	class InputHandler;

	class UITestScene : public Scene
	{
	private:
		void initButtons(const float labelsY);
		void initCheckBoxes(const float labelsY);
		void initProgressTimers(const float labelsY);
		void initSliders();
		void initAnimatedImage();
		void initImages();
		void initTexts();
		void initActions();
		void initNinePatchImages();
		void initInputField();
		void initUIHierarchy();

		Voxel::UI::Button* addButton(const std::string& name, const glm::vec2& pos, const bool disable, Voxel::UI::TransformNode* parent);
		Voxel::UI::CheckBox* addCheckbox(const std::string& name, const glm::vec2& pos, const bool disable, Voxel::UI::TransformNode* parent);
		Voxel::UI::ProgressTimer* addHorProgBar(const std::string& name, const glm::vec2& pos, const float percentage, const bool ccw, Voxel::UI::TransformNode* parent);
		Voxel::UI::ProgressTimer* addVerProgBar(const std::string& name, const glm::vec2& pos, const float percentage, const bool ccw, Voxel::UI::TransformNode* parent);
		Voxel::UI::ProgressTimer* addProgRadial(const std::string& name, const glm::vec2& pos, const float percentage, const bool ccw, Voxel::UI::TransformNode* parent);
		Voxel::UI::Slider* addHorSlider(const std::string& name, const glm::vec2& pos, const float minValue, const float maxValue, const float value, const bool disable, Voxel::UI::TransformNode* parent);
		Voxel::UI::Slider* addVerSlider(const std::string& name, const glm::vec2& pos, const float minValue, const float maxValue, const float value, const bool disable, Voxel::UI::TransformNode* parent);
		Voxel::UI::NinePatchImage* addNinePatchImage(const std::string& name, const glm::vec2& pos, const glm::vec4& paddings, const glm::vec2& bodySize, Voxel::UI::TransformNode* parent);
		Voxel::UI::Image* addUIHiers(const std::string& name, const glm::vec2& pos, const int number, const int z, Voxel::UI::TransformNode* parent);
	public:
		UITestScene();
		~UITestScene();
		// canvs
		Voxel::UI::Canvas* canvas;
		
		// ui info
		Voxel::UI::Text* uiInfo;
		
		// exit & return to menu
		Voxel::UI::Button* exitButton;
		Voxel::UI::Button* returnToMainMenuButton;

		// FPS label
		Voxel::UI::Text* fpsLabel;
	
		// cursor
		Cursor* cursor;

		// ih
		InputHandler* input;

		// mouse update
		bool updateMouseMove(const float delta);
		bool updateMousePress();
		bool updateMouseRelease();
		// key update
		void updateKey();

		// overrides
		void init() override;
		void onEnter() override;
		void onEnterFinished() override;
		void onExit() override;
		void onExitFinished() override;
		void update(const float delta) override;
		void render() override;
		void release() override;

		// callbacks
		void onExitButtonClicked(Voxel::UI::Button* sender);
		void onReturnToMainMenuClicked(Voxel::UI::Button* sender);

		// global button callback
		void onButtonTriggered(Voxel::UI::Button* sender);
		void onButtonCancelled(Voxel::UI::Button* sender);
		// global check box callback
		void onCheckBoxSelected(Voxel::UI::CheckBox* sender);
		void onCheckBoxDeselected(Voxel::UI::CheckBox* sender);
		void onCheckBoxCancelled(Voxel::UI::CheckBox* sender);
		// global slider callback
		void onSliderButtonPrssed(Voxel::UI::Slider* sender);
		void onSliderBarPressed(Voxel::UI::Slider* sender);
		void onSliderValueChange(Voxel::UI::Slider* sender);
		void onSliderFinished(Voxel::UI::Slider* sender);
		// global mouse event callbacks
		void onMouseEnter(Voxel::UI::TransformNode* sender);
		void onMouseExit(Voxel::UI::TransformNode* sender);
		void onMouseMove(Voxel::UI::TransformNode* sender);
		void onMousePressed(Voxel::UI::TransformNode* sender, const int button);
		void onMouseReleased(Voxel::UI::TransformNode* sender, const int button);
		// fps callback
		void onFPSUpdate(int fps);
	};
}

#endif