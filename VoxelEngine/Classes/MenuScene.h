#ifndef MENU_SCENE_H
#define MENU_SCENE_H

// voxel
#include "Scene.h"
#include "UI.h"

namespace Voxel
{
	class MenuScene : public Scene
	{
	private:
		// UI
		Voxel::UI::Canvas* canvas;
		Voxel::UI::Image* buttonBg;
		Voxel::UI::Button* playButton;
		Voxel::UI::Button* editorButton;
		Voxel::UI::Button* optionsButton;
		Voxel::UI::Button* creditsButton;
		Voxel::UI::Button* exitGameButton;

		// temp
		Voxel::UI::Image* bg;
	public:
		// Constructor
		MenuScene();

		// Destructor
		~MenuScene();

		// overrides
		void init() override;
		void onEnter() override;
		void onExit() override;
		void update(const float delta) override;
		void render() override;
	};
}

#endif