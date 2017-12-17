#include "Editor.h"

// voxel
#include "Application.h"
#include "SpriteSheet.h"
#include "InputHandler.h"
#include "Cursor.h"
#include "Director.h"
#include "Utility.h"
#include "Camera.h"
#include "ProgramManager.h"
#include "Program.h"

// cpp
#include <functional>

// glm
#include <glm\gtx\transform.hpp>

using namespace Voxel;

Editor::Editor()
	: Scene()
	, canvas(nullptr)
	, fileBtn(nullptr)
	, editBtn(nullptr)
	, returnToMainMenuBtn(nullptr)
	, exitGameBtn(nullptr)
	, fileDropDownBg(nullptr)
	, input(&InputHandler::getInstance())
	, cursor(&Cursor::getInstance())
	, menuBarDropDowned(false)
	, newCreateWindow(nullptr)
	, createBtn(nullptr)
	, newFileNameInputField(nullptr)
	, schematic(nullptr)
	, floorVao(0)
	, floorAngleX(0)
	, floorAngleY(0)
	, floorModelMat(1.0f)
	, floorColor(glm::vec4(142.0f/255.0f, 1.0f, 237.0f/255.0f, 0.75f))
	, originLineVao(0)
	, floorPosition(0.0f)
	, mouseState(MouseState::IDLE)
	, zoomLevel(0)
	, slider(nullptr)
{}

Editor::~Editor()
{
	if (floorVao)
	{
		glDeleteVertexArrays(1, &floorVao);
	}
}

void Voxel::Editor::init()
{
	Application::getInstance().getGLView()->setClearColor(glm::vec3(0.4375f));

	auto program = ProgramManager::getInstance().getProgram(ProgramManager::PROGRAM_NAME::POLYGON_SHADER);
	program->use(true);

	program->setUniformMat4("projMat", Camera::mainCamera->getProjection());
	program->setUniformMat4("viewMat", Camera::mainCamera->getViewMat());

	initEditor();
	initUI();
}

void Voxel::Editor::initEditor()
{
	float size = 50.0f;

	std::vector<float> floorVert = 
	{
		-size, 0.0f, -size,
		-size, 0.0f, size,
		size, 0.0f, -size,
		size, 0.0f, size,
		/*
		-size, -size, 0.0f,
		-size, size, 0.0f,
		size, -size, 0.0f,
		size, size, 0.0f
		*/
	};

	std::vector<unsigned int> floorIndices =
	{
		0, 1, 2, 1, 2, 3,
	};

	// gen vao
	glGenVertexArrays(1, &floorVao);
	glBindVertexArray(floorVao);

	GLuint floorVbo;
	glGenBuffers(1, &floorVbo);
	glBindBuffer(GL_ARRAY_BUFFER, floorVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * floorVert.size(), &floorVert.front(), GL_STATIC_DRAW);

	// Enable vertices attrib
	auto program = ProgramManager::getInstance().getProgram(ProgramManager::PROGRAM_NAME::POLYGON_SHADER);
	GLint vertLoc = program->getAttribLocation("vert");

	// vert
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLuint floorIbo;
	glGenBuffers(1, &floorIbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorIbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * floorIndices.size(), &floorIndices.front(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	glDeleteBuffers(1, &floorVbo);
	glDeleteBuffers(1, &floorIbo);
	floorVbo = 0;
	floorIbo = 0;

	std::vector<float> oLineVert = 
	{
		0.0f, 500.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, -500.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	};

	// gen vao
	glGenVertexArrays(1, &originLineVao);
	glBindVertexArray(originLineVao);

	// Generate buffer object
	GLuint oLineVbo;
	glGenBuffers(1, &oLineVbo);
	glBindBuffer(GL_ARRAY_BUFFER, oLineVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * oLineVert.size(), &oLineVert.front(), GL_STATIC_DRAW);

	auto lineProgram = ProgramManager::getInstance().getProgram(ProgramManager::PROGRAM_NAME::LINE_SHADER);
	GLint lineVertLoc = lineProgram->getAttribLocation("vert");
	GLint colorLoc = lineProgram->getAttribLocation("color");

	// vert
	glEnableVertexAttribArray(lineVertLoc);
	glVertexAttribPointer(lineVertLoc, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), nullptr);

	// color
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));

	glBindVertexArray(0);

	glDeleteBuffers(1, &oLineVbo);
}

void Voxel::Editor::initUI()
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

	exitGameBtn = Voxel::UI::Button::create("exitBtn", ss, "exit_game_button.png");
	exitGameBtn->setCoordinateOrigin(glm::vec2(0.5f, 0.5f));
	exitGameBtn->setPosition(glm::vec2(-26.0f, -14.0f));
	exitGameBtn->setOnButtonClickCallbackFunc(std::bind(&Editor::onExitButtonClicked, this));
	menuBar->addChild(exitGameBtn);

	returnToMainMenuBtn = Voxel::UI::Button::create("rtmmBtn", ss, "return_to_main_menu_button.png");
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
	newBtn->setOnButtonClickCallbackFunc(std::bind(&Editor::onNewButtonClicked, this));
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

	newCreateWindow = Voxel::UI::NinePatchImage::create("ncw", ss, "new_window_bg.png", 8.0f, 8.0f, 44.0f, 16.0f, glm::vec2(250.0f, 60.0f));
	newCreateWindow->setPosition(0.0f, 0.0f);
	newCreateWindow->setVisibility(false);
	canvas->addChild(newCreateWindow);

	newFileNameInputField = Voxel::UI::InputField::create("nfIF", "Enter file name", "DebugSpriteSheet", 1, "debug_input_field_cursor.png");
	newFileNameInputField->setCoordinateOrigin(glm::vec2(-0.5f, 0.5f));
	newFileNameInputField->setAlign(Voxel::UI::InputField::Align::LEFT);
	newFileNameInputField->setPosition(8.0f, -33.0f);
	newFileNameInputField->setOnEditCallback(std::bind(&Voxel::Editor::onNewFileNameEdit, this, std::placeholders::_1));
	newFileNameInputField->setOnEditSubmitted(std::bind(&Voxel::Editor::onNewFileNameEditSubmitted, this, std::placeholders::_1));
	newCreateWindow->addChild(newFileNameInputField);

	auto newLabel = Voxel::UI::Text::create("newLabel", "NEW", 1);
	newLabel->setCoordinateOrigin(glm::vec2(0.0f, 0.5f));
	newLabel->setPosition(0.0f, -9.0f);
	newCreateWindow->addChild(newLabel);

	createBtn = Voxel::UI::Button::create("cBtn", ss, "new_create_button.png");
	createBtn->setCoordinateOrigin(glm::vec2(-0.5f, -0.5f));
	createBtn->setPosition(35.0f, 15.0f);
	createBtn->setOnButtonClickCallbackFunc(std::bind(&Editor::onNewCreateButtonClicked, this));
	createBtn->disable();
	newCreateWindow->addChild(createBtn);

	auto cancelBtn = Voxel::UI::Button::create("clBtn", ss, "new_cancel_button.png");
	cancelBtn->setCoordinateOrigin(glm::vec2(0.5f, -0.5f));
	cancelBtn->setPosition(-35.0f, 15.0f);
	cancelBtn->setOnButtonClickCallbackFunc(std::bind(&Editor::onNewCancelButtonClicked, this));
	newCreateWindow->addChild(cancelBtn);
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
{
	// reset camera
	Camera* mc = Camera::mainCamera;

	mc->setPosition(glm::vec3(0.0f, 100.0f, -150.0f));
	mc->setAngle(glm::vec3(30.0f, 180.0f, 0.0f));

	Application::getInstance().getGLView()->setVsync(true);
}

void Voxel::Editor::onExit()
{
	Application::getInstance().getGLView()->setVsync(false);
}

void Voxel::Editor::onExitFinished()
{}

void Voxel::Editor::update(const float delta)
{
	if (canvas)
	{
		canvas->update(delta);

		updateMouseMove(delta);
		updateMousePress();
		updateMouseRelease();
	}

	// debug
	if (input->getKeyDown(GLFW_KEY_T, true))
	{
		if (slider)
		{
			slider->setPosition(0, 0);
		}
		else
		{
			slider = Voxel::UI::Slider::create("slider", "EditorUISpriteSheet", "dimension_slider_bar.png", "dimension_slider_button.png", Voxel::UI::Slider::Type::HORIZONTAL, 0, 100);
			canvas->addChild(slider);
		}
	}
	else if (input->getKeyDown(GLFW_KEY_Y, true))
	{
		slider->setValue(10.0f);
	}
	else if (input->getKeyDown(GLFW_KEY_U, true))
	{
		slider->setPosition(glm::vec2(50.0f, 100.0f));
	}
	else if (input->getKeyDown(GLFW_KEY_I, true))
	{
		slider->setScale(2.0f);
	}

	if (input->getKeyDown(GLFW_KEY_ESCAPE, true))
	{
		if (menuBarDropDowned)
		{
			fileDropDownBg->setVisibility(false);
			menuBarDropDowned = false;
		}
		else if(newCreateWindow->getVisibility())
		{
			onNewCancelButtonClicked();
		}
	}

	if (input->getKeyDown(GLFW_KEY_P, true))
	{
		// reset model pos
		floorPosition.x = 0.0f;
		floorPosition.y = 0.0f;
		floorPosition.z = 0.0f;
		floorModelMat = glm::rotate(glm::rotate(glm::translate(glm::mat4(1.0f), -floorPosition), glm::radians(floorAngleY), glm::vec3(0, 1, 0)), glm::radians(floorAngleX), glm::vec3(1, 0, 0));
	}
	else if (input->getKeyDown(GLFW_KEY_O, true))
	{
		//reset model orientation
		floorAngleX = 0.0f;
		floorAngleY = 0.0f;
		floorModelMat = glm::rotate(glm::rotate(glm::translate(glm::mat4(1.0f), -floorPosition), glm::radians(floorAngleY), glm::vec3(0, 1, 0)), glm::radians(floorAngleX), glm::vec3(1, 0, 0));
	}

	/*
	if (input->getKeyDown(GLFW_KEY_W))
	{
		Camera::mainCamera->addPosition(glm::vec3(0, 0, 3.0f) * delta);
		std::cout << "c pos = " << Utility::Log::vec3ToStr(Camera::mainCamera->getPosition()) << std::endl;
	}
	else if (input->getKeyDown(GLFW_KEY_S))
	{
		Camera::mainCamera->addPosition(glm::vec3(0, 0, -3.0f) * delta);
		std::cout << "c pos = " << Utility::Log::vec3ToStr(Camera::mainCamera->getPosition()) << std::endl;
	}
	else if (input->getKeyDown(GLFW_KEY_A))
	{
		Camera::mainCamera->addPosition(glm::vec3(3.0f, 0, 0.0f) * delta);
		std::cout << "c pos = " << Utility::Log::vec3ToStr(Camera::mainCamera->getPosition()) << std::endl;
	}
	else if (input->getKeyDown(GLFW_KEY_D))
	{
		Camera::mainCamera->addPosition(glm::vec3(-13.0f, 0, 0.0f) * delta);
		std::cout << "c pos = " << Utility::Log::vec3ToStr(Camera::mainCamera->getPosition()) << std::endl;
	}

	if (input->getKeyDown(GLFW_KEY_R))
	{
		Camera::mainCamera->addAngle(glm::vec3(-20.0f, 0.0f, 0.0f) * delta);
		std::cout << "c ang = " << Utility::Log::vec3ToStr(Camera::mainCamera->getAngle()) << std::endl;
	}
	else if (input->getKeyDown(GLFW_KEY_T))
	{
		Camera::mainCamera->addAngle(glm::vec3(20.0f, 0.0f, 0.0f) * delta);
		std::cout << "c ang = " << Utility::Log::vec3ToStr(Camera::mainCamera->getAngle()) << std::endl;
	}

	if (input->getKeyDown(GLFW_KEY_Q))
	{
		Camera::mainCamera->addAngle(glm::vec3(0.0f, -20.0f, 0.0f) * delta);
		std::cout << "c ang = " << Utility::Log::vec3ToStr(Camera::mainCamera->getAngle()) << std::endl;
	}
	else if (input->getKeyDown(GLFW_KEY_E))
	{
		Camera::mainCamera->addAngle(glm::vec3(0.0f, 20.0f, 0.0f) * delta);
		std::cout << "c ang = " << Utility::Log::vec3ToStr(Camera::mainCamera->getAngle()) << std::endl;
	}
	*/

	updateMouseScroll();
}

bool Voxel::Editor::updateMouseMove(const float delta)
{
	auto movedOnUI = false;

	if (canvas)
	{
		auto mouseMovedDist = input->getMouseMovedDistance();
		movedOnUI = canvas->updateMouseMove(cursor->getPosition(), glm::vec2(mouseMovedDist.x, -mouseMovedDist.y));
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
	else
	{
		if (mouseState == MouseState::CLICKED_RIGHT_BUTTON)
		{
			auto mouseMovedDist = input->getMouseMovedDistance();

			if (mouseMovedDist.x != 0.0f)
			{
				floorAngleY += (mouseMovedDist.x * 20.0f * delta);
			}

			if (mouseMovedDist.y != 0.0f)
			{
				floorAngleX += (mouseMovedDist.y * 20.0f * delta);
			}

			floorModelMat = glm::rotate(glm::rotate(glm::translate(glm::mat4(1.0f), -floorPosition), glm::radians(floorAngleY), glm::vec3(0, 1, 0)), glm::radians(floorAngleX), glm::vec3(1, 0, 0));
		}
		else if (mouseState == MouseState::CLICKED_MIDDLE_BUTTON)
		{
			auto mouseMovedDist = input->getMouseMovedDistance();

			if (mouseMovedDist.x != 0.0f)
			{
				floorPosition.x += (mouseMovedDist.x * 15.0f * delta);
			}
			
			if(mouseMovedDist.y != 0.0f)
			{
				floorPosition.z += (mouseMovedDist.y * 15.0f * delta);
			}

			//floorModelMat = glm::translate(glm::rotate(glm::mat4(1.0f), glm::radians(floorAngle), glm::vec3(0, 1, 0)), floorPosition);
			floorModelMat = glm::rotate(glm::rotate(glm::translate(glm::mat4(1.0f), -floorPosition), glm::radians(floorAngleY), glm::vec3(0, 1, 0)), glm::radians(floorAngleX), glm::vec3(1, 0, 0));
		}
		else
		{
			// raycast floor
		}
	}


	return movedOnUI;
}

bool Voxel::Editor::updateMousePress()
{
	bool pressedOnUI = false;

	if (canvas)
	{
		if (input->getMouseDown(GLFW_MOUSE_BUTTON_1, true))
		{
			pressedOnUI = canvas->updateMousePress(cursor->getPosition(), GLFW_MOUSE_BUTTON_1);
		}
	}

	if (!pressedOnUI)
	{
		// Didn't press mouse over ui
		if (mouseState == MouseState::IDLE)
		{
			if (input->getMouseDown(GLFW_MOUSE_BUTTON_2, true))
			{
				mouseState = MouseState::CLICKED_RIGHT_BUTTON;
			}
			else if (input->getMouseDown(GLFW_MOUSE_BUTTON_3, true))
			{
				mouseState = MouseState::CLICKED_MIDDLE_BUTTON;
			}
		}
	}

	return pressedOnUI;
}

bool Voxel::Editor::updateMouseRelease()
{
	bool releasedOnUI = false;

	if (canvas)
	{
		if (input->getMouseUp(GLFW_MOUSE_BUTTON_1, true))
		{
			releasedOnUI = canvas->updateMouseRelease(cursor->getPosition(), GLFW_MOUSE_BUTTON_1);
		}
	}

	if (input->getMouseUp(GLFW_MOUSE_BUTTON_2, true))
	{
		if (mouseState == MouseState::CLICKED_RIGHT_BUTTON)
		{
			mouseState = MouseState::IDLE;
		}
	}
	else if (input->getMouseUp(GLFW_MOUSE_BUTTON_3, true))
	{
		if (mouseState == MouseState::CLICKED_MIDDLE_BUTTON)
		{
			mouseState = MouseState::IDLE;
		}
	}

	return releasedOnUI;
}

void Voxel::Editor::updateMouseScroll()
{
	auto mouseScroll = input->getMouseScrollValue();

	if (mouseScroll == 1)
	{
		// zoom in
		zoomIn();
	}
	else if (mouseScroll == -1)
	{
		// zoom out
		zoomOut();
	}
}

void Voxel::Editor::zoomIn()
{
	if (zoomLevel < 5)
	{
		zoomLevel++;
		Camera::mainCamera->setPosition(Camera::mainCamera->getPosition() - glm::vec3(0.0f, 10.0f, -15.0f));
	}
}

void Voxel::Editor::zoomOut()
{
	if (zoomLevel > 0)
	{
		zoomLevel--;
		Camera::mainCamera->setPosition(Camera::mainCamera->getPosition() - glm::vec3(0.0f, -10.0f, 15.0f));
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

void Voxel::Editor::onNewButtonClicked()
{
	newCreateWindow->setVisibility(true);
	fileDropDownBg->setVisibility(false);
	menuBarDropDowned = false;
	fileBtn->disable();
	editBtn->disable();
	returnToMainMenuBtn->disable();
	exitGameBtn->disable();
	createBtn->disable();
}

void Voxel::Editor::onNewCreateButtonClicked()
{
	// create file.
	// reset input field
	// hide window
	newCreateWindow->setVisibility(false);

	fileBtn->enable();
	editBtn->enable();
	returnToMainMenuBtn->enable();
	exitGameBtn->enable();
	
	std::cout << "Create new file: " << newFileName << "\n";

	std::string rawName, ext;

	Utility::String::fileNameToNameAndExt(newFileName, rawName, ext);

	if (rawName == newFileName && ext.empty())
	{
		newFileName = rawName + ".schematic";
	}
	else if (ext != "schematic")
	{
		newFileName = newFileName + ".schematic";
	}

	std::cout << "Saving to \"" + newFileName + "\"\n";

	newFileNameInputField->setToDefaultText();
}

void Voxel::Editor::onNewCancelButtonClicked()
{
	// reset input field
	// hide window
	newCreateWindow->setVisibility(false);

	fileBtn->enable();
	editBtn->enable();
	returnToMainMenuBtn->enable();
	exitGameBtn->enable();

	newFileName = "";

	newFileNameInputField->setToDefaultText();
}

void Voxel::Editor::onNewFileNameEdit(const std::string text)
{
	if (text.empty() == false)
	{
		createBtn->enable();
	}
	else
	{
		createBtn->disable();
	}

	newFileName = text;
}

void Voxel::Editor::onNewFileNameEditSubmitted(const std::string text)
{
	newFileName = text;
	onNewCreateButtonClicked();
}

void Voxel::Editor::render()
{
	auto program = ProgramManager::getInstance().getProgram(ProgramManager::PROGRAM_NAME::POLYGON_SHADER);
	program->use(true);
	program->setUniformMat4("viewMat", Camera::mainCamera->getViewMat() * Camera::mainCamera->getWorldMat());
	program->setUniformMat4("modelMat", floorModelMat);
	program->setUniformVec4("color", floorColor);

	glBindVertexArray(floorVao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	auto lineProgram = ProgramManager::getInstance().getProgram(ProgramManager::PROGRAM_NAME::LINE_SHADER);
	lineProgram->use(true);
	lineProgram->setUniformMat4("viewMat", Camera::mainCamera->getViewMat() * Camera::mainCamera->getWorldMat());
	lineProgram->setUniformMat4("modelMat", glm::mat4(1.0f));
	glBindVertexArray(originLineVao);
	glDrawArrays(GL_LINES, 0, 2);

	glClear(GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_ALWAYS);

	if (canvas)
	{
		canvas->render();
	}
}