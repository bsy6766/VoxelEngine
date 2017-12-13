#include "Editor.h"

// voxel
#include "Application.h"
#include "SpriteSheet.h"
#include "InputHandler.h"

using namespace Voxel;

Editor::Editor()
	: Scene()
	, canvas(nullptr)
	, fileDropDownBg(nullptr)
	, input(&InputHandler::getInstance())
{
	for (unsigned int i = 0; i < menuBarButtons.size(); ++i)
	{
		menuBarButtons.at(i) = nullptr;
	}

	for (unsigned int i = 0; i < fileDropDownButtons.size(); ++i)
	{
		fileDropDownButtons.at(i) = nullptr;
	}
}

Editor::~Editor()
{}

void Voxel::Editor::init()
{
	canvas = new Voxel::UI::Canvas(Application::getInstance().getGLView()->getScreenSize(), glm::vec2(0.0f));

	const auto ss = "EditorUISpriteSheet";

	auto& sm = SpriteSheetManager::getInstance();

	sm.addSpriteSheet("EditorUISpriteSheet.json");

	auto menuBar = Voxel::UI::Image::createFromSpriteSheet("mb", ss, "menu_bar.png");
	menuBar->setScale(glm::vec2(Application::getInstance().getGLView()->getScreenSize().x, 1.0f));
	menuBar->setPivot(glm::vec2(-0.5f, 0.5f));
	menuBar->setCoordinateOrigin(glm::vec2(-0.5f, 0.5f));
	menuBar->setPosition(0.0f, 0.0f);
	canvas->addChild(menuBar);

	auto fileBtn = Voxel::UI::Button::create("fBtn", ss, "file_button.png");
	fileBtn->setPosition(26.0f, 14.0f);
	fileBtn->setCoordinateOrigin(glm::vec2(-0.5f, 0.5f));
	menuBar->addChild(fileBtn);
}

void Voxel::Editor::release()
{
	if (canvas)
	{
		delete canvas;
		canvas = nullptr;
	}

	auto& sm = SpriteSheetManager::getInstance();

	sm.removeSpriteSheetByKey("EditorUISpriteSheet");
}

void Voxel::Editor::onEnter()
{}

void Voxel::Editor::onEnterFinished()
{}

void Voxel::Editor::onExit()
{}

void Voxel::Editor::onExitFinished()
{}

void Voxel::Editor::update(const float delta)
{
	if (canvas)
	{
		canvas->update(delta);
	}

	updateMouseMove();
	updateMouseClick();
	updateMouseScroll();
}

void Voxel::Editor::updateMouseMove()
{
}

void Voxel::Editor::updateMouseClick()
{
}

void Voxel::Editor::updateMouseScroll()
{
}

void Voxel::Editor::render()
{
	if (canvas)
	{
		canvas->render();
	}
}