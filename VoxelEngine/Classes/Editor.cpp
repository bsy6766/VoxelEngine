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
#include "Ray.h"
#include "Quad.h"
#include "FileSystem.h"

// cpp
#include <functional>

// glm
#include <glm\gtx\transform.hpp>
#include <glm/gtx/compatibility.hpp>

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
	, dimensionXSlider(nullptr)
	, dimensionYSlider(nullptr)
	, dimensionZSlider(nullptr)
	, dimensionLabel(nullptr)
	, dimensionXLabel(nullptr)
	, dimensionYLabel(nullptr)
	, dimensionZLabel(nullptr)
	, dimension(0)
	, schematic(nullptr)
	, schematicState(SchematicState::NONE)
	, schematicModified(false)
	, floorVao(0)
	, floorAngleX(0)
	, floorAngleY(0)
	, floorPosition(0.0f)
	, floorSize(0.0f)
	, floorModelMat(1.0f)
	, floorColor(glm::vec4(142.0f / 255.0f, 1.0f, 237.0f / 255.0f, 0.75f))
	, axisLineVao(0)
	, faceIndicatorVao(0)
	, faceIndicatorPos(0.0f)
	, faceIndicatorCubeFace(Voxel::Cube::Face::NONE)
	, faceIndicatorVisibility(false)
	, faceIndicatorModelMat(1.0f)
	, curFaceIndicatorPos(0)
	, faceIndicatorColor(glm::vec4(1.0f, 0.57f, 0.0f, 1.0f))
	, mouseState(MouseState::IDLE)
	, zoomLevel(0)
	, fpsLabel(nullptr)
	, camPosTarget(0.0f)
	, defaultCamPos(0.0f)
	, editState(EditState::NONE)
{}

Editor::~Editor()
{
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
	initFaceIndicator();
}

void Voxel::Editor::initFloor()
{
	releaseFloor();

	floorSize = glm::vec2(dimension.x, dimension.z) * 0.5f;

	std::vector<float> vertices =
	{
		-floorSize.x, 0.0f, -floorSize.y,
		-floorSize.x, 0.0f, floorSize.y,
		floorSize.x, 0.0f, -floorSize.y,
		floorSize.x, 0.0f, floorSize.y,
	};

	std::vector<unsigned int> indices =
	{
		0, 1, 2, 1, 2, 3,
	};

	// gen vao
	glGenVertexArrays(1, &floorVao);
	glBindVertexArray(floorVao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices.front(), GL_STATIC_DRAW);

	// Enable vertices attrib
	auto program = ProgramManager::getInstance().getProgram(ProgramManager::PROGRAM_NAME::POLYGON_SHADER);
	GLint vertLoc = program->getAttribLocation("vert");

	// vert
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices.front(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
}

void Voxel::Editor::initFaceIndicator()
{
	if (faceIndicatorVao)
	{
		glDeleteVertexArrays(1, &faceIndicatorVao);
		faceIndicatorVao = 0;
	}

	glGenVertexArrays(1, &faceIndicatorVao);
	glBindVertexArray(faceIndicatorVao);

	const float indicatorSize = 0.5f;

	std::vector<float> vertices =
	{
		-indicatorSize, 0.0f, -indicatorSize,
		-indicatorSize, 0.0f, indicatorSize,
		indicatorSize, 0.0f, -indicatorSize,
		indicatorSize, 0.0f, indicatorSize,
	};

	std::vector<unsigned int> indices =
	{
		0, 1, 2, 1, 2, 3,
	};

	auto program = ProgramManager::getInstance().getProgram(ProgramManager::PROGRAM_NAME::POLYGON_SHADER);
	GLint vertLoc = program->getAttribLocation("vert");

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices.front(), GL_STATIC_DRAW);

	// vert
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices.front(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
}

void Voxel::Editor::initAxisGuide()
{
	const glm::vec3 size = glm::vec3(dimension);

	std::vector<float> vertices =
	{
		// y, green
		0.0f, size.x, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		// x, red
		size.y, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		// z, blue
		0.0f, 0.0f, size.z, 0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
	};

	// gen vao
	glGenVertexArrays(1, &axisLineVao);
	glBindVertexArray(axisLineVao);

	// Generate buffer object
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices.front(), GL_STATIC_DRAW);

	auto lineProgram = ProgramManager::getInstance().getProgram(ProgramManager::PROGRAM_NAME::LINE_SHADER);
	GLint vertLoc = lineProgram->getAttribLocation("vert");
	GLint colorLoc = lineProgram->getAttribLocation("color");

	// vert
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), nullptr);

	// color
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));

	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);
}

void Voxel::Editor::initUI()
{
	canvas = new Voxel::UI::Canvas(Application::getInstance().getGLView()->getScreenSize(), glm::vec2(0.0f));

	const auto ss = "EditorUISpriteSheet";

	auto& sm = SpriteSheetManager::getInstance();

	sm.addSpriteSheet("EditorUISpriteSheet.json");

	fpsLabel = Voxel::UI::Text::createWithOutline("fpsLabel", "FPS: 00000", 2);
	fpsLabel->setPivot(glm::vec2(-0.5f, -0.5f));
	fpsLabel->setCoordinateOrigin(glm::vec2(-0.5f, -0.5f));
	fpsLabel->setPosition(5.0f, 5.0f);
	canvas->addChild(fpsLabel);

	initMenuBar();
	initFileDropDownMenu();
	initNewCreateWindow();
}

void Voxel::Editor::initMenuBar()
{
	const auto ss = "EditorUISpriteSheet";

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
}

void Voxel::Editor::initFileDropDownMenu()
{
	const auto ss = "EditorUISpriteSheet";
	
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
	saveBtn->disable();
	fileDropDownBg->addChild(saveBtn);

	auto saveAsBtn = Voxel::UI::Button::create("saBtn", ss, "file_save_as_button.png");
	saveAsBtn->setCoordinateOrigin(glm::vec2(0.0f, 0.5f));
	saveAsBtn->setPosition(0.0f, -92.0f);
	saveAsBtn->disable();
	fileDropDownBg->addChild(saveAsBtn);
}

void Voxel::Editor::initNewCreateWindow()
{
	const auto ss = "EditorUISpriteSheet";

	newCreateWindow = Voxel::UI::NinePatchImage::create("ncw", ss, "new_window_bg.png", 8.0f, 8.0f, 44.0f, 16.0f, glm::vec2(250.0f, 80.0f));
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

	dimensionLabel = Voxel::UI::Text::create("dimLabel", "Dimension", 1);
	dimensionLabel->setCoordinateOrigin(glm::vec2(-0.5f, 0.5f));
	dimensionLabel->setPivot(-0.5f, 0.0f);
	dimensionLabel->setPosition(8.0f, -52.0f);
	newCreateWindow->addChild(dimensionLabel);

	dimensionXLabel = Voxel::UI::Text::create("dimXLabel", "X: 000", 1);
	dimensionXLabel->setCoordinateOrigin(glm::vec2(-0.5f, 0.5f));
	dimensionXLabel->setPosition(12.0f, -66.0f);
	dimensionXLabel->setPivot(-0.5f, 0.0f);
	newCreateWindow->addChild(dimensionXLabel);

	dimensionYLabel = Voxel::UI::Text::create("dimYLabel", "Y: 000", 1);
	dimensionYLabel->setCoordinateOrigin(glm::vec2(-0.5f, 0.5f));
	dimensionYLabel->setPosition(12.0f, -84.0f);
	dimensionYLabel->setPivot(-0.5f, 0.0f);
	newCreateWindow->addChild(dimensionYLabel);

	dimensionZLabel = Voxel::UI::Text::create("dimZLabel", "Z: 000", 1);
	dimensionZLabel->setCoordinateOrigin(glm::vec2(-0.5f, 0.5f));
	dimensionZLabel->setPosition(12.0f, -102.0f);
	dimensionZLabel->setPivot(-0.5f, 0.0f);
	newCreateWindow->addChild(dimensionZLabel);

	dimensionXSlider = Voxel::UI::Slider::create("dimXSlider", ss, "dimension_slider_bar.png", "dimension_slider_button.png", Voxel::UI::Slider::Type::HORIZONTAL, 1, 100);
	dimensionXSlider->setCoordinateOrigin(glm::vec2(-0.5f, 0.5f));
	dimensionXSlider->setPosition(154.0f, -66.0f);
	dimensionXSlider->setOnSliderMove(std::bind(&Editor::onDimensionXSliderMove, this, std::placeholders::_1));
	newCreateWindow->addChild(dimensionXSlider);

	dimensionYSlider = Voxel::UI::Slider::create("dimYSlider", ss, "dimension_slider_bar.png", "dimension_slider_button.png", Voxel::UI::Slider::Type::HORIZONTAL, 1, 100);
	dimensionYSlider->setCoordinateOrigin(glm::vec2(-0.5f, 0.5f));
	dimensionYSlider->setPosition(154.0f, -84.0f);
	dimensionYSlider->setOnSliderMove(std::bind(&Editor::onDimensionYSliderMove, this, std::placeholders::_1));
	newCreateWindow->addChild(dimensionYSlider);

	dimensionZSlider = Voxel::UI::Slider::create("dimZSlider", ss, "dimension_slider_bar.png", "dimension_slider_button.png", Voxel::UI::Slider::Type::HORIZONTAL, 1, 100);
	dimensionZSlider->setCoordinateOrigin(glm::vec2(-0.5f, 0.5f));
	dimensionZSlider->setPosition(154.0f, -102.0f);
	dimensionZSlider->setOnSliderMove(std::bind(&Editor::onDimensionZSliderMove, this, std::placeholders::_1));
	newCreateWindow->addChild(dimensionZSlider);
}

void Voxel::Editor::releaseFloor()
{
	if (floorVao)
	{
		glDeleteVertexArrays(1, &floorVao);
		floorVao = 0;
		floorSize = glm::vec2(0.0f);
		floorModelMat = glm::mat4(1.0f);
		floorAngleX = 0.0f;
		floorAngleY = 0.0f;
		floorPosition = glm::vec3(0.0f);
	}
}

void Voxel::Editor::releaseAxisGuide()
{
	if (axisLineVao)
	{
		glDeleteVertexArrays(1, &axisLineVao);
		axisLineVao = 0;
	}
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

	releaseFloor();

	releaseAxisGuide();

	if (faceIndicatorVao)
	{
		glDeleteVertexArrays(1, &faceIndicatorVao);
		faceIndicatorVao = 0;
	}
}

void Voxel::Editor::onEnter()
{}

void Voxel::Editor::onEnterFinished()
{
	// reset camera
	Camera* mc = Camera::mainCamera;

	defaultCamPos = camPosTarget = glm::vec3(0.0f, 100.0f, 100.0f / (glm::tan(glm::radians(-30.0f))));
	mc->setPosition(camPosTarget);
	mc->setAngle(glm::vec3(30.0f, 180.0f, 0.0f));

	Application::getInstance().getGLView()->setVsync(true);
	Application::getInstance().getGLView()->onFPSCounted = std::bind(&Editor::onFPSCount, this, std::placeholders::_1);
}

void Voxel::Editor::onExit()
{
	Application::getInstance().getGLView()->setVsync(false);
	Application::getInstance().getGLView()->onFPSCounted = nullptr;
}

void Voxel::Editor::onExitFinished()
{}

void Voxel::Editor::update(const float delta)
{
	if (canvas)
	{
		canvas->update(delta);

		updateKey();
		updateMouseMove(delta);
		updateMousePress();
		updateMouseRelease();
	}

	auto camPos = Camera::mainCamera->getPosition();
	if (camPos != camPosTarget)
	{
		auto newPos = glm::lerp(camPos, camPosTarget, 10.0f * delta);
		if (glm::abs(glm::distance(newPos, camPosTarget)) <= 0.05f)
		{
			newPos = camPosTarget;
		}

		Camera::mainCamera->setPosition(newPos);
	}
	
	updateMouseScroll();
}

void Voxel::Editor::updateKey()
{
	// debug
	if (input->getKeyDown(GLFW_KEY_T, true))
	{
	}
	else if (input->getKeyDown(GLFW_KEY_Y, true))
	{
	}
	else if (input->getKeyDown(GLFW_KEY_U, true))
	{
	}
	else if (input->getKeyDown(GLFW_KEY_I, true))
	{
	}

	if (input->getKeyDown(GLFW_KEY_ESCAPE, true))
	{
		if (menuBarDropDowned)
		{
			fileDropDownBg->setVisibility(false);
			menuBarDropDowned = false;
		}
		else if (newCreateWindow->getVisibility())
		{
			onNewCancelButtonClicked();
		}
	}

	if (input->getKeyDown(GLFW_KEY_KP_5, true))
	{
		// view top
		floorAngleX = 60.0f;
		floorAngleY = 0.0f;
		floorPosition.x = 0.0f;
		floorPosition.y = 0.0f;
		floorPosition.z = 0.0f;
		updateFloorModelMat();
	}
	else if (input->getKeyDown(GLFW_KEY_KP_2, true))
	{
		// view front
		floorAngleX = -30.0f;
		floorAngleY = 0.0f;
		floorPosition.x = 0.0f;
		floorPosition.y = 0.0f;
		floorPosition.z = 0.0f;
		updateFloorModelMat();
	}
	else if (input->getKeyDown(GLFW_KEY_KP_8, true))
	{
		// view back
		floorAngleX = -30.0f;
		floorAngleY = 180.0f;
		floorPosition.x = 0.0f;
		floorPosition.y = 0.0f;
		floorPosition.z = 0.0f;
		updateFloorModelMat();
	}
	else if (input->getKeyDown(GLFW_KEY_KP_4, true))
	{
		// view left
		floorAngleX = -30.0f;
		floorAngleY = 270.0f;
		floorPosition.x = 0.0f;
		floorPosition.y = 0.0f;
		floorPosition.z = 0.0f;
		updateFloorModelMat();
	}
	else if (input->getKeyDown(GLFW_KEY_KP_6, true))
	{
		// view right
		floorAngleX = -30.0f;
		floorAngleY = 90.0f;
		floorPosition.x = 0.0f;
		floorPosition.y = 0.0f;
		floorPosition.z = 0.0f;
		updateFloorModelMat();
	}
	else if (input->getKeyDown(GLFW_KEY_KP_ENTER, true))
	{
		// reset model pos
		floorPosition.x = 0.0f;
		floorPosition.y = 0.0f;
		floorPosition.z = 0.0f;
		updateFloorModelMat();
	}
	else if (input->getKeyDown(GLFW_KEY_KP_0, true))
	{
		//reset model orientation
		floorAngleX = 0.0f;
		floorAngleY = 0.0f;
		updateFloorModelMat();
	}
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

			updateFloorModelMat();
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

			updateFloorModelMat();
		}
		else
		{
			// raycast floor
			glm::vec3 intersectingPoint;
			bool result = raycastFloor(intersectingPoint);

			if (result)
			{
				//std::cout << "Intersecting: " << Utility::Log::vec3ToStr(intersectingPoint) << "\n";
				faceIndicatorCubeFace = Cube::Face::TOP;
				faceIndicatorVisibility = true;

				curFaceIndicatorPos = intersectingFloorPointToCoordinate(intersectingPoint);

				std::cout << "coordinate: " << Utility::Log::vec3ToStr(curFaceIndicatorPos) << "\n";

				updateFaceIndicatorModelMat();
			}
			else
			{
				faceIndicatorVisibility = false;
			}
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
	if (zoomLevel < 9)
	{
		zoomLevel++;
		camPosTarget -= (defaultCamPos * 0.1f);
	}
}

void Voxel::Editor::zoomOut()
{
	if (zoomLevel > 0)
	{
		zoomLevel--;
		if (zoomLevel == 0)
		{
			// calibrate
			camPosTarget = defaultCamPos;
		}
		else
		{
			camPosTarget += (defaultCamPos * 0.1f);
		}
	}
}

bool Voxel::Editor::raycastFloor(glm::vec3 & intersectingPoint)
{
	// raycast floor
	auto screenSize = glm::vec2(Application::getInstance().getGLView()->getScreenSize());

	glm::mat4 proj = Camera::mainCamera->getProjection();

	glm::mat4 mat = Camera::mainCamera->getViewMat() *Camera::mainCamera->getWorldMat() *  floorModelMat;

	auto openglXY = cursor->getPosition() + (screenSize * 0.5f);

	auto near = glm::unProject(glm::vec3(openglXY.x, openglXY.y, 0.0f), mat, proj, glm::vec4(0, 0, 1920, 1080));

	auto far = glm::unProject(glm::vec3(openglXY.x, openglXY.y, 1.0f), mat, proj, glm::vec4(0, 0, 1920, 1080));

	Ray ray(near, far);
	
	return ray.doesIntersectsQuad(Shape::Quad(glm::vec3(-floorSize.x, 0.0f, -floorSize.y), glm::vec3(-floorSize.x, 0.0f, floorSize.y), glm::vec3(floorSize.x, 0.0f, -floorSize.y), glm::vec3(floorSize.x, 0.0f, floorSize.y), glm::vec3(0, 1, 0)), intersectingPoint);
}

void Voxel::Editor::updateFloorModelMat()
{
	floorModelMat = glm::translate(glm::mat4(1.0f), -floorPosition) * glm::rotate(glm::mat4(1.0f), glm::radians(-floorAngleX), glm::vec3(1, 0, 0)) * glm::rotate(glm::mat4(1.0f), glm::radians(floorAngleY), glm::vec3(0, 1, 0));
}

void Voxel::Editor::updateFaceIndicatorModelMat()
{
	switch (faceIndicatorCubeFace)
	{
	case Voxel::Cube::FRONT:
		break;
	case Voxel::Cube::LEFT:
		break;
	case Voxel::Cube::BACK:
		break;
	case Voxel::Cube::RIGHT:
		break;
	case Voxel::Cube::TOP:
		if (curFaceIndicatorPos.y == -1)
		{
			// floor
			faceIndicatorModelMat = glm::translate(glm::mat4(1.0f), glm::vec3(curFaceIndicatorPos.x, 0.0f, curFaceIndicatorPos.z));
		}
		else
		{
			faceIndicatorModelMat = glm::translate(glm::mat4(1.0f), glm::vec3(curFaceIndicatorPos));
		}
		break;
	case Voxel::Cube::BOTTOM:
		break;
	case Voxel::Cube::ALL:
	case Voxel::Cube::NONE:
	default:
		break;
	}

	faceIndicatorModelMat = (glm::rotate(glm::mat4(1.0f), glm::radians(-floorAngleX), glm::vec3(1, 0, 0)) * glm::rotate(glm::mat4(1.0f), glm::radians(floorAngleY), glm::vec3(0, 1, 0))) * faceIndicatorModelMat;
}

glm::ivec3 Voxel::Editor::intersectingFloorPointToCoordinate(const glm::vec3 & intersectingPoint)
{
	glm::vec3 shift = intersectingPoint;
	shift.x += 0.5f;
	shift.z += 0.5f;
	
	glm::ivec3 result = glm::ivec3(shift.x, -1.0, shift.z);

	if (result.x < 0)
	{
		result.x -= 1;
	}

	if (result.z < 0)
	{
		result.z -= 1;
	}

	return result;
}

bool Voxel::Editor::attempToCreateFile()
{
	std::cout << "Attemp to create file: " << newFileName << "\n";

	// Get raw name and file ext.
	std::string rawName, ext;

	Utility::String::fileNameToNameAndExt(newFileName, rawName, ext);

	if (rawName == newFileName && ext.empty())
	{
		// Didn't provided file ext.
		newFileName = rawName + ".schematic";
	}
	else if (ext != "schematic")
	{
		// file ext wrong.
		newFileName = newFileName + ".schematic";
	}

	// Check 
	std::cout << "fileName: \"" + newFileName + "\"\n";

	auto fs = &Voxel::FileSystem::getInstance();
	auto wd = fs->getWorkingDirectory();

	auto path = wd + "/Schematics/" + newFileName;

	if (fs->doesPathExists(path))
	{
		return false;
	}

	return true;
}

void Voxel::Editor::askOverwrite()
{
}

void Voxel::Editor::onFPSCount(int fps)
{
	if (fpsLabel)
	{
		fpsLabel->setText("FPS: " + std::to_string(fps));
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

	dimensionXLabel->setText("X: 1");
	dimensionYLabel->setText("Y: 1");
	dimensionZLabel->setText("Z: 1");

	dimensionXSlider->setValue(1);
	dimensionYSlider->setValue(1);
	dimensionZSlider->setValue(1);
}

void Voxel::Editor::onNewCreateButtonClicked()
{
	// Attempt to create new file.
	bool result = attempToCreateFile();

	if (result)
	{
		// Can create file

		newCreateWindow->setVisibility(false);

		// set button states
		fileBtn->enable();
		editBtn->enable();
		returnToMainMenuBtn->enable();
		exitGameBtn->enable();


		// reset inputfield
		newFileNameInputField->setToDefaultText();


		// dimension
		dimension.x = static_cast<int>(dimensionXSlider->getValue());
		dimension.y = static_cast<int>(dimensionYSlider->getValue());
		dimension.z = static_cast<int>(dimensionZSlider->getValue());

		initFloor();
		initAxisGuide();
	}
	else
	{
		// duplicate exist
		askOverwrite();
	}
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

void Voxel::Editor::onDimensionXSliderMove(float value)
{
	dimensionXLabel->setText("X: " + std::to_string((int)value));
}

void Voxel::Editor::onDimensionYSliderMove(float value)
{
	dimensionYLabel->setText("Y: " + std::to_string((int)value));
}

void Voxel::Editor::onDimensionZSliderMove(float value)
{
	dimensionZLabel->setText("Z: " + std::to_string((int)value));
}

void Voxel::Editor::render()
{
	if (axisLineVao)
	{
		auto lineProgram = ProgramManager::getInstance().getProgram(ProgramManager::PROGRAM_NAME::LINE_SHADER);
		lineProgram->use(true);
		lineProgram->setUniformMat4("viewMat", Camera::mainCamera->getViewMat() * Camera::mainCamera->getWorldMat());
		lineProgram->setUniformMat4("modelMat", floorModelMat);

		glBindVertexArray(axisLineVao);
		glDrawArrays(GL_LINES, 0, 6);
	}

	auto program = ProgramManager::getInstance().getProgram(ProgramManager::PROGRAM_NAME::POLYGON_SHADER);
	program->use(true);

	if (floorVao)
	{
		program->setUniformMat4("viewMat", Camera::mainCamera->getViewMat() * Camera::mainCamera->getWorldMat());
		program->setUniformMat4("modelMat", floorModelMat);
		program->setUniformVec4("color", floorColor);

		glBindVertexArray(floorVao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	if (faceIndicatorVisibility)
	{
		glClear(GL_DEPTH_BUFFER_BIT);
		program->setUniformMat4("modelMat", faceIndicatorModelMat);
		program->setUniformVec4("color", faceIndicatorColor);

		glBindVertexArray(faceIndicatorVao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	glClear(GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_ALWAYS);

	if (canvas)
	{
		canvas->render();
	}
}