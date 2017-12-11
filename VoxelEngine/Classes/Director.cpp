#include "Director.h"

// voxel
#include "MenuScene.h"
#include "Application.h"
#include "SpriteSheet.h"

Voxel::Director::Director()
	: currentScene(nullptr)
	, nextScene(nullptr)
	, state(State::IDLE)
	, duration(0.0f)
	, elapsedTime(0.0f)
	, canvas(nullptr)
	, fadeImage(nullptr)
{
	initFadeImage();
}

Voxel::Director::~Director()
{
	if (currentScene)
	{
		delete currentScene;
	}

	if (nextScene)
	{
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
		newScene = new MenuScene();
		break;
	case Voxel::Director::SceneName::GAME_SCENE:
		break;
	case Voxel::Director::SceneName::EDITOR_SCENE:
		break;
	case Voxel::Director::SceneName::NONE:
	default:
		newScene = nullptr;
		break;
	}

	if (newScene)
	{
		newScene->init();
	}

	return newScene;
}

void Voxel::Director::initFadeImage()
{
	auto& sm = SpriteSheetManager::getInstance();
	sm.addSpriteSheet("GlobalSpriteSheet.json");

	canvas = new Voxel::UI::Canvas(Application::getInstance().getGLView()->getScreenSize(), glm::vec2(0.0f));

	fadeImage = Voxel::UI::Image::createFromSpriteSheet("fadeImage", "GlobalSpriteSheet", "1x1_white.png");
	fadeImage->setColor(glm::vec3(0.0f));
	fadeImage->setOpacity(0.0f);

	canvas->addChild(fadeImage);

	canvas->setVisibility(false);
}

void Voxel::Director::runScene(const SceneName sceneName)
{
	if (currentScene)
	{
		delete currentScene;
	}

	currentScene = createScene(sceneName);
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

			this->duration = duration;

			currentScene->onExit();
			nextScene->onEnter();

			canvas->setVisibility(true);

			fadeImage->setColor(fadeColor);
		}
	}
}

void Voxel::Director::update(const float delta)
{
	if (state == State::IDLE)
	{
		if (currentScene)
		{
			currentScene->update(delta);
		}
	}
	else
	{
		// transitioing
		if (nextScene == nullptr)
		{
			state = State::IDLE;

			duration = 0.0f;
			elapsedTime = 0.0f;
		}
		else
		{
			elapsedTime += delta;

			if (elapsedTime <= duration)
			{
				if (elapsedTime <= (duration * 0.5f))
				{
					// on first half, update current scene
					if (currentScene)
					{
						currentScene->update(delta);
					}
				}
				else
				{
					// on second half, update next scene
					nextScene->update(delta);
				}
			}
			else
			{
				// finished
				delete currentScene;

				currentScene = nextScene;
				nextScene = nullptr;

				state = State::IDLE;

				duration = 0.0f;
				elapsedTime = 0.0f;

				canvas->setVisibility(false);
			}
		}
	}
}

void Voxel::Director::render()
{
	if (currentScene)
	{
		currentScene->render();
	}

	if (state == State::TRANSITIONING)
	{
		if (elapsedTime <= duration)
		{
			const float halfDuration = duration * 0.5f;

			if (elapsedTime <= halfDuration)
			{
				// on first half, update current scene
				float ratio = elapsedTime / halfDuration;

				fadeImage->setOpacity(ratio);
			}
			else
			{
				// on second half, update next scene
				float ratio = ((elapsedTime  - halfDuration) / halfDuration);

				fadeImage->setOpacity(1.0f - ratio);
			}
		}
		// else, done.

		if (canvas)
		{
			canvas->render();
		}
	}
}
