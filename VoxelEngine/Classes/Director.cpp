#include "Director.h"

// voxel
#include "MenuScene.h"
#include "GameScene.h"
#include "Application.h"
#include "SpriteSheet.h"

// temp
#include "InputHandler.h"

Voxel::Director::Director()
	: currentScene(nullptr)
	, nextScene(nullptr)
	, state(State::IDLE)
	, duration(0.0f)
	, elapsedTime(0.0f)
	, canvas(nullptr)
	, fadeImage(nullptr)
	, fadeState(FadeState::READY)
{
	initFadeImage();
}

Voxel::Director::~Director()
{
	if (currentScene)
	{
		currentScene->onExit();
		delete currentScene;
	}

	if (nextScene)
	{
		nextScene->onExit();
		delete nextScene;
	}

	if (canvas)
	{
		delete canvas;
	}
}

Voxel::Scene * Voxel::Director::createScene(const SceneName sceneName)
{
	Scene* newScene = nullptr;

	switch (sceneName)
	{
	case Voxel::Director::SceneName::INIT_SCENE:
		break;
	case Voxel::Director::SceneName::TITLE_SCENE:
		break;
	case Voxel::Director::SceneName::MENU_SCENE:
	{
		newScene = new MenuScene();
		newScene->init();
	}
		break;
	case Voxel::Director::SceneName::GAME_SCENE:
	{
		GameScene* newGameScene = new GameScene();
		newGameScene->init();
		//newGameScene->createNew("New world");
		newScene = newGameScene;
	}
		break;
	case Voxel::Director::SceneName::EDITOR_SCENE:
		break;
	case Voxel::Director::SceneName::NONE:
	default:
		newScene = nullptr;
		break;
	}

	return newScene;
}

void Voxel::Director::initFadeImage()
{
	auto& sm = SpriteSheetManager::getInstance();
	sm.addSpriteSheet("GlobalSpriteSheet.json");
	sm.addSpriteSheet("CursorSpriteSheet.json");

	canvas = new Voxel::UI::Canvas(Application::getInstance().getGLView()->getScreenSize(), glm::vec2(0.0f));

	fadeImage = Voxel::UI::Image::createFromSpriteSheet("fadeImage", "GlobalSpriteSheet", "1x1_white.png");
	fadeImage->setColor(glm::vec3(0.0f));
	fadeImage->setOpacity(0.0f);
	fadeImage->setPosition(0.0f, 0.0f);
	fadeImage->setScale(Application::getInstance().getGLView()->getScreenSize());

	canvas->addChild(fadeImage);

	canvas->setVisibility(false);
}

void Voxel::Director::runScene(const SceneName sceneName)
{
	if (currentScene)
	{
		currentScene->onExit();
		currentScene->onExitFinished();
		delete currentScene;
	}

	currentScene = createScene(sceneName);
	currentScene->onEnter();
	currentScene->onEnterFinished();
}

void Voxel::Director::replaceScene(const SceneName sceneName, const float duration, const glm::vec3 & fadeColor)
{
	if (state == State::TRANSITIONING)
	{
		return;
	}
	else
	{
		auto newScene = createScene(sceneName);

		if (newScene)
		{
			nextScene = newScene;

			state = State::TRANSITIONING;
			fadeState = FadeState::FADE_IN;

			this->duration = duration;
			this->elapsedTime = 0.0f;

			currentScene->onExit();

			canvas->setVisibility(true);

			fadeImage->setColor(fadeColor);
		}
	}
}

void Voxel::Director::popScene()
{
}

void Voxel::Director::update(const float delta)
{
	if (state == State::IDLE)
	{
		if (currentScene)
		{
			currentScene->update(delta);
		}

		if (InputHandler::getInstance().getKeyDown(GLFW_KEY_H, true))
		{
			currentScene->onExit();
			currentScene->onExitFinished();
			delete currentScene;
			currentScene = nullptr;
		}
	}
	else
	{
		// transitioing
		elapsedTime += delta;

		if (fadeState == FadeState::FADE_IN)
		{
			// Fade effect fades in

			if (elapsedTime >= (duration * 0.5f))
			{
				// Fade in finished. Set opacity to 1
				fadeImage->setOpacity(1.0f);
				// update state
				fadeState = FadeState::FADE_IN_FINISHED;
			}
		}
		else if (fadeState == FadeState::FADE_IN_FINISHED)
		{
			// Fade in is finished. Give 0.2 seconds delay.
			if (elapsedTime >= ((duration * 0.5f) + 0.2f))
			{
				// Delay finished. 
				elapsedTime -= 0.2f;
				// Update state
				fadeState = FadeState::SWAP_SCENE;

				// current scene finished exit
				currentScene->onExitFinished();

				// next scene enters
				nextScene->onEnter();
			}
		}
		else if (fadeState == FadeState::SWAP_SCENE)
		{
			// revert
			elapsedTime -= delta;
			
			// release current scene
			delete currentScene;

			// swap scene
			currentScene = nextScene;

			// set back to nullptr
			nextScene = nullptr;

			// update state
			fadeState = FadeState::FADE_OUT;
		}
		else if (fadeState == FadeState::FADE_OUT)
		{
			if (elapsedTime >= duration)
			{
				// finished.
				std::cout << "Finished: " << elapsedTime << "\n";

				// current scene (next scene) finished enter
				currentScene->onEnterFinished();

				// update states
				state = State::IDLE;
				fadeState = FadeState::FINISHED;

				// reset values
				duration = 0.0f;
				elapsedTime = 0.0f;

				// hide canvas
				canvas->setVisibility(false);
			}

			if (currentScene)
			{
				currentScene->update(delta);
			}
		}
	}
}

void Voxel::Director::render()
{
	if (state == State::TRANSITIONING)
	{
		if (elapsedTime <= duration)
		{
			const float halfDuration = duration * 0.5f;

			if (fadeState == FadeState::FADE_IN)
			{
				// on first half, update current scene
				fadeImage->setOpacity(elapsedTime / halfDuration);
			}
			else if (fadeState == FadeState::FADE_OUT)
			{
				// on second half, update next scene
				fadeImage->setOpacity(1.0f - ((elapsedTime - halfDuration) / halfDuration));
			}

			if (currentScene)
			{
				currentScene->render();
			}

			if (canvas)
			{
				// Clear depth buffer and render above current buffer
				glClear(GL_DEPTH_BUFFER_BIT);
				glDepthFunc(GL_ALWAYS);

				canvas->render();
			}
		}
		// else, done.
	}
	else
	{
		if (currentScene)
		{
			currentScene->render();
		}
	}
}
