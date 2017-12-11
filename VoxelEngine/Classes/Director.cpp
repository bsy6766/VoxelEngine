#include "Director.h"

// voxel
#include "MenuScene.h"
#include "GameScene.h"
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
	, fadeState(FadeState::READY)
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
			fadeState = FadeState::FADE_IN;

			this->duration = duration;
			this->elapsedTime = 0.0f;

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
			fadeState = FadeState::FINISHED;

			duration = 0.0f;
			elapsedTime = 0.0f;
		}
		else
		{
			elapsedTime += delta;

			if (elapsedTime <= duration)
			{
				if (fadeState == FadeState::FADE_IN)
				{
					// on first half, update current scene
					if (currentScene)
					{
						currentScene->update(delta);
					}

					//std::cout << "fade_in: " << elapsedTime << "\n";

					if (elapsedTime >= (duration * 0.5f))
					{
						fadeImage->setOpacity(1.0f);
						fadeState = FadeState::DELAY;
						//std::cout << "Finished.\n";
					}
				}
				else if (fadeState == FadeState::DELAY)
				{
					//std::cout << "delay: " << elapsedTime << "\n";
					if (elapsedTime >= ((duration * 0.5f) + 0.2f))
					{
						elapsedTime -= 0.2f;
						fadeState = FadeState::FADE_OUT;
						//std::cout << "Finished.\n";
					}
				}
				else if (fadeState == FadeState::FADE_OUT)
				{
					//std::cout << "Fade_out: " << elapsedTime << "\n";
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

				currentScene->onEnterFinished();

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
	if (elapsedTime <= duration)
	{
		if (elapsedTime <= (duration * 0.5f))
		{
			// on first half, render current scene
			if (currentScene)
			{
				currentScene->render();
			}
		}
		else
		{
			// on second half, update next scene
			if (nextScene)
			{
				nextScene->render();
			}
		}
	}

	if (state == State::TRANSITIONING)
	{
		if (elapsedTime <= duration)
		{
			const float halfDuration = duration * 0.5f;


			if (fadeState == FadeState::FADE_IN)
			{
				// on first half, update current scene
				float ratio = elapsedTime / halfDuration;
				//std::cout << "r = " << ratio << "\n";
				fadeImage->setOpacity(ratio);
			}
			else if (fadeState == FadeState::DELAY)
			{
			}
			else if (fadeState == FadeState::FADE_OUT)
			{
				// on second half, update next scene
				float ratio = ((elapsedTime - halfDuration) / halfDuration);
				//std::cout << "r = " << 1.0f - ratio << "\n";

				fadeImage->setOpacity(1.0f - ratio);
			}

			//std::cout << "opacity = " << fadeImage->getOpacity() << "\n";
		}
		// else, done.

		if (canvas)
		{
			canvas->render();
		}
	}
}
