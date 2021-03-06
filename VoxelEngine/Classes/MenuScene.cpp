// pch
#include "PreCompiled.h"

#include "MenuScene.h"

// voxel
#include "Application.h"
#include "SpriteSheet.h"
#include "Cursor.h"
#include "InputHandler.h"
#include "UIActions.h"
#include "Director.h"
#include "Color.h"

Voxel::MenuScene::MenuScene()
	: Scene()
	, canvas(nullptr)
	, curHoveringButtonIndex(-1)
	, cursor(nullptr)
	, input(nullptr)
{}

Voxel::MenuScene::~MenuScene()
{}

void Voxel::MenuScene::init()
{
	// Initialize menu scene
	auto rs = Application::getInstance().getGLView()->getScreenSize();
	
	canvas = new Voxel::UI::Canvas(rs, glm::vec2(0.0f));
	const auto ss = "MenuSceneUISpriteSheet";
	auto& sm = SpriteSheetManager::getInstance();

	sm.addSpriteSheet("MenuSceneUISpriteSheet.json");
	
	// temp. Todo: replace menu scene background with randomly generated oak + birch tree woods biome on plain terrain. 
	// Todo: rotate background world slightly based on players cursor coordinate.
	// Idea: Change background world to different biome type after X sceonds and repeat
	bg = Voxel::UI::Image::createFromSpriteSheet("bg", "GlobalSpriteSheet", "1x1_white.png");
	bg->setScale(rs);
	bg->setColor(glm::vec3(0.5f, 0.5f, 0.5f));
	canvas->addChild(bg);

	// button bg
	buttonBg = Voxel::UI::Image::createFromSpriteSheet("bg", "GlobalSpriteSheet", "1x1_white.png");
	buttonBg->setScale(glm::vec2(450.0f, 50.0f));
	buttonBg->setCoordinateOrigin(glm::vec2(0.0f, -0.5f));
	buttonBg->setColor(glm::vec3(0.0f));
	buttonBg->setOpacity(0.0f);
	canvas->addChild(buttonBg);
	
	float btnY = 250.0f; 
#if V_DEBUG
#if V_DEBUG_EDITOR
	btnY += 50.0f;
#endif
#if V_DEBUG_PARTICLE_SYSTEM_EDITOR
	btnY += 50.0f;
	buttonBg->setScale(glm::vec2(600.0f, 50.0f));
#endif
#if V_DEBUG_UI_TEST
	btnY += 50.0f;
#endif
#endif

	float offset = 50.0f;

	// buttons
	buttons.at(ButtonIndex::PLAY) = Voxel::UI::Button::create("gBtn", ss, "play_button.png");
	buttons.at(ButtonIndex::PLAY)->setOnTriggeredCallbackFunc(std::bind(&Voxel::MenuScene::onPlayClicked, this, std::placeholders::_1));
#if V_DEBUG 
#if V_DEBUG_EDITOR
	buttons.at(ButtonIndex::EDITOR) = Voxel::UI::Button::create("eBtn", ss, "editor_button.png");
	buttons.at(ButtonIndex::EDITOR)->setOnTriggeredCallbackFunc(std::bind(&Voxel::MenuScene::onEditorClicked, this, std::placeholders::_1));
#endif
#if V_DEBUG_PARTICLE_SYSTEM_EDITOR
	buttons.at(ButtonIndex::PARTICLE_SYSTEM_EDITOR) = Voxel::UI::Button::create("pseBtn", ss, "particle_system_editor_button.png");
	buttons.at(ButtonIndex::PARTICLE_SYSTEM_EDITOR)->setOnTriggeredCallbackFunc(std::bind(&Voxel::MenuScene::onParticleSystemEditorClicked, this, std::placeholders::_1));
#endif
#if V_DEBUG_UI_TEST
	buttons.at(ButtonIndex::UI_TEST) = Voxel::UI::Button::create("uiTestBtn", ss, "ui_test_button.png");
	buttons.at(ButtonIndex::UI_TEST)->setOnTriggeredCallbackFunc(std::bind(&Voxel::MenuScene::onUITestClicked, this, std::placeholders::_1));
#endif
#endif
	buttons.at(ButtonIndex::OPTIONS) = Voxel::UI::Button::create("oBtn", ss, "options_button.png");
	buttons.at(ButtonIndex::CREDITS) = Voxel::UI::Button::create("cBtn", ss, "credits_button.png");
	buttons.at(ButtonIndex::EXIT_GAME) = Voxel::UI::Button::create("egBtn", ss, "exit_game_button.png");
	buttons.at(ButtonIndex::EXIT_GAME)->setOnTriggeredCallbackFunc(std::bind(&Voxel::MenuScene::onExitGameClicked, this, std::placeholders::_1));

	for (auto btn : buttons)
	{
		btn->setCoordinateOrigin(glm::vec2(0.0f, -0.5f));
		btn->setPosition(0.0f, btnY - offset);
		canvas->addChild(btn);

		offset += 50.0f;
	}

	// cursor
	cursor = &Voxel::Cursor::getInstance();

	// input
	input = &InputHandler::getInstance();
	
	sm.print(false);
	TextureManager::getInstance().print();
}

void Voxel::MenuScene::release()
{
	if (canvas)
	{
		delete canvas;
	}

	auto& sm = SpriteSheetManager::getInstance();

	sm.removeSpriteSheetByKey("MenuSceneUISpriteSheet");

	std::cout << "MenuScene released spritesheet\n";
	sm.print(false);
}

void Voxel::MenuScene::onEnter()
{
	cursor->setVisibility(true);
	cursor->setPosition(glm::vec2(0.0f));

	input->setCursorToCenter();

	Application::getInstance().getGLView()->setClearColor(glm::vec3(0.0f));
}

void Voxel::MenuScene::onEnterFinished()
{
	Application::getInstance().getGLView()->setVsync(true);
}

void Voxel::MenuScene::onExit()
{
	Application::getInstance().getGLView()->setVsync(false);
}

void Voxel::MenuScene::onExitFinished()
{}

void Voxel::MenuScene::update(const float delta)
{
	if (canvas)
	{
		canvas->update(delta);
	}

	updateKeyboardInput();
	updateMouseMoveInput();
	updateMouseClickInput();
	updateMouseScrollInput();
	
	if (curHoveringButtonIndex == -1)
	{
		auto co = buttonBg->getOpacity();
		const float t = 10.0f * delta;
		buttonBg->setOpacity(((1.0f - t) * co) + 0.0f);
	}
	else
	{
		auto co = buttonBg->getOpacity();
		const float t = 10.0f * delta;
		buttonBg->setOpacity(((1.0f - t) * co) + (t * 1.0f));
	}
}

void Voxel::MenuScene::updateKeyboardInput()
{
	if (input->getKeyDown(Voxel::InputHandler::KEY_INPUT::GLOBAL_ESCAPE, true))
	{
		Application::getInstance().getGLView()->close();
		return;
	}

	if (input->getKeyDown(GLFW_KEY_T, true))
	{
		auto n1 = Voxel::UI::Node::create("0");
		canvas->addChild(n1, 2);
		canvas->addChild(Voxel::UI::Node::create("1"), 2);
		canvas->addChild(Voxel::UI::Node::create("2"), 2);
		canvas->addChild(Voxel::UI::Node::create("3"), 2);
		canvas->addChild(Voxel::UI::Node::create("4"), 2);
		canvas->addChild(Voxel::UI::Node::create("5"), 2);

		canvas->print();
	}
	else if (input->getKeyDown(GLFW_KEY_R, true))
	{
	}
	else if (input->getKeyDown(GLFW_KEY_Y, true))
	{
	}
}

void Voxel::MenuScene::updateMouseMoveInput()
{
	auto mouseMovedDist = input->getMouseMovedDistance();
	
	auto cursorPos = cursor->getPosition();

	canvas->updateMouseMove(cursorPos, glm::vec2(mouseMovedDist.x, -mouseMovedDist.y));

	if (mouseMovedDist.x != 0.0f || mouseMovedDist.y != 0.0f)
	{
		unsigned int index = 0;

		for (auto btn : buttons)
		{
			auto bb = btn->getBoundingBox();
			bb.size.y += 14.0f;
			bb.size.x = 450.0f;
			if (bb.containsPoint(cursorPos))
			{
				if (curHoveringButtonIndex == -1)
				{
					buttonBg->stopAllActions();
					buttonBg->setPosition(btn->getPosition());
				}
				else if (index != curHoveringButtonIndex)
				{
					buttonBg->stopAllActions();
					buttonBg->runAction(Voxel::UI::MoveTo::create(0.05f, btn->getPosition()));
				}

				curHoveringButtonIndex = index;
				return;
			}

			index++;
		}

		// Didn't hover any 
		if (curHoveringButtonIndex != -1)
		{
			curHoveringButtonIndex = -1;
		}
	}
}

void Voxel::MenuScene::updateMouseClickInput()
{
	if (input->getMouseDown(GLFW_MOUSE_BUTTON_1, true))
	{
		canvas->updateMousePress(cursor->getPosition(), GLFW_MOUSE_BUTTON_1);
	}
	else if (input->getMouseUp(GLFW_MOUSE_BUTTON_1, true))
	{
		canvas->updateMouseRelease(cursor->getPosition(), GLFW_MOUSE_BUTTON_1);
	}
}

void Voxel::MenuScene::updateMouseScrollInput()
{
	auto mouseScroll = input->getMouseScrollValue();

	bool changed = false;

	if (mouseScroll == 1)
	{
		curHoveringButtonIndex--;
		changed = true;

		if (curHoveringButtonIndex < 0)
		{
			curHoveringButtonIndex = 0;
			changed = false;
		}
	}
	else if(mouseScroll == -1)
	{
		curHoveringButtonIndex++;
		changed = true;

		if (curHoveringButtonIndex >= ButtonIndex::MAX_BUTTON_COUNT)
		{
			curHoveringButtonIndex = ButtonIndex::MAX_BUTTON_COUNT - 1;
			changed = false;
		}
	}

	if (changed)
	{
		buttonBg->stopAllActions();
		buttonBg->runAction(Voxel::UI::MoveTo::create(0.05f, buttons[curHoveringButtonIndex]->getPosition()));
	}
}

void Voxel::MenuScene::updateControllerInput(const float delta)
{}

void Voxel::MenuScene::onPlayClicked(Voxel::UI::Button* sender)
{
	Application::getInstance().getDirector()->replaceScene(Voxel::Director::SceneName::GAME_SCENE, 1.5f);
}

void Voxel::MenuScene::onExitGameClicked(Voxel::UI::Button* sender)
{
	Application::getInstance().getGLView()->close();
}

void Voxel::MenuScene::render()
{
	glClear(GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_ALWAYS);

	if (canvas)
	{
		canvas->render();
	}
}

#if V_DEBUG
#if V_DEBUG_EDITOR
void Voxel::MenuScene::onEditorClicked(Voxel::UI::Button* sender)
{
	Application::getInstance().getDirector()->replaceScene(Voxel::Director::SceneName::EDITOR_SCENE, 0.5f);
}
#endif
#if V_DEBUG_PARTICLE_SYSTEM_EDITOR
void Voxel::MenuScene::onParticleSystemEditorClicked(Voxel::UI::Button * sender)
{
	Application::getInstance().getDirector()->replaceScene(Voxel::Director::SceneName::PARTICLE_SYSTEM_EDITOR_SCENE, 0.5f);
}
#endif
#if V_DEBUG_UI_TEST
void Voxel::MenuScene::onUITestClicked(Voxel::UI::Button* sender)
{
	Application::getInstance().getDirector()->replaceScene(Voxel::Director::SceneName::UI_TEST_SCENE, 0.5f);
}
#endif
#endif