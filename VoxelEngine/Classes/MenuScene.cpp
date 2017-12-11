#include "MenuScene.h"

// voxel
#include "Application.h"
#include "SpriteSheet.h"

Voxel::MenuScene::MenuScene()
	: canvas(nullptr)
	, playButton(nullptr)
	, editorButton(nullptr)
	, optionsButton(nullptr)
	, creditsButton(nullptr)
	, exitGameButton(nullptr)
{}

Voxel::MenuScene::~MenuScene()
{
	if (canvas)
	{
		delete canvas;
	}
}

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
	buttonBg->setColor(glm::vec3(0.0f));
	buttonBg->setVisibility(false);
	canvas->addChild(buttonBg);

	const float btnY = 280.0f;
	float offset = 50.0f;

	// buttons
	playButton = Voxel::UI::Button::create("gBtn", ss, "play_button.png");
	playButton->setCoordinateOrigin(glm::vec2(0.0f, -0.5f));
	playButton->setPosition(0.0f, btnY);
	canvas->addChild(playButton);

#if V_DEBUG && V_DEBUG_EDITOR
	editorButton = Voxel::UI::Button::create("eBtn", ss, "editor_button.png");
	editorButton->setCoordinateOrigin(glm::vec2(0.0f, -0.5f));
	editorButton->setPosition(0.0f, btnY - offset);
	canvas->addChild(editorButton);

	offset += 50.0f;
#endif

	optionsButton = Voxel::UI::Button::create("oBtn", ss, "options_button.png");
	optionsButton->setCoordinateOrigin(glm::vec2(0.0f, -0.5f));
	optionsButton->setPosition(0.0f, btnY - offset);
	canvas->addChild(optionsButton);

	offset += 50.0f;

	creditsButton = Voxel::UI::Button::create("cBtn", ss, "credits_button.png");
	creditsButton->setCoordinateOrigin(glm::vec2(0.0f, -0.5f));
	creditsButton->setPosition(0.0f, btnY - offset);
	canvas->addChild(creditsButton);

	offset += 50.0f;

	exitGameButton = Voxel::UI::Button::create("egBtn", ss, "exit_game_button.png");
	exitGameButton->setCoordinateOrigin(glm::vec2(0.0f, -0.5f));
	exitGameButton->setPosition(0.0f, btnY - offset);
	canvas->addChild(exitGameButton);
}

void Voxel::MenuScene::onEnter()
{
	
}

void Voxel::MenuScene::onExit()
{
	auto& sm = SpriteSheetManager::getInstance();

	sm.removeSpriteSheetByKey("MenuSceneUISpriteSheet");
}

void Voxel::MenuScene::update(const float delta)
{
	if (canvas)
	{
		canvas->update(delta);
	}
}

void Voxel::MenuScene::render()
{
	if (canvas)
	{
		canvas->render();
	}
}
