// pch
#include "PreCompiled.h"

#include "UITestScene.h"

// voxel
#include "Application.h"
#include "UIActions.h"
#include "Director.h"
#include "Cursor.h"
#include "InputHandler.h"

using namespace Voxel::UI;

Voxel::UITestScene::UITestScene()
	: Scene()
	, input(&InputHandler::getInstance())
	, cursor(&Cursor::getInstance())
	, fpsLabel(nullptr)
	, canvas(nullptr)
	, uiInfo(nullptr)
	, cp(nullptr)
	, cpSlider(nullptr)
{}

Voxel::UITestScene::~UITestScene()
{
}

void Voxel::UITestScene::init()
{
	auto res = Voxel::Application::getInstance().getGLView()->getScreenSize();

	Application::getInstance().getGLView()->setClearColor(glm::vec3(226.0f / 255.0f, 0.0f, 1.0f));

	canvas = new Canvas(res, glm::vec2(0.0f));

	// title
	auto title = Text::create("title", "UI TESTS", 1);
	title->setPosition(-861.0f, 515.0f);
	title->setScale(3.0f);
	canvas->addChild(title);

	fpsLabel = Voxel::UI::Text::createWithOutline("fpsLabel", "FPS: 00000", 2);
	fpsLabel->setPivot(glm::vec2(-0.5f, 0.0f));
	fpsLabel->setPosition(637.0f, 527.0f);
	canvas->addChild(fpsLabel);

	// buttons
	const float labelsY = 476.0f;

	auto buttonsLabel = Text::create("btnLabel", "Buttons", 1);
	buttonsLabel->setScale(2.0f);
	buttonsLabel->setPosition(-891.0f, labelsY);
	canvas->addChild(buttonsLabel);

	initButtons(labelsY);

	// check boxes
	auto checkBoxLabel = Text::create("cbLabel", "Check Boxes", 1);
	checkBoxLabel->setPosition(-407.0f, 476.0f);
	checkBoxLabel->setScale(2.0f);
	canvas->addChild(checkBoxLabel);

	initCheckBoxes(labelsY);

	// progress timer
	auto ptLabel = Text::create("pgLabel", "Progress Timers", 1);
	ptLabel->setPosition(-158.0f, labelsY);
	ptLabel->setScale(2.0f);
	canvas->addChild(ptLabel);

	initProgressTimers(labelsY);

	// sliders
	auto sliderLabel = Text::create("sliderLabel", "Sliders", 1);
	sliderLabel->setPosition(-895.0f, 224.0f);
	sliderLabel->setScale(2.0f);
	canvas->addChild(sliderLabel);

	initSliders();

	// animated image
	auto animImageLabel = Text::create("animLabel", "Animated\nImage", 1);
	animImageLabel->setScale(2.0f);
	animImageLabel->setPosition(-141.0f, 208.0f);
	canvas->addChild(animImageLabel);

	initAnimatedImage();

	// images
	auto imageLabel = Text::create("imgLabel", "Images", 1);
	imageLabel->setScale(2.0f);
	imageLabel->setPosition(28.0f, 222.0f);
	canvas->addChild(imageLabel);

	initImages();

	// 9 patch images
	auto ninePatchImageLabel = Text::create("9PImg", "Nine Patch Images", 1);
	ninePatchImageLabel->setScale(2.0f);
	ninePatchImageLabel->setPosition(562.0f, 224.0f);
	canvas->addChild(ninePatchImageLabel);

	initNinePatchImages();

	// texts
	auto textLabel = Text::create("textLabel", "Texts", 1);
	textLabel->setScale(2.0f);
	textLabel->setPosition(-907.0f, -11.0f);
	canvas->addChild(textLabel);

	initTexts();

	// input fields
	auto inputFieldLabel = Text::create("ifLabel", "Input Fields", 1);
	inputFieldLabel->setScale(2.0f);
	inputFieldLabel->setPosition(488.0f, -11.0f);
	canvas->addChild(inputFieldLabel);

	initInputField();

	// actions
	auto actionsLabel = Text::create("actionLabel", "Actions", 1);
	actionsLabel->setScale(2.0f);
	actionsLabel->setPosition(-888.0f, -247.0f);
	canvas->addChild(actionsLabel);

	initActions();

	// ui heirarchy
	auto uiHierLabel = Text::create("uiHierLabel", "UI Hierarchy", 1);
	uiHierLabel->setPosition(-396.0f, -247.0f);
	uiHierLabel->setScale(2.0f);
	canvas->addChild(uiHierLabel);

	initUIHierarchy();

	// color picker
	auto cpLabel = Text::create("cpLabel", "Color picker", 1);
	cpLabel->setPosition(76.0f, -247.0f);
	cpLabel->setScale(2.0f);
	canvas->addChild(cpLabel);

	initColorPicker();

	const std::string ss = "DebugSpriteSheet";

	exitButton = Voxel::UI::Button::create("exitBtn", ss, "exit_game_button.png");
	exitButton->setPosition(936.0f, 526.0f);
	exitButton->setOnTriggeredCallbackFunc(std::bind(&UITestScene::onExitButtonClicked, this, std::placeholders::_1));
	canvas->addChild(exitButton);

	returnToMainMenuButton = Voxel::UI::Button::create("rtmmBtn", ss, "return_to_main_menu_button.png");
	returnToMainMenuButton->setPosition(818.0f, 526.0f);
	returnToMainMenuButton->setOnTriggeredCallbackFunc(std::bind(&UITestScene::onReturnToMainMenuClicked, this, std::placeholders::_1));
	canvas->addChild(returnToMainMenuButton);
}

void Voxel::UITestScene::initButtons(const float labelsY)
{
	const std::string ss = "DebugSpriteSheet";

	const float y = labelsY - 47.0f;
	const float x = -903.0f;

	const float baseY = y - 62.0f;
	const float xOffset = 110.0f;
	const float yOffset = 24.0f;

	const std::string buttonSpriteName = "debug_button.png";

	addButton("nBtn", glm::vec2(x, y), false, canvas);
	addButton("dBtn", glm::vec2(x + (xOffset * 2.0f), y), true, canvas);

	auto base1Button = addButton("b1Btn", glm::vec2(x, baseY), false, canvas);
	addButton("b1c1Btn", glm::vec2(0, -yOffset), false, base1Button);
	addButton("b1c2Btn", glm::vec2(0, -(yOffset * 2.0f)), false, base1Button);
	addButton("b1c3Btn", glm::vec2(0, -(yOffset * 3.0f)), false, base1Button);
	addButton("b1c4Btn", glm::vec2(0, -(yOffset * 4.0f)), false, base1Button);

	auto base2Button = addButton("b2Btn", glm::vec2(x + xOffset, baseY), false, canvas);
	auto base2c1Button = addButton("b2c1Btn", glm::vec2(0, -yOffset), false, base2Button);
	auto base2c2Button = addButton("b2c2Btn", glm::vec2(0, -yOffset), false, base2c1Button);
	auto base2c3Button = addButton("b2c3Btn", glm::vec2(0, -yOffset), false, base2c2Button);
	addButton("b2c4Btn", glm::vec2(0, -yOffset), false, base2c3Button);

	auto base3Button = addButton("b3Btn", glm::vec2(x + (xOffset * 2.0f), baseY), true, canvas);
	addButton("b3c1Btn", glm::vec2(0, -yOffset), false, base3Button);
	addButton("b3c2Btn", glm::vec2(0, -(yOffset * 2.0f)), true, base3Button);
	addButton("b3c3Btn", glm::vec2(0, -(yOffset * 3.0f)), false, base3Button);
	addButton("b3c4Btn", glm::vec2(0, -(yOffset * 4.0f)), true, base3Button);

	auto base4Button = addButton("b4Btn", glm::vec2(x + (xOffset * 3.0f), baseY), true, canvas);
	auto base4c1Button = addButton("b4c1Btn", glm::vec2(0, -yOffset), false, base4Button);
	auto base4c2Button = addButton("b4c2Btn", glm::vec2(0, -yOffset), true, base4c1Button);
	auto base4c3Button = addButton("b4c3Btn", glm::vec2(0, -yOffset), false, base4c2Button);
	addButton("b4c4Btn", glm::vec2(0, -yOffset), true, base4c3Button);
}

void Voxel::UITestScene::initCheckBoxes(const float labelsY)
{
	const std::string ss = "DebugSpriteSheet";
	const std::string cbImageName = "debug_checkbox.png";

	const float y = labelsY - 44.0f;
	const float x = -483.0f;

	const float baseY = y - 61.0f;
	const float xOffset = 50.0f;
	const float yOffset = 19.0f;

	addCheckbox("nCB", glm::vec2(x, y), false, canvas);
	addCheckbox("dCB", glm::vec2(x + (xOffset * 2.0f), y), true, canvas);

	auto base1CB = addCheckbox("b1CB", glm::vec2(x, baseY), false, canvas);
	addCheckbox("b1c1CB", glm::vec2(0, -yOffset), false, base1CB);
	addCheckbox("b1c2CB", glm::vec2(0, -(yOffset * 2.0f)), false, base1CB);
	addCheckbox("b1c3CB", glm::vec2(0, -(yOffset * 3.0f)), false, base1CB);
	addCheckbox("b1c4CB", glm::vec2(0, -(yOffset * 4.0f)), false, base1CB);

	auto base2CB = addCheckbox("b2CB", glm::vec2(x + xOffset, baseY), false, canvas);
	auto base2c1CB = addCheckbox("b2c1CB", glm::vec2(0, -yOffset), false, base2CB);
	auto base2c2CB = addCheckbox("b2c2CB", glm::vec2(0, -yOffset), false, base2c1CB);
	auto base2c3CB = addCheckbox("b2c3CB", glm::vec2(0, -yOffset), false, base2c2CB);
	addCheckbox("b2c4CB", glm::vec2(0, -yOffset), false, base2c3CB);

	auto base3CB = addCheckbox("b3CB", glm::vec2(x + (xOffset * 2.0f), baseY), true, canvas);
	addCheckbox("b3c1CB", glm::vec2(0, -yOffset), false, base3CB);
	addCheckbox("b3c2CB", glm::vec2(0, -(yOffset * 2.0f)), true, base3CB);
	addCheckbox("b3c3CB", glm::vec2(0, -(yOffset * 3.0f)), false, base3CB);
	addCheckbox("b3c4CB", glm::vec2(0, -(yOffset * 4.0f)), true, base3CB);

	auto base4CB = addCheckbox("b4CB", glm::vec2(x + (xOffset * 3.0f), baseY), true, canvas);
	auto base4c1CB = addCheckbox("b4c1CB", glm::vec2(0, -yOffset), false, base4CB);
	auto base4c2CB = addCheckbox("b4c2CB", glm::vec2(0, -yOffset), true, base4c1CB);
	auto base4c3CB = addCheckbox("b4c3CB", glm::vec2(0, -yOffset), false, base4c2CB);
	addCheckbox("b4c4CB", glm::vec2(0, -yOffset), true, base4c3CB);
}

void Voxel::UITestScene::initProgressTimers(const float labelsY)
{
	const std::string ss = "DebugSpriteSheet";
	const std::string horBarImageName = "debug_progress_hor_bar.png";
	const std::string verBarImageName = "debug_progress_ver_bar.png";
	const std::string radialImageName = "debug_progress_radial.png";

	const float y = labelsY - 39.0f;
	const float x = -240.0f;

	const float yOffset = 38.0f;
	const float horXOffset = 108.0f;

	auto horProgressBarWithAction = addHorProgBar("horPGWithAction", glm::vec2(x, y), 0, false, canvas);
	auto horBarSeq = Voxel::UI::RepeatForever::create(Voxel::UI::Sequence::create({ Voxel::UI::ProgressTo::create(1.0f, 100), Voxel::UI::Delay::create(0.5f), Voxel::UI::ProgressTo::create(1.0f, 0), Voxel::UI::Delay::create(0.5f) }));
	horProgressBarWithAction->runAction(horBarSeq);

	addHorProgBar("horPG0", glm::vec2(x, y - yOffset), 0, false, canvas);
	addHorProgBar("horPG50", glm::vec2(x, y - (yOffset * 2.0f)), 50, false, canvas);
	addHorProgBar("horPG100", glm::vec2(x, y - (yOffset * 3.0f)), 100, false, canvas);

	auto horCcwProgressBarWithAction = addHorProgBar("horCcwPGWithAction", glm::vec2(x + horXOffset, y), 0, true, canvas);
	auto horCcwBarSeq = Voxel::UI::RepeatForever::create(Voxel::UI::Sequence::create({ Voxel::UI::ProgressTo::create(1.0f, 100), Voxel::UI::Delay::create(0.5f), Voxel::UI::ProgressTo::create(1.0f, 0), Voxel::UI::Delay::create(0.5f) }));
	horCcwProgressBarWithAction->runAction(horCcwBarSeq);

	addHorProgBar("horCcwPG0", glm::vec2(x + horXOffset, y - yOffset), 0, true, canvas);
	addHorProgBar("horCcwPG50", glm::vec2(x + horXOffset, y - (yOffset * 2.0f)), 50, true, canvas);
	addHorProgBar("horCcwPG100", glm::vec2(x + horXOffset, y - (yOffset * 3.0f)), 100, true, canvas);

	const float verX = -59.0f;
	const float verY = 402.0f;
	const float verXOffset = 38.0f;
	const float verYOffset = 108.0f;

	auto verPGBarWithAction = addVerProgBar("verPGWithAction", glm::vec2(verX, verY), 0, false, canvas);
	auto verBarSeq = Voxel::UI::RepeatForever::create(Voxel::UI::Sequence::create({ Voxel::UI::ProgressTo::create(1.0f, 100), Voxel::UI::Delay::create(0.5f), Voxel::UI::ProgressTo::create(1.0f, 0), Voxel::UI::Delay::create(0.5f) }));
	verPGBarWithAction->runAction(verBarSeq);

	addVerProgBar("verPG0", glm::vec2(verX + verXOffset, verY), 0, false, canvas);
	addVerProgBar("verPG50", glm::vec2(verX + (verXOffset * 2.0f), verY), 50, false, canvas);
	addVerProgBar("verPG100", glm::vec2(verX + (verXOffset * 3.0f), verY), 100, false, canvas);

	auto verCcwPGBarWithAction = addVerProgBar("verCcwPGWithAction", glm::vec2(verX, verY - verYOffset), 0, true, canvas);
	auto verCcwBarSeq = Voxel::UI::RepeatForever::create(Voxel::UI::Sequence::create({ Voxel::UI::ProgressTo::create(1.0f, 100), Voxel::UI::Delay::create(0.5f), Voxel::UI::ProgressTo::create(1.0f, 0), Voxel::UI::Delay::create(0.5f) }));
	verCcwPGBarWithAction->runAction(verCcwBarSeq);

	addVerProgBar("verCcwPG0", glm::vec2(verX + verXOffset, verY - verYOffset), 0, true, canvas);
	addVerProgBar("verCcwPG50", glm::vec2(verX + (verXOffset * 2.0f), verY - verYOffset), 50, true, canvas);
	addVerProgBar("verCcwPG100", glm::vec2(verX + (verXOffset * 3.0f), verY - verYOffset), 100, true, canvas);

	const float rX = 115.0f;
	const float rY = 417.0f;
	const float rOffset = 78.0f;

	auto radialPGWAction = addProgRadial("radialPGWAction", glm::vec2(rX, rY), 0, false, canvas);
	auto radialSeq = Voxel::UI::RepeatForever::create(Voxel::UI::Sequence::create({ Voxel::UI::ProgressTo::create(1.0f, 100), Voxel::UI::Delay::create(0.5f), Voxel::UI::ProgressTo::create(1.0f, 0), Voxel::UI::Delay::create(0.5f) }));
	radialPGWAction->runAction(radialSeq);

	addProgRadial("rPG0", glm::vec2(rX + rOffset, rY), 0, false, canvas);
	addProgRadial("rPG12", glm::vec2(rX + (rOffset * 2.0f), rY), 12.5f, false, canvas);
	addProgRadial("rPG25", glm::vec2(rX + (rOffset * 3.0f), rY), 25.0f, false, canvas);
	addProgRadial("rPG37", glm::vec2(rX + (rOffset * 4.0f), rY), 37.5f, false, canvas);
	addProgRadial("rPG50", glm::vec2(rX + (rOffset * 5.0f), rY), 50.0f, false, canvas);
	addProgRadial("rPG62", glm::vec2(rX + (rOffset * 6.0f), rY), 62.5f, false, canvas);
	addProgRadial("rPG75", glm::vec2(rX + (rOffset * 7.0f), rY), 75.0f, false, canvas);
	addProgRadial("rPG87", glm::vec2(rX + (rOffset * 8.0f), rY), 87.5f, false, canvas);
	addProgRadial("rPG100", glm::vec2(rX + (rOffset * 9.0f), rY), 100, false, canvas);

	auto radialCcwPGWAction = addProgRadial("radialCcwPGWAction", glm::vec2(rX, rY - rOffset), 0, false, canvas);
	auto radialCcwSeq = Voxel::UI::RepeatForever::create(Voxel::UI::Sequence::create({ Voxel::UI::ProgressTo::create(1.0f, 100), Voxel::UI::Delay::create(0.5f), Voxel::UI::ProgressTo::create(1.0f, 0), Voxel::UI::Delay::create(0.5f) }));
	radialCcwPGWAction->runAction(radialCcwSeq);

	addProgRadial("rCcwPG0", glm::vec2(rX + rOffset, rY - rOffset), 0, true, canvas);
	addProgRadial("rCcwPG12", glm::vec2(rX + (rOffset * 2.0f), rY - rOffset), 12.5f, true, canvas);
	addProgRadial("rCcwPG25", glm::vec2(rX + (rOffset * 3.0f), rY - rOffset), 25.0f, true, canvas);
	addProgRadial("rCcwPG37", glm::vec2(rX + (rOffset * 4.0f), rY - rOffset), 37.5f, true, canvas);
	addProgRadial("rCcwPG50", glm::vec2(rX + (rOffset * 5.0f), rY - rOffset), 50.0f, true, canvas);
	addProgRadial("rCcwPG62", glm::vec2(rX + (rOffset * 6.0f), rY - rOffset), 62.5f, true, canvas);
	addProgRadial("rCcwPG75", glm::vec2(rX + (rOffset * 7.0f), rY - rOffset), 75.0f, true, canvas);
	addProgRadial("rCcwPG87", glm::vec2(rX + (rOffset * 8.0f), rY - rOffset), 87.5f, true, canvas);
	addProgRadial("rCcwPG100", glm::vec2(rX + (rOffset * 9.0f), rY - rOffset), 100, true, canvas);
}

void Voxel::UITestScene::initSliders() 
{
	const float x = -861.0f;
	const float y = 192.0f;
	const float y1 = y - 32.0f;
	const float y2 = y - 97.0f;
	const float yOffset = 8.0f;

	addHorSlider("nSlider", glm::vec2(x, y), 0, 100, 50, false, canvas);

	auto s1HorBase = addHorSlider("s1HorBase", glm::vec2(x, y1), 0, 100, 50, false, canvas);
	addHorSlider("s1c1", glm::vec2(0, -yOffset), 0, 100, 50, false, s1HorBase);
	addHorSlider("s1c2", glm::vec2(0, -(yOffset * 2.0f)), 0, 100, 50, false, s1HorBase);
	addHorSlider("s1c3", glm::vec2(0, -(yOffset * 3.0f)), 0, 100, 50, false, s1HorBase);
	addHorSlider("s1c4", glm::vec2(0, -(yOffset * 4.0f)), 0, 100, 50, false, s1HorBase);

	auto s2HorBase = addHorSlider("s2Horbase", glm::vec2(x, y2), 0, 100, 50, false, canvas);
	auto s2c1 = addHorSlider("s2c1", glm::vec2(0, -yOffset), 0, 100, 50, false, s2HorBase);
	auto s2c2 = addHorSlider("s2c2", glm::vec2(0, -yOffset), 0, 100, 50, false, s2c1);
	auto s2c3 = addHorSlider("s2c3", glm::vec2(0, -yOffset), 0, 100, 50, false, s2c2);
	addHorSlider("s2c4", glm::vec2(0, -yOffset), 0, 100, 50, false, s2c3);

	const float x2 = x + 192.0f;

	addHorSlider("dSlider", glm::vec2(x2, y), 0, 100, 50, true, canvas);

	auto s3HorBase = addHorSlider("s3HorBase", glm::vec2(x2, y1), 0, 100, 50, true, canvas);
	addHorSlider("s3c1", glm::vec2(0, -yOffset), 0, 100, 50, false, s3HorBase);
	addHorSlider("s3c2", glm::vec2(0, -(yOffset * 2.0f)), 0, 100, 50, true, s3HorBase);
	addHorSlider("s3c3", glm::vec2(0, -(yOffset * 3.0f)), 0, 100, 50, false, s3HorBase);
	addHorSlider("s3c4", glm::vec2(0, -(yOffset * 4.0f)), 0, 100, 50, true, s3HorBase);

	auto s4HorBase = addHorSlider("s4Horbase", glm::vec2(x2, y2), 0, 100, 50, true, canvas);
	auto s4c1 = addHorSlider("s4c1", glm::vec2(0, -yOffset), 0, 100, 50, false, s4HorBase);
	auto s4c2 = addHorSlider("s4c2", glm::vec2(0, -yOffset), 0, 100, 50, true, s4c1);
	auto s4c3 = addHorSlider("s4c3", glm::vec2(0, -yOffset), 0, 100, 50, false, s4c2);
	addHorSlider("s4c4", glm::vec2(0, -yOffset), 0, 100, 50, true, s4c3);


	const float verX = -555.0f;
	const float verY = 131.0f;
	const float verXBaseOffset = 37.0f;
	const float verXOffset = 8.0f;

	addVerSlider("nVerSlider", glm::vec2(verX, verY), 0, 100, 50, false, canvas);

	auto s1VerBase = addVerSlider("s1VerBase", glm::vec2(verX + verXBaseOffset, verY), 0, 100, 50, false, canvas);
	addVerSlider("s1c1v", glm::vec2(verXOffset, 0), 0, 100, 50, false, s1VerBase);
	addVerSlider("s1c2v", glm::vec2(verXOffset * 2.0f, 0), 0, 100, 50, false, s1VerBase);
	addVerSlider("s1c3v", glm::vec2(verXOffset * 3.0f, 0), 0, 100, 50, false, s1VerBase);
	addVerSlider("s1c4v", glm::vec2(verXOffset * 4.0f, 0), 0, 100, 50, false, s1VerBase);

	const float verX2 = verX + verXBaseOffset + 64.0f;

	auto s2VerBase = addVerSlider("s2Verbase", glm::vec2(verX2, verY), 0, 100, 50, false, canvas);
	auto s2c1v = addVerSlider("s2c1v", glm::vec2(verXOffset, 0), 0, 100, 50, false, s2VerBase);
	auto s2c2v = addVerSlider("s2c2v", glm::vec2(verXOffset, 0), 0, 100, 50, false, s2c1v);
	auto s2c3v = addVerSlider("s2c3v", glm::vec2(verXOffset, 0), 0, 100, 50, false, s2c2v);
	addVerSlider("s2c4v", glm::vec2(verXOffset, 0), 0, 100, 50, false, s2c3v);

	const float verX3 = verX2 + 70.0f;

	addVerSlider("dVerSlider", glm::vec2(verX3, verY), 0, 100, 50, true, canvas);

	auto s3VerBase = addVerSlider("s3VerBase", glm::vec2(verX3 + verXBaseOffset, verY), 0, 100, 50, true, canvas);
	addVerSlider("s3c1v", glm::vec2(verXOffset, 0), 0, 100, 50, false, s3VerBase);
	addVerSlider("s3c2v", glm::vec2(verXOffset * 2.0f, 0), 0, 100, 50, true, s3VerBase);
	addVerSlider("s3c3v", glm::vec2(verXOffset * 3.0f, 0), 0, 100, 50, false, s3VerBase);
	addVerSlider("s3c4v", glm::vec2(verXOffset * 4.0f, 0), 0, 100, 50, true, s3VerBase);

	const float verX4 = verX3 + verXBaseOffset + 64.0f;

	auto s4VerBase = addVerSlider("s4Verbase", glm::vec2(verX4, verY), 0, 100, 50, true, canvas);
	auto s4c1v = addVerSlider("s4c1v", glm::vec2(verXOffset, 0), 0, 100, 50, false, s4VerBase);
	auto s4c2v = addVerSlider("s4c2v", glm::vec2(verXOffset, 0), 0, 100, 50, true, s4c1v);
	auto s4c3v = addVerSlider("s4c3v", glm::vec2(verXOffset, 0), 0, 100, 50, false, s4c2v);
	addVerSlider("s4c4v", glm::vec2(verXOffset, 0), 0, 100, 50, true, s4c3v);
}

void Voxel::UITestScene::initAnimatedImage()
{
	auto animImage = Voxel::UI::AnimatedImage::create("animImage", "DebugSpriteSheet", "debug_anim_frame.png", 5, 0.5f, true);
	animImage->setPosition(-180.0f, 133.0f);
	animImage->setOnMouseEnterCallback(std::bind(&UITestScene::onMouseEnter, this, std::placeholders::_1));
	animImage->setOnMouseExitCallback(std::bind(&UITestScene::onMouseExit, this, std::placeholders::_1));
	animImage->setOnMouseMoveCallback(std::bind(&UITestScene::onMouseMove, this, std::placeholders::_1));
	animImage->setOnMousePressedCallback(std::bind(&UITestScene::onMousePressed, this, std::placeholders::_1, std::placeholders::_2));
	animImage->setOnMouseReleasedCallback(std::bind(&UITestScene::onMouseReleased, this, std::placeholders::_1, std::placeholders::_2));
	animImage->setInteractable();
	canvas->addChild(animImage);

	auto slime = Voxel::UI::AnimatedImage::create("slime", "DebugSpriteSheet", "debug_slime.png", 6, 0.1f, true);
	slime->setPosition(-110.0f, 125.0f);
	slime->setPivot(0.0f, -0.5f);
	slime->setScale(2.0f);
	slime->setOnMouseEnterCallback(std::bind(&UITestScene::onMouseEnter, this, std::placeholders::_1));
	slime->setOnMouseExitCallback(std::bind(&UITestScene::onMouseExit, this, std::placeholders::_1));
	slime->setOnMouseMoveCallback(std::bind(&UITestScene::onMouseMove, this, std::placeholders::_1));
	slime->setOnMousePressedCallback(std::bind(&UITestScene::onMousePressed, this, std::placeholders::_1, std::placeholders::_2));
	slime->setOnMouseReleasedCallback(std::bind(&UITestScene::onMouseReleased, this, std::placeholders::_1, std::placeholders::_2));
	slime->setInteractable();
	canvas->addChild(slime);
}

void Voxel::UITestScene::initImages()
{
	auto image = Image::createFromSpriteSheet("image", "DebugSpriteSheet", "debug_image.png");
	image->setPosition(48.0f, 99.0f);
	image->setOnMouseEnterCallback(std::bind(&UITestScene::onMouseEnter, this, std::placeholders::_1));
	image->setOnMouseExitCallback(std::bind(&UITestScene::onMouseExit, this, std::placeholders::_1));
	image->setOnMouseMoveCallback(std::bind(&UITestScene::onMouseMove, this, std::placeholders::_1));
	image->setOnMousePressedCallback(std::bind(&UITestScene::onMousePressed, this, std::placeholders::_1, std::placeholders::_2));
	image->setOnMouseReleasedCallback(std::bind(&UITestScene::onMouseReleased, this, std::placeholders::_1, std::placeholders::_2));
	image->setInteractable();
	canvas->addChild(image);

	/*
	auto boundary = NinePatchImage::create("bdry", "DebugSpriteSheet", "debug_red_boundary.png", 4.0f, 4.0f, 4.0f, 4.0f, glm::vec2(242.0f, 172.0f));
	boundary->setPosition(274.0f, 109.0f);
	canvas->addChild(boundary);
	*/
}

void Voxel::UITestScene::initTexts()
{
	const float x = -948.0f;
	const float y = -42.0f;

	auto textOneLine = Text::create("txtOneLine", "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s,", 1);
	textOneLine->setPosition(x, -42.0f);
	textOneLine->setPivot(-0.5f, 0.0f);
	textOneLine->setOnMouseEnterCallback(std::bind(&UITestScene::onMouseEnter, this, std::placeholders::_1));
	textOneLine->setOnMouseExitCallback(std::bind(&UITestScene::onMouseExit, this, std::placeholders::_1));
	textOneLine->setOnMouseMoveCallback(std::bind(&UITestScene::onMouseMove, this, std::placeholders::_1));
	textOneLine->setOnMousePressedCallback(std::bind(&UITestScene::onMousePressed, this, std::placeholders::_1, std::placeholders::_2));
	textOneLine->setOnMouseReleasedCallback(std::bind(&UITestScene::onMouseReleased, this, std::placeholders::_1, std::placeholders::_2));
	textOneLine->setInteractable();
	canvas->addChild(textOneLine);

	auto textTwoLine = Text::create("txtTwoLine", "Lorem Ipsum is simply dummy text of the printing and typesetting industry.\nLorem Ipsum has been the industry's standard dummy text ever since the 1500s,", 1);
	textTwoLine->setPosition(x, y - 30.0f);
	textTwoLine->setPivot(-0.5f, 0.0f);
	textTwoLine->setOnMouseEnterCallback(std::bind(&UITestScene::onMouseEnter, this, std::placeholders::_1));
	textTwoLine->setOnMouseExitCallback(std::bind(&UITestScene::onMouseExit, this, std::placeholders::_1));
	textTwoLine->setOnMouseMoveCallback(std::bind(&UITestScene::onMouseMove, this, std::placeholders::_1));
	textTwoLine->setOnMousePressedCallback(std::bind(&UITestScene::onMousePressed, this, std::placeholders::_1, std::placeholders::_2));
	textTwoLine->setOnMouseReleasedCallback(std::bind(&UITestScene::onMouseReleased, this, std::placeholders::_1, std::placeholders::_2));
	textTwoLine->setInteractable();
	canvas->addChild(textTwoLine);

	auto textTwoLineOutlined = Text::createWithOutline("txtTwoLineO", "Lorem Ipsum is simply dummy text of the printing and typesetting industry.\nLorem Ipsum has been the industry's standard dummy text ever since the 1500s,", 2);
	textTwoLineOutlined->setPosition(x, y - 66.0f);
	textTwoLineOutlined->setPivot(-0.5f, 0.0f);
	textTwoLineOutlined->setOnMouseEnterCallback(std::bind(&UITestScene::onMouseEnter, this, std::placeholders::_1));
	textTwoLineOutlined->setOnMouseExitCallback(std::bind(&UITestScene::onMouseExit, this, std::placeholders::_1));
	textTwoLineOutlined->setOnMouseMoveCallback(std::bind(&UITestScene::onMouseMove, this, std::placeholders::_1));
	textTwoLineOutlined->setOnMousePressedCallback(std::bind(&UITestScene::onMousePressed, this, std::placeholders::_1, std::placeholders::_2));
	textTwoLineOutlined->setOnMouseReleasedCallback(std::bind(&UITestScene::onMouseReleased, this, std::placeholders::_1, std::placeholders::_2));
	textTwoLineOutlined->setInteractable();
	canvas->addChild(textTwoLineOutlined);

	const float sizeYOffset = 140.0f;

	auto textSizedL = Text::create("txtSizedL", "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s,", 1, Voxel::UI::Text::ALIGN::LEFT, 212);
	textSizedL->setPosition(x, y - sizeYOffset);
	textSizedL->setPivot(-0.5f, 0.0f);
	textSizedL->setOnMouseEnterCallback(std::bind(&UITestScene::onMouseEnter, this, std::placeholders::_1));
	textSizedL->setOnMouseExitCallback(std::bind(&UITestScene::onMouseExit, this, std::placeholders::_1));
	textSizedL->setOnMouseMoveCallback(std::bind(&UITestScene::onMouseMove, this, std::placeholders::_1));
	textSizedL->setOnMousePressedCallback(std::bind(&UITestScene::onMousePressed, this, std::placeholders::_1, std::placeholders::_2));
	textSizedL->setOnMouseReleasedCallback(std::bind(&UITestScene::onMouseReleased, this, std::placeholders::_1, std::placeholders::_2));
	textSizedL->setInteractable();
	canvas->addChild(textSizedL);

	const float xOffset = 240.0f;

	auto textSizedC = Text::create("txtSizedC", "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s,", 1, Voxel::UI::Text::ALIGN::CENTER, 212);
	textSizedC->setPosition(x + xOffset, y - sizeYOffset);
	textSizedC->setPivot(-0.5f, 0.0f);
	textSizedC->setOnMouseEnterCallback(std::bind(&UITestScene::onMouseEnter, this, std::placeholders::_1));
	textSizedC->setOnMouseExitCallback(std::bind(&UITestScene::onMouseExit, this, std::placeholders::_1));
	textSizedC->setOnMouseMoveCallback(std::bind(&UITestScene::onMouseMove, this, std::placeholders::_1));
	textSizedC->setOnMousePressedCallback(std::bind(&UITestScene::onMousePressed, this, std::placeholders::_1, std::placeholders::_2));
	textSizedC->setOnMouseReleasedCallback(std::bind(&UITestScene::onMouseReleased, this, std::placeholders::_1, std::placeholders::_2));
	textSizedC->setInteractable();
	canvas->addChild(textSizedC);

	auto textSizedR = Text::create("txtSizedR", "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s,", 1, Voxel::UI::Text::ALIGN::RIGHT, 212);
	textSizedR->setPosition(x + (2.0f * xOffset), y - sizeYOffset);
	textSizedR->setPivot(-0.5f, 0.0f);
	textSizedR->setOnMouseEnterCallback(std::bind(&UITestScene::onMouseEnter, this, std::placeholders::_1));
	textSizedR->setOnMouseExitCallback(std::bind(&UITestScene::onMouseExit, this, std::placeholders::_1));
	textSizedR->setOnMouseMoveCallback(std::bind(&UITestScene::onMouseMove, this, std::placeholders::_1));
	textSizedR->setOnMousePressedCallback(std::bind(&UITestScene::onMousePressed, this, std::placeholders::_1, std::placeholders::_2));
	textSizedR->setOnMouseReleasedCallback(std::bind(&UITestScene::onMouseReleased, this, std::placeholders::_1, std::placeholders::_2));
	textSizedR->setInteractable();
	canvas->addChild(textSizedR);
}

void Voxel::UITestScene::initActions()
{
	const float y = -300.0f;
	const float fadeX = -914.0f;
	const float xOffset = 114.0f;
	const float rotateX = fadeX + xOffset;
	const float moveX = rotateX + xOffset;
	const float tintX = moveX + 107.0f;

	auto fadeImg = Image::createFromSpriteSheet("fadeImg", "DebugSpriteSheet", "debug_fade_image.png");
	fadeImg->setPosition(fadeX, y);
	fadeImg->setOnMouseEnterCallback(std::bind(&UITestScene::onMouseEnter, this, std::placeholders::_1));
	fadeImg->setOnMouseExitCallback(std::bind(&UITestScene::onMouseExit, this, std::placeholders::_1));
	fadeImg->setOnMouseMoveCallback(std::bind(&UITestScene::onMouseMove, this, std::placeholders::_1));
	fadeImg->setOnMousePressedCallback(std::bind(&UITestScene::onMousePressed, this, std::placeholders::_1, std::placeholders::_2));
	fadeImg->setOnMouseReleasedCallback(std::bind(&UITestScene::onMouseReleased, this, std::placeholders::_1, std::placeholders::_2));
	fadeImg->setInteractable();

	auto fadeSeq = RepeatForever::create(Sequence::create({ Delay::create(0.5f), FadeTo::create(0.5f, 0.0f), Delay::create(0.5f), FadeTo::create(0.5f, 1.0f) }));

	fadeImg->runAction(fadeSeq);

	canvas->addChild(fadeImg);


	auto rotateImg = Image::createFromSpriteSheet("rotateImg", "DebugSpriteSheet", "debug_rotate_image.png");
	rotateImg->setPosition(rotateX, y);
	rotateImg->setOnMouseEnterCallback(std::bind(&UITestScene::onMouseEnter, this, std::placeholders::_1));
	rotateImg->setOnMouseExitCallback(std::bind(&UITestScene::onMouseExit, this, std::placeholders::_1));
	rotateImg->setOnMouseMoveCallback(std::bind(&UITestScene::onMouseMove, this, std::placeholders::_1));
	rotateImg->setOnMousePressedCallback(std::bind(&UITestScene::onMousePressed, this, std::placeholders::_1, std::placeholders::_2));
	rotateImg->setOnMouseReleasedCallback(std::bind(&UITestScene::onMouseReleased, this, std::placeholders::_1, std::placeholders::_2));
	rotateImg->setInteractable();

	auto rotateSeq = RepeatForever::create(Sequence::create({ Delay::create(0.5f), RotateTo::create(0.5f, 270.0f), Delay::create(0.5f), RotateTo::create(0.5f, -270.0f) }));

	rotateImg->runAction(rotateSeq);

	canvas->addChild(rotateImg);


	auto moveImg = Image::createFromSpriteSheet("moveImg", "DebugSpriteSheet", "debug_move_image.png");
	moveImg->setPosition(moveX, y);
	moveImg->setOnMouseEnterCallback(std::bind(&UITestScene::onMouseEnter, this, std::placeholders::_1));
	moveImg->setOnMouseExitCallback(std::bind(&UITestScene::onMouseExit, this, std::placeholders::_1));
	moveImg->setOnMouseMoveCallback(std::bind(&UITestScene::onMouseMove, this, std::placeholders::_1));
	moveImg->setOnMousePressedCallback(std::bind(&UITestScene::onMousePressed, this, std::placeholders::_1, std::placeholders::_2));
	moveImg->setOnMouseReleasedCallback(std::bind(&UITestScene::onMouseReleased, this, std::placeholders::_1, std::placeholders::_2));
	moveImg->setInteractable();

	auto pos = moveImg->getPosition();

	auto moveSeq = RepeatForever::create(Sequence::create({ Delay::create(0.5f), MoveTo::create(0.5f, pos + glm::vec2(0.0f, -177.0f)), Delay::create(0.5f), MoveTo::create(0.5f, pos + glm::vec2(107.0f, -177.0f)), Delay::create(0.5f), MoveTo::create(0.5f, pos) }));

	moveImg->runAction(moveSeq);

	canvas->addChild(moveImg);


	const glm::vec2 scalePos = glm::vec2(fadeX + 57.0f, y - 138.0f);

	auto scaleImg = Image::createFromSpriteSheet("scaleImg", "DebugSpriteSheet", "debug_scale_image.png");
	scaleImg->setPosition(scalePos);
	scaleImg->setOnMouseEnterCallback(std::bind(&UITestScene::onMouseEnter, this, std::placeholders::_1));
	scaleImg->setOnMouseExitCallback(std::bind(&UITestScene::onMouseExit, this, std::placeholders::_1));
	scaleImg->setOnMouseMoveCallback(std::bind(&UITestScene::onMouseMove, this, std::placeholders::_1));
	scaleImg->setOnMousePressedCallback(std::bind(&UITestScene::onMousePressed, this, std::placeholders::_1, std::placeholders::_2));
	scaleImg->setOnMouseReleasedCallback(std::bind(&UITestScene::onMouseReleased, this, std::placeholders::_1, std::placeholders::_2));
	scaleImg->setInteractable();
	
	auto scaleSeq = RepeatForever::create(Sequence::create({ Delay::create(0.5f), ScaleTo::create(0.5f, glm::vec2(2.0f)), Delay::create(0.5f), ScaleTo::create(0.5f, glm::vec2(1.0f)) }));

	scaleImg->runAction(scaleSeq);

	canvas->addChild(scaleImg);


	auto tintImg = Image::createFromSpriteSheet("tintImg", "DebugSpriteSheet", "debug_tint_image.png");
	tintImg->setPosition(tintX, y);
	tintImg->setOnMouseEnterCallback(std::bind(&UITestScene::onMouseEnter, this, std::placeholders::_1));
	tintImg->setOnMouseExitCallback(std::bind(&UITestScene::onMouseExit, this, std::placeholders::_1));
	tintImg->setOnMouseMoveCallback(std::bind(&UITestScene::onMouseMove, this, std::placeholders::_1));
	tintImg->setOnMousePressedCallback(std::bind(&UITestScene::onMousePressed, this, std::placeholders::_1, std::placeholders::_2));
	tintImg->setOnMouseReleasedCallback(std::bind(&UITestScene::onMouseReleased, this, std::placeholders::_1, std::placeholders::_2));
	tintImg->setInteractable();
	tintImg->setColor(glm::vec3(1.0f));

	auto tintSeq = RepeatForever::create(Sequence::create({ Delay::create(0.5f), TintTo::create(0.5f, glm::vec3(0.0f, 1.0f, 0.0f)), Delay::create(0.5f), TintTo::create(0.5f, glm::vec3(0.0f, 0.0f, 1.0f)), Delay::create(0.5f), TintTo::create(0.5f, glm::vec3(1.0f, 0.0f, 0.0f)) }));

	tintImg->runAction(tintSeq);

	canvas->addChild(tintImg);
}

void Voxel::UITestScene::initNinePatchImages()
{
	const float x = 445.0f;
	const float y = 177.0f;

	addNinePatchImage("NinePatchImg1", glm::vec2(x, y), glm::vec4(12.0f), glm::vec2(20.0f), canvas);
	addNinePatchImage("NinePatchImg2", glm::vec2(x + 80.0f, y - 21.0f), glm::vec4(12.0f), glm::vec2(64.0f), canvas);
	addNinePatchImage("NinePatchImg3", glm::vec2(x + 204.0f, y - 44.0f), glm::vec4(12.0f), glm::vec2(108.0f), canvas);
}

void Voxel::UITestScene::initInputField()
{
	auto bg = Image::createFromSpriteSheet("ifBg", "DebugSpriteSheet", "debug_input_field_bg.png");
	bg->setPosition(513.0f, -48.0f);
	canvas->addChild(bg);

	auto inputField = InputField::create("if", "Click here to enter text!", "DebugSpriteSheet", 1, "debug_input_field_cursor.png", Voxel::UI::Text::ALIGN::LEFT, 28);
	inputField->setPivot(-0.5f, 0.0f);
	inputField->setCoordinateOrigin(glm::vec2(-0.5f, 0.0f));
	inputField->setPosition(8.0f, 0.0f);
	bg->addChild(inputField);
}

void Voxel::UITestScene::initUIHierarchy()
{
	glm::vec2 basePos(-222.0f, -317.0f);

	auto ui8 = addUIHiers("ui8", basePos, 8, 100, canvas);

	const float offset = 50.0f;

	auto ui6 = addUIHiers("ui6", glm::vec2(-offset, -offset), 6, -1, ui8);
	auto ui4 = addUIHiers("uiHier4", glm::vec2(-offset, -offset), 4, -1, ui6);
	auto ui1 = addUIHiers("uiHier1", glm::vec2(-offset * 3.0f, -offset), 1, -3, ui4);
	auto ui2 = addUIHiers("uiHier2", glm::vec2(-offset * 2.0f, -offset), 2, -2, ui4);
	auto ui3 = addUIHiers("uiHier3", glm::vec2(-offset, -offset), 3, -1, ui4);
	auto ui5 = addUIHiers("uiHier5", glm::vec2(offset, -offset), 5, 1, ui4);
	auto ui7 = addUIHiers("uiHier7", glm::vec2(offset, -offset), 7, 1, ui6);
	auto ui9 = addUIHiers("uiHier9", glm::vec2(offset, -offset), 9, 1, ui8);
	auto ui10 = addUIHiers("uiHier10", glm::vec2(offset, -offset), 10, 0, ui9);
	auto ui11 = addUIHiers("uiHier11", glm::vec2(offset * 2.0f, -offset), 11, 1, ui9);
}

void Voxel::UITestScene::initColorPicker()
{
	setBgColorButton = Button::create("setBGColor", "DebugSpriteSheet", "debug_square_button.png");
	setBgColorButton->setPosition(208.0f, -247.0f);
	setBgColorButton->setOnMouseEnterCallback(std::bind(&UITestScene::onMouseEnter, this, std::placeholders::_1));
	setBgColorButton->setOnMouseExitCallback(std::bind(&UITestScene::onMouseExit, this, std::placeholders::_1));
	setBgColorButton->setOnMouseMoveCallback(std::bind(&UITestScene::onMouseMove, this, std::placeholders::_1));
	setBgColorButton->setOnMousePressedCallback(std::bind(&UITestScene::onMousePressed, this, std::placeholders::_1, std::placeholders::_2));
	setBgColorButton->setOnMouseReleasedCallback(std::bind(&UITestScene::onMouseReleased, this, std::placeholders::_1, std::placeholders::_2));
	setBgColorButton->setOnTriggeredCallbackFunc(std::bind(&UITestScene::onButtonTriggered, this, std::placeholders::_1));
	setBgColorButton->setOnCancelledCallbackFunc(std::bind(&UITestScene::onButtonCancelled, this, std::placeholders::_1));
	canvas->addChild(setBgColorButton);

	auto cpBg = NinePatchImage::create("cpBg", "DebugSpriteSheet", "debug_color_picker_bg.png", 4.0f, 4.0f, 4.0f, 4.0f, glm::vec2(256.0f));
	cpBg->setPosition(98.0f, -396.0f);
	canvas->addChild(cpBg);

	cp = ColorPicker::create("cp", glm::vec2(256.0f), "DebugSpriteSheet", "debug_color_picker_icon.png"); 
	cp->setOnMouseEnterCallback(std::bind(&UITestScene::onMouseEnter, this, std::placeholders::_1));
	cp->setOnMouseExitCallback(std::bind(&UITestScene::onMouseExit, this, std::placeholders::_1));
	cp->setOnMouseMoveCallback(std::bind(&UITestScene::onMouseMove, this, std::placeholders::_1));
	cp->setOnMousePressedCallback(std::bind(&UITestScene::onMousePressed, this, std::placeholders::_1, std::placeholders::_2));
	cp->setOnMouseReleasedCallback(std::bind(&UITestScene::onMouseReleased, this, std::placeholders::_1, std::placeholders::_2));
	cpBg->addChild(cp);

	cpSlider = Slider::create("cpSlider", "DebugSpriteSheet", "debug_color_slider.png", "debug_color_slider_button.png", Slider::Type::VERTICAL, 0.0f, 360.0f);
	cpSlider->setPosition(256.0f, -396.0f);
	cpSlider->setOnMouseEnterCallback(std::bind(&UITestScene::onMouseEnter, this, std::placeholders::_1));
	cpSlider->setOnMouseExitCallback(std::bind(&UITestScene::onMouseExit, this, std::placeholders::_1));
	cpSlider->setOnMouseMoveCallback(std::bind(&UITestScene::onMouseMove, this, std::placeholders::_1));
	cpSlider->setOnMousePressedCallback(std::bind(&UITestScene::onMousePressed, this, std::placeholders::_1, std::placeholders::_2));
	cpSlider->setOnMouseReleasedCallback(std::bind(&UITestScene::onMouseReleased, this, std::placeholders::_1, std::placeholders::_2));
	cpSlider->setOnButtonPressed(std::bind(&UITestScene::onSliderButtonPrssed, this, std::placeholders::_1));
	cpSlider->setOnBarPressed(std::bind(&UITestScene::onSliderBarPressed, this, std::placeholders::_1));
	cpSlider->setOnValueChange(std::bind(&UITestScene::onSliderValueChange, this, std::placeholders::_1));
	cpSlider->setOnFinished(std::bind(&UITestScene::onSliderFinished, this, std::placeholders::_1));
	canvas->addChild(cpSlider);
}

Voxel::UI::Button* Voxel::UITestScene::addButton(const std::string & name, const glm::vec2 & pos, const bool disable, Voxel::UI::TransformNode * parent)
{
	auto newButton = Button::create(name, "DebugSpriteSheet", "debug_button.png");

	newButton->setPosition(pos);
	newButton->setOnMouseEnterCallback(std::bind(&UITestScene::onMouseEnter, this, std::placeholders::_1));
	newButton->setOnMouseExitCallback(std::bind(&UITestScene::onMouseExit, this, std::placeholders::_1));
	newButton->setOnMouseMoveCallback(std::bind(&UITestScene::onMouseMove, this, std::placeholders::_1));
	newButton->setOnMousePressedCallback(std::bind(&UITestScene::onMousePressed, this, std::placeholders::_1, std::placeholders::_2));
	newButton->setOnMouseReleasedCallback(std::bind(&UITestScene::onMouseReleased, this, std::placeholders::_1, std::placeholders::_2));
	newButton->setOnTriggeredCallbackFunc(std::bind(&UITestScene::onButtonTriggered, this, std::placeholders::_1));
	newButton->setOnCancelledCallbackFunc(std::bind(&UITestScene::onButtonCancelled, this, std::placeholders::_1));

	if (disable)
	{
		newButton->disable();
	}

	parent->addChild(newButton);

	return newButton;
}

Voxel::UI::CheckBox * Voxel::UITestScene::addCheckbox(const std::string & name, const glm::vec2 & pos, const bool disable, Voxel::UI::TransformNode * parent)
{
	auto newCB = CheckBox::create(name, "DebugSpriteSheet", "debug_checkbox.png");

	newCB->setPosition(pos);
	newCB->setOnMouseEnterCallback(std::bind(&UITestScene::onMouseEnter, this, std::placeholders::_1));
	newCB->setOnMouseExitCallback(std::bind(&UITestScene::onMouseExit, this, std::placeholders::_1));
	newCB->setOnMouseMoveCallback(std::bind(&UITestScene::onMouseMove, this, std::placeholders::_1));
	newCB->setOnMousePressedCallback(std::bind(&UITestScene::onMousePressed, this, std::placeholders::_1, std::placeholders::_2));
	newCB->setOnMouseReleasedCallback(std::bind(&UITestScene::onMouseReleased, this, std::placeholders::_1, std::placeholders::_2));
	newCB->setOnSelectedCallbackFunc(std::bind(&UITestScene::onCheckBoxSelected, this, std::placeholders::_1));
	newCB->setOnDeselectedCallbackFunc(std::bind(&UITestScene::onCheckBoxDeselected, this, std::placeholders::_1));
	newCB->setOnCancelledCallbackFunc(std::bind(&UITestScene::onCheckBoxCancelled, this, std::placeholders::_1));

	if (disable)
	{
		newCB->disable();
	}

	parent->addChild(newCB);

	return newCB;
}

Voxel::UI::ProgressTimer* Voxel::UITestScene::addHorProgBar(const std::string & name, const glm::vec2 & pos, const float percentage, const bool ccw, Voxel::UI::TransformNode * parent)
{
	auto bg = Voxel::UI::Image::createFromSpriteSheet(name + "Bg", "DebugSpriteSheet", "debug_progress_hor_bg.png");
	bg->setPosition(pos);
	parent->addChild(bg);

	Voxel::UI::ProgressTimer* pg = nullptr;

	if (ccw)
	{
		pg = Voxel::UI::ProgressTimer::create(name, "DebugSpriteSheet", "debug_progress_hor.png", Voxel::UI::ProgressTimer::Type::HORIZONTAL, Voxel::UI::ProgressTimer::Direction::COUNTER_CLOCK_WISE);
	}
	else
	{
		pg = Voxel::UI::ProgressTimer::create(name, "DebugSpriteSheet", "debug_progress_hor.png");
	}

	pg->setPercentage(percentage);
	pg->setOnMouseEnterCallback(std::bind(&UITestScene::onMouseEnter, this, std::placeholders::_1));
	pg->setOnMouseExitCallback(std::bind(&UITestScene::onMouseExit, this, std::placeholders::_1));
	pg->setOnMouseMoveCallback(std::bind(&UITestScene::onMouseMove, this, std::placeholders::_1));
	pg->setOnMousePressedCallback(std::bind(&UITestScene::onMousePressed, this, std::placeholders::_1, std::placeholders::_2));
	pg->setOnMouseReleasedCallback(std::bind(&UITestScene::onMouseReleased, this, std::placeholders::_1, std::placeholders::_2));

	bg->addChild(pg);

	return pg;
}

Voxel::UI::ProgressTimer * Voxel::UITestScene::addVerProgBar(const std::string & name, const glm::vec2 & pos, const float percentage, const bool ccw, Voxel::UI::TransformNode * parent)
{
	auto bg = Voxel::UI::Image::createFromSpriteSheet(name + "Bg", "DebugSpriteSheet", "debug_progress_ver_bg.png");
	bg->setPosition(pos);
	parent->addChild(bg);

	Voxel::UI::ProgressTimer* pg = nullptr;

	if (ccw)
	{
		pg = Voxel::UI::ProgressTimer::create(name, "DebugSpriteSheet", "debug_progress_ver.png", Voxel::UI::ProgressTimer::Type::VERTICAL, Voxel::UI::ProgressTimer::Direction::COUNTER_CLOCK_WISE);
	}
	else
	{
		pg = Voxel::UI::ProgressTimer::create(name, "DebugSpriteSheet", "debug_progress_ver.png", Voxel::UI::ProgressTimer::Type::VERTICAL);
	}

	pg->setPercentage(percentage);
	pg->setOnMouseEnterCallback(std::bind(&UITestScene::onMouseEnter, this, std::placeholders::_1));
	pg->setOnMouseExitCallback(std::bind(&UITestScene::onMouseExit, this, std::placeholders::_1));
	pg->setOnMouseMoveCallback(std::bind(&UITestScene::onMouseMove, this, std::placeholders::_1));
	pg->setOnMousePressedCallback(std::bind(&UITestScene::onMousePressed, this, std::placeholders::_1, std::placeholders::_2));
	pg->setOnMouseReleasedCallback(std::bind(&UITestScene::onMouseReleased, this, std::placeholders::_1, std::placeholders::_2));

	bg->addChild(pg);

	return pg;
}

Voxel::UI::ProgressTimer * Voxel::UITestScene::addProgRadial(const std::string & name, const glm::vec2 & pos, const float percentage, const bool ccw, Voxel::UI::TransformNode * parent)
{
	auto bg = Voxel::UI::Image::createFromSpriteSheet(name + "Bg", "DebugSpriteSheet", "debug_progress_radial_bg.png");
	bg->setPosition(pos);
	parent->addChild(bg);

	Voxel::UI::ProgressTimer* pg = nullptr;
	if (ccw)
	{
		pg = Voxel::UI::ProgressTimer::create(name, "DebugSpriteSheet", "debug_progress_radial.png", Voxel::UI::ProgressTimer::Type::RADIAL, Voxel::UI::ProgressTimer::Direction::COUNTER_CLOCK_WISE);
	}
	else
	{
		pg = Voxel::UI::ProgressTimer::create(name, "DebugSpriteSheet", "debug_progress_radial.png", Voxel::UI::ProgressTimer::Type::RADIAL);
	}

	pg->setPercentage(percentage);
	pg->setOnMouseEnterCallback(std::bind(&UITestScene::onMouseEnter, this, std::placeholders::_1));
	pg->setOnMouseExitCallback(std::bind(&UITestScene::onMouseExit, this, std::placeholders::_1));
	pg->setOnMouseMoveCallback(std::bind(&UITestScene::onMouseMove, this, std::placeholders::_1));
	pg->setOnMousePressedCallback(std::bind(&UITestScene::onMousePressed, this, std::placeholders::_1, std::placeholders::_2));
	pg->setOnMouseReleasedCallback(std::bind(&UITestScene::onMouseReleased, this, std::placeholders::_1, std::placeholders::_2));

	bg->addChild(pg);

	return pg;
}

Voxel::UI::Slider * Voxel::UITestScene::addHorSlider(const std::string & name, const glm::vec2 & pos, const float minValue, const float maxValue, const float value, const bool disable, Voxel::UI::TransformNode * parent)
{
	auto newSlider = Voxel::UI::Slider::create(name, "DebugSpriteSheet", "debug_hor_slider_bar.png", "debug_hor_slider_button.png", Voxel::UI::Slider::Type::HORIZONTAL, minValue, maxValue);

	newSlider->setValue(value);
	newSlider->setPosition(pos);
	newSlider->setOnMouseEnterCallback(std::bind(&UITestScene::onMouseEnter, this, std::placeholders::_1));
	newSlider->setOnMouseExitCallback(std::bind(&UITestScene::onMouseExit, this, std::placeholders::_1));
	newSlider->setOnMouseMoveCallback(std::bind(&UITestScene::onMouseMove, this, std::placeholders::_1));
	newSlider->setOnMousePressedCallback(std::bind(&UITestScene::onMousePressed, this, std::placeholders::_1, std::placeholders::_2));
	newSlider->setOnMouseReleasedCallback(std::bind(&UITestScene::onMouseReleased, this, std::placeholders::_1, std::placeholders::_2));
	newSlider->setOnButtonPressed(std::bind(&UITestScene::onSliderButtonPrssed, this, std::placeholders::_1));
	newSlider->setOnBarPressed(std::bind(&UITestScene::onSliderBarPressed, this, std::placeholders::_1));
	newSlider->setOnValueChange(std::bind(&UITestScene::onSliderValueChange, this, std::placeholders::_1));
	newSlider->setOnFinished(std::bind(&UITestScene::onSliderFinished, this, std::placeholders::_1));

	if (disable)
	{
		newSlider->disable();
	}

	parent->addChild(newSlider);

	return newSlider;
}

Voxel::UI::Slider * Voxel::UITestScene::addVerSlider(const std::string & name, const glm::vec2 & pos, const float minValue, const float maxValue, const float value, const bool disable, Voxel::UI::TransformNode * parent)
{
	auto newSlider = Voxel::UI::Slider::create(name, "DebugSpriteSheet", "debug_ver_slider_bar.png", "debug_ver_slider_button.png", Voxel::UI::Slider::Type::VERTICAL, minValue, maxValue);

	newSlider->setValue(value);
	newSlider->setPosition(pos);
	newSlider->setOnMouseEnterCallback(std::bind(&UITestScene::onMouseEnter, this, std::placeholders::_1));
	newSlider->setOnMouseExitCallback(std::bind(&UITestScene::onMouseExit, this, std::placeholders::_1));
	newSlider->setOnMouseMoveCallback(std::bind(&UITestScene::onMouseMove, this, std::placeholders::_1));
	newSlider->setOnMousePressedCallback(std::bind(&UITestScene::onMousePressed, this, std::placeholders::_1, std::placeholders::_2));
	newSlider->setOnMouseReleasedCallback(std::bind(&UITestScene::onMouseReleased, this, std::placeholders::_1, std::placeholders::_2));
	newSlider->setOnButtonPressed(std::bind(&UITestScene::onSliderButtonPrssed, this, std::placeholders::_1));
	newSlider->setOnBarPressed(std::bind(&UITestScene::onSliderBarPressed, this, std::placeholders::_1));
	newSlider->setOnValueChange(std::bind(&UITestScene::onSliderValueChange, this, std::placeholders::_1));
	newSlider->setOnFinished(std::bind(&UITestScene::onSliderFinished, this, std::placeholders::_1));

	if (disable)
	{
		newSlider->disable();
	}

	parent->addChild(newSlider);

	return newSlider;
}

Voxel::UI::NinePatchImage * Voxel::UITestScene::addNinePatchImage(const std::string & name, const glm::vec2 & pos, const glm::vec4 & paddings, const glm::vec2& bodySize, Voxel::UI::TransformNode * parent)
{
	auto newNinePatchImg = NinePatchImage::create("name", "DebugSpriteSheet", "debug_nine_patch_image.png", paddings.x, paddings.y, paddings.z, paddings.w, bodySize);
	newNinePatchImg->setPosition(pos);
	newNinePatchImg->setOnMouseEnterCallback(std::bind(&UITestScene::onMouseEnter, this, std::placeholders::_1));
	newNinePatchImg->setOnMouseExitCallback(std::bind(&UITestScene::onMouseExit, this, std::placeholders::_1));
	newNinePatchImg->setOnMouseMoveCallback(std::bind(&UITestScene::onMouseMove, this, std::placeholders::_1));
	newNinePatchImg->setOnMousePressedCallback(std::bind(&UITestScene::onMousePressed, this, std::placeholders::_1, std::placeholders::_2));
	newNinePatchImg->setOnMouseReleasedCallback(std::bind(&UITestScene::onMouseReleased, this, std::placeholders::_1, std::placeholders::_2));

	parent->addChild(newNinePatchImg);

	return newNinePatchImg;
}

Voxel::UI::Image * Voxel::UITestScene::addUIHiers(const std::string & name, const glm::vec2 & pos, const int number, const int z, Voxel::UI::TransformNode * parent)
{
	auto uiHier = Image::createFromSpriteSheet(name, "DebugSpriteSheet", "debug_" + std::to_string(number) + ".png");

	uiHier->setPosition(pos);

	uiHier->setOnMouseEnterCallback(std::bind(&UITestScene::onMouseEnter, this, std::placeholders::_1));
	uiHier->setOnMouseExitCallback(std::bind(&UITestScene::onMouseExit, this, std::placeholders::_1));
	uiHier->setOnMouseMoveCallback(std::bind(&UITestScene::onMouseMove, this, std::placeholders::_1));
	uiHier->setOnMousePressedCallback(std::bind(&UITestScene::onMousePressed, this, std::placeholders::_1, std::placeholders::_2));
	uiHier->setOnMouseReleasedCallback(std::bind(&UITestScene::onMouseReleased, this, std::placeholders::_1, std::placeholders::_2));
	uiHier->setInteractable();

	parent->addChild(uiHier, z);

	return uiHier;
}

void Voxel::UITestScene::onEnter()
{
}

void Voxel::UITestScene::onEnterFinished()
{
	Application::getInstance().getGLView()->setVsync(true);
	Application::getInstance().getGLView()->onFPSCounted = std::bind(&UITestScene::onFPSUpdate, this, std::placeholders::_1);
}

void Voxel::UITestScene::onExit()
{
	Application::getInstance().getGLView()->setVsync(false);
	Application::getInstance().getGLView()->onFPSCounted = nullptr;
}

void Voxel::UITestScene::onExitFinished()
{
}

void Voxel::UITestScene::update(const float delta)
{
	if (canvas)
	{
		canvas->update(delta);

		updateMouseMove(delta);
		updateMousePress();
		updateMouseRelease();
	}
}

bool Voxel::UITestScene::updateMouseMove(const float delta)
{
	if (canvas)
	{
		auto mouseMovedDist = input->getMouseMovedDistance();
		return canvas->updateMouseMove(cursor->getPosition(), glm::vec2(mouseMovedDist.x, -mouseMovedDist.y));
	}

	return false;
}

bool Voxel::UITestScene::updateMousePress()
{
	if (canvas)
	{
		if (input->getMouseDown(GLFW_MOUSE_BUTTON_1, true))
		{
			return canvas->updateMousePress(cursor->getPosition(), GLFW_MOUSE_BUTTON_1);
		}
	}

	return false;
}

bool Voxel::UITestScene::updateMouseRelease()
{
	if (canvas)
	{
		if (input->getMouseUp(GLFW_MOUSE_BUTTON_1, true))
		{
			return canvas->updateMouseRelease(cursor->getPosition(), GLFW_MOUSE_BUTTON_1);
		}
	}

	return false;
}

void Voxel::UITestScene::updateKey()
{
	if (input->getKeyDown(GLFW_KEY_V, true))
	{
		Voxel::Application::getInstance().getGLView()->setVsync(!Voxel::Application::getInstance().getGLView()->isVsyncEnabled());
	}
}

void Voxel::UITestScene::render()
{
	if (canvas)
	{
		canvas->render();
	}
}

void Voxel::UITestScene::release()
{
	if (canvas)
	{
		delete canvas;
	}
}

void Voxel::UITestScene::onExitButtonClicked(Voxel::UI::Button* sender)
{
	Voxel::Application::getInstance().getGLView()->close();
}

void Voxel::UITestScene::onReturnToMainMenuClicked(Voxel::UI::Button* sender)
{
	Voxel::Application::getInstance().getDirector()->replaceScene(Voxel::Director::SceneName::MENU_SCENE, 0.5f);
}

void Voxel::UITestScene::onButtonTriggered(Voxel::UI::Button * sender)
{
	std::cout << "Button \"" + sender->getName() + "\" triggered\n";

	if (sender->getID() == setBgColorButton->getID())
	{
		Application::getInstance().getGLView()->setClearColor(cp->getRGB());
	}
}

void Voxel::UITestScene::onButtonCancelled(Voxel::UI::Button * sender)
{
	std::cout << "Button \"" + sender->getName() + "\" cancelled\n";
}

void Voxel::UITestScene::onCheckBoxSelected(Voxel::UI::CheckBox * sender)
{
	std::cout << "Checkbox \"" + sender->getName() + "\" selected\n";
}

void Voxel::UITestScene::onCheckBoxDeselected(Voxel::UI::CheckBox * sender)
{
	std::cout << "Checkbox \"" + sender->getName() + "\" deselected\n";
}

void Voxel::UITestScene::onCheckBoxCancelled(Voxel::UI::CheckBox * sender)
{
	std::cout << "Checkbox \"" + sender->getName() + "\" cancelled\n";
}

void Voxel::UITestScene::onSliderButtonPrssed(Voxel::UI::Slider * sender)
{
	std::cout << "Slider \"" + sender->getName() + "\"'s button pressed. Value = " + std::to_string(sender->getValue()) + "\n";
}

void Voxel::UITestScene::onSliderBarPressed(Voxel::UI::Slider * sender)
{
	std::cout << "Slider \"" + sender->getName() + "\"'s bar pressed. Value = " + std::to_string(sender->getValue()) + "\n";
}

void Voxel::UITestScene::onSliderValueChange(Voxel::UI::Slider * sender)
{
	std::cout << "Slider \"" + sender->getName() + "\" value changed to " + std::to_string(sender->getValue()) + "\n";

	if (sender->getID() == cpSlider->getID())
	{
		cp->setH(cpSlider->getValue());
	}
}

void Voxel::UITestScene::onSliderFinished(Voxel::UI::Slider * sender)
{
	std::cout << "Slider \"" + sender->getName() + "\" finished with value = " + std::to_string(sender->getValue()) + "\n";
}

void Voxel::UITestScene::onMouseEnter(Voxel::UI::TransformNode * sender)
{
	std::cout << "Mouse enters \"" + sender->getName() + "\"\n";
}

void Voxel::UITestScene::onMouseExit(Voxel::UI::TransformNode * sender)
{
	std::cout << "Mouse exits \"" + sender->getName() + "\"\n";
}

void Voxel::UITestScene::onMouseMove(Voxel::UI::TransformNode * sender)
{
	std::cout << "Mouse moves on \"" + sender->getName() + "\"\n";
}

void Voxel::UITestScene::onMousePressed(Voxel::UI::TransformNode * sender, const int button)
{
	std::cout << "Mouse pressed on \"" + sender->getName() + "\" with mouse button: " + std::to_string(button) + "\n";
}

void Voxel::UITestScene::onMouseReleased(Voxel::UI::TransformNode * sender, const int button)
{
	std::cout << "Mouse released in \"" + sender->getName() + "\"'s bounding box with mouse button: " + std::to_string(button) + "\n";
}

void Voxel::UITestScene::onFPSUpdate(int fps)
{
	if (fpsLabel)
	{
		fpsLabel->setText("FPS: " + std::to_string(fps));
	}
}
