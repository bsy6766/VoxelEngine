#include "Editor.h"

// voxel
#include "Application.h"
#include "SpriteSheet.h"
#include "InputHandler.h"
#include "Cursor.h"
#include "Director.h"

// cpp
#include <functional>

using namespace Voxel;

Editor::Editor()
	: Scene()
	, canvas(nullptr)
	, fileBtn(nullptr)
	, editBtn(nullptr)
	, fileDropDownBg(nullptr)
	, input(&InputHandler::getInstance())
	, cursor(&Cursor::getInstance())
	, menuBarDropDowned(false)
{
	for (unsigned int i = 0; i < fileDropDownButtons.size(); ++i)
	{
		fileDropDownButtons.at(i) = nullptr;
	}
}

Editor::~Editor()
{}

void Voxel::Editor::init()
{
	Application::getInstance().getGLView()->setClearColor(glm::vec3(0.4375f));

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

	fileBtn = Voxel::UI::Button::create("fBtn", ss, "file_button.png");
	fileBtn->setPosition(26.0f, -14.0f);
	fileBtn->setCoordinateOrigin(glm::vec2(-0.5f, 0.5f));
	fileBtn->setOnButtonClickCallbackFunc(std::bind(&Editor::onFileButtonClicked, this));
	menuBar->addChild(fileBtn);

	editBtn = Voxel::UI::Button::create("eBtn", ss, "edit_button.png");
	editBtn->setPosition(72.0f, -14.0f);
	editBtn->setCoordinateOrigin(glm::vec2(-0.5f, 0.5f));
	editBtn->disable();
	editBtn->setOnButtonClickCallbackFunc(std::bind(&Editor::onEditButtonClicked, this));
	menuBar->addChild(editBtn);

	auto exitGameBtn = Voxel::UI::Button::create("exitBtn", ss, "exit_game_button.png");
	exitGameBtn->setCoordinateOrigin(glm::vec2(0.5f, 0.5f));
	exitGameBtn->setPosition(glm::vec2(-26.0f, -14.0f));
	exitGameBtn->setOnButtonClickCallbackFunc(std::bind(&Editor::onExitButtonClicked, this));
	menuBar->addChild(exitGameBtn);

	auto returnToMainMenuBtn = Voxel::UI::Button::create("rtmmBtn", ss, "return_to_main_menu_button.png");
	returnToMainMenuBtn->setCoordinateOrigin(glm::vec2(0.5f, 0.5f));
	returnToMainMenuBtn->setPosition(glm::vec2(-141.0f, -14.0f));
	returnToMainMenuBtn->setOnButtonClickCallbackFunc(std::bind(&Editor::onReturnToMainMenuButtonClicked, this));
	menuBar->addChild(returnToMainMenuBtn);

	fileDropDownBg = Voxel::UI::NinePatchImage::create("fddBg", ss, "file_drop_down_bg.png", 2.0f, 2.0f, 2.0f, 2.0f, glm::vec2(116.0f, 108.0f));
	fileDropDownBg->setPivot(glm::vec2(-0.5f, 0.5f));
	fileDropDownBg->setCoordinateOrigin(glm::vec2(-0.5f, 0.5f));
	fileDropDownBg->setPosition(0.0f, -30.0f);
	fileDropDownBg->setVisibility(false);
	canvas->addChild(fileDropDownBg);

	auto newBtn = Voxel::UI::Button::create("nBtn", ss, "file_new_button.png");
	newBtn->setCoordinateOrigin(glm::vec2(0.0f, 0.5f));
	newBtn->setPosition(0.0f, -14.0f);
	fileDropDownBg->addChild(newBtn);

	auto openBtn = Voxel::UI::Button::create("oBtn", ss, "file_open_button.png");
	openBtn->setCoordinateOrigin(glm::vec2(0.0f, 0.5f));
	openBtn->setPosition(0.0f, -40.0f);
	fileDropDownBg->addChild(openBtn);

	auto saveBtn = Voxel::UI::Button::create("sBtn", ss, "file_save_button.png");
	saveBtn->setCoordinateOrigin(glm::vec2(0.0f, 0.5f));
	saveBtn->setPosition(0.0f, -66.0f);
	fileDropDownBg->addChild(saveBtn);

	auto saveAsBtn = Voxel::UI::Button::create("saBtn", ss, "file_save_as_button.png");
	saveAsBtn->setCoordinateOrigin(glm::vec2(0.0f, 0.5f));
	saveAsBtn->setPosition(0.0f, -92.0f);
	fileDropDownBg->addChild(saveAsBtn);
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

		updateMouseMove();
		updateMouseClick();
	}

	updateMouseScroll();
}

void Voxel::Editor::updateMouseMove()
{
	if (canvas)
	{
		auto mouseMovedDist = input->getMouseMovedDistance();
		canvas->updateMouseMove(cursor->getPosition(), glm::vec2(mouseMovedDist.x, -mouseMovedDist.y));
	}

	if (menuBarDropDowned)
	{
		auto mp = cursor->getPosition();

		auto fileBtnBB = fileBtn->getBoundingBox();
		fileBtnBB.size.x += 2.0f;
		fileBtnBB.size.y += 8.0f;

		auto editBtnBB = editBtn->getBoundingBox();
		editBtnBB.size.x += 2.0f;
		editBtnBB.size.y += 8.0f;

		bool fileBtnCheck = fileBtnBB.containsPoint(mp);
		bool editBtnCheck = editBtnBB.containsPoint(mp);

		if (!fileBtnCheck && !editBtnCheck)
		{
			if (!fileDropDownBg->getBoundingBox().containsPoint(mp))
			{
				fileDropDownBg->setVisibility(false);
				menuBarDropDowned = false;
			}
		}
		else if (fileBtnCheck)
		{
			onFileButtonClicked();
		}
		else if (editBtnCheck)
		{
			onEditButtonClicked();
		}
	}
}

void Voxel::Editor::updateMouseClick()
{
	if (canvas)
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
}

void Voxel::Editor::updateMouseScroll()
{
	auto mouseScroll = input->getMouseScrollValue();

	if (mouseScroll == 1)
	{
		// zoom int
	}
	else if (mouseScroll == -1)
	{
		// zoom out
	}
}

void Voxel::Editor::onFileButtonClicked()
{
	fileDropDownBg->setVisibility(true);
	menuBarDropDowned = true;
}

void Voxel::Editor::onEditButtonClicked()
{
	fileDropDownBg->setVisibility(false);
	menuBarDropDowned = true;
}

void Voxel::Editor::onReturnToMainMenuButtonClicked()
{
	Application::getInstance().getDirector()->replaceScene(Voxel::Director::SceneName::MENU_SCENE, 1.5f);
}

void Voxel::Editor::onExitButtonClicked()
{
	Application::getInstance().getGLView()->close();
}

void Voxel::Editor::render()
{
	if (canvas)
	{
		canvas->render();
	}
}