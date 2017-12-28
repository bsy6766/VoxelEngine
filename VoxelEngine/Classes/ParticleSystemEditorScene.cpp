// pch
#include "PreCompiled.h"

#include "ParticleSystemEditorScene.h"

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
#include "Schematic.h"
#include "DataTree.h"
#include "Color.h"

Voxel::ParticleSystemEditorScene::ParticleSystemEditorScene()
	: canvas(nullptr)
	, fileBtn(nullptr)
	, returnToMainMenuBtn(nullptr)
	, exitGameBtn(nullptr)
	, fileDropDownBg(nullptr)
	, saveBtn(nullptr)
	, saveAsBtn(nullptr)
	, newCreateWindow(nullptr)
	, createBtn(nullptr)
	, newFileNameInputField(nullptr)
	, overwriteWindow(nullptr)
	, overwritePrompt(nullptr)
	, openWindowNode(nullptr)
	, openConfirmBtn(nullptr)
	, openFileNameInputField(nullptr)
	, fpsLabel(nullptr)
	, workingParticleSystem(nullptr)
	, workingParticleSystemDataTree(nullptr)
	, particleSystemModified(false)
	, modifierNode(nullptr)
	, colorPickerNode(nullptr)
	, colorPicker(nullptr)
	, colorSlider(nullptr)
	, alphaSlider(nullptr)
	, startColorPreview(nullptr)
	, startColorVarPreview(nullptr)
	, endColorPreview(nullptr)
	, endColorVarPreview(nullptr)
	, emissionAreaImage(nullptr)
	, emissionAreaCB(nullptr)
	, emitPosXLine(nullptr)
	, emitPosYLine(nullptr)
	, emitPosLineNode(nullptr)
	, state(State::IDLE)
	, mouseState(MouseState::IDLE)
	, input(&Voxel::InputHandler::getInstance())
	, cursor(&Voxel::Cursor::getInstance())
{
}

Voxel::ParticleSystemEditorScene::~ParticleSystemEditorScene()
{
}

void Voxel::ParticleSystemEditorScene::init()
{
	Application::getInstance().getGLView()->setClearColor(glm::vec3(0.4375f));

	initUI();
}

void Voxel::ParticleSystemEditorScene::initUI()
{
	canvas = new Voxel::UI::Canvas(Application::getInstance().getGLView()->getScreenSize(), glm::vec2(0.0f));

	const auto ss = "EditorUISpriteSheet";

	auto& sm = SpriteSheetManager::getInstance();

	sm.addSpriteSheet("EditorUISpriteSheet.json");
	sm.addSpriteSheet("ParticleSpriteSheet.json");

	fpsLabel = Voxel::UI::Text::createWithOutline("fpsLabel", "FPS: 00000", 2);
	fpsLabel->setPivot(glm::vec2(-0.5f, -0.5f));
	fpsLabel->setCoordinateOrigin(glm::vec2(-0.5f, -0.5f));
	fpsLabel->setPosition(5.0f, 5.0f);
	canvas->addChild(fpsLabel, static_cast<int>(ZOrder::FPS_LABEL));

	initMenuBar();
	initFileDropDownMenu();
	initNewCreateWindow();
	initOverwriteWindow();
	initOpenWindow();
	initModifiers();
	initSpawnBoundaryImage();
	initEmitPosLine();

	canvas->print();
}

void Voxel::ParticleSystemEditorScene::initMenuBar()
{
	const auto ss = "EditorUISpriteSheet";

	auto menuBar = Voxel::UI::Image::createFromSpriteSheet("mb", ss, "menu_bar.png");
	menuBar->setScale(glm::vec2(Application::getInstance().getGLView()->getScreenSize().x, 1.0f));
	menuBar->setPivot(glm::vec2(-0.5f, 0.5f));
	menuBar->setCoordinateOrigin(glm::vec2(-0.5f, 0.5f));
	menuBar->setPosition(0.0f, 0.0f);
	menuBar->setInteractable();
	canvas->addChild(menuBar, static_cast<int>(ZOrder::MENU_BAR));

	fileBtn = Voxel::UI::Button::create("fBtn", ss, "file_button.png");
	fileBtn->setPosition(26.0f, -14.0f);
	fileBtn->setCoordinateOrigin(glm::vec2(-0.5f, 0.5f));
	fileBtn->setOnTriggeredCallbackFunc(std::bind(&ParticleSystemEditorScene::onFileButtonClicked, this, std::placeholders::_1));
	menuBar->addChild(fileBtn);
	
	exitGameBtn = Voxel::UI::Button::create("exitBtn", ss, "exit_game_button.png");
	exitGameBtn->setCoordinateOrigin(glm::vec2(0.5f, 0.5f));
	exitGameBtn->setPosition(glm::vec2(-26.0f, -14.0f));
	exitGameBtn->setOnTriggeredCallbackFunc(std::bind(&ParticleSystemEditorScene::onExitButtonClicked, this, std::placeholders::_1));
	menuBar->addChild(exitGameBtn);

	returnToMainMenuBtn = Voxel::UI::Button::create("rtmmBtn", ss, "return_to_main_menu_button.png");
	returnToMainMenuBtn->setCoordinateOrigin(glm::vec2(0.5f, 0.5f));
	returnToMainMenuBtn->setPosition(glm::vec2(-141.0f, -14.0f));
	returnToMainMenuBtn->setOnTriggeredCallbackFunc(std::bind(&ParticleSystemEditorScene::onReturnToMainMenuButtonClicked, this, std::placeholders::_1));
	menuBar->addChild(returnToMainMenuBtn);
}

void Voxel::ParticleSystemEditorScene::initFileDropDownMenu()
{
	const auto ss = "EditorUISpriteSheet";

	fileDropDownBg = Voxel::UI::NinePatchImage::create("fddBg", ss, "file_drop_down_bg.png", 2.0f, 2.0f, 2.0f, 2.0f, glm::vec2(116.0f, 108.0f));
	fileDropDownBg->setPivot(glm::vec2(-0.5f, 0.5f));
	fileDropDownBg->setCoordinateOrigin(glm::vec2(-0.5f, 0.5f));
	fileDropDownBg->setPosition(0.0f, -30.0f);
	fileDropDownBg->setVisibility(false);
	canvas->addChild(fileDropDownBg, static_cast<int>(ZOrder::FILE_DROP_DOWN));

	auto newBtn = Voxel::UI::Button::create("nBtn", ss, "file_new_button.png");
	newBtn->setCoordinateOrigin(glm::vec2(0.0f, 0.5f));
	newBtn->setPosition(0.0f, -14.0f);
	newBtn->setOnTriggeredCallbackFunc(std::bind(&ParticleSystemEditorScene::onNewButtonClicked, this, std::placeholders::_1));
	fileDropDownBg->addChild(newBtn);

	auto openBtn = Voxel::UI::Button::create("oBtn", ss, "file_open_button.png");
	openBtn->setCoordinateOrigin(glm::vec2(0.0f, 0.5f));
	openBtn->setPosition(0.0f, -40.0f);
	openBtn->setOnTriggeredCallbackFunc(std::bind(&ParticleSystemEditorScene::onOpenButtonClicked, this, std::placeholders::_1));
	fileDropDownBg->addChild(openBtn);

	saveBtn = Voxel::UI::Button::create("sBtn", ss, "file_save_button.png");
	saveBtn->setCoordinateOrigin(glm::vec2(0.0f, 0.5f));
	saveBtn->setPosition(0.0f, -66.0f);
	saveBtn->disable();
	fileDropDownBg->addChild(saveBtn);

	saveAsBtn = Voxel::UI::Button::create("saBtn", ss, "file_save_as_button.png");
	saveAsBtn->setCoordinateOrigin(glm::vec2(0.0f, 0.5f));
	saveAsBtn->setPosition(0.0f, -92.0f);
	saveAsBtn->disable();
	fileDropDownBg->addChild(saveAsBtn);
}

void Voxel::ParticleSystemEditorScene::initNewCreateWindow()
{
	const auto ss = "EditorUISpriteSheet";

	newCreateWindow = Voxel::UI::NinePatchImage::create("ncw", ss, "new_window_bg.png", 8.0f, 8.0f, 44.0f, 16.0f, glm::vec2(250.0f, 40.0f));
	newCreateWindow->setPosition(0.0f, 0.0f);
	newCreateWindow->setVisibility(false);
	canvas->addChild(newCreateWindow, static_cast<int>(ZOrder::WINDOWS));
	
	newFileNameInputField = Voxel::UI::InputField::create("nfIF", "Enter file name", "DebugSpriteSheet", 1, "debug_input_field_cursor.png");
	newFileNameInputField->setCoordinateOrigin(glm::vec2(-0.5f, 0.5f));
	newFileNameInputField->setPivot(-0.5f, 0.0f);
	newFileNameInputField->setPosition(8.0f, -33.0f);
	newFileNameInputField->setOnEditCallback(std::bind(&Voxel::ParticleSystemEditorScene::onNewFileNameEdit, this, std::placeholders::_1, std::placeholders::_2));
	newCreateWindow->addChild(newFileNameInputField);

	auto newLabel = Voxel::UI::Text::create("newLabel", "NEW", 1);
	newLabel->setCoordinateOrigin(glm::vec2(0.0f, 0.5f));
	newLabel->setPosition(0.0f, -9.0f);
	newCreateWindow->addChild(newLabel);

	createBtn = Voxel::UI::Button::create("cBtn", ss, "new_create_button.png");
	createBtn->setCoordinateOrigin(glm::vec2(-0.5f, -0.5f));
	createBtn->setPosition(35.0f, 15.0f);
	createBtn->setOnTriggeredCallbackFunc(std::bind(&ParticleSystemEditorScene::onNewCreateButtonClicked, this, std::placeholders::_1));
	createBtn->disable();
	newCreateWindow->addChild(createBtn);

	auto cancelBtn = Voxel::UI::Button::create("clBtn", ss, "new_cancel_button.png");
	cancelBtn->setCoordinateOrigin(glm::vec2(0.5f, -0.5f));
	cancelBtn->setPosition(-35.0f, 15.0f);
	cancelBtn->setOnTriggeredCallbackFunc(std::bind(&ParticleSystemEditorScene::onNewCancelButtonClicked, this, std::placeholders::_1));
	newCreateWindow->addChild(cancelBtn);
}

void Voxel::ParticleSystemEditorScene::initOverwriteWindow()
{
	overwriteWindow = Voxel::UI::NinePatchImage::create("owWin", "EditorUISpriteSheet", "overwrite_window_bg.png", 4, 4, 20, 4.0f, glm::vec2(162.0f, 83.0f));
	overwriteWindow->setVisibility(false);
	canvas->addChild(overwriteWindow, static_cast<int>(ZOrder::WINDOWS));
	
	auto title = Voxel::UI::Text::create("owWinTitle", "Overwrite?", 1);
	title->setCoordinateOrigin(glm::vec2(0.0f, 0.5f));
	title->setPosition(0.0f, -9.0f);
	overwriteWindow->addChild(title);

	overwritePrompt = Voxel::UI::Text::create("owPrompt", "File name\n" + newFileName + "\nalready exists.\nOverwrite?", 1);
	overwritePrompt->setPivot(-0.5f, 0.5f);
	overwritePrompt->setPosition(-80.0f, 34.0f);
	overwriteWindow->addChild(overwritePrompt);

	auto yesBtn = Voxel::UI::Button::create("owYes", "EditorUISpriteSheet", "yes_button.png");
	yesBtn->setPosition(-51.0f, -40.0f);
	yesBtn->setOnTriggeredCallbackFunc(std::bind(&ParticleSystemEditorScene::onOverwrite, this, std::placeholders::_1));
	overwriteWindow->addChild(yesBtn);

	auto noBtn = Voxel::UI::Button::create("owNo", "EditorUISpriteSheet", "no_button.png");
	noBtn->setPosition(51.0f, -40.0f);
	noBtn->setOnTriggeredCallbackFunc(std::bind(&ParticleSystemEditorScene::onOverwriteCancel, this, std::placeholders::_1));
	overwriteWindow->addChild(noBtn);
}

void Voxel::ParticleSystemEditorScene::initOpenWindow()
{
	const auto ss = "EditorUISpriteSheet";

	openWindowNode = Voxel::UI::Node::create("oWinNode");
	canvas->addChild(openWindowNode, static_cast<int>(ZOrder::WINDOWS));

	auto bg = Voxel::UI::NinePatchImage::create("oBg", ss, "new_window_bg.png", 8.0f, 8.0f, 44.0f, 16.0f, glm::vec2(250.0f, 40.0f));
	openWindowNode->addChild(bg);

	openFileNameInputField = Voxel::UI::InputField::create("nfIF", "Enter file name", "DebugSpriteSheet", 1, "debug_input_field_cursor.png");
	openFileNameInputField->setCoordinateOrigin(glm::vec2(-0.5f, 0.5f));
	openFileNameInputField->setPivot(-0.5f, 0.0f);
	openFileNameInputField->setPosition(8.0f, -33.0f);
	openFileNameInputField->setOnEditCallback(std::bind(&Voxel::ParticleSystemEditorScene::onNewFileNameEdit, this, std::placeholders::_1, std::placeholders::_2));
	bg->addChild(openFileNameInputField);

	auto openLabel = Voxel::UI::Text::create("oLabel", "open", 1);
	openLabel->setCoordinateOrigin(glm::vec2(0.0f, 0.5f));
	openLabel->setPosition(0.0f, -9.0f);
	bg->addChild(openLabel);

	openConfirmBtn = Voxel::UI::Button::create("cBtn", ss, "new_create_button.png");
	openConfirmBtn->setCoordinateOrigin(glm::vec2(-0.5f, -0.5f));
	openConfirmBtn->setPosition(35.0f, 15.0f);
	openConfirmBtn->setOnTriggeredCallbackFunc(std::bind(&ParticleSystemEditorScene::onOpenConfirmButtonClicked, this, std::placeholders::_1));
	openConfirmBtn->disable();
	bg->addChild(openConfirmBtn);

	auto cancelBtn = Voxel::UI::Button::create("clBtn", ss, "new_cancel_button.png");
	cancelBtn->setCoordinateOrigin(glm::vec2(0.5f, -0.5f));
	cancelBtn->setPosition(-35.0f, 15.0f);
	cancelBtn->setOnTriggeredCallbackFunc(std::bind(&ParticleSystemEditorScene::onOpenCancelButtonClicked, this, std::placeholders::_1));
	bg->addChild(cancelBtn);
}

void Voxel::ParticleSystemEditorScene::initModifiers()
{
	modifierNode = Voxel::UI::Node::create("modNode");
	modifierNode->setPosition(-957.0f, 382.0f);
	modifierNode->setVisibility(false);
	canvas->addChild(modifierNode, static_cast<int>(ZOrder::MOIFIER));
	
	float y = 0.0f;

	auto durSlider = initModifierSlider("dur", "duration: 000000", "duration: inf", -1.0f, 120.0f, -1.0f, y);
	durSlider->setOnValueChange(std::bind(&ParticleSystemEditorScene::onDurationChange, this, std::placeholders::_1));

	auto livingParticleLabel = Voxel::UI::Text::create("lpLabel", "Living particle: 00000", 1);
	livingParticleLabel->setPivot(-0.5f, 0.0f);
	livingParticleLabel->setPosition(0, y);
	modifierNode->addChild(livingParticleLabel);
	livingParticleLabel->setText("Living particle: 0");
	modifierLabels.push_back(livingParticleLabel);
	modifierSliders.push_back(nullptr);

	y -= 15.0f;

	auto tpSlider = initModifierSlider("ps", "total particles: 00000000", "total particles: 0", 0.0f, 2000.0f, 0.0f, y);
	tpSlider->setOnValueChange(std::bind(&ParticleSystemEditorScene::onTotalParticlesChange, this, std::placeholders::_1));

	auto emissionRateLabel = Voxel::UI::Text::create("erLabel", "Emission rate: 00000", 1);
	emissionRateLabel->setPivot(-0.5f, 0.0f);
	emissionRateLabel->setPosition(0, y);
	modifierNode->addChild(emissionRateLabel);
	emissionRateLabel->setText("Emission rate: 0");
	modifierLabels.push_back(emissionRateLabel);
	modifierSliders.push_back(nullptr);

	y -= 15.0f;

	auto pLifeSpanSlider = initModifierSlider("psLs", "particle life span: 000000", "particle life span: 0", 0.0f, 120.0f, 0.0f, y);
	pLifeSpanSlider->setOnValueChange(std::bind(&ParticleSystemEditorScene::onParticleLifeSpanChange, this, std::placeholders::_1));

	auto pLifeSpanVarSlider = initModifierSlider("psLsVar", "particle life span var: 000000", "particle life span var: 0", 0.0f, 120.0f, 0.0f, y);
	pLifeSpanVarSlider->setOnValueChange(std::bind(&ParticleSystemEditorScene::onParticleLifeSpanVarChange, this, std::placeholders::_1));

	auto spdSlider = initModifierSlider("spd", "speed: 000000", "speed: 0", 0.0f, 200.0f, 0.0f, y);
	spdSlider->setOnValueChange(std::bind(&ParticleSystemEditorScene::onSpeedChange, this, std::placeholders::_1));

	auto spdVSlider = initModifierSlider("spdV", "speed var: 000000", "speed var: 0", 0.0f, 200.0f, 0.0f, y);
	spdVSlider->setOnValueChange(std::bind(&ParticleSystemEditorScene::onSpeedVarChange, this, std::placeholders::_1));

	auto posLabel = Voxel::UI::Text::create("posLabel", "pos (00000, 00000)", 1);
	posLabel->setPivot(-0.5f, 0.0f);
	posLabel->setPosition(0, y);
	modifierNode->addChild(posLabel);
	posLabel->setText("pos (0, 0)");
	modifierLabels.push_back(posLabel);
	modifierSliders.push_back(nullptr);

	y -= 15.0f;

	auto posVarXSlider = initModifierSlider("posVx", "pos var x: 00000", "pos var x: 0", 0.0f, 960.0f, 0.0f, y);
	posVarXSlider->setOnValueChange(std::bind(&ParticleSystemEditorScene::onPosXVarChange, this, std::placeholders::_1));

	auto posVarYSlider = initModifierSlider("posVx", "pos var y: 00000", "pos var y: 0", 0.0f, 540.0f, 0.0f, y);
	posVarYSlider->setOnValueChange(std::bind(&ParticleSystemEditorScene::onPosYVarChange, this, std::placeholders::_1));

	auto gravityXSlider = initModifierSlider("gx", "gravity x: 000000", "gravity x: 0", -500.0f, 500.0f, 0.0f, y);
	gravityXSlider->setOnValueChange(std::bind(&ParticleSystemEditorScene::onGravityXChange, this, std::placeholders::_1));

	auto gravityYSlider = initModifierSlider("gy", "gravity y: 000000", "gravity y: 0", -500.0f, 500.0f, 0.0f, y);
	gravityYSlider->setOnValueChange(std::bind(&ParticleSystemEditorScene::onGravityYChange, this, std::placeholders::_1));

	auto emitAngleSlider = initModifierSlider("eAng", "emit angle: 000000", "emit angle: 0", -180.0f, 180.0f, 0.0f, y);
	emitAngleSlider->setOnValueChange(std::bind(&ParticleSystemEditorScene::onEmitAngleChange, this, std::placeholders::_1));

	auto emitAngleVarSlider = initModifierSlider("eAngV", "emit angle var: 000000", "emit angle var: 0", -180.0f, 180.0f, 0.0f, y);
	emitAngleVarSlider->setOnValueChange(std::bind(&ParticleSystemEditorScene::onEmitAngleVarChange, this, std::placeholders::_1));

	auto accelRadSlider = initModifierSlider("accRad", "radial accel: 000000", "radial accel: 0", -500.0f, 500.0f, 0.0f, y);
	accelRadSlider->setOnValueChange(std::bind(&ParticleSystemEditorScene::onAccelRadChange, this, std::placeholders::_1));

	auto accelRadVarSlider = initModifierSlider("accRadV", "radial accel var: 000000", "radial accel var: 0", -500.0f, 500.0f, 0.0f, y);
	accelRadVarSlider->setOnValueChange(std::bind(&ParticleSystemEditorScene::onAccelRadVarChange, this, std::placeholders::_1));

	auto accelTanSlider = initModifierSlider("accTan", "tangential accel: 000000", "tangential accel: 0", -500.0f, 500.0f, 0.0f, y);
	accelTanSlider->setOnValueChange(std::bind(&ParticleSystemEditorScene::onAccelTanChange, this, std::placeholders::_1));

	auto accelTanVarSlider = initModifierSlider("accTanV", "tangential accel var: 000000", "tangential accel var: 0", -500.0f, 500.0f, 0.0f, y);
	accelTanVarSlider->setOnValueChange(std::bind(&ParticleSystemEditorScene::onAccelTanVarChange, this, std::placeholders::_1));

	auto startSizeSlider = initModifierSlider("startSize", "Start size: 0000000", "start size: 32 px", 0, 256.0f, 32.0f, y);
	startSizeSlider->setOnValueChange(std::bind(&ParticleSystemEditorScene::onStartSizeChange, this, std::placeholders::_1));

	auto startSizeVarSlider = initModifierSlider("startSizeV", "Start size var: 0000000", "start size var: 32 px", 0, 256.0f, 0.0f, y);
	startSizeVarSlider->setOnValueChange(std::bind(&ParticleSystemEditorScene::onStartSizeVarChange, this, std::placeholders::_1));

	auto endSizeSlider = initModifierSlider("endSize", "End size: 0000000", "End size: 32 px", 0, 256.0f, 32.0f, y);
	endSizeSlider->setOnValueChange(std::bind(&ParticleSystemEditorScene::onEndSizeChange, this, std::placeholders::_1));

	auto endSizeVarSlider = initModifierSlider("startSize", "Start size: 0000000", "start size: 32 px", 0, 256.0f, 0.0f, y);
	endSizeVarSlider->setOnValueChange(std::bind(&ParticleSystemEditorScene::onEndSizeVarChange, this, std::placeholders::_1));

	auto startAngleSlider = initModifierSlider("startAngle", "Start angle: 0000", "start angle: 0", -180.0f, 180.0f, 0.0f, y);
	startAngleSlider->setOnValueChange(std::bind(&ParticleSystemEditorScene::onStartAngleChange, this, std::placeholders::_1));

	auto startAngleVarSlider = initModifierSlider("startAngleV", "Start angle var: 0000", "start angle var: 0", -180.0f, 180.0f, 0.0f, y);
	startAngleVarSlider->setOnValueChange(std::bind(&ParticleSystemEditorScene::onStartAngleVarChange, this, std::placeholders::_1));

	auto endAngleSlider = initModifierSlider("endAngle", "End angle: 0000", "End angle: 0", -180.0f, 180.0f, 0.0f, y);
	endAngleSlider->setOnValueChange(std::bind(&ParticleSystemEditorScene::onEndAngleChange, this, std::placeholders::_1));

	auto endAngleVarSlider = initModifierSlider("endAngleV", "End angle: 0000", "End angle var: 0", -180.0f, 180.0f, 0.0f, y);
	endAngleVarSlider->setOnValueChange(std::bind(&ParticleSystemEditorScene::onEndAngleVarChange, this, std::placeholders::_1));

	// color picker
	colorPickerNode = Voxel::UI::Node::create("cpNode");
	colorPickerNode->setPosition(721.0f, 371.0f);
	colorPickerNode->setVisibility(false);
	canvas->addChild(colorPickerNode, static_cast<int>(ZOrder::MOIFIER));

	auto cpBg = Voxel::UI::NinePatchImage::create("cpBg", "DebugSpriteSheet", "debug_color_picker_bg.png", 4.0f, 4.0f, 4.0f, 4.0f, glm::vec2(256.0f));
	colorPickerNode->addChild(cpBg);

	colorPicker= Voxel::UI::ColorPicker::create("cp", glm::vec2(256.0f), "DebugSpriteSheet", "debug_color_picker_icon.png");
	colorPicker->setOnValueChangeCallback(std::bind(&ParticleSystemEditorScene::onColorPickerValueChange, this, std::placeholders::_1));
	cpBg->addChild(colorPicker);

	colorSlider = Voxel::UI::Slider::create("cpSlider", "DebugSpriteSheet", "debug_color_slider.png", "debug_color_slider_button.png", Voxel::UI::Slider::Type::VERTICAL, 0.0f, 360.0f);
	colorSlider->setPosition(154.0f, 0.0f);
	colorSlider->setOnValueChange(std::bind(&ParticleSystemEditorScene::onColorSliderValueChange, this, std::placeholders::_1));
	colorPickerNode->addChild(colorSlider);

	alphaSlider = Voxel::UI::Slider::create("aSlider", "DebugSpriteSheet", "debug_alpha_slider.png", "debug_color_slider_button.png", Voxel::UI::Slider::Type::VERTICAL, 0.0f, 1.0f);
	alphaSlider->setPosition(190.0f, 0.0f);
	alphaSlider->setValue(1.0f);
	alphaSlider->setOnValueChange(std::bind(&ParticleSystemEditorScene::onAlphaSliderValueChange, this, std::placeholders::_1));
	colorPickerNode->addChild(alphaSlider);

	auto colorLabel = Voxel::UI::Text::create("cLabel", "RGBA (1.00, 1.00, 1.00, 1.00)", 1);
	colorLabel->setPivot(-0.5f, 0.0f);
	colorLabel->setPosition(-129.0f, -142.0f);
	colorLabel->setText("RGBA (0.5, 0.25, 0.25, 1.0)");
	colorPickerNode->addChild(colorLabel);
	modifierLabels.push_back(colorLabel);

	float colorY = -167.0f;
	const float buttonX = -113.0f;
	const float previewX = -80.0f;
	const float labelX = -62.0f;

	auto startColorBtn = Voxel::UI::Button::create("startCBtn", "DebugSpriteSheet", "debug_square_button.png");
	startColorBtn->setPosition(buttonX, colorY);
	startColorBtn->setOnTriggeredCallbackFunc(std::bind(&ParticleSystemEditorScene::onStartColorButtonTriggered, this, std::placeholders::_1));
	colorPickerNode->addChild(startColorBtn);

	startColorPreview = Voxel::UI::Image::createFromSpriteSheet("startColorPreview", "GlobalSpriteSheet", "26x26_white_square.png");
	startColorPreview->setPosition(previewX, colorY);
	colorPickerNode->addChild(startColorPreview);

	auto startColorLabel = Voxel::UI::Text::create("startCL", "start color (1.00, 1.00, 1.00, 1.00)", 1);
	startColorLabel->setPivot(-0.5f, 0.0f);
	startColorLabel->setPosition(labelX, colorY);
	colorPickerNode->addChild(startColorLabel);
	modifierLabels.push_back(startColorLabel);

	colorY -= 35.0f;

	auto startColorVarBtn = Voxel::UI::Button::create("startCVBtn", "DebugSpriteSheet", "debug_square_button.png");
	startColorVarBtn->setPosition(buttonX, colorY);
	startColorVarBtn->setOnTriggeredCallbackFunc(std::bind(&ParticleSystemEditorScene::onStartColorVarButtonTriggered, this, std::placeholders::_1));
	colorPickerNode->addChild(startColorVarBtn);

	startColorVarPreview = Voxel::UI::Image::createFromSpriteSheet("startColorVPreview", "GlobalSpriteSheet", "26x26_white_square.png");
	startColorVarPreview->setPosition(previewX, colorY);
	colorPickerNode->addChild(startColorVarPreview);

	auto startColorVarLabel = Voxel::UI::Text::create("startCVL", "start color var (1.00, 1.00, 1.00, 1.00)", 1);
	startColorVarLabel->setPivot(-0.5f, 0.0f);
	startColorVarLabel->setPosition(labelX, colorY);
	colorPickerNode->addChild(startColorVarLabel);
	modifierLabels.push_back(startColorVarLabel);

	colorY -= 35.0f;

	auto endColorBtn = Voxel::UI::Button::create("endCBtn", "DebugSpriteSheet", "debug_square_button.png");
	endColorBtn->setPosition(buttonX, colorY);
	endColorBtn->setOnTriggeredCallbackFunc(std::bind(&ParticleSystemEditorScene::onEndColorButtonTriggered, this, std::placeholders::_1));
	colorPickerNode->addChild(endColorBtn);

	endColorPreview = Voxel::UI::Image::createFromSpriteSheet("endColorPreview", "GlobalSpriteSheet", "26x26_white_square.png");
	endColorPreview->setPosition(previewX, colorY);
	colorPickerNode->addChild(endColorPreview);

	auto endColorLabel = Voxel::UI::Text::create("endCL", "end color (1.00, 1.00, 1.00, 1.00)", 1);
	endColorLabel->setPivot(-0.5f, 0.0f);
	endColorLabel->setPosition(labelX, colorY);
	colorPickerNode->addChild(endColorLabel);
	modifierLabels.push_back(endColorLabel);

	colorY -= 35.0f;

	auto endColorVarBtn = Voxel::UI::Button::create("endCVBtn", "DebugSpriteSheet", "debug_square_button.png");
	endColorVarBtn->setPosition(buttonX, colorY);
	endColorVarBtn->setOnTriggeredCallbackFunc(std::bind(&ParticleSystemEditorScene::onEndColorVarButtonTriggered, this, std::placeholders::_1));
	colorPickerNode->addChild(endColorVarBtn);

	endColorVarPreview = Voxel::UI::Image::createFromSpriteSheet("endColorVPreview", "GlobalSpriteSheet", "26x26_white_square.png");
	endColorVarPreview->setPosition(previewX, colorY);
	colorPickerNode->addChild(endColorVarPreview);

	auto endColorVarLabel = Voxel::UI::Text::create("endCVL", "end color var (1.00, 1.00, 1.00, 1.00)", 1);
	endColorVarLabel->setPivot(-0.5f, 0.0f);
	endColorVarLabel->setPosition(labelX, colorY);
	colorPickerNode->addChild(endColorVarLabel);
	modifierLabels.push_back(endColorVarLabel);
}

Voxel::UI::Slider* Voxel::ParticleSystemEditorScene::initModifierSlider(const std::string & name, const std::string & initLabel, const std::string & label, const float min, const float max, const float value, float& y)
{
	auto text = Voxel::UI::Text::create(name + "Label", initLabel, 1);
	text->setPivot(-0.5f, 0.0f);
	text->setPosition(0.0f, y);
	modifierNode->addChild(text);

	if (label.empty() == false)
	{
		text->setText(label);
	}

	modifierLabels.push_back(text);

	auto slider = Voxel::UI::Slider::create(name + "Slider", "EditorUISpriteSheet", "slider_bar.png", "slider_button.png", Voxel::UI::Slider::Type::HORIZONTAL, min, max);
	slider->setPivot(-0.5f, 0.0f);
	slider->setPosition(0.0f, y - 16.0f);
	modifierNode->addChild(slider);
	slider->setValue(value);
	modifierSliders.push_back(slider);

	y -= 33.0f;

	return slider;
}

void Voxel::ParticleSystemEditorScene::initSpawnBoundaryImage()
{
	emissionAreaImage = Voxel::UI::Image::createFromSpriteSheet("spwnBB", "GlobalSpriteSheet", "1x1_white.png");
	emissionAreaImage->setColor(glm::vec3(1.0f, 0.0f, 0.0f));
	emissionAreaImage->setOpacity(0.25f);
	emissionAreaImage->setVisibility(false);

	canvas->addChild(emissionAreaImage, static_cast<int>(ZOrder::EMIT_AREA));

	emissionAreaCB = Voxel::UI::CheckBox::create("spwnBtn", "EditorUISpriteSheet", "checkbox.png");
	emissionAreaCB->setPosition(469.0f, -524.0f);
	emissionAreaCB->setOnSelectedCallbackFunc(std::bind(&ParticleSystemEditorScene::onEmissionAreaCheckBoxSelected, this, std::placeholders::_1));
	emissionAreaCB->setOnDeselectedCallbackFunc(std::bind(&ParticleSystemEditorScene::onEmissionAreaCheckBoxDeselected, this, std::placeholders::_1));
	canvas->addChild(emissionAreaCB, static_cast<int>(ZOrder::CHECK_BOXES));

	auto label = Voxel::UI::Text::create("spwnCBLabel", "Show emission area", 1);
	label->setPosition(19.0f, 0.0f);
	label->setPivot(-0.5f, 0.0f);
	emissionAreaCB->addChild(label);
}

void Voxel::ParticleSystemEditorScene::initEmitPosLine()
{
	emitPosLineNode = Voxel::UI::Node::create("lineNode");
	emitPosLineNode->setVisibility(false);
	canvas->addChild(emitPosLineNode, static_cast<int>(ZOrder::GUIDE_LINES));

	emitPosXLine = Voxel::UI::Line::create("emitXLine", glm::vec2(-960.0f, 0.0f), glm::vec2(960.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
	emitPosLineNode->addChild(emitPosXLine);

	emitPosYLine = Voxel::UI::Line::create("emitYLine", glm::vec2(0.0f, -540.0f), glm::vec2(0.0f, 540.0f), glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
	emitPosLineNode->addChild(emitPosYLine);
}

void Voxel::ParticleSystemEditorScene::release()
{
	if (canvas)
	{
		delete canvas;
		canvas = nullptr;
	}

	auto& sm = SpriteSheetManager::getInstance();
	
	sm.removeSpriteSheetByKey("EditorUISpriteSheet");
	sm.removeSpriteSheetByKey("ParticleSpriteSheet");
}

void Voxel::ParticleSystemEditorScene::updateKey()
{
}

bool Voxel::ParticleSystemEditorScene::updateMouseMove(const float delta)
{
	if (mouseState == MouseState::SETTING_EMIT_POS)
	{
		updateEmissionPos();

		return true;
	}
	
	auto movedOnUI = false;

	if (canvas)
	{
		auto mouseMovedDist = input->getMouseMovedDistance();
		movedOnUI = canvas->updateMouseMove(cursor->getPosition(), glm::vec2(mouseMovedDist.x, -mouseMovedDist.y));
	}

	if (state == State::FILE_MENU_DROP_DOWN)
	{
		auto fileBtnBB = fileBtn->getBoundingBox();
		fileBtnBB.size.x += 2.0f;
		fileBtnBB.size.y += 8.0f;

		auto cp = cursor->getPosition();

		if (!fileDropDownBg->getBoundingBox().containsPoint(cp) && !fileBtnBB.containsPoint(cp))
		{
			fileDropDownBg->setVisibility(false);
			state = State::IDLE;	
		}
	}

	return movedOnUI;
}

bool Voxel::ParticleSystemEditorScene::updateMousePress()
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
		if (input->getMouseDown(GLFW_MOUSE_BUTTON_1, true))
		{
			if (workingParticleSystem)
			{
				if (state == State::IDLE && mouseState == MouseState::IDLE)
				{
					updateEmissionPos();

					mouseState = MouseState::SETTING_EMIT_POS;
				}
			}
		}
	}

	return pressedOnUI;
}

bool Voxel::ParticleSystemEditorScene::updateMouseRelease()
{
	if (mouseState == MouseState::SETTING_EMIT_POS)
	{
		if (input->getMouseUp(GLFW_MOUSE_BUTTON_1, true))
		{
			updateEmissionPos();

			mouseState = MouseState::IDLE;

			return true;
		}
	}

	bool releasedOnUI = false;

	if (canvas)
	{
		if (input->getMouseUp(GLFW_MOUSE_BUTTON_1, true))
		{
			releasedOnUI = canvas->updateMouseRelease(cursor->getPosition(), GLFW_MOUSE_BUTTON_1);
		}
	}

	return releasedOnUI;
}

void Voxel::ParticleSystemEditorScene::onEnter()
{
}

void Voxel::ParticleSystemEditorScene::onEnterFinished()
{
	//Application::getInstance().getGLView()->setVsync(true);
	Application::getInstance().getGLView()->onFPSCounted = std::bind(&ParticleSystemEditorScene::onFPSCount, this, std::placeholders::_1);
}

void Voxel::ParticleSystemEditorScene::onExit()
{
	Application::getInstance().getGLView()->setVsync(false);
	Application::getInstance().getGLView()->onFPSCounted = nullptr;
}

void Voxel::ParticleSystemEditorScene::onExitFinished()
{
}

void Voxel::ParticleSystemEditorScene::update(const float delta)
{
	if (canvas)
	{
		canvas->update(delta);

		updateKey();
		updateMouseMove(delta);
		updateMousePress();
		updateMouseRelease();

		if (workingParticleSystem)
		{
			modifierLabels.at(static_cast<int>(ModifierLabel::LIVING_PARTICLES))->setText("Living particles: " + std::to_string(workingParticleSystem->getLivingParticleNumber()));
		}
	}
}

void Voxel::ParticleSystemEditorScene::render()
{
	glClear(GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_ALWAYS);

	if (canvas)
	{
		canvas->render();
	}
}

void Voxel::ParticleSystemEditorScene::updateEmissionPos()
{
	auto cp = cursor->getPosition();
	workingParticleSystem->setEmitPosition(cp);

	modifierLabels.at(static_cast<int>(ModifierLabel::EMIT_POS))->setText("Pos (" + Utility::String::floatToStrTwoDPoints(cp.x) + ", " + Utility::String::floatToStrTwoDPoints(cp.y) + ")");

	emissionAreaImage->setPosition(cp);

	auto emitPos = workingParticleSystem->getEmissionPosition();

	emitPosXLine->setPosition(0.0f, emitPos.y);
	emitPosYLine->setPosition(emitPos.x, 0.0f);
}

void Voxel::ParticleSystemEditorScene::createEmptyParticleSystem()
{
	const std::string path = Voxel::FileSystem::getInstance().getWorkingDirectory() + "/Data/ParticleSystem/" + newFileName;

	workingParticleSystemDataTree = Voxel::DataTree::create(path);

	if (workingParticleSystemDataTree)
	{
		workingParticleSystemDataTree->setFloat("duration", -1);
		workingParticleSystemDataTree->setInt("totalParticles", 0);
		workingParticleSystemDataTree->setFloat("particleLifeSpan", 0.0f);
		workingParticleSystemDataTree->setFloat("particleLifeSpanVar", 0);
		workingParticleSystemDataTree->setInt("blendSrc", 0);
		workingParticleSystemDataTree->setInt("blendDest", 0);
		workingParticleSystemDataTree->setInt("position.x", 0);
		workingParticleSystemDataTree->setInt("position.y", 0);
		workingParticleSystemDataTree->setInt("positionVar.x", 0);
		workingParticleSystemDataTree->setInt("positionVar.y", 0);
		workingParticleSystemDataTree->setFloat("color.start.r", 1);
		workingParticleSystemDataTree->setFloat("color.start.g", 1);
		workingParticleSystemDataTree->setFloat("color.start.b", 1);
		workingParticleSystemDataTree->setFloat("color.start.a", 1);
		workingParticleSystemDataTree->setFloat("color.startVar.r", 0);
		workingParticleSystemDataTree->setFloat("color.startVar.g", 0);
		workingParticleSystemDataTree->setFloat("color.startVar.b", 0);
		workingParticleSystemDataTree->setFloat("color.startVar.a", 0);
		workingParticleSystemDataTree->setFloat("color.end.r", 1);
		workingParticleSystemDataTree->setFloat("color.end.g", 1);
		workingParticleSystemDataTree->setFloat("color.end.b", 1);
		workingParticleSystemDataTree->setFloat("color.end.a", 1);
		workingParticleSystemDataTree->setFloat("color.endVar.r", 0);
		workingParticleSystemDataTree->setFloat("color.endVar.g", 0);
		workingParticleSystemDataTree->setFloat("color.endVar.b", 0);
		workingParticleSystemDataTree->setFloat("color.endVar.a", 0);
		workingParticleSystemDataTree->setFloat("speed", 0);
		workingParticleSystemDataTree->setFloat("speedVar", 0);
		workingParticleSystemDataTree->setFloat("gravity.x", 0);
		workingParticleSystemDataTree->setFloat("gravity.y", 0);
		workingParticleSystemDataTree->setFloat("accel.tan", 0);
		workingParticleSystemDataTree->setFloat("accel.tanVar", 0);
		workingParticleSystemDataTree->setFloat("accel.rad", 0);
		workingParticleSystemDataTree->setFloat("accel.radVar", 0);
		workingParticleSystemDataTree->setFloat("emitAngle", 0);
		workingParticleSystemDataTree->setFloat("emitAngleVar", 0);
		workingParticleSystemDataTree->setFloat("size.start", 32);
		workingParticleSystemDataTree->setFloat("size.startVar", 0);
		workingParticleSystemDataTree->setFloat("size.end", 32);
		workingParticleSystemDataTree->setFloat("size.endVar", 0);
		workingParticleSystemDataTree->setFloat("angle.start", 0);
		workingParticleSystemDataTree->setFloat("angle.startVar", 0);
		workingParticleSystemDataTree->setFloat("angle.end", 0);
		workingParticleSystemDataTree->setFloat("angle.endVar", 0);
		workingParticleSystemDataTree->setString("spriteSheetName", "ParticleSpriteSheet");
		//workingParticleSystemDataTree->setString("textureName", "32x32_white_circle.png");
		workingParticleSystemDataTree->setString("textureName", "32x32_white_star.png");

		workingParticleSystemDataTree->save(path);

		workingParticleSystem = Voxel::UI::ParticleSystem::create("wps", "Data/ParticleSystem/" + newFileName);
		canvas->addChild(workingParticleSystem, static_cast<int>(ZOrder::WORKING_PARTICLE_SYSTEM));

		emissionAreaImage->setScale(0.0f);

		resetModifierLabelAndSlider();
	} 
	else
	{
		// Failed to create new file
		assert(false);
	}
}

bool Voxel::ParticleSystemEditorScene::attempToCreateFile()
{
	std::cout << "Attemp to create file: " << newFileName << "\n";

	// Check 
	std::cout << "fileName: \"" + newFileName + "\"\n";

	auto fs = &Voxel::FileSystem::getInstance();
	auto wd = fs->getWorkingDirectory();

	auto path = wd + "/Data/ParticleSystem/" + newFileName;

	if (fs->doesPathExists(path))
	{
		return false;
	}

	return true;
}

void Voxel::ParticleSystemEditorScene::askOverwrite()
{
	newCreateWindow->setVisibility(false);

	overwriteWindow->setVisibility(true);

	state = State::ASK_OVERWRITE;

	overwritePrompt->setText("File name\n" + newFileName + "\nalready exists.\nOverwrite?");
}

void Voxel::ParticleSystemEditorScene::resetModifierLabelAndSlider()
{// reset modifiers
		
		// duration
	modifierLabels.at(static_cast<int>(ModifierLabel::DURATION))->setText("Duration: INF");
	modifierSliders.at(static_cast<int>(ModifierLabel::DURATION))->setValue(-1.0f);

	// living particle
	modifierLabels.at(static_cast<int>(ModifierLabel::LIVING_PARTICLES))->setText("Living particleS: 0");

	// total particles
	modifierLabels.at(static_cast<int>(ModifierLabel::TOTAL_PARTICLES))->setText("Total particles: 0");

	// emit rate
	modifierLabels.at(static_cast<int>(ModifierLabel::EMISSION_RATE))->setText("Emission rate: 0");

	// particle life span
	modifierLabels.at(static_cast<int>(ModifierLabel::PARTICLE_LIFE_SPAN))->setText("Particle life span: 0");
	modifierSliders.at(static_cast<int>(ModifierLabel::PARTICLE_LIFE_SPAN))->setValue(0.0f);

	// particle life span var
	modifierLabels.at(static_cast<int>(ModifierLabel::PARTICLE_LIFE_SPAN_VAR))->setText("Particle life span: 0");
	modifierSliders.at(static_cast<int>(ModifierLabel::PARTICLE_LIFE_SPAN_VAR))->setValue(0.0f);

	// speed
	modifierLabels.at(static_cast<int>(ModifierLabel::SPEED))->setText("Speed: 0");
	modifierSliders.at(static_cast<int>(ModifierLabel::SPEED))->setValue(0.0f);

	// speed var
	modifierLabels.at(static_cast<int>(ModifierLabel::SPEED_VAR))->setText("Speed var: 0");
	modifierSliders.at(static_cast<int>(ModifierLabel::SPEED_VAR))->setValue(0.0f);

	// emit pos
	modifierLabels.at(static_cast<int>(ModifierLabel::EMIT_POS))->setText("Pos var (0, 0)");

	// emit pos var  x
	modifierLabels.at(static_cast<int>(ModifierLabel::EMIT_POS_X_VAR))->setText("Pos var x: 0");
	modifierSliders.at(static_cast<int>(ModifierLabel::EMIT_POS_X_VAR))->setValue(0.0f);

	// emit pos var  y
	modifierLabels.at(static_cast<int>(ModifierLabel::EMIT_POS_Y_VAR))->setText("Pos var y: 0");
	modifierSliders.at(static_cast<int>(ModifierLabel::EMIT_POS_Y_VAR))->setValue(0.0f);

	// gravity  x
	modifierLabels.at(static_cast<int>(ModifierLabel::GRAVITY_X))->setText("Gravity x: 0");
	modifierSliders.at(static_cast<int>(ModifierLabel::GRAVITY_X))->setValue(0.0f);

	// gravity  y
	modifierLabels.at(static_cast<int>(ModifierLabel::GRAVITY_Y))->setText("Gravity y: 0");
	modifierSliders.at(static_cast<int>(ModifierLabel::GRAVITY_Y))->setValue(0.0f);

	// emit angle
	modifierLabels.at(static_cast<int>(ModifierLabel::EMIT_ANGLE))->setText("Emit angle: 0");
	modifierSliders.at(static_cast<int>(ModifierLabel::EMIT_ANGLE))->setValue(0.0f);

	// emit angle var
	modifierLabels.at(static_cast<int>(ModifierLabel::EMIT_ANGLE_VAR))->setText("Emit angle var: 0");
	modifierSliders.at(static_cast<int>(ModifierLabel::EMIT_ANGLE_VAR))->setValue(0.0f);

	// radial accel
	modifierLabels.at(static_cast<int>(ModifierLabel::ACCEL_RAD))->setText("Radial accel: 0");
	modifierSliders.at(static_cast<int>(ModifierLabel::ACCEL_RAD))->setValue(0.0f);

	// radial accel var
	modifierLabels.at(static_cast<int>(ModifierLabel::ACCEL_RAD_VAR))->setText("Radial accel var: 0");
	modifierSliders.at(static_cast<int>(ModifierLabel::ACCEL_RAD_VAR))->setValue(0.0f);

	// tangential accel
	modifierLabels.at(static_cast<int>(ModifierLabel::ACCEL_TAN))->setText("Tangential accel: 0");
	modifierSliders.at(static_cast<int>(ModifierLabel::ACCEL_TAN))->setValue(0.0f);

	// tangential accel var
	modifierLabels.at(static_cast<int>(ModifierLabel::ACCEL_TAN_VAR))->setText("Tangential accel var: 0");
	modifierSliders.at(static_cast<int>(ModifierLabel::ACCEL_TAN_VAR))->setValue(0.0f);

	// start size
	modifierLabels.at(static_cast<int>(ModifierLabel::START_SIZE))->setText("Start size: 32 px");
	modifierSliders.at(static_cast<int>(ModifierLabel::START_SIZE))->setValue(0.0f);

	// start size var
	modifierLabels.at(static_cast<int>(ModifierLabel::START_SIZE_VAR))->setText("Start size var: 0");
	modifierSliders.at(static_cast<int>(ModifierLabel::START_SIZE_VAR))->setValue(0.0f);

	// end size
	modifierLabels.at(static_cast<int>(ModifierLabel::END_SIZE))->setText("End size: 32 px");
	modifierSliders.at(static_cast<int>(ModifierLabel::END_SIZE))->setValue(0.0f);

	// end size var
	modifierLabels.at(static_cast<int>(ModifierLabel::END_SIZE_VAR))->setText("End size var: 0");
	modifierSliders.at(static_cast<int>(ModifierLabel::END_SIZE_VAR))->setValue(0.0f);

	// start angle
	modifierLabels.at(static_cast<int>(ModifierLabel::START_ANGLE))->setText("Start angle: 0");
	modifierSliders.at(static_cast<int>(ModifierLabel::START_ANGLE))->setValue(0.0f);

	// start angle var
	modifierLabels.at(static_cast<int>(ModifierLabel::START_ANGLE_VAR))->setText("Start angle var: 0");
	modifierSliders.at(static_cast<int>(ModifierLabel::START_ANGLE_VAR))->setValue(0.0f);

	// end angle
	modifierLabels.at(static_cast<int>(ModifierLabel::END_ANGLE))->setText("End angle: 0");
	modifierSliders.at(static_cast<int>(ModifierLabel::END_ANGLE))->setValue(0.0f);

	// end angle var
	modifierLabels.at(static_cast<int>(ModifierLabel::END_ANGLE_VAR))->setText("End angle var: 0");
	modifierSliders.at(static_cast<int>(ModifierLabel::END_ANGLE_VAR))->setValue(0.0f);

	// color picker
	colorPicker->setHSB(glm::vec3(0.0f, 0.5f, 0.5f));
	colorSlider->setValue(0.0f);
	alphaSlider->setValue(1.0f);
	modifierLabels.at(static_cast<int>(ModifierLabel::COLOR_PICKER))->setText("RGBA (1.00, 1.00, 1.00, 1.00)");

	// color
	modifierLabels.at(static_cast<int>(ModifierLabel::START_COLOR))->setText("Start color (1.00, 1.00, 1.00, 1.00)");
	modifierLabels.at(static_cast<int>(ModifierLabel::START_COLOR_VAR))->setText("Start color var (1.00, 1.00, 1.00, 1.00)");
	modifierLabels.at(static_cast<int>(ModifierLabel::END_COLOR))->setText("End color (1.00, 1.00, 1.00, 1.00)");
	modifierLabels.at(static_cast<int>(ModifierLabel::END_COLOR_VAR))->setText("End color var(1.00, 1.00, 1.00, 1.00)");
}

void Voxel::ParticleSystemEditorScene::removeExistingWorkingParticleSystem()
{
	if (workingParticleSystem)
	{
		canvas->removeChild(workingParticleSystem->getID(), true);
		workingParticleSystem = nullptr;
	}

	if (workingParticleSystemDataTree)
	{
		delete workingParticleSystemDataTree;
		workingParticleSystemDataTree = nullptr;
	}
}

void Voxel::ParticleSystemEditorScene::onFPSCount(int fps)
{
	if (fpsLabel)
	{
		fpsLabel->setText("FPS: " + std::to_string(fps));
	}
}

void Voxel::ParticleSystemEditorScene::onFileButtonClicked(Voxel::UI::Button * sender)
{
	fileDropDownBg->setVisibility(true);
	state = State::FILE_MENU_DROP_DOWN;
}

void Voxel::ParticleSystemEditorScene::onReturnToMainMenuButtonClicked(Voxel::UI::Button * sender)
{
	Application::getInstance().getDirector()->replaceScene(Voxel::Director::SceneName::MENU_SCENE, 0.5f);
}

void Voxel::ParticleSystemEditorScene::onExitButtonClicked(Voxel::UI::Button * sender)
{
	Application::getInstance().end();
}

void Voxel::ParticleSystemEditorScene::onNewButtonClicked(Voxel::UI::Button * sender)
{
	newCreateWindow->setVisibility(true);
	fileDropDownBg->setVisibility(false);
	fileBtn->disable();
	returnToMainMenuBtn->disable();
	exitGameBtn->disable();
	createBtn->disable();

	newFileNameInputField->setToDefaultText();

	if (workingParticleSystem)
	{
		modifierNode->setNonInteractable();
		colorPickerNode->setNonInteractable();
	}

	state = State::CREATE_NEW_FILE;
}

void Voxel::ParticleSystemEditorScene::onOpenButtonClicked(Voxel::UI::Button * sender)
{
	state = State::OPEN_FILE;

	openWindowNode->setVisibility(true);
	fileDropDownBg->setVisibility(false);
	fileBtn->disable();
	returnToMainMenuBtn->disable();
	exitGameBtn->disable();
	createBtn->disable();

	if (workingParticleSystem)
	{
		modifierNode->setNonInteractable();
		colorPickerNode->setNonInteractable();
	}
}

void Voxel::ParticleSystemEditorScene::onNewCancelButtonClicked(Voxel::UI::Button * sender)
{
	newCreateWindow->setVisibility(false);

	fileBtn->enable();
	returnToMainMenuBtn->enable();
	exitGameBtn->enable();

	newFileName = "";

	newFileNameInputField->setToDefaultText();

	state = State::IDLE;	
	
	if (workingParticleSystem)
	{
		modifierNode->setInteractable();
		colorPickerNode->setInteractable();
	}
}

void Voxel::ParticleSystemEditorScene::onNewFileNameEdit(Voxel::UI::InputField * sender, const std::string text)
{
	if (sender->isTextDefault())
	{
		createBtn->disable();
	}
	else
	{
		if (text.empty())
		{
			createBtn->disable();
		}
		else
		{
			createBtn->enable();
		}
	}

	newFileName = text;
}

void Voxel::ParticleSystemEditorScene::onNewCreateButtonClicked(Voxel::UI::Button * sender)
{
	if (workingParticleSystem != nullptr && particleSystemModified)
	{
		// ask to save current file
		state = State::ASK_SAVE_MODIFIED_FILE;

	}
	else
	{
		bool result = attempToCreateFile();

		if (result)
		{
			// create new file
			createEmptyParticleSystem();

			particleSystemModified = true;

			newCreateWindow->setVisibility(false);

			fileBtn->enable();
			returnToMainMenuBtn->enable();
			exitGameBtn->enable();
			saveBtn->enable();
			saveAsBtn->enable();
			
			newFileNameInputField->setToDefaultText();

			modifierNode->setVisibility(true);
			colorPickerNode->setVisibility(true);
			emitPosLineNode->setVisibility(true);

			state = State::IDLE;
		}
		else
		{
			// same file exists. overwrite?
			state = State::ASK_OVERWRITE;
			
			askOverwrite();
		}
	}
}

void Voxel::ParticleSystemEditorScene::onOpenFileNameEdit(Voxel::UI::InputField * sender, const std::string text)
{
	if (sender->isTextDefault())
	{
		openConfirmBtn->disable();
	}
	else
	{
		if (text.empty())
		{
			openConfirmBtn->disable();
		}
		else
		{
			openConfirmBtn->enable();
		}
	}

	newFileName = text;
}

void Voxel::ParticleSystemEditorScene::onOpenConfirmButtonClicked(Voxel::UI::Button * sender)
{
}

void Voxel::ParticleSystemEditorScene::onOpenCancelButtonClicked(Voxel::UI::Button * sender)
{
}

void Voxel::ParticleSystemEditorScene::onOverwrite(Voxel::UI::Button * sender)
{
	overwriteWindow->setVisibility(false);

	auto fs = &Voxel::FileSystem::getInstance();

	const std::string path = Voxel::FileSystem::getInstance().getWorkingDirectory() + "/Data/ParticleSystem/" + newFileName;
	fs->deleteFile(path);

	createEmptyParticleSystem();

	fileBtn->enable();
	returnToMainMenuBtn->enable();
	exitGameBtn->enable();
	saveBtn->enable();
	saveAsBtn->enable();

	newFileNameInputField->setToDefaultText();

	modifierNode->setVisibility(true);
	colorPickerNode->setVisibility(true);
	emitPosLineNode->setVisibility(true);

	state = State::IDLE;
}

void Voxel::ParticleSystemEditorScene::onOverwriteCancel(Voxel::UI::Button * sender)
{
	overwriteWindow->setVisibility(false);

	fileBtn->enable();
	returnToMainMenuBtn->enable();
	exitGameBtn->enable();

	newFileName = "";

	newFileNameInputField->setToDefaultText();

	state = State::IDLE;
}

void Voxel::ParticleSystemEditorScene::onEmissionAreaCheckBoxSelected(Voxel::UI::CheckBox * sender)
{
	emissionAreaImage->setVisibility(true);
}

void Voxel::ParticleSystemEditorScene::onEmissionAreaCheckBoxDeselected(Voxel::UI::CheckBox * sender)
{
	emissionAreaImage->setVisibility(false);
}

void Voxel::ParticleSystemEditorScene::onDurationChange(Voxel::UI::Slider * sender)
{
	float value = sender->getValue();

	if (value < 0.0f)
	{
		workingParticleSystem->setDuration(-1.0f);
		modifierLabels.at(static_cast<int>(ModifierLabel::DURATION))->setText("Duration: Inf");
	}
	else if (value == 0.0f)
	{
		workingParticleSystem->setDuration(0.0f);
		modifierLabels.at(static_cast<int>(ModifierLabel::DURATION))->setText("Duration: 0.0");
	}
	else
	{
		const float newDuration = glm::floor(value * 100.0f) * 0.01f;
		const std::string newDurStr = std::to_string(newDuration);
		modifierLabels.at(static_cast<int>(ModifierLabel::DURATION))->setText("Duration: " + newDurStr.substr(0, (newDurStr).find_first_of('.') + 3));
		workingParticleSystem->setDuration(newDuration);
	}

}

void Voxel::ParticleSystemEditorScene::onTotalParticlesChange(Voxel::UI::Slider * sender)
{
	const int tp = static_cast<int>(sender->getValue());
	workingParticleSystem->setTotalParticles(tp);

	modifierLabels.at(static_cast<int>(ModifierLabel::TOTAL_PARTICLES))->setText("Total particles: " + std::to_string(tp));

	modifierLabels.at(static_cast<int>(ModifierLabel::EMISSION_RATE))->setText("Emission rate: " + Utility::String::floatToStrTwoDPoints(workingParticleSystem->getEmissionRate()));
}

void Voxel::ParticleSystemEditorScene::onParticleLifeSpanChange(Voxel::UI::Slider * sender)
{
	const float newLifeSpan = Utility::Math::floorTwoDecimalPoint(sender->getValue());
	workingParticleSystem->setParticleLifeSpan(newLifeSpan);

	modifierLabels.at(static_cast<int>(ModifierLabel::PARTICLE_LIFE_SPAN))->setText("Particle life span: " + Utility::String::floatToStrTwoDPoints(newLifeSpan));

	modifierLabels.at(static_cast<int>(ModifierLabel::EMISSION_RATE))->setText("Emission rate: " + Utility::String::floatToStrTwoDPoints(workingParticleSystem->getEmissionRate()));
}

void Voxel::ParticleSystemEditorScene::onParticleLifeSpanVarChange(Voxel::UI::Slider * sender)
{
	const float newLSVar = Utility::Math::floorTwoDecimalPoint(sender->getValue());
	workingParticleSystem->setParticleLifeSpanVar(newLSVar);

	modifierLabels.at(static_cast<int>(ModifierLabel::PARTICLE_LIFE_SPAN_VAR))->setText("Particle life span var: " + Utility::String::floatToStrTwoDPoints(newLSVar));

	modifierLabels.at(static_cast<int>(ModifierLabel::EMISSION_RATE))->setText("Emission rate: " + Utility::String::floatToStrTwoDPoints(workingParticleSystem->getEmissionRate()));
}

void Voxel::ParticleSystemEditorScene::onSpeedChange(Voxel::UI::Slider * sender)
{
	const float speed = Utility::Math::floorTwoDecimalPoint(sender->getValue());
	workingParticleSystem->setSpeed(speed);

	modifierLabels.at(static_cast<int>(ModifierLabel::SPEED))->setText("Speed: " + Utility::String::floatToStrTwoDPoints(speed));
}

void Voxel::ParticleSystemEditorScene::onSpeedVarChange(Voxel::UI::Slider * sender)
{
	const float speed = Utility::Math::floorTwoDecimalPoint(sender->getValue());
	workingParticleSystem->setSpeedVar(speed);

	modifierLabels.at(static_cast<int>(ModifierLabel::SPEED_VAR))->setText("Speed var: " + Utility::String::floatToStrTwoDPoints(speed));
}

void Voxel::ParticleSystemEditorScene::onPosXVarChange(Voxel::UI::Slider * sender)
{
	const float x = Utility::Math::floorTwoDecimalPoint(sender->getValue());
	workingParticleSystem->setEmitPosXVar(x);

	modifierLabels.at(static_cast<int>(ModifierLabel::EMIT_POS_X_VAR))->setText("Pos var x: " + Utility::String::floatToStrTwoDPoints(x));

	auto emitPosX = workingParticleSystem->getEmissionPosition().x;
	auto emitPosVarX = workingParticleSystem->getEmissionPositionVar().x;
	
	emissionAreaImage->setScaleX(glm::abs((emitPosX + emitPosVarX) - (emitPosX - emitPosVarX)));
}

void Voxel::ParticleSystemEditorScene::onPosYVarChange(Voxel::UI::Slider * sender)
{
	const float y = Utility::Math::floorTwoDecimalPoint(sender->getValue());
	workingParticleSystem->setEmitPosYVar(y);

	modifierLabels.at(static_cast<int>(ModifierLabel::EMIT_POS_Y_VAR))->setText("Pos var y: " + Utility::String::floatToStrTwoDPoints(y));	

	auto emitPosY = workingParticleSystem->getEmissionPosition().y;
	auto emitPosVarY = workingParticleSystem->getEmissionPositionVar().y;

	emissionAreaImage->setScaleY(glm::abs((emitPosY + emitPosVarY) - (emitPosY - emitPosVarY)));
}

void Voxel::ParticleSystemEditorScene::onGravityXChange(Voxel::UI::Slider * sender)
{
	const float gx = Utility::Math::floorTwoDecimalPoint(sender->getValue());
	workingParticleSystem->setGravityX(gx);

	modifierLabels.at(static_cast<int>(ModifierLabel::GRAVITY_X))->setText("Gravity x: " + Utility::String::floatToStrTwoDPoints(gx));
}

void Voxel::ParticleSystemEditorScene::onGravityYChange(Voxel::UI::Slider * sender)
{
	const float gy = Utility::Math::floorTwoDecimalPoint(sender->getValue());
	workingParticleSystem->setGravityY(gy);

	modifierLabels.at(static_cast<int>(ModifierLabel::GRAVITY_Y))->setText("Gravity y: " + Utility::String::floatToStrTwoDPoints(gy));
}

void Voxel::ParticleSystemEditorScene::onEmitAngleChange(Voxel::UI::Slider * sender)
{
	const float angle = Utility::Math::floorTwoDecimalPoint(sender->getValue());
	workingParticleSystem->setEmitAngle(angle);

	modifierLabels.at(static_cast<int>(ModifierLabel::EMIT_ANGLE))->setText("Emit angle: " + Utility::String::floatToStrTwoDPoints(angle));
}

void Voxel::ParticleSystemEditorScene::onEmitAngleVarChange(Voxel::UI::Slider * sender)
{
	const float angle = Utility::Math::floorTwoDecimalPoint(sender->getValue());
	workingParticleSystem->setEmitAngleVar(angle);

	modifierLabels.at(static_cast<int>(ModifierLabel::EMIT_ANGLE_VAR))->setText("Emit angle var: " + Utility::String::floatToStrTwoDPoints(angle));
}

void Voxel::ParticleSystemEditorScene::onAccelRadChange(Voxel::UI::Slider * sender)
{
	const float accelRad = Utility::Math::floorTwoDecimalPoint(sender->getValue());
	workingParticleSystem->setAccelRad(accelRad);

	modifierLabels.at(static_cast<int>(ModifierLabel::ACCEL_RAD))->setText("Accel rad: " + Utility::String::floatToStrTwoDPoints(accelRad));
}

void Voxel::ParticleSystemEditorScene::onAccelRadVarChange(Voxel::UI::Slider * sender)
{
	const float accelRad = Utility::Math::floorTwoDecimalPoint(sender->getValue());
	workingParticleSystem->setAccelRadVar(accelRad);

	modifierLabels.at(static_cast<int>(ModifierLabel::ACCEL_RAD_VAR))->setText("Accel rad var: " + Utility::String::floatToStrTwoDPoints(accelRad));
}

void Voxel::ParticleSystemEditorScene::onAccelTanChange(Voxel::UI::Slider * sender)
{
	const float accelTan = Utility::Math::floorTwoDecimalPoint(sender->getValue());
	workingParticleSystem->setAccelTan(accelTan);

	modifierLabels.at(static_cast<int>(ModifierLabel::ACCEL_TAN))->setText("Accel tan: " + Utility::String::floatToStrTwoDPoints(accelTan));
}

void Voxel::ParticleSystemEditorScene::onAccelTanVarChange(Voxel::UI::Slider * sender)
{
	const float accelTanVar = Utility::Math::floorTwoDecimalPoint(sender->getValue());
	workingParticleSystem->setAccelTanVar(accelTanVar);

	modifierLabels.at(static_cast<int>(ModifierLabel::ACCEL_TAN_VAR))->setText("Accel tan var: " + Utility::String::floatToStrTwoDPoints(accelTanVar));
}

void Voxel::ParticleSystemEditorScene::onStartSizeChange(Voxel::UI::Slider * sender)
{
	const float size = Utility::Math::floorTwoDecimalPoint(sender->getValue());
	workingParticleSystem->setStartSize(size);

	modifierLabels.at(static_cast<int>(ModifierLabel::START_SIZE))->setText("Start size: " + Utility::String::floatToStrTwoDPoints(size) + " px");
}

void Voxel::ParticleSystemEditorScene::onStartSizeVarChange(Voxel::UI::Slider * sender)
{
	const float size = Utility::Math::floorTwoDecimalPoint(sender->getValue());
	workingParticleSystem->setStartSizeVar(size);

	modifierLabels.at(static_cast<int>(ModifierLabel::START_SIZE_VAR))->setText("Start size var: " + Utility::String::floatToStrTwoDPoints(size) + " px");
}

void Voxel::ParticleSystemEditorScene::onEndSizeChange(Voxel::UI::Slider * sender)
{
	const float size = Utility::Math::floorTwoDecimalPoint(sender->getValue());
	workingParticleSystem->setEndSize(size);

	modifierLabels.at(static_cast<int>(ModifierLabel::END_SIZE))->setText("End size: " + Utility::String::floatToStrTwoDPoints(size) + " px");
}

void Voxel::ParticleSystemEditorScene::onEndSizeVarChange(Voxel::UI::Slider * sender)
{
	const float size = Utility::Math::floorTwoDecimalPoint(sender->getValue());
	workingParticleSystem->setEndSizeVar(size);

	modifierLabels.at(static_cast<int>(ModifierLabel::END_SIZE_VAR))->setText("Ebd size var: " + Utility::String::floatToStrTwoDPoints(size) + " px");
}

void Voxel::ParticleSystemEditorScene::onStartAngleChange(Voxel::UI::Slider * sender)
{
	const float angle = Utility::Math::floorTwoDecimalPoint(sender->getValue());
	workingParticleSystem->setStartAngle(angle);

	modifierLabels.at(static_cast<int>(ModifierLabel::START_ANGLE))->setText("Start angle: " + Utility::String::floatToStrTwoDPoints(angle));
}

void Voxel::ParticleSystemEditorScene::onStartAngleVarChange(Voxel::UI::Slider * sender)
{
	const float angle = Utility::Math::floorTwoDecimalPoint(sender->getValue());
	workingParticleSystem->setStartAngleVar(angle);

	modifierLabels.at(static_cast<int>(ModifierLabel::START_ANGLE_VAR))->setText("Start angle var: " + Utility::String::floatToStrTwoDPoints(angle));
}

void Voxel::ParticleSystemEditorScene::onEndAngleChange(Voxel::UI::Slider * sender)
{
	const float angle = Utility::Math::floorTwoDecimalPoint(sender->getValue());
	workingParticleSystem->setEndAngle(angle);

	modifierLabels.at(static_cast<int>(ModifierLabel::END_ANGLE))->setText("End angle: " + Utility::String::floatToStrTwoDPoints(angle));
}

void Voxel::ParticleSystemEditorScene::onEndAngleVarChange(Voxel::UI::Slider * sender)
{
	const float angle = Utility::Math::floorTwoDecimalPoint(sender->getValue());
	workingParticleSystem->setEndAngleVar(angle);

	modifierLabels.at(static_cast<int>(ModifierLabel::END_ANGLE_VAR))->setText("End angle var: " + Utility::String::floatToStrTwoDPoints(angle));
}

void Voxel::ParticleSystemEditorScene::onColorPickerValueChange(Voxel::UI::ColorPicker * sender)
{
	auto rgb = sender->getRGB();
	modifierLabels.at(static_cast<int>(ModifierLabel::COLOR_PICKER))->setText("RGBA (" + Utility::String::floatToStrTwoDPoints(rgb.r) + ", " + Utility::String::floatToStrTwoDPoints(rgb.g) + ", " + Utility::String::floatToStrTwoDPoints(rgb.b) + ", " + Utility::String::floatToStrTwoDPoints(alphaSlider->getValue()) + ")");
}

void Voxel::ParticleSystemEditorScene::onColorSliderValueChange(Voxel::UI::Slider * sender)
{
	colorPicker->setH(sender->getValue());

	auto rgb = colorPicker->getRGB();
	modifierLabels.at(static_cast<int>(ModifierLabel::COLOR_PICKER))->setText("RGBA (" + Utility::String::floatToStrTwoDPoints(rgb.r) + ", " + Utility::String::floatToStrTwoDPoints(rgb.g) + ", " + Utility::String::floatToStrTwoDPoints(rgb.b) + ", " + Utility::String::floatToStrTwoDPoints(alphaSlider->getValue()) + ")");
}

void Voxel::ParticleSystemEditorScene::onAlphaSliderValueChange(Voxel::UI::Slider * sender)
{
	auto rgb = colorPicker->getRGB();
	modifierLabels.at(static_cast<int>(ModifierLabel::COLOR_PICKER))->setText("RGBA (" + Utility::String::floatToStrTwoDPoints(rgb.r) + ", " + Utility::String::floatToStrTwoDPoints(rgb.g) + ", " + Utility::String::floatToStrTwoDPoints(rgb.b) + ", " + Utility::String::floatToStrTwoDPoints(sender->getValue()) + ")");
}

void Voxel::ParticleSystemEditorScene::onStartColorButtonTriggered(Voxel::UI::Button * sender)
{
	auto rgb = colorPicker->getRGB();

	startColorPreview->setColor(rgb);

	float a = alphaSlider->getValue();
	modifierLabels.at(static_cast<int>(ModifierLabel::START_COLOR))->setText("start color (" + Utility::String::floatToStrTwoDPoints(rgb.r) + ", " + Utility::String::floatToStrTwoDPoints(rgb.g) + ", " + Utility::String::floatToStrTwoDPoints(rgb.b) + ", " + Utility::String::floatToStrTwoDPoints(a) + ")");

	workingParticleSystem->setStartColor(glm::vec4(rgb, a));
}

void Voxel::ParticleSystemEditorScene::onStartColorVarButtonTriggered(Voxel::UI::Button * sender)
{
	auto rgb = colorPicker->getRGB();

	startColorVarPreview->setColor(rgb);

	float a = alphaSlider->getValue();
	modifierLabels.at(static_cast<int>(ModifierLabel::START_COLOR_VAR))->setText("start color var (" + Utility::String::floatToStrTwoDPoints(rgb.r) + ", " + Utility::String::floatToStrTwoDPoints(rgb.g) + ", " + Utility::String::floatToStrTwoDPoints(rgb.b) + ", " + Utility::String::floatToStrTwoDPoints(a) + ")");

	workingParticleSystem->setStartColorVar(glm::vec4(rgb, a));
}

void Voxel::ParticleSystemEditorScene::onEndColorButtonTriggered(Voxel::UI::Button * sender)
{
	auto rgb = colorPicker->getRGB();

	endColorPreview->setColor(rgb);

	float a = alphaSlider->getValue();
	modifierLabels.at(static_cast<int>(ModifierLabel::END_COLOR))->setText("end color (" + Utility::String::floatToStrTwoDPoints(rgb.r) + ", " + Utility::String::floatToStrTwoDPoints(rgb.g) + ", " + Utility::String::floatToStrTwoDPoints(rgb.b) + ", " + Utility::String::floatToStrTwoDPoints(a) + ")");

	workingParticleSystem->setEndColor(glm::vec4(rgb, a));
}

void Voxel::ParticleSystemEditorScene::onEndColorVarButtonTriggered(Voxel::UI::Button * sender)
{
	auto rgb = colorPicker->getRGB();

	endColorVarPreview->setColor(rgb);

	float a = alphaSlider->getValue();
	modifierLabels.at(static_cast<int>(ModifierLabel::END_COLOR_VAR))->setText("end color var (" + Utility::String::floatToStrTwoDPoints(rgb.r) + ", " + Utility::String::floatToStrTwoDPoints(rgb.g) + ", " + Utility::String::floatToStrTwoDPoints(rgb.b) + ", " + Utility::String::floatToStrTwoDPoints(a) + ")");

	workingParticleSystem->setEndColorVar(glm::vec4(rgb, a));
}
