#include "GameMenu.h"

// voxel
#include "Application.h"
#include "Director.h"
#include "GameScene.h"

// cpp
#include <functional>

Voxel::GameMenu::GameMenu()
	: canvas(nullptr)
	, gameMenu(nullptr)
	, optionsMenu(nullptr)
	, menuState(STATE::CLOSED)
{}

Voxel::GameMenu::~GameMenu()
{
	if (canvas)
	{
		delete canvas;
	}
}

void Voxel::GameMenu::init(GameScene* gameScenePtr)
{
	// init all game menu ui
	auto screenSize = Application::getInstance().getGLView()->getScreenSize();
	canvas = new Voxel::UI::Canvas(screenSize, glm::vec2(0.0f));

	initGameMenu(gameScenePtr);
	initOptionMenu(gameScenePtr);

	reset();
}

void Voxel::GameMenu::initGameMenu(GameScene * gameScenePtr)
{
	auto ss = "UISpriteSheet";

	const float btnYOffset = 34.0f;
	const float btnY = -63.0f;

	gameMenu = Voxel::UI::NinePatchImage::create("gm", ss, "game_menu_bg.png", 12.0f, 12.0f, 12.0f, 12.0f, glm::vec2(75.0f, 70.0f));
	gameMenu->setScale(2.0f);
	canvas->addChild(gameMenu, Z_ORDER::GAME_MENU_BG);

	auto label = Voxel::UI::Image::createFromSpriteSheet("gmLabel", ss, "game_menu_label.png");
	label->setPosition(0.0f, -22.0f);
	label->setCoordinateOrigin(glm::vec2(0.0f, 0.5f));
	gameMenu->addChild(label);

	auto bar = Voxel::UI::Image::createFromSpriteSheet("labelBar", ss, "bar.png");
	bar->setScale(glm::vec2(80.0f, 1.0f));
	bar->setPosition(0.0f, -34.0f);
	bar->setCoordinateOrigin(glm::vec2(0.0f, 0.5f));
	gameMenu->addChild(bar);

	auto returnToGameBtn = Voxel::UI::Button::create("rtgBtn", ss, "game_menu_return_to_game_button.png");
	returnToGameBtn->setScale(2.0f);
	returnToGameBtn->setPosition(0.0f, btnY);
	returnToGameBtn->setCoordinateOrigin(glm::vec2(0.0f, 0.5f));
	returnToGameBtn->setOnButtonClickCallbackFunc(std::bind(&GameScene::onReturnToGameClicked, gameScenePtr));
	gameMenu->addChild(returnToGameBtn);

	auto optionBtn = Voxel::UI::Button::create("oBtn", ss, "game_menu_option_button.png");
	optionBtn->setScale(2.0f);
	optionBtn->setPosition(0.0f, btnY - btnYOffset);
	optionBtn->setCoordinateOrigin(glm::vec2(0.0f, 0.5f));
	optionBtn->setOnButtonClickCallbackFunc(std::bind(&GameMenu::onOptionClicked, this));
	gameMenu->addChild(optionBtn);

	auto exitGameBtn = Voxel::UI::Button::create("exitGameBtn", ss, "game_menu_exit_game_button.png");
	exitGameBtn->setScale(2.0f);
	exitGameBtn->setPosition(0.0f, btnY - (btnYOffset * 2.5f));
	exitGameBtn->setCoordinateOrigin(glm::vec2(0.0f, 0.5f));
	exitGameBtn->setOnButtonClickCallbackFunc(std::bind(&GameMenu::onExitGameClicked, this));
	gameMenu->addChild(exitGameBtn);
}

void Voxel::GameMenu::initOptionMenu(GameScene * gameScenePtr)
{
	auto screenSize = Application::getInstance().getGLView()->getScreenSize();

	auto ss = "UISpriteSheet";
	const float btnYOffset = 34.0f;
	const float btnY = -63.0f;

	optionsMenu = Voxel::UI::NinePatchImage::create("gm", ss, "game_menu_bg.png", 12.0f, 12.0f, 12.0f, 12.0f, glm::vec2(75.0f, 121.0f));
	optionsMenu->setScale(2.0f);
	canvas->addChild(optionsMenu, Z_ORDER::OPTIONS_MENU_BG); 
	
	auto label = Voxel::UI::Image::createFromSpriteSheet("oLabel", ss, "options_label.png");
	label->setPosition(0.0f, -22.0f);
	label->setCoordinateOrigin(glm::vec2(0.0f, 0.5f));
	optionsMenu->addChild(label);

	auto bar = Voxel::UI::Image::createFromSpriteSheet("labelBar", ss, "bar.png");
	bar->setScale(glm::vec2(80.0f, 1.0f));
	bar->setPosition(0.0f, -34.0f);
	bar->setCoordinateOrigin(glm::vec2(0.0f, 0.5f));
	optionsMenu->addChild(bar);

	auto gamePlayButton = Voxel::UI::Button::create("gpBtn", ss, "option_gameplay_button.png");
	gamePlayButton->setScale(2.0f);
	gamePlayButton->setPosition(0.0f, btnY);
	gamePlayButton->setCoordinateOrigin(glm::vec2(0.0f, 0.5f));
	optionsMenu->addChild(gamePlayButton);

	auto controlsBtn = Voxel::UI::Button::create("cBtn", ss, "option_controls_button.png");
	controlsBtn->setScale(2.0f);
	controlsBtn->setPosition(0.0f, btnY - btnYOffset);
	controlsBtn->setCoordinateOrigin(glm::vec2(0.0f, 0.5f));
	controlsBtn->setOnButtonClickCallbackFunc(std::bind(&GameMenu::onControlsClicked, this));
	optionsMenu->addChild(controlsBtn);

	auto videoButton = Voxel::UI::Button::create("vBtn", ss, "option_video_button.png");
	videoButton->setScale(2.0f);
	videoButton->setPosition(0.0f, btnY - (btnYOffset * 2.0f));
	videoButton->setCoordinateOrigin(glm::vec2(0.0f, 0.5f));
	optionsMenu->addChild(videoButton);

	auto audioButton = Voxel::UI::Button::create("aBtn", ss, "option_audio_button.png");
	audioButton->setScale(2.0f);
	audioButton->setPosition(0.0f, btnY - (btnYOffset * 3.0f));
	audioButton->setCoordinateOrigin(glm::vec2(0.0f, 0.5f));
	optionsMenu->addChild(audioButton);

	auto returnToMenuBtn = Voxel::UI::Button::create("rtmBtn", ss, "return_to_menu_button.png");
	returnToMenuBtn->setScale(2.0f);
	returnToMenuBtn->setPosition(0.0f, btnY - (btnYOffset * 4.0f));
	returnToMenuBtn->setCoordinateOrigin(glm::vec2(0.0f, 0.5f));
	returnToMenuBtn->setOnButtonClickCallbackFunc(std::bind(&GameMenu::onRetrunToMenuClicked, this));
	optionsMenu->addChild(returnToMenuBtn);

	auto returnToGameBtn = Voxel::UI::Button::create("rtgBtn", ss, "game_menu_return_to_game_button.png");
	returnToGameBtn->setScale(2.0f);
	returnToGameBtn->setPosition(0.0f, btnY - (btnYOffset * 5.5f));
	returnToGameBtn->setCoordinateOrigin(glm::vec2(0.0f, 0.5f));
	returnToGameBtn->setOnButtonClickCallbackFunc(std::bind(&GameScene::onReturnToGameClicked, gameScenePtr));
	optionsMenu->addChild(returnToGameBtn);
}

void Voxel::GameMenu::reset()
{
	gameMenu->setVisibility(true);
	optionsMenu->setVisibility(false);
}

void Voxel::GameMenu::onOptionClicked()
{
	// view option menu
	gameMenu->setVisibility(false);
	optionsMenu->setVisibility(true);
}

void Voxel::GameMenu::onControlsClicked()
{
}

void Voxel::GameMenu::onExitGameClicked()
{
	Application::getInstance().getGLView()->close();
}

void Voxel::GameMenu::onRetrunToMenuClicked()
{
	reset();
}

void Voxel::GameMenu::open()
{
	// open menu
	if (menuState == STATE::CLOSED)
	{
		menuState = STATE::OPENED;
		canvas->setVisibility(true);
	}
}

void Voxel::GameMenu::close()
{
	// close menu
	if (menuState != STATE::CLOSED)
	{
		menuState = STATE::CLOSED;
		canvas->setVisibility(false);

		reset();
	}
}

bool Voxel::GameMenu::isOpened() const
{
	return menuState != STATE::CLOSED;
}

bool Voxel::GameMenu::isClosed() const
{
	return menuState == STATE::CLOSED;
}

void Voxel::GameMenu::updateMouseMove(const glm::vec2 & mousePosition)
{
	canvas->updateMouseMove(mousePosition, glm::vec2(0.0f));
}

void Voxel::GameMenu::updateMousePress(const glm::vec2 & mousePosition, const int button)
{
	canvas->updateMousePress(mousePosition, button);
}

void Voxel::GameMenu::updateMouseReleased(const glm::vec2 & mousePosition, const int button)
{
	canvas->updateMouseRelease(mousePosition, button);
}

void Voxel::GameMenu::render()
{
	canvas->render();
}
