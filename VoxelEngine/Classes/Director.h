#ifndef DIRECTOR_H
#define DIRECTOR_H

// cpp
#include <list>

// voxel
#include <Config.h>
#include <UI.h>
#include <Scene.h>

// glm
#include <glm\glm.hpp>

// gl
#include <GL\glew.h>

namespace Voxel
{
	/**
	*	@class Director
	*	@brief A director that decides which to update and which to render.
	*
	*	Director manages multiple scenes in the game.
	*	Like real director, it directs update and render to specific scene.
	*	Scene can be title scene, menu scene, etc.
	*	
	*	
	*/
	class Director
	{
	public:
		enum class SceneName
		{
			NONE = 0,
			INIT_SCENE,	// initialize game, show splash screen, etc.
			TITLE_SCENE,	// Show title of the game
			MENU_SCENE,		// Show menu of the game. Manage character, world, saves, option, etc.
			GAME_SCENE,		// Gameplay
#if V_DEBUG 
#if V_DEBUG_EDITOR
			EDITOR_SCENE,	// Debug editor
#endif
#if V_DEBUG_UI_TEST
			UI_TEST_SCENE,
#endif
#endif
		};

		enum class State
		{
			IDLE = 0,			// update and renders current scene if there is current scene
			TRANSITIONING,		// Transition between current scene and next scene
		};

		enum class FadeState
		{
			READY = 0,
			FADE_IN,
			FADE_IN_FINISHED,
			SWAP_SCENE,
			FADE_OUT,
			FINISHED
		};
	private:
		// scene
		Scene* currentScene;
		Scene* nextScene;
		
		// State
		State state;

		// Fade state
		FadeState fadeState;

		// Transition attribs
		float duration;
		float elapsedTime;

		// fade ui
		Voxel::UI::Canvas* canvas;
		Voxel::UI::Image* fadeImage;

		// create scene
		Scene* createScene(const SceneName sceneName);

		// init fade quad
		void initFadeImage();
	public:
		// Constructor
		Director();

		// Destructor
		~Director();

		// get scene
		template<class T>
		T* getCurrentScene()
		{
			if (currentScene)
			{
				if (T* d = dynamic_cast<T*>(currentScene))
				{
					return d;
				}
				else
				{
					return nullptr;
				}
			}
			else
			{
				return nullptr;
			}
		}

		// run scene. This switches current scene to new scene without transition or waiting.
		void runScene(const SceneName sceneName);

		// Replace scene with transition. If scene is transitioing, rejects.
		void replaceScene(const SceneName sceneName, const float duration, const glm::vec3& fadeColor = glm::vec3(0.0f));

		// pop current scene
		void popScene();
		
		// Update current scene
		void update(const float delta);

		// Render current scene
		void render();
		void renderFade();
	};

}
#endif