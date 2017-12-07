#include "DebugConsole.h"

#if V_DEBUG && V_DEBUG_CONSOLE

// cpp
#include <sstream>
#include <iomanip>
#include <algorithm>

// voxel
#include "Utility.h"
#include "Random.h"
#include "Application.h"
#include "ChunkUtil.h"
#include "InputHandler.h"
#include "Player.h"
#include "Game.h"
#include "World.h"
#include "Region.h"
#include "Camera.h"
#include "ChunkMap.h"
#include "Setting.h"
#include "Calendar.h"
#include "TreeBuilder.h"
#include "UIActions.h"

using namespace Voxel;

DebugConsole::DebugConsole()
	: openingConsole(false)
	, debugCanvas(nullptr)
	, debugOutputVisibility(false)
	, fpsNumber(nullptr)
	, resolutionNumber(nullptr)
	, vsyncMode(nullptr)
	, hardwardInfo(nullptr)
	, mousePosition(nullptr)
	, cameraPosition(nullptr)
	, cameraRotation(nullptr)
	, playerPosition(nullptr)
	, playerRotation(nullptr)
	, playerLookingAt(nullptr)
	, chunkNumbers(nullptr)
	, lastCommandIndex(0)
	, player(nullptr)
	, game(nullptr)
	, world(nullptr)
	, chunkMap(nullptr)
	, settingPtr(nullptr)
	, calendar(nullptr)
#if V_DEBUG && V_DEBUG_UI_TEST
	, testImage(nullptr)
	, testAnimatedImage(nullptr)
	, testText(nullptr)
	, testButton(nullptr)
#endif
{
	auto res = Application::getInstance().getGLView()->getScreenSize();
	debugCanvas = new UI::Canvas(glm::vec2(res), glm::vec2(0));

	init();

	GLView::onFPSCounted = std::bind(&DebugConsole::onFPSUpdate, this, std::placeholders::_1);
}

DebugConsole::~DebugConsole()
{
	if (debugCanvas)
	{
		delete debugCanvas;
	}
}

void Voxel::DebugConsole::init()
{
	auto resolution = Application::getInstance().getGLView()->getScreenSize();
	this->settingPtr = &Setting::getInstance();

	commandInputField = UI::Image::createFromSpriteSheet("cmdInputField", "UISpriteSheet", "2x2.png");
	commandInputField->setPivot(glm::vec2(0, -0.5f));
	commandInputField->setCoordinateOrigin(glm::vec2(0, -0.5f));
	commandInputField->setScale(glm::vec2(resolution.x * 0.5f, 10.0f));
	commandInputField->setOpacity(0.45f);
	commandInputField->setVisibility(false);

	debugCanvas->addChild(commandInputField, 0);

	commandHistoryBg = UI::Image::createFromSpriteSheet("cmdHistoryBg", "UISpriteSheet", "2x2.png");
	commandHistoryBg->setCoordinateOrigin(glm::vec2(0, -0.5f));
	commandHistoryBg->setPivot(glm::vec2(0, -0.5f));
	commandHistoryBg->setScale(glm::vec2(resolution.x * 0.5f, 85.0f));
	commandHistoryBg->setOpacity(0.45f);
	commandHistoryBg->setVisibility(false);

	debugCanvas->addChild(commandHistoryBg, 0);

	command = UI::Text::create("cmd", DefaultCommandInputText, 1);
	command->setPosition(glm::vec2(5.0f, 15.0f));
	command->setPivot(glm::vec2(-0.5f, 0.5f));
	command->setCoordinateOrigin(glm::vec2(-0.5f, -0.5f));
	command->setVisibility(false);

	debugCanvas->addChild(command, 0);

	commandHistorys = UI::Text::create("cmdHistorys", "_", 1);
	commandHistorys->setPosition(glm::vec2(5.0f, 23.0f));
	commandHistorys->setText("");
	commandHistorys->setPivot(glm::vec2(-0.5f, -0.5f));
	commandHistorys->setCoordinateOrigin(glm::vec2(-0.5f, -0.5f));
	commandHistorys->setVisibility(false);

	debugCanvas->addChild(commandHistorys, 0);

	auto glview = Application::getInstance().getGLView();

	auto CPUName = glview->getCPUName();
	auto GLVersion = glview->getGLVersion();
	auto GPUVendor = glview->getGPUVendor();
	auto GPURenderer = glview->getGPURenderer();

	glm::vec4 color = glm::vec4(1.0f);
	glm::vec4 outlineColor = glm::vec4(0, 0, 0, 1.0f);
	const int fontID = 2;

	fpsNumber = UI::Text::createWithOutline("fpsNumber", "fps: 00000", fontID, outlineColor);
	fpsNumber->setPosition(glm::vec2(5.0f, -5.0f));
	fpsNumber->setPivot(glm::vec2(-0.5f, 0.5f));
	fpsNumber->setCoordinateOrigin(glm::vec2(-0.5f, 0.5f));
	fpsNumber->setVisibility(false);
	debugCanvas->addChild(fpsNumber, 0);

	resolutionNumber = UI::Text::createWithOutline("resolutionNumber", "resolution: " + std::to_string(resolution.x) + " x " + std::to_string(resolution.y), fontID, outlineColor);
	resolutionNumber->setPivot(glm::vec2(-0.5f, 0.5f));
	resolutionNumber->setPosition(glm::vec2(5.0f, -21.0f));
	resolutionNumber->setCoordinateOrigin(glm::vec2(-0.5f, 0.5f));
	resolutionNumber->setVisibility(false);
	debugCanvas->addChild(resolutionNumber, 0);

	auto vsync = Application::getInstance().getGLView()->isVsyncEnabled();
	vsyncMode = UI::Text::createWithOutline("vsyncMode", "vsync: " + (vsync ? std::string("true") : std::string("false")), fontID, outlineColor);
	vsyncMode->setPivot(glm::vec2(-0.5f, 0.5f));
	vsyncMode->setPosition(glm::vec2(5.0f, -37.0f));
	vsyncMode->setCoordinateOrigin(glm::vec2(-0.5f, 0.5f));
	vsyncMode->setVisibility(false);
	debugCanvas->addChild(vsyncMode, 0);

	hardwardInfo = UI::Text::createWithOutline("hardwardInfo", "CPU:" + CPUName + "\n" + GPUVendor + "\n" + GPURenderer + "\n" + GLVersion, fontID, outlineColor);
	hardwardInfo->setPivot(glm::vec2(-0.5f, 0.5f));
	hardwardInfo->setPosition(glm::vec2(5.0f, -61.0f));
	hardwardInfo->setCoordinateOrigin(glm::vec2(-0.5f, 0.5f));
	hardwardInfo->setVisibility(false);
	debugCanvas->addChild(hardwardInfo, 0);

	playerPosition = UI::Text::createWithOutline("playerPosition", "player position: 00000.00, 00000.00, 00000.00", fontID, outlineColor);
	playerPosition->setPivot(glm::vec2(-0.5f, 0.5f));
	playerPosition->setPosition(glm::vec2(5.0f, -131.0f));
	playerPosition->setCoordinateOrigin(glm::vec2(-0.5f, 0.5f));
	playerPosition->setVisibility(false);
	debugCanvas->addChild(playerPosition, 0);

	playerRotation = UI::Text::createWithOutline("playerRotation", "player rotation: 00000.00, 00000.00, 00000.00 (Facing north)", fontID, outlineColor);
	playerRotation->setPivot(glm::vec2(-0.5f, 0.5f));
	playerRotation->setPosition(glm::vec2(5.0f, -147.0f));
	playerRotation->setCoordinateOrigin(glm::vec2(-0.5f, 0.5f));
	playerRotation->setVisibility(false);
	debugCanvas->addChild(playerRotation, 0);

	playerChunkPosition = UI::Text::createWithOutline("playerChunkPosition", "player chunk position: 00000.00, 00000.00, 00000.00", fontID, outlineColor);
	playerChunkPosition->setPivot(glm::vec2(-0.5f, 0.5f));
	playerChunkPosition->setPosition(glm::vec2(5.0f, -163.0f));
	playerChunkPosition->setCoordinateOrigin(glm::vec2(-0.5f, 0.5f));
	playerChunkPosition->setVisibility(false);
	debugCanvas->addChild(playerChunkPosition, 0);

	playerLookingAt = UI::Text::createWithOutline("playerLookingAt", "player looking at: 000000, 000000, 000000 Face (front)", fontID, outlineColor);
	playerLookingAt->setPivot(glm::vec2(-0.5f, 0.5f));
	playerLookingAt->setPosition(glm::vec2(5.0f, -179.0f));
	playerLookingAt->setCoordinateOrigin(glm::vec2(-0.5f, 0.5f));
	playerLookingAt->setVisibility(false);
	debugCanvas->addChild(playerLookingAt, 0);

	chunkNumbers = UI::Text::createWithOutline("chunkNumbers", "Chunks: 00000 / 00000 / 00000", fontID, outlineColor);
	chunkNumbers->setPosition(glm::vec2(5.0f, -201.f));
	chunkNumbers->setPivot(glm::vec2(-0.5f, 0.5f));
	chunkNumbers->setCoordinateOrigin(glm::vec2(-0.5f, 0.5f));
	chunkNumbers->setVisibility(false);
	debugCanvas->addChild(chunkNumbers, 0);

	biomeAndTerrainInfo = UI::Text::createWithOutline("biome", "biome: type / 00.00 / 00.00", fontID, outlineColor);
	biomeAndTerrainInfo->setPosition(glm::vec2(5.0f, -217.0f));
	biomeAndTerrainInfo->setPivot(glm::vec2(-0.5f, 0.5f));
	biomeAndTerrainInfo->setCoordinateOrigin(glm::vec2(-0.5f, 0.5f));
	biomeAndTerrainInfo->setVisibility(false);
	debugCanvas->addChild(biomeAndTerrainInfo, 0);

	regionID = UI::Text::createWithOutline("region", "region: 000", fontID, outlineColor);
	regionID->setPivot(glm::vec2(-0.5f, 0.5f));
	regionID->setPosition(glm::vec2(5.0f, -233.0f));
	regionID->setCoordinateOrigin(glm::vec2(-0.5f, 0.5f));
	regionID->setVisibility(false);
	debugCanvas->addChild(regionID, 0);

	drawCallAndVertCount = UI::Text::createWithOutline("drawCallAndVertCount", "Draw calls: ----, vertices: -------", fontID, outlineColor);
	drawCallAndVertCount->setPivot(glm::vec2(-0.5f, 0.5f));
	drawCallAndVertCount->setPosition(glm::vec2(5.0f, -265.0f));
	drawCallAndVertCount->setCoordinateOrigin(glm::vec2(-0.5f, 0.5f));
	drawCallAndVertCount->setVisibility(false);
	debugCanvas->addChild(drawCallAndVertCount, 0);

	debugCanvas->print(0);

#if V_DEBUG && V_DEBUG_UI_TEST
	testImage = Voxel::UI::Image::createFromSpriteSheet("testImage", "UISpriteSheet", "debug/debug_image.png");
	testImage->setPosition(glm::vec2(-145.0f, 140.0f));
	debugCanvas->addChild(testImage, 100);

	testAnimatedImage = Voxel::UI::AnimatedImage::create("testAnimImage", "UISpriteSheet", "debug/debug_anim_frame.png", 5, 1.0f, true);
	testAnimatedImage->setPosition(glm::vec2(-300.0f, 320.0f));
	debugCanvas->addChild(testAnimatedImage, 100);

	testText = Voxel::UI::Text::createWithOutline("testText", "Lorem Ipsum is simply dummy text of the printing and typesetting industry.\nLorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. \nIt has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. \nIt was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.", 2, glm::vec3(0.0f), Voxel::UI::Text::ALIGN::CENTER);
	//testText = Voxel::UI::Text::createWithOutline("testText", "Lorem Ipsum is simply dummy text of the printing and typesetting i", 2, glm::vec3(0.0f), Voxel::UI::Text::ALIGN::CENTER);
	//testText = Voxel::UI::Text::createWithOutline("testText", "ABCD", 2, glm::vec3(0.0f), Voxel::UI::Text::ALIGN::CENTER);
	//testText = Voxel::UI::Text::create("testText", "Vsync: false", 1, Voxel::UI::Text::ALIGN::LEFT);
	//testText->setPosition(glm::vec2(5.0f, -281.0f));
	testText->setPosition(glm::vec2(0.0f, -420.0f));
	//testText->setPivot(glm::vec2(-0.5f, 0.0f));
	//testText->setPivot(glm::vec2(0.0f, 0.0f));
	//testText->setCoordinateOrigin(glm::vec2(-0.5f, 0.5f));
	debugCanvas->addChild(testText, 100);

	testButton = Voxel::UI::Button::create("testButton", "UISpriteSheet", "debug/debug_button.png");
	testButton->setPosition(glm::vec2(55.0f, 265.0f));
	debugCanvas->addChild(testButton, 100);

	testDisabledButton = Voxel::UI::Button::create("testDisabledButton", "UISpriteSheet", "debug/debug_button.png");
	testDisabledButton->setPosition(glm::vec2(165.0f, 265.0f));
	testDisabledButton->disable();
	debugCanvas->addChild(testDisabledButton, 100);

	auto tempBtn = Voxel::UI::Button::create("tempBtn", "UISpriteSheet", "debug/debug_button.png");
	tempBtn->setPosition(0, -10.0f);
	testButton->addChild(tempBtn, 100);
	auto tempBtn1 = Voxel::UI::Button::create("tempBtn1", "UISpriteSheet", "debug/debug_button.png");
	tempBtn1->setPosition(0, -10.0f);
	tempBtn->addChild(tempBtn1, 100);
	auto tempBtn2 = Voxel::UI::Button::create("tempBtn2", "UISpriteSheet", "debug/debug_button.png");
	tempBtn2->setPosition(0, -10.0f);
	tempBtn1->addChild(tempBtn2, 100);

	testCheckBox = Voxel::UI::CheckBox::create("testCheckBox", "UISpriteSheet", "debug/debug_checkbox.png");
	testCheckBox->setPosition(250.0f, 265.0f);
	debugCanvas->addChild(testCheckBox, 100);

	testDisabledCheckBox = Voxel::UI::CheckBox::create("testDisabledCheckBox", "UISpriteSheet", "debug/debug_checkbox.png");
	testDisabledCheckBox->setPosition(300.0f, 265.0f);
	testDisabledCheckBox->disable();
	debugCanvas->addChild(testDisabledCheckBox, 100);

	const float horY = 400.0f;
	const float horX = -200.0f;
	const float horGapY = 40.0f;

	// horizontal progress timer 0 percent

	auto testProgHorBar0Bg = Voxel::UI::Image::createFromSpriteSheet("testProgHorBar0Bg", "UISpriteSheet", "debug/debug_progress_hor_bg.png");
	testProgHorBar0Bg->setPosition(glm::vec2(horX, horY));
	debugCanvas->addChild(testProgHorBar0Bg, 100);

	testProgHorBar0 = Voxel::UI::ProgressTimer::create("testProgHorBar0", "UISpriteSheet", "debug/debug_progress_hor.png", Voxel::UI::ProgressTimer::Type::HORIZONTAL, Voxel::UI::ProgressTimer::Direction::CLOCK_WISE);
	testProgHorBar0->setPercentage(0);
	testProgHorBar0Bg->addChild(testProgHorBar0, 100);

	// horizontal progress timer 50 percent

	auto testProgHorBar50Bg = Voxel::UI::Image::createFromSpriteSheet("testProgHorBar50Bg", "UISpriteSheet", "debug/debug_progress_hor_bg.png");
	testProgHorBar50Bg->setPosition(glm::vec2(horX, horY - (horGapY)));
	debugCanvas->addChild(testProgHorBar50Bg, 100);

	testProgHorBar50 = Voxel::UI::ProgressTimer::create("testProgHorBar50", "UISpriteSheet", "debug/debug_progress_hor.png", Voxel::UI::ProgressTimer::Type::HORIZONTAL, Voxel::UI::ProgressTimer::Direction::CLOCK_WISE);
	testProgHorBar50->setPercentage(50);
	testProgHorBar50Bg->addChild(testProgHorBar50, 100);

	// horizontal progress timer 100 percent

	auto testProgHorBar100Bg = Voxel::UI::Image::createFromSpriteSheet("testProgHorBar100Bg", "UISpriteSheet", "debug/debug_progress_hor_bg.png");
	testProgHorBar100Bg->setPosition(glm::vec2(horX, horY - (horGapY * 2.0f)));
	debugCanvas->addChild(testProgHorBar100Bg, 100);

	testProgHorBar100 = Voxel::UI::ProgressTimer::create("testProgHorBar100", "UISpriteSheet", "debug/debug_progress_hor.png", Voxel::UI::ProgressTimer::Type::HORIZONTAL, Voxel::UI::ProgressTimer::Direction::CLOCK_WISE);
	testProgHorBar100->setPercentage(100);
	testProgHorBar100Bg->addChild(testProgHorBar100, 100);

	// horizontal progress timer action test

	auto testProgHorBarActionBg = Voxel::UI::Image::createFromSpriteSheet("testProgHorBarActionBg", "UISpriteSheet", "debug/debug_progress_hor_bg.png");
	testProgHorBarActionBg->setPosition(glm::vec2(horX, horY - (horGapY * 3.0f)));
	debugCanvas->addChild(testProgHorBarActionBg, 100);

	auto testProgHorActionBar = Voxel::UI::ProgressTimer::create("testProgHorActionBar", "UISpriteSheet", "debug/debug_progress_hor.png", Voxel::UI::ProgressTimer::Type::HORIZONTAL, Voxel::UI::ProgressTimer::Direction::CLOCK_WISE);
	testProgHorActionBar->setPercentage(100);
	auto testProgHorSeq = Voxel::UI::Sequence::create({ Voxel::UI::Delay::create(1.0f), Voxel::UI::ProgressTo::create(1.0f, 0), Voxel::UI::Delay::create(1.0f), Voxel::UI::ProgressTo::create(1.0f, 100) }, true);
	testProgHorActionBar->runAction(testProgHorSeq);
	testProgHorBarActionBg->addChild(testProgHorActionBar, 100);

	const float horXFlip = horX + 110.0f;

	// horizontal ccw progress timer 0 percent

	auto testProgHorBar0ccwBg = Voxel::UI::Image::createFromSpriteSheet("testProgHorBar0ccwBg", "UISpriteSheet", "debug/debug_progress_hor_bg.png");
	testProgHorBar0ccwBg->setPosition(glm::vec2(horXFlip, 400.0f));
	debugCanvas->addChild(testProgHorBar0ccwBg, 100);

	testProgHorBar0ccw = Voxel::UI::ProgressTimer::create("testProgHorBar0ccw", "UISpriteSheet", "debug/debug_progress_hor.png", Voxel::UI::ProgressTimer::Type::HORIZONTAL, Voxel::UI::ProgressTimer::Direction::COUNTER_CLOCK_WISE);
	testProgHorBar0ccw->setPercentage(0);
	testProgHorBar0ccwBg->addChild(testProgHorBar0ccw, 100);

	// horizontal ccw progress timer 50 percent

	auto testProgHorBar50ccwBg = Voxel::UI::Image::createFromSpriteSheet("testProgHorBar50ccwBg", "UISpriteSheet", "debug/debug_progress_hor_bg.png");
	testProgHorBar50ccwBg->setPosition(glm::vec2(horXFlip, 360.0f));
	debugCanvas->addChild(testProgHorBar50ccwBg, 100);

	testProgHorBar50ccw = Voxel::UI::ProgressTimer::create("testProgHorBar50ccw", "UISpriteSheet", "debug/debug_progress_hor.png", Voxel::UI::ProgressTimer::Type::HORIZONTAL, Voxel::UI::ProgressTimer::Direction::COUNTER_CLOCK_WISE);
	testProgHorBar50ccw->setPercentage(50);
	testProgHorBar50ccwBg->addChild(testProgHorBar50ccw, 100);

	// horizontal ccw progress timer 100 percent

	auto testProgHorBar100ccwBg = Voxel::UI::Image::createFromSpriteSheet("testProgHorBar100ccwBg", "UISpriteSheet", "debug/debug_progress_hor_bg.png");
	testProgHorBar100ccwBg->setPosition(glm::vec2(horXFlip, 320.0f));
	debugCanvas->addChild(testProgHorBar100ccwBg, 100);

	testProgHorBar100ccw = Voxel::UI::ProgressTimer::create("testProgHorBar100ccw", "UISpriteSheet", "debug/debug_progress_hor.png", Voxel::UI::ProgressTimer::Type::HORIZONTAL, Voxel::UI::ProgressTimer::Direction::COUNTER_CLOCK_WISE);
	testProgHorBar100ccw->setPercentage(100);
	testProgHorBar100ccwBg->addChild(testProgHorBar100ccw, 100);

	// horizontal ccw progress timer action test

	auto testProgHorCcwActionBarBg = Voxel::UI::Image::createFromSpriteSheet("testProgHorCcwActionBarBg", "UISpriteSheet", "debug/debug_progress_hor_bg.png");
	testProgHorCcwActionBarBg->setPosition(glm::vec2(horXFlip, horY - (horGapY * 3.0f)));
	debugCanvas->addChild(testProgHorCcwActionBarBg, 100);

	auto testProgHorCcwActionBar = Voxel::UI::ProgressTimer::create("testProgHorCcwActionBar", "UISpriteSheet", "debug/debug_progress_hor.png", Voxel::UI::ProgressTimer::Type::HORIZONTAL, Voxel::UI::ProgressTimer::Direction::COUNTER_CLOCK_WISE);
	testProgHorCcwActionBar->setPercentage(100);
	auto testProgHorCcwSeq = Voxel::UI::Sequence::create({ Voxel::UI::Delay::create(1.0f), Voxel::UI::ProgressTo::create(1.0f, 0.0f), Voxel::UI::Delay::create(1.0f), Voxel::UI::ProgressTo::create(1.0f, 100.0f) }, true);
	testProgHorCcwActionBar->runAction(testProgHorCcwSeq);
	testProgHorCcwActionBarBg->addChild(testProgHorCcwActionBar, 100);

	const float verX = horXFlip + 110.0f;
	const float verXGap = 40.0f;
	const float verY = 360.0f;

	// vertical progress timer 0 percent

	auto testProgVerBar0Bg = Voxel::UI::Image::createFromSpriteSheet("testProgVerBar0Bg", "UISpriteSheet", "debug/debug_progress_ver_bg.png");
	testProgVerBar0Bg->setPosition(glm::vec2(verX, verY));
	debugCanvas->addChild(testProgVerBar0Bg, 100);

	testProgVerBar0 = Voxel::UI::ProgressTimer::create("testProgVerBar0", "UISpriteSheet", "debug/debug_progress_ver.png", Voxel::UI::ProgressTimer::Type::VERTICAL, Voxel::UI::ProgressTimer::Direction::CLOCK_WISE);
	testProgVerBar0->setPercentage(0);
	testProgVerBar0Bg->addChild(testProgVerBar0, 100);

	// vertical progress timer 50 percent

	auto testProgVerBar50Bg = Voxel::UI::Image::createFromSpriteSheet("testProgVerBar50Bg", "UISpriteSheet", "debug/debug_progress_ver_bg.png");
	testProgVerBar50Bg->setPosition(glm::vec2(verX + verXGap, verY));
	debugCanvas->addChild(testProgVerBar50Bg, 100);

	testProgVerBar50 = Voxel::UI::ProgressTimer::create("testProgVerBar50", "UISpriteSheet", "debug/debug_progress_ver.png", Voxel::UI::ProgressTimer::Type::VERTICAL, Voxel::UI::ProgressTimer::Direction::CLOCK_WISE);
	testProgVerBar50->setPercentage(50);
	testProgVerBar50Bg->addChild(testProgVerBar50, 100);

	// vertical progress timer 100 percent

	auto testProgVerBar100Bg = Voxel::UI::Image::createFromSpriteSheet("testProgVerBar100Bg", "UISpriteSheet", "debug/debug_progress_ver_bg.png");
	testProgVerBar100Bg->setPosition(glm::vec2(verX + (verXGap * 2.0f), verY));
	debugCanvas->addChild(testProgVerBar100Bg, 100);

	testProgVerBar100 = Voxel::UI::ProgressTimer::create("testProgVerBar100", "UISpriteSheet", "debug/debug_progress_ver.png", Voxel::UI::ProgressTimer::Type::VERTICAL, Voxel::UI::ProgressTimer::Direction::CLOCK_WISE);
	testProgVerBar100->setPercentage(100);
	testProgVerBar100Bg->addChild(testProgVerBar100, 100);

	// vertical progress timer action test

	auto testProgVerActionBarBg = Voxel::UI::Image::createFromSpriteSheet("testProgVerActionBarBg", "UISpriteSheet", "debug/debug_progress_ver_bg.png");
	testProgVerActionBarBg->setPosition(glm::vec2(verX + (verXGap * 3.0f), verY));
	debugCanvas->addChild(testProgVerActionBarBg, 100);

	auto testProgVerActionBar = Voxel::UI::ProgressTimer::create("testProgVerActionBar", "UISpriteSheet", "debug/debug_progress_ver.png", Voxel::UI::ProgressTimer::Type::VERTICAL, Voxel::UI::ProgressTimer::Direction::CLOCK_WISE);
	testProgVerActionBar->setPercentage(100);
	auto testProgVerSeq = Voxel::UI::Sequence::create({ Voxel::UI::Delay::create(1.0f), Voxel::UI::ProgressTo::create(1.0f, 0.0f), Voxel::UI::Delay::create(1.0f), Voxel::UI::ProgressTo::create(1.0f, 100.0f) }, true);
	testProgVerActionBar->runAction(testProgVerSeq);
	testProgVerActionBarBg->addChild(testProgVerActionBar, 100);

	// vertical ccw progress timer 0 percent

	auto testProgVerBar0ccwBg = Voxel::UI::Image::createFromSpriteSheet("testProgVerBar0ccwBg", "UISpriteSheet", "debug/debug_progress_ver_bg.png");
	testProgVerBar0ccwBg->setPosition(glm::vec2(verX + (verXGap * 4.0f), verY));
	debugCanvas->addChild(testProgVerBar0ccwBg, 100);

	testProgVerBar0ccw = Voxel::UI::ProgressTimer::create("testProgVerBar0ccw", "UISpriteSheet", "debug/debug_progress_ver.png", Voxel::UI::ProgressTimer::Type::VERTICAL, Voxel::UI::ProgressTimer::Direction::COUNTER_CLOCK_WISE);
	testProgVerBar0ccw->setPercentage(0);
	testProgVerBar0ccwBg->addChild(testProgVerBar0ccw, 100);

	// vertical ccw progress timer 50 percent

	auto testProgVerBar50ccwBg = Voxel::UI::Image::createFromSpriteSheet("testProgVerBar50ccwBg", "UISpriteSheet", "debug/debug_progress_ver_bg.png");
	testProgVerBar50ccwBg->setPosition(glm::vec2(verX + (verXGap * 5.0f), verY));
	debugCanvas->addChild(testProgVerBar50ccwBg, 100);

	testProgVerBar50ccw = Voxel::UI::ProgressTimer::create("testProgVerBar50ccw", "UISpriteSheet", "debug/debug_progress_ver.png", Voxel::UI::ProgressTimer::Type::VERTICAL, Voxel::UI::ProgressTimer::Direction::COUNTER_CLOCK_WISE);
	testProgVerBar50ccw->setPercentage(50);
	testProgVerBar50ccwBg->addChild(testProgVerBar50ccw, 100);

	// vertical ccw progress timer 100 percent

	auto testProgVerBar100ccwBg = Voxel::UI::Image::createFromSpriteSheet("testProgVerBar100ccwBg", "UISpriteSheet", "debug/debug_progress_ver_bg.png");
	testProgVerBar100ccwBg->setPosition(glm::vec2(verX + (verXGap * 6.0f), verY));
	debugCanvas->addChild(testProgVerBar100ccwBg, 100);

	testProgVerBar100ccw = Voxel::UI::ProgressTimer::create("testProgVerBar100ccw", "UISpriteSheet", "debug/debug_progress_ver.png", Voxel::UI::ProgressTimer::Type::VERTICAL, Voxel::UI::ProgressTimer::Direction::COUNTER_CLOCK_WISE);
	testProgVerBar100ccw->setPercentage(100);
	testProgVerBar100ccwBg->addChild(testProgVerBar100ccw, 100);

	// vertical ccw progress timer action test

	auto testProgVerCcwActionBarBg = Voxel::UI::Image::createFromSpriteSheet("testProgVerCcwActionBarBg", "UISpriteSheet", "debug/debug_progress_ver_bg.png");
	testProgVerCcwActionBarBg->setPosition(glm::vec2(verX + (verXGap * 7.0f), verY));
	debugCanvas->addChild(testProgVerCcwActionBarBg, 100);

	auto testProgVerCcwActionBar = Voxel::UI::ProgressTimer::create("testProgVerActionBar", "UISpriteSheet", "debug/debug_progress_ver.png", Voxel::UI::ProgressTimer::Type::VERTICAL, Voxel::UI::ProgressTimer::Direction::COUNTER_CLOCK_WISE);
	testProgVerCcwActionBar->setPercentage(100);
	auto testProgVerCcwSeq = Voxel::UI::Sequence::create({ Voxel::UI::Delay::create(1.0f), Voxel::UI::ProgressTo::create(1.0f, 0.0f), Voxel::UI::Delay::create(1.0f), Voxel::UI::ProgressTo::create(1.0f, 100.0f) }, true);
	testProgVerCcwActionBar->runAction(testProgVerCcwSeq);
	testProgVerCcwActionBarBg->addChild(testProgVerCcwActionBar, 100);

	const float radialY = 400.0f;
	const float radialX = 400.0f;
	const float radialXGap = 80.0f;

	// radial progress timer 0 percent

	auto testProgRadial0cwBg = Voxel::UI::Image::createFromSpriteSheet("testProgRadial0cwBg", "UISpriteSheet", "debug/debug_progress_radial_bg.png");
	testProgRadial0cwBg->setPosition(radialX, radialY);
	debugCanvas->addChild(testProgRadial0cwBg, 100);

	testProgRadial0 = Voxel::UI::ProgressTimer::create("testProgRadial0", "UISpriteSheet", "debug/debug_progress_radial.png", Voxel::UI::ProgressTimer::Type::RADIAL, Voxel::UI::ProgressTimer::Direction::CLOCK_WISE);
	testProgRadial0->setPercentage(0);
	testProgRadial0cwBg->addChild(testProgRadial0, 100);

	// radial progress timer 25 percent

	auto testProgRadial25cwBg = Voxel::UI::Image::createFromSpriteSheet("testProgRadial25cwBg", "UISpriteSheet", "debug/debug_progress_radial_bg.png");
	testProgRadial25cwBg->setPosition(radialX + radialXGap, radialY);
	debugCanvas->addChild(testProgRadial25cwBg, 100);

	testProgRadial25 = Voxel::UI::ProgressTimer::create("testProgRadial25", "UISpriteSheet", "debug/debug_progress_radial.png", Voxel::UI::ProgressTimer::Type::RADIAL, Voxel::UI::ProgressTimer::Direction::CLOCK_WISE);
	testProgRadial25->setPercentage(25);
	testProgRadial25cwBg->addChild(testProgRadial25, 100);

	// radial progress timer 50 percent

	auto testProgRadial50cwBg = Voxel::UI::Image::createFromSpriteSheet("testProgRadial50cwBg", "UISpriteSheet", "debug/debug_progress_radial_bg.png");
	testProgRadial50cwBg->setPosition(radialX + (radialXGap * 2.0f), radialY);
	debugCanvas->addChild(testProgRadial50cwBg, 100);

	testProgRadial50 = Voxel::UI::ProgressTimer::create("testProgRadial50", "UISpriteSheet", "debug/debug_progress_radial.png", Voxel::UI::ProgressTimer::Type::RADIAL, Voxel::UI::ProgressTimer::Direction::CLOCK_WISE);
	testProgRadial50->setPercentage(50);
	testProgRadial50cwBg->addChild(testProgRadial50, 100);

	// radial progress timer 75 percent

	auto testProgRadial75cwBg = Voxel::UI::Image::createFromSpriteSheet("testProgRadial75cwBg", "UISpriteSheet", "debug/debug_progress_radial_bg.png");
	testProgRadial75cwBg->setPosition(radialX + (radialXGap * 3.0f), radialY);
	debugCanvas->addChild(testProgRadial75cwBg, 100);

	testProgRadial75 = Voxel::UI::ProgressTimer::create("testProgRadial75", "UISpriteSheet", "debug/debug_progress_radial.png", Voxel::UI::ProgressTimer::Type::RADIAL, Voxel::UI::ProgressTimer::Direction::CLOCK_WISE);
	testProgRadial75->setPercentage(75);
	testProgRadial75cwBg->addChild(testProgRadial75, 100);

	// radial progress timer 100 percent

	auto testProgRadial100cwBg = Voxel::UI::Image::createFromSpriteSheet("testProgRadial100cwBg", "UISpriteSheet", "debug/debug_progress_radial_bg.png");
	testProgRadial100cwBg->setPosition(radialX + (radialXGap * 4.0f), radialY);
	debugCanvas->addChild(testProgRadial100cwBg, 100);

	testProgRadial100 = Voxel::UI::ProgressTimer::create("testProgRadial100", "UISpriteSheet", "debug/debug_progress_radial.png", Voxel::UI::ProgressTimer::Type::RADIAL, Voxel::UI::ProgressTimer::Direction::CLOCK_WISE);
	testProgRadial100->setPercentage(100);
	testProgRadial100cwBg->addChild(testProgRadial100, 100);

	// radial progress timer action test

	auto testProgRadialActionBg = Voxel::UI::Image::createFromSpriteSheet("testProgRadialActionBg", "UISpriteSheet", "debug/debug_progress_radial_bg.png");
	testProgRadialActionBg->setPosition(radialX + (radialXGap * 5.0f), radialY);
	debugCanvas->addChild(testProgRadialActionBg, 100);

	auto testProgRadialAction = Voxel::UI::ProgressTimer::create("testProgRadialAction", "UISpriteSheet", "debug/debug_progress_radial.png", Voxel::UI::ProgressTimer::Type::RADIAL, Voxel::UI::ProgressTimer::Direction::CLOCK_WISE);
	testProgRadialAction->setPercentage(100);
	auto testProgRadialSeq = Voxel::UI::Sequence::create({ Voxel::UI::Delay::create(1.0f), Voxel::UI::ProgressTo::create(1.0f, 0.0f), Voxel::UI::Delay::create(1.0f), Voxel::UI::ProgressTo::create(1.0f, 100.0f) }, true);
	testProgRadialAction->runAction(testProgRadialSeq);
	testProgRadialActionBg->addChild(testProgRadialAction, 100);

	const float radialCcwY = radialY - 80.0f;

	// radial ccw progress timer 0 percent

	auto testProgRadial0ccwBg = Voxel::UI::Image::createFromSpriteSheet("testProgRadial0ccwBg", "UISpriteSheet", "debug/debug_progress_radial_bg.png");
	testProgRadial0ccwBg->setPosition(radialX, radialCcwY);
	debugCanvas->addChild(testProgRadial0ccwBg, 100);

	testProgRadial0ccw = Voxel::UI::ProgressTimer::create("testProgRadial0ccw", "UISpriteSheet", "debug/debug_progress_radial.png", Voxel::UI::ProgressTimer::Type::RADIAL, Voxel::UI::ProgressTimer::Direction::COUNTER_CLOCK_WISE);
	testProgRadial0ccw->setPercentage(0);
	testProgRadial0ccwBg->addChild(testProgRadial0ccw, 100);

	// radial ccw progress timer 25 percent

	auto testProgRadial25ccwBg = Voxel::UI::Image::createFromSpriteSheet("testProgRadial25ccwBg", "UISpriteSheet", "debug/debug_progress_radial_bg.png");
	testProgRadial25ccwBg->setPosition(radialX + radialXGap, radialCcwY);
	debugCanvas->addChild(testProgRadial25ccwBg, 100);

	auto testProgRadial25ccw = Voxel::UI::ProgressTimer::create("testProgRadial25ccw", "UISpriteSheet", "debug/debug_progress_radial.png", Voxel::UI::ProgressTimer::Type::RADIAL, Voxel::UI::ProgressTimer::Direction::COUNTER_CLOCK_WISE);
	testProgRadial25ccw->setPercentage(25);
	testProgRadial25ccwBg->addChild(testProgRadial25ccw, 100);

	// radial ccw progress timer 50 percent

	auto testProgRadial50ccwBg = Voxel::UI::Image::createFromSpriteSheet("testProgRadial50ccwBg", "UISpriteSheet", "debug/debug_progress_radial_bg.png");
	testProgRadial50ccwBg->setPosition(radialX + (radialXGap * 2.0f), radialCcwY);
	debugCanvas->addChild(testProgRadial50ccwBg, 100);

	auto testProgRadial50ccw = Voxel::UI::ProgressTimer::create("testProgRadial50ccw", "UISpriteSheet", "debug/debug_progress_radial.png", Voxel::UI::ProgressTimer::Type::RADIAL, Voxel::UI::ProgressTimer::Direction::COUNTER_CLOCK_WISE);
	testProgRadial50ccw->setPercentage(50);
	testProgRadial50ccwBg->addChild(testProgRadial50ccw, 100);

	// radial ccw progress timer 75 percent

	auto testProgRadial75ccwBg = Voxel::UI::Image::createFromSpriteSheet("testProgRadial75ccwBg", "UISpriteSheet", "debug/debug_progress_radial_bg.png");
	testProgRadial75ccwBg->setPosition(radialX + (radialXGap * 3.0f), radialCcwY);
	debugCanvas->addChild(testProgRadial75ccwBg, 100);

	auto testProgRadial75ccw = Voxel::UI::ProgressTimer::create("testProgRadial75ccw", "UISpriteSheet", "debug/debug_progress_radial.png", Voxel::UI::ProgressTimer::Type::RADIAL, Voxel::UI::ProgressTimer::Direction::COUNTER_CLOCK_WISE);
	testProgRadial75ccw->setPercentage(75);
	testProgRadial75ccwBg->addChild(testProgRadial75ccw, 100);

	// radial ccw progress timer 100 percent

	auto testProgRadial100ccwBg = Voxel::UI::Image::createFromSpriteSheet("testProgRadial100ccwBg", "UISpriteSheet", "debug/debug_progress_radial_bg.png");
	testProgRadial100ccwBg->setPosition(radialX + (radialXGap * 4.0f), radialCcwY);
	debugCanvas->addChild(testProgRadial100ccwBg, 100);

	auto testProgRadial100ccw = Voxel::UI::ProgressTimer::create("testProgRadial100ccw", "UISpriteSheet", "debug/debug_progress_radial.png", Voxel::UI::ProgressTimer::Type::RADIAL, Voxel::UI::ProgressTimer::Direction::COUNTER_CLOCK_WISE);
	testProgRadial100ccw->setPercentage(100);
	testProgRadial100ccwBg->addChild(testProgRadial100ccw, 100);

	// radial ccw progress timer action test

	auto testProgRadialCcwActionBg = Voxel::UI::Image::createFromSpriteSheet("testProgRadialCcwActionBg", "UISpriteSheet", "debug/debug_progress_radial_bg.png");
	testProgRadialCcwActionBg->setPosition(radialX + (radialXGap * 5.0f), radialCcwY);
	debugCanvas->addChild(testProgRadialCcwActionBg, 100);

	auto testProgRadialCcwAction = Voxel::UI::ProgressTimer::create("testProgRadialCcwAction", "UISpriteSheet", "debug/debug_progress_radial.png", Voxel::UI::ProgressTimer::Type::RADIAL, Voxel::UI::ProgressTimer::Direction::COUNTER_CLOCK_WISE);
	testProgRadialCcwAction->setPercentage(100);
	auto testProgRadialCcwSeq = Voxel::UI::Sequence::create({ Voxel::UI::Delay::create(1.0f), Voxel::UI::ProgressTo::create(1.0f, 0.0f), Voxel::UI::Delay::create(1.0f), Voxel::UI::ProgressTo::create(1.0f, 100.0f) }, true);
	testProgRadialCcwAction->runAction(testProgRadialCcwSeq);
	testProgRadialCcwActionBg->addChild(testProgRadialCcwAction, 100);

	// rotate image
	auto testRotateImage = Voxel::UI::Image::createFromSpriteSheet("testRotateImage", "UISpriteSheet", "debug/debug_rotate_image.png");
	testRotateImage->setPosition(55.0f, 180.0f);
	auto rotateSeq = Voxel::UI::Sequence::create({ Voxel::UI::Delay::create(1.0f), Voxel::UI::RotateTo::create(1.5f, 360.0f), Voxel::UI::Delay::create(1.0f), Voxel::UI::RotateTo::create(1.5f, 0.0f), Voxel::UI::Delay::create(1.0f), Voxel::UI::RotateTo::create(1.5f, -360.0f), Voxel::UI::Delay::create(1.0f), Voxel::UI::RotateTo::create(1.5f, 0.0f) }, true);
	testRotateImage->runAction(rotateSeq);
	debugCanvas->addChild(testRotateImage, 100);

	// fade image
	auto testFadeImage = Voxel::UI::Image::createFromSpriteSheet("testFadeImage", "UISpriteSheet", "debug/debug_fade_image.png");
	testFadeImage->setPosition(165.0f, 180.0f);
	auto fadeSeq = Voxel::UI::Sequence::create({ Voxel::UI::Delay::create(1.0f), Voxel::UI::FadeTo::create(1.5f, 0.5f), Voxel::UI::Delay::create(1.0f), Voxel::UI::FadeTo::create(1.5f, 0.0f), Voxel::UI::Delay::create(1.0f), Voxel::UI::FadeTo::create(1.5f, 1.0f)}, true);
	testFadeImage->runAction(fadeSeq);
	debugCanvas->addChild(testFadeImage, 100);

	// scale image
	auto testScaleImage = Voxel::UI::Image::createFromSpriteSheet("testFadeImage", "UISpriteSheet", "debug/debug_scale_image.png");
	testScaleImage->setPosition(110.0f, 60.0f);
	auto scaleSEq = Voxel::UI::Sequence::create({ Voxel::UI::Delay::create(1.0f), Voxel::UI::ScaleTo::create(1.5f, glm::vec2(2.0f, 2.0f)), Voxel::UI::Delay::create(1.0f), Voxel::UI::ScaleTo::create(1.5f, glm::vec2(0.5f, 0.5f)), Voxel::UI::Delay::create(1.0f), Voxel::UI::ScaleTo::create(1.5f, glm::vec2(1.0f)) }, true);
	testScaleImage->runAction(scaleSEq);
	debugCanvas->addChild(testScaleImage, 100);

	// tint image
	auto testTintImage = Voxel::UI::Image::createFromSpriteSheet("testTintImage", "UISpriteSheet", "debug/debug_square_0.png");
	testTintImage->setPosition(glm::vec2(-300.0f, 400.0f));
	testTintImage->setColor(glm::vec3(1.0f, 0.0f, 0.0f));
	auto tintSeq = Voxel::UI::Sequence::create({ Voxel::UI::Delay::create(1.0f), Voxel::UI::TintTo::create(1.5f, glm::vec3(1.0f, 0.0f, 0.0f)), Voxel::UI::Delay::create(1.0f), Voxel::UI::TintTo::create(1.5f, glm::vec3(0.0f, 1.0f, 0.0f)) , Voxel::UI::Delay::create(1.0f), Voxel::UI::TintTo::create(1.5f, glm::vec3(0.0f, 0.0f, 1.0f)) }, true);
	testTintImage->runAction(tintSeq);
	debugCanvas->addChild(testTintImage, 100);

	// ui hierarchy action
	auto hierBase = Voxel::UI::Image::createFromSpriteSheet("hierBase", "UISpriteSheet", "debug/debug_hier_bar.png");
	hierBase->setPosition(glm::vec2(400.0f, 180.0f));
	auto hierAction1 = Voxel::UI::Sequence::create({ Voxel::UI::ScaleTo::create(1.0f, glm::vec2(1.5f, 1.5f)), Voxel::UI::ScaleTo::create(1.0f, glm::vec2(1.0f, 1.0f)) }, true);
	hierBase->runAction(hierAction1);
	debugCanvas->addChild(hierBase, 100);

	auto hierChild1 = Voxel::UI::Image::createFromSpriteSheet("hierChild1", "UISpriteSheet", "debug/debug_hier_bar.png");
	hierChild1->setPivot(glm::vec2(-0.5f, 0.0f));
	hierChild1->setPosition(38.0f, 0.0f);
	auto hierAction2 = Voxel::UI::Sequence::create({ Voxel::UI::RotateTo::create(1.0f, 15.0f), Voxel::UI::RotateTo::create(1.0f, -15.0f) }, true);
	hierChild1->runAction(hierAction2);
	hierBase->addChild(hierChild1, 100);

	auto hierChild2 = Voxel::UI::Image::createFromSpriteSheet("hierChild2", "UISpriteSheet", "debug/debug_hier_bar.png");
	hierChild2->setPivot(glm::vec2(-0.5f, 0.0f));
	hierChild2->setPosition(38.0f, 0.0f);
	auto hierAction3 = Voxel::UI::Sequence::create({ Voxel::UI::RotateTo::create(1.0f, 15.0f), Voxel::UI::RotateTo::create(1.0f, -15.0f) }, true);
	hierChild2->runAction(hierAction3);
	hierChild1->addChild(hierChild2, 100);

	// draggable image test
	auto testDraggableImage = Voxel::UI::Image::createFromSpriteSheet("testDraggableImage", "UISpriteSheet", "debug/debug_draggable_image.png");
	testDraggableImage->setPosition(glm::vec2(800.0f, 180.0f));
	testDraggableImage->setDraggable();
	debugCanvas->addChild(testDraggableImage, 100);
#endif
}

void Voxel::DebugConsole::openConsole()
{
	commandInputField->setVisibility(true);
	commandHistoryBg->setVisibility(true);
	command->setVisibility(true);
	commandHistorys->setVisibility(true);
	openingConsole = true;

	InputHandler::getInstance().setBufferMode(true);
}

void Voxel::DebugConsole::closeConsole()
{
	commandInputField->setVisibility(false);
	commandHistoryBg->setVisibility(false);
	command->setVisibility(false);
	command->setText(DefaultCommandInputText);
	commandHistorys->setVisibility(false);
	openingConsole = false;
	lastCommandIndex = 0;

	InputHandler::getInstance().setBufferMode(false);
}

bool Voxel::DebugConsole::isConsoleOpened()
{
	return openingConsole;
}

void Voxel::DebugConsole::updateConsoleInputText(const std::string & c)
{
	if (!c.empty())
	{
		auto curText = command->getText();
		if (curText == DefaultCommandInputText)
		{
			curText = "";
		}

		std::string strCpy = c;
		//std::cout << "strcpy = " << strCpy << std::endl;

		while (strCpy.empty() == false)
		{
			auto size = strCpy.size();

			if (size >= 17)
			{
				{
					// Check for up 
					std::string token = strCpy.substr(0, 18);
					//std::cout << "Token: " << token << std::endl;
					if (token == "VOXEL_GLFW_KEY_UP")
					{
						// repeat last command
						if (lastCommandIndex >= lastCommands.size())
						{
							lastCommandIndex = 0;
						}

						auto it = std::begin(lastCommands);
						std::advance(it, lastCommandIndex);
						curText = *it;

						lastCommandIndex++;

						if (size == 17)
						{
							strCpy.clear();
							break;
						}
						else
						{
							strCpy = strCpy.substr(18);
							continue;
						}
					}
				}

				if (size >= 20)
				{
					{
						// Check for enter
						std::string token = strCpy.substr(0, 21);
						//std::cout << "Token: " << token << std::endl;
						if (token == "VOXEL_GLFW_KEY_ENTER")
						{
							// execute command
							std::cout << "Execute command: " << curText << std::endl;
							bool result = executeCommand(curText);
							if (result)
							{
								std::cout << "Success.\n";
								updateCommandHistory();
							}
							else
							{
								std::cout << "Fail.\n";
							}
							closeConsole();
							Application::getInstance().getGame()->toggleCursorMode(false);
							return;
						}
					}

					if (size >= 24)
					{
						// Check for back space
						std::string token = strCpy.substr(0, 25);
						//std::cout << "Token: " << token << std::endl;

						if (token == "VOXEL_GLFW_KEY_BACKSPACE")
						{
							//std::cout << "Backspace\n";

							auto curSize = curText.size();
							if (curSize == 0)
							{
								// do nothing
							}
							else if (curSize == 1)
							{
								curText = DefaultCommandInputText;
							}
							else
							{
								curText = curText.substr(0, curSize - 1);
							}

							if (size == 24)
							{
								strCpy.clear();

								//std::cout << "strcpy = " << strCpy << std::endl;
								break;
							}
							else
							{
								strCpy = strCpy.substr(25);
								continue;
							}
						}
					}
				}
			}

			// if it wasn't enter or backspace, keep update
			std::string first = strCpy.substr(0, 1);
			curText += first;
			strCpy = strCpy.substr(1);

			//std::cout << "strcpy = " << strCpy << std::endl;
		}


		command->setText(curText);
	}
}

bool Voxel::DebugConsole::executeCommand(const std::string & command)
{
	std::string cmd = command;
	std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
	std::vector<std::string> split;

	std::stringstream ss(cmd);
	std::string token;

	while (std::getline(ss, token, ' '))
	{
		split.push_back(token);
	}

	auto size = split.size();

	if (size > 0)
	{
		auto commandStr = split.at(0);

		if (size == 1)
		{
			// Only command
			if (commandStr == "exit")
			{
				Application::getInstance().getGLView()->close();
			}
		}
		else
		{
			if (commandStr == "player")
			{
				if (size == 3)
				{
					auto arg1 = split.at(1);
					if (arg1 == "viewmode" || arg1 == "vm")
					{
						// player viewmode [mode]
						auto arg2 = split.at(2);
						int mode = -1;

						if (arg2 == "fps")
						{
							mode = 0;
						}
						else if (arg2 == "tps")
						{
							mode = 1;
						}

						if (mode == -1)
						{
							try
							{
								mode = std::stoi(arg2);
							}
							catch (...)
							{
								return false;
							}
						}

						if (mode >= 0 && mode <= 1)
						{
							player->setViewMode(mode);
							executedCommandHistory.push_back("Set player view mode to " + std::string((mode == 0) ? "First person view" : "Third person view"));
							addCommandHistory(command);
							return true;
						}
						else
						{
							return false;
						}
					}
					else if (arg1 == "fly")
					{
						//player fly [mode]
						bool arg2Bool = split.at(2) == "true" ? true : false;
						player->setFly(arg2Bool);

						if (arg2Bool)
						{
							executedCommandHistory.push_back("Enabled player fly");
						}
						else
						{
							executedCommandHistory.push_back("Disabled player fly");
						}
						addCommandHistory(command);
						return true;
					}
					else if (arg1 == "autojump" || arg1 == "aj")
					{
						//player autojump [mode]
						bool arg2Bool = split.at(2) == "true" ? true : false;
						settingPtr->setAutoJumpMode(arg2Bool);

						if (arg2Bool)
						{
							executedCommandHistory.push_back("Enabled auto jump");
						}
						else
						{
							executedCommandHistory.push_back("Disabled auto jump");
						}
						addCommandHistory(command);
						return true;
					}
				}
				else if (size == 4)
				{
					auto arg1 = split.at(1);
					auto arg2 = split.at(2);
					if (arg1 == "set")
					{
						if (arg2 == "speed")
						{
							try
							{
								float speed = std::stof(split.at(3));
								player->setMovementSpeed(speed);
								executedCommandHistory.push_back("Set player speed to " + split.at(3));
								addCommandHistory(command);
								return true;
							}
							catch (...)
							{
								if (split.at(2) == "default")
								{
									player->setMovementSpeed(15.0f);
									executedCommandHistory.push_back("Set player speed to default (15)");
									addCommandHistory(command);
									return true;
								}
							}
						}
					}
					else if (arg1 == "add")
					{
						if (arg2 == "speed")
						{
							try
							{
								float speed = std::stof(split.at(3));
								float curSpeed = player->getMovementSpeed();
								player->setMovementSpeed(speed + curSpeed);
								executedCommandHistory.push_back("Added player speed by " + split.at(3));
								addCommandHistory(command);
								return true;
							}
							catch (...)
							{
								// do nothing
							}
						}
					}
					else if (arg1 == "tp")
					{
						if (arg2 == "region" || arg2 == "rg")
						{
							int regionID = 0;
							try
							{
								regionID = std::stoi(split.at(3));
							}
							catch (...)
							{
								return false;
							}

							auto region = world->getRegion(regionID);
							if (region)
							{
								auto regionSitePos = region->getSitePosition();

								// y is -1 because we are traveling to place where chunk doesn exsits, so we don't know the top y value
								game->teleportPlayer(glm::vec3(regionSitePos.x, -1, regionSitePos.y));

								addCommandHistory(command);
								return true;
							}
							else
							{
								return false;
							}
						}
					}
				}
				else if (size == 5)
				{
					auto arg1 = split.at(1);

					if (arg1 == "tp")
					{
						float x = 0;
						try
						{
							x = std::stof(split.at(2));
						}
						catch (...)
						{
							return false;
						}

						float y = 0;
						try
						{
							y = std::stof(split.at(3));
						}
						catch (...)
						{
							return false;
						}

						float z = 0;
						try
						{
							z = std::stof(split.at(4));
						}
						catch (...)
						{
							return false;
						}

						game->teleportPlayer(glm::vec3(x, y, z));
						executedCommandHistory.push_back("Teleported player to (" + split.at(2) + ", " + split.at(3) + ", " + split.at(4) + ")");
						addCommandHistory(command);
						return true;
					}
				}
				else if (size == 6)
				{
					//player position x y z
					auto arg1 = split.at(1);
					auto arg2 = split.at(2);
					if (arg1 == "set")
					{
						if (arg2 == "position" || arg2 == "pos")
						{
							float x = 0;
							try
							{
								x = std::stof(split.at(3));
							}
							catch (...)
							{
								return false;
							}

							float y = 0;
							try
							{
								y = std::stof(split.at(4));
							}
							catch (...)
							{
								return false;
							}

							float z = 0;
							try
							{
								z = std::stof(split.at(5));
							}
							catch (...)
							{
								return false;
							}

							player->setPosition(glm::vec3(x, y, z), false);
							executedCommandHistory.push_back("Set player position to (" + split.at(3) + ", " + split.at(4) + ", " + split.at(5) + ")");
							addCommandHistory(command);
							return true;
						}
						else if (arg2 == "rotation" || arg2 == "rot")
						{
							float x = 0;
							try
							{
								x = std::stof(split.at(3));
							}
							catch (...)
							{
								return false;
							}

							float y = 0;
							try
							{
								y = std::stof(split.at(4));
							}
							catch (...)
							{
								return false;
							}

							float z = 0;
							try
							{
								z = std::stof(split.at(5));
							}
							catch (...)
							{
								return false;
							}

							player->setRotation(glm::vec3(x, y, z), false);
							executedCommandHistory.push_back("Set player rotation to (" + split.at(3) + ", " + split.at(4) + ", " + split.at(5) + ")");
							addCommandHistory(command);
							return true;
						}
					}
					else if (arg1 == "add")
					{
						if (arg2 == "position" || arg2 == "pos")
						{
							float x = 0;
							try
							{
								x = std::stof(split.at(3));
							}
							catch (...)
							{
								return false;
							}

							float y = 0;
							try
							{
								y = std::stof(split.at(4));
							}
							catch (...)
							{
								return false;
							}

							float z = 0;
							try
							{
								z = std::stof(split.at(5));
							}
							catch (...)
							{
								return false;
							}

							player->setPosition(glm::vec3(x, y, z) + player->getPosition(), false);
							executedCommandHistory.push_back("Added player position by (" + split.at(3) + ", " + split.at(4) + ", " + split.at(5) + ")");
							addCommandHistory(command);
							return true;
						}
						else if (arg2 == "rotation" || arg2 == "rot")
						{
							float x = 0;
							try
							{
								x = std::stof(split.at(3));
							}
							catch (...)
							{
								return false;
							}

							float y = 0;
							try
							{
								y = std::stof(split.at(4));
							}
							catch (...)
							{
								return false;
							}

							float z = 0;
							try
							{
								z = std::stof(split.at(5));
							}
							catch (...)
							{
								return false;
							}

							player->setRotation(glm::vec3(x, y, z) + player->getRotation(), false);
							executedCommandHistory.push_back("Added player rotation by (" + split.at(3) + ", " + split.at(4) + ", " + split.at(5) + ")");
							addCommandHistory(command);
							return true;
						}
					}
				}
			}
			else if(commandStr == "game")
			{
				if (size == 3)
				{
					// game mode bool
					auto arg1 = split.at(1);
					auto arg2 = split.at(2);

					bool arg2Bool = arg2 == "true" ? true : false;

					if (arg1 == "fogenabled" || arg1 == "fog")
					{
						game->setFogEnabled(arg2Bool);
						if (arg2Bool)
						{
							executedCommandHistory.push_back("Enabled fog");
						}
						else
						{
							executedCommandHistory.push_back("Disabled fog");
						}
						addCommandHistory(command);
						return true;
					}
				}
			}
			else if (commandStr == "world")
			{
				if (size == 2)
				{
					auto arg1 = split.at(1);
					if (arg1 == "rebuild" || arg1 == "rb")
					{
						game->rebuildWorld();
						executedCommandHistory.push_back("Rebuilding world");
						addCommandHistory(command);
						return true;
					}
				}
				else if (size == 3)
				{
					// world arg1 arg2
					auto arg1 = split.at(1);
					auto arg2 = split.at(2);

					bool arg2Bool = arg2 == "true" ? true : false;

					if (arg1 == "voronoi" || arg1 == "v")
					{
						if (arg2 == "rebuild" || arg2 == "rb")
						{
							world->rebuildWorldMap();
							return true;
						}
					}
				}
				else if (size == 4)
				{
#if V_DEBUG && V_DEBUG_VORONOI_LINE
					auto arg1 = split.at(1);
					auto arg2 = split.at(2);
					bool mode = split.at(3) == "true" ? true : false;
					if (arg1 == "voronoi" || arg1 == "v")
					{
						if (arg2 == "render" || arg2 == "r")
						{
							world->setRenderVoronoiMode(mode);
							if (mode)
							{
								executedCommandHistory.push_back("Enabled voronoi render mode");
							}
							else
							{
								executedCommandHistory.push_back("Disabled voronoi render mode");
							}
							addCommandHistory(command);
							return true;
						}
					}
#endif
				}
			}
			else if (commandStr == "chunkmap" || commandStr == "cm")
			{
				if (size == 2)
				{
					auto arg1 = split.at(1);
					if (arg1 == "print" || arg1 == "p")
					{
						chunkMap->printCurrentChunk();
						return true;
					}
					else if (arg1 == "rebuild" || arg1 == "rb")
					{
						game->rebuildChunkMap();
						executedCommandHistory.push_back("Refreshing chunk map");
						addCommandHistory(command);
						return true;
					}
					else if (arg1 == "refresh" || arg1 == "rf")
					{
						game->refreshChunkMap();
						executedCommandHistory.push_back("Refreshing chunk map");
						addCommandHistory(command);
						return true;
					}
				}
				else if (size == 3)
				{
					auto arg1 = split.at(1);
					auto arg2 = split.at(2);

					if (arg1 == "update" || arg1 == "u")
					{
						bool arg2Bool = arg2 == "true" ? true : false;

						chunkMap->setUpdateChunkMapMode(arg2Bool);
						if (arg2Bool)
						{
							executedCommandHistory.push_back("Enabled chunk update");
						}
						else
						{
							executedCommandHistory.push_back("Disabled chunk update");
						}
						addCommandHistory(command);
						return true;
					}
					else if (arg1 == "print" || arg1 == "p")
					{
						if (arg2 == "all" || arg2 == "a")
						{
							chunkMap->printChunkMap();
							addCommandHistory(command);
							return true;
						}
						else if (arg2 == "active" || arg2 == "ac")
						{
							chunkMap->printActiveChunks();
							addCommandHistory(command);
							return true;
						}
					}
				}
				else if (size == 4)
				{
					auto arg1 = split.at(1);
					auto arg2 = split.at(2);

					if (arg1 == "render" || arg1 == "r")
					{
						bool arg3Bool = split.at(3) == "true" ? true : false;

						if (arg2 == "chunk" || arg2 == "c")
						{
							chunkMap->setRenderChunksMode(arg3Bool);
							if (arg3Bool)
							{
								executedCommandHistory.push_back("Enabled chunk rendering");
							}
							else
							{
								executedCommandHistory.push_back("Disabled chunk rendering");
							}
							addCommandHistory(command);
							return true;
						}
#if V_DEBUG_CHUNK_BORDER_LINE
						else if (arg2 == "chunkborder" || arg2 == "cb")
						{
							chunkMap->setRenderChunkBorderMode(arg3Bool);
							if (arg3Bool)
							{
								executedCommandHistory.push_back("Enabled chunk border rendering");
							}
							else
							{
								executedCommandHistory.push_back("Disabled chunk border rendering");
							}
							addCommandHistory(command);
							return true;
						}
#endif
					}
					else if (arg1 == "print" || arg1 == "p")
					{
						int x = 0;
						try
						{
							x = std::stoi(split.at(2));
						}
						catch (...)
						{
							return false;
						}

						int z = 0;
						try
						{
							z = std::stoi(split.at(3));
						}
						catch (...)
						{
							return false;
						}

						chunkMap->printChunk(glm::ivec2(x, z));
					}
				}
			}
			else if (commandStr == "camera")
			{
				if (size == 4)
				{
					// camera arg1 arg2
					auto arg1 = split.at(1);
					auto arg2 = split.at(2);

					if (arg1 == "set")
					{
						float value = 0.0f;

						try
						{
							value = std::stof(split.at(3));
						}
						catch (...)
						{
							return false;
						}

						if (arg2 == "speed")
						{
							Camera::mainCamera->setSpeed(value);
							executedCommandHistory.push_back("Set camera speed to " + split.at(3));
							addCommandHistory(command);
							return true;
						}
						else if (arg2 == "fovy")
						{
							Camera::mainCamera->setFovy(value);
							executedCommandHistory.push_back("Set camera fovy to " + split.at(3));
							addCommandHistory(command);
							return true;
						}
						else if (arg2 == "near")
						{
							const float far = Camera::mainCamera->getFar();

							if (value < far)
							{
								Camera::mainCamera->setNear(value);
								executedCommandHistory.push_back("Set camera near clipping plane to " + split.at(3));
								addCommandHistory(command);
								return true;
							}
							else
							{
								std::cout << "Near: " << value << " can't be large than far: " << far << "\n";
								return false;
							}
						}
						else if (arg2 == "far")
						{
							const float near = Camera::mainCamera->getNear();

							if (value > near)
							{
								Camera::mainCamera->setNear(value);
								executedCommandHistory.push_back("Set camera far clipping plane to " + split.at(3));
								addCommandHistory(command);
								return true;
							}
							else
							{
								std::cout << "Far: " << value << " can't be small than near: " << near << "\n";
								return false;
							}
						}
					}
					else if (arg1 == "add")
					{

					}

				}
				else if (size == 6)
				{
					//player position x y z
					auto arg1 = split.at(1);
					auto arg2 = split.at(2);
					if (arg1 == "set")
					{
						if (arg2 == "position" || arg2 == "pos")
						{
							float x = 0;
							try
							{
								x = std::stof(split.at(3));
							}
							catch (...)
							{
								return false;
							}

							float y = 0;
							try
							{
								y = std::stof(split.at(4));
							}
							catch (...)
							{
								return false;
							}

							float z = 0;
							try
							{
								z = std::stof(split.at(5));
							}
							catch (...)
							{
								return false;
							}

							Camera::mainCamera->setPosition(glm::vec3(x, y, z));
							executedCommandHistory.push_back("Set camera position to (" + split.at(3) + ", " + split.at(4) + ", " + split.at(5) + ")");
							addCommandHistory(command);
							return true;
						}
						else if (arg2 == "rotation" || arg2 == "rot")
						{
							float x = 0;
							try
							{
								x = std::stof(split.at(3));
							}
							catch (...)
							{
								return false;
							}

							float y = 0;
							try
							{
								y = std::stof(split.at(4));
							}
							catch (...)
							{
								return false;
							}

							float z = 0;
							try
							{
								z = std::stof(split.at(5));
							}
							catch (...)
							{
								return false;
							}

							Camera::mainCamera->setAngle(glm::vec3(x, y, z));
							executedCommandHistory.push_back("Set player rotation by (" + split.at(3) + ", " + split.at(4) + ", " + split.at(5) + ")");
							addCommandHistory(command);
							return true;
						}
					}
					else if (arg1 == "add")
					{
						if (arg2 == "position" || arg2 == "pos")
						{
							float x = 0;
							try
							{
								x = std::stof(split.at(3));
							}
							catch (...)
							{
								return false;
							}

							float y = 0;
							try
							{
								y = std::stof(split.at(4));
							}
							catch (...)
							{
								return false;
							}

							float z = 0;
							try
							{
								z = std::stof(split.at(5));
							}
							catch (...)
							{
								return false;
							}

							Camera::mainCamera->setPosition(glm::vec3(x, y, z) + Camera::mainCamera->getPosition());
							executedCommandHistory.push_back("Added camera position by (" + split.at(3) + ", " + split.at(4) + ", " + split.at(5) + ")");
							addCommandHistory(command);
							return true;
						}
						else if (arg2 == "rotation" || arg2 == "rot")
						{
							float x = 0;
							try
							{
								x = std::stof(split.at(3));
							}
							catch (...)
							{
								return false;
							}

							float y = 0;
							try
							{
								y = std::stof(split.at(4));
							}
							catch (...)
							{
								return false;
							}

							float z = 0;
							try
							{
								z = std::stof(split.at(5));
							}
							catch (...)
							{
								return false;
							}

							Camera::mainCamera->setAngle(glm::vec3(x, y, z) + Camera::mainCamera->getAngle());
							executedCommandHistory.push_back("Added player rotation by (" + split.at(3) + ", " + split.at(4) + ", " + split.at(5) + ")");
							addCommandHistory(command);
							return true;
						}
					}
				}
			}
			else if (commandStr == "random" || commandStr == "rand")
			{
				if (size == 2)
				{
					auto arg1 = split.at(1);
					if (arg1 == "reset")
					{
						Voxel::Utility::Random::resetGenerator();
						Random::getInstance().resetAll();
						executedCommandHistory.push_back("Reseting random generator");
						addCommandHistory(command);
						return true;
					}
				}
			}
			else if (commandStr == "app")
			{
				if (size == 3)
				{
					auto arg1 = split.at(1);
					// app rs [mode] 
					if (arg1 == "resolution" || arg1 == "rs")
					{
						auto arg2 = split.at(2);
						if (arg2 == "fullscreen" || arg2 == "f")
						{
							Application::getInstance().getGLView()->setFullScreen();
							executedCommandHistory.push_back("Setting window to fullscreen on primary monitor");
							addCommandHistory(command);
							return true;
						}
						else if (arg2 == "borderless" || arg2 == "b")
						{
							Application::getInstance().getGLView()->setWindowedFullScreen();
							executedCommandHistory.push_back("Setting window to windowed fullscreen on primary monitor");
							addCommandHistory(command);
							return true;
						}
					}
					else if (arg1 == "mouse" || arg1 == "m")
					{
						auto arg2 = split.at(2);

						if (arg2 == "show")
						{
							Application::getInstance().getGLView()->setCursorMode(GLFW_CURSOR_NORMAL);
						}
						else if (arg2 == "hide")
						{
							Application::getInstance().getGLView()->setCursorMode(GLFW_CURSOR_HIDDEN);
						}
						else if (arg2 == "disable")
						{
							Application::getInstance().getGLView()->setCursorMode(GLFW_CURSOR_DISABLED);
						}
					}
				}
				else if (size == 4)
				{
					auto arg1 = split.at(1);
					// app rs [mode] [monitor#] 
					if (arg1 == "resolution" || arg1 == "rs")
					{
						auto arg2 = split.at(2); 
						int monitorNum;
						try
						{
							monitorNum = std::stoi(split.at(3));
						}
						catch (...)
						{
							return false;
						}

						if (arg2 == "fullscreen" || arg2 == "f")
						{
							Application::getInstance().getGLView()->setFullScreen(monitorNum);
							executedCommandHistory.push_back("Setting window to fullscreen on monitor #" + std::to_string(monitorNum));
							addCommandHistory(command);
							return true;
						}
						else if (arg2 == "borderless" || arg2 == "b")
						{
							Application::getInstance().getGLView()->setWindowedFullScreen(monitorNum);
							executedCommandHistory.push_back("Setting window to windowed fullscreen on monitor #" + std::to_string(monitorNum));
							addCommandHistory(command);
							return true;
						}
					}
					else if (arg1 == "window" || arg1 == "w")
					{
						auto arg2 = split.at(2);
						if (arg2 == "decoration" || arg2 == "d")
						{
							bool arg3Bool = split.at(3) == "true" ? true : false;

							Application::getInstance().getGLView()->setWindowDecoration(arg3Bool);
							return true;
						}
						else if (arg2 == "floating" || arg2 == "f")
						{
							bool arg3Bool = split.at(3) == "true" ? true : false;

							Application::getInstance().getGLView()->setWindowDecoration(arg3Bool);
							return true;
						}
					}
				}
				else if (size == 5)
				{
					// app rs w width height
					auto arg1 = split.at(1);
					// app rs [mode] 
					if (arg1 == "resolution" || arg1 == "rs")
					{
						auto arg2 = split.at(2);
						if (arg2 == "windowed" || arg2 == "w")
						{
							int width;
							try
							{
								width = std::stoi(split.at(3));
							}
							catch (...)
							{
								return false;
							}

							int height;
							try
							{
								height = std::stoi(split.at(4));
							}
							catch (...)
							{
								return false;
							}

							Application::getInstance().getGLView()->setWindowed(width, height);
							executedCommandHistory.push_back("Setting window to windowed (" + std::to_string(width) + ", " + std::to_string(height) + ") on primary monitor");
							addCommandHistory(command);
							return true;
						}
					}
					// app window pos x y
					else if (arg1 == "window" || arg1 == "w")
					{
						auto arg2 = split.at(2);
						if (arg2 == "position" || arg2 == "pos")
						{
							int x;
							try
							{
								x = std::stoi(split.at(3));
							}
							catch (...)
							{
								return false;
							}

							int y;
							try
							{
								y = std::stoi(split.at(4));
							}
							catch (...)
							{
								return false;
							}

							Application::getInstance().getGLView()->setWindowPosition(x, y);
							executedCommandHistory.push_back("Setting window position to (" + std::to_string(x) + ", " + std::to_string(y) + ")");
							addCommandHistory(command);
							return true;
						}
						else if (arg2 == "size")
							{
								int w;
								try
								{
									w = std::stoi(split.at(3));
								}
								catch (...)
								{
									return false;
								}

								int h;
								try
								{
									h = std::stoi(split.at(4));
								}
								catch (...)
								{
									return false;
								}

								Application::getInstance().getGLView()->setWindowSize(w, h);
								executedCommandHistory.push_back("Setting window size to (" + std::to_string(w) + ", " + std::to_string(h) + ")");
								addCommandHistory(command);
								return true;
							}
					}
				}
			}
			else if (commandStr == "time")
			{
				if (size == 3)
				{
					auto arg1 = split.at(1);
					auto arg2 = split.at(2);

					std::vector<std::string> split;

					std::stringstream ss(arg2);
					std::string token;

					while (std::getline(ss, token, ':'))
					{
						split.push_back(token);
					}

					int h;
					try
					{
						h = std::stoi(split.at(0));
					}
					catch (...)
					{
						return false;
					}

					int m;
					try
					{
						m = std::stoi(split.at(1));
					}
					catch (...)
					{
						return false;
					}
				
					if (arg1 == "set")
					{
						calendar->setTime(h, m);
						executedCommandHistory.push_back("Time set to (" + std::to_string(h) + ", " + std::to_string(m) + ")");
						addCommandHistory(command);
						return true;
					}
					else if (arg1 == "add")
					{
						calendar->addTime(h, m);
						executedCommandHistory.push_back("Time added by (" + std::to_string(h) + ", " + std::to_string(m) + ")");
						addCommandHistory(command);
						return true;
					}
				}
			}
			else if (commandStr == "tree")
			{
				// tree type w h
				if (size == 4)
				{
					if (player->isLookingAtBlock())
					{
						auto pos = player->getLookingBlock()->getWorldCoordinate();
						pos.y++;

						glm::ivec3 treeLocalPos;
						glm::ivec3 chunkPos;
						chunkMap->blockWorldCoordinateToLocalAndChunkSectionCoordinate(pos, treeLocalPos, chunkPos);
						treeLocalPos.y = pos.y;

						auto arg1 = split.at(1);
						auto arg2 = split.at(2);
						auto arg3 = split.at(3);

						TreeBuilder::TrunkWidthType w;
						TreeBuilder::TrunkHeightType h;

						if (arg2 == "small" || arg2 == "s")
						{
							w = TreeBuilder::TrunkWidthType::SMALL;
						}
						else if (arg2 == "medium" || arg2 == "m")
						{
							w = TreeBuilder::TrunkWidthType::MEDIUM;
						}
						else if (arg2 == "large" || arg2 == "l")
						{
							w = TreeBuilder::TrunkWidthType::LARGE;
						}
						else
						{
							return false;
						}

						if (arg3 == "small" || arg3 == "s")
						{
							h = TreeBuilder::TrunkHeightType::SMALL;
						}
						else if (arg3 == "medium" || arg3 == "m")
						{
							h = TreeBuilder::TrunkHeightType::MEDIUM;
						}
						else if (arg3 == "large" || arg3 == "l")
						{
							h = TreeBuilder::TrunkHeightType::LARGE;
						}
						else
						{
							return false;
						}

						Voxel::Vegitation::Tree type;

						if (arg1 == "oak")
						{
							type = Voxel::Vegitation::Tree::OAK;
						}
						else if (arg1 == "birch")
						{
							type = Voxel::Vegitation::Tree::BIRCH;
						}
						else if (arg1 == "spruce")
						{
							type = Voxel::Vegitation::Tree::SPRUCE;
						}
						else if (arg1 == "pine")
						{
							type = Voxel::Vegitation::Tree::PINE;
						}
						else
						{
							return false;
						}

						TreeBuilder::createTree(type, h, w, chunkMap, glm::ivec2(chunkPos.x, chunkPos.z), treeLocalPos, std::mt19937());
						return true;
					}
				}
			}
			else if (commandStr == "testui")
			{
				// /testui uiName 
			}
		}
	}

	return false;
}

void Voxel::DebugConsole::addCommandHistory(const std::string & command)
{
	lastCommands.push_front(command);

	if (lastCommands.size() > 10)
	{
		lastCommands.pop_back();
	}
}

void Voxel::DebugConsole::updateCommandHistory()
{
	if (executedCommandHistory.size() > 10)
	{
		executedCommandHistory.pop_front();
	}

	std::string historyStr = "";

	unsigned int index = 0;
	auto size = executedCommandHistory.size();

	for (auto& cmdStr : executedCommandHistory)
	{
		historyStr += cmdStr;

		if (index < (size - 1))
		{
			historyStr += "\n";
		}

		//std::cout << "ch: " << cmdStr << std::endl;

		index++;
	}

	commandHistorys->setText(historyStr);
}

void Voxel::DebugConsole::render()
{
	debugCanvas->render();
}

void Voxel::DebugConsole::toggleDubugOutputs()
{
	debugOutputVisibility = !debugOutputVisibility;

	//staticLabels->setVisibility(debugOutputVisibility);
	fpsNumber->setVisibility(debugOutputVisibility);
	resolutionNumber->setVisibility(debugOutputVisibility);
	vsyncMode->setVisibility(debugOutputVisibility);
	hardwardInfo->setVisibility(debugOutputVisibility);
	playerPosition->setVisibility(debugOutputVisibility);
	playerRotation->setVisibility(debugOutputVisibility);
	playerChunkPosition->setVisibility(debugOutputVisibility);

	if (debugOutputVisibility)
	{
		if (playerLookingAt->getVisibility())
		{
			playerLookingAt->setVisibility(true);
		}
		else
		{
			playerLookingAt->setVisibility(false);
		}
	}
	else
	{
		playerLookingAt->setVisibility(debugOutputVisibility);
	}

	chunkNumbers->setVisibility(debugOutputVisibility);
	biomeAndTerrainInfo->setVisibility(debugOutputVisibility);
	regionID->setVisibility(debugOutputVisibility);
	drawCallAndVertCount->setVisibility(debugOutputVisibility);
}

void Voxel::DebugConsole::onFPSUpdate(int fps)
{
	fpsNumber->setText("fps: " + std::to_string(fps));
}

void Voxel::DebugConsole::update(const float delta)
{
	debugCanvas->update(delta);
}

void Voxel::DebugConsole::updateMouseMove(const glm::vec2 & mousePosition, const glm::vec2& mouseDelta)
{
	debugCanvas->updateMouseMove(mousePosition, mouseDelta);
}

void Voxel::DebugConsole::updateMouseClick(const glm::vec2 & mousePosition, const int button)
{
	debugCanvas->updateMouseClick(mousePosition, button);
}

void Voxel::DebugConsole::updateMouseRelease(const glm::vec2 & mousePosition, const int button)
{
	debugCanvas->updateMouseRelease(mousePosition, button);
}

void Voxel::DebugConsole::updateResolution(int width, int height)
{
	debugCanvas->setSize(glm::vec2(width, height));

	resolutionNumber->setText("resolution: " + std::to_string(width) + ", " + std::to_string(height));

	commandInputField->setScale(glm::vec2(static_cast<float>(width) * 0.5f, 10.0f));
	commandHistoryBg->setScale(glm::vec2(static_cast<float>(width) * 0.5f, 100.0f));
}

void Voxel::DebugConsole::updateVsync(bool vsync)
{
	if (vsync)
	{
		vsyncMode->setText("vsync: True");
	}
	else
	{
		vsyncMode->setText("vsync: False");
	}
}

void Voxel::DebugConsole::updatePlayerPosition(const glm::vec3 & position)
{
	std::stringstream x, y, z;
	x << std::fixed << std::showpoint << std::setprecision(2) << position.x;
	y << std::fixed << std::showpoint << std::setprecision(2) << position.y;
	z << std::fixed << std::showpoint << std::setprecision(2) << position.z;

	playerPosition->setText("player position: " + x.str() + ", " + y.str() + ", " + z.str());

	auto iPos = glm::ivec3(position);
	int chunkX = iPos.x / Constant::CHUNK_SECTION_WIDTH;
	int chunkY = iPos.y / Constant::CHUNK_SECTION_HEIGHT;
	int chunkZ = iPos.z / Constant::CHUNK_SECTION_LENGTH;

	// Shift by 1 if player is in negative position in XZ axis.
	if (position.x < 0) chunkX -= 1;
	if (position.y < 0) chunkY -= 1;
	if (position.z < 0) chunkZ -= 1;

	playerChunkPosition->setText("player chunk position: " + std::to_string(chunkX) + ", " + std::to_string(chunkY) + ", " + std::to_string(chunkZ));
}

void Voxel::DebugConsole::updatePlayerRotation(const glm::vec3 & rotation)
{
	std::stringstream x, y, z;
	x << std::fixed << std::showpoint << std::setprecision(2) << rotation.x;
	y << std::fixed << std::showpoint << std::setprecision(2) << rotation.y;
	z << std::fixed << std::showpoint << std::setprecision(2) << rotation.z;

	std::string facing = "  Facing ";
	// 0
	if ((rotation.y >= 0 && rotation.y < 45) || (rotation.y <= 360.0f && rotation.y >  315.0f))
	{
		facing += "North (Toward negative z)";
	}
	// 90
	else if (rotation.y > 45 && rotation.y <= 135)
	{
		facing += "East (Toward positive x)";
	}
	// 180
	else if (rotation.y > 135 && rotation.y <= 225)
	{
		facing += "South (Toward positive z)";
	}
	// 270
	else if (rotation.y > 225 && rotation.y <= 315)
	{
		facing += "West (Toward negative x)";
	}

	playerRotation->setText("player rotation: " + x.str() + ", " + y.str() + ", " + z.str() + facing);
}

void Voxel::DebugConsole::updatePlayerLookingAt(const glm::ivec3 & lookingAt, const Cube::Face& face)
{
	playerLookingAt->setText("player looking at: " + std::to_string(lookingAt.x) + ", " + std::to_string(lookingAt.y) + ", " + std::to_string(lookingAt.z) + " FACE: " + Cube::faceToString(face));
}

void Voxel::DebugConsole::setPlayerLookingAtVisibility(const bool visibility)
{
	if (debugOutputVisibility)
	{
		playerLookingAt->setVisibility(visibility);
	}
}

void Voxel::DebugConsole::updateChunkNumbers(const int visible, const int active, const int total, const std::string& workOrder)
{
	chunkNumbers->setText("chunks: " + std::to_string(visible) + " / " + std::to_string(active) + " / " + std::to_string(total) + " / " + workOrder);
}

void Voxel::DebugConsole::updateBiome(const std::string & biomeType, const std::string& terrainType, const float t, const float m)
{
	std::stringstream temp, moist;
	temp << std::fixed << std::showpoint << std::setprecision(2) << t;
	moist << std::fixed << std::showpoint << std::setprecision(2) << m;

	biomeAndTerrainInfo->setText("biome: " + biomeType + " / " + terrainType + " / " + temp.str() + " / " + moist.str());
}

void Voxel::DebugConsole::updateRegion(const unsigned int regionID)
{
	this->regionID->setText("region: " + std::to_string(regionID));
}

/*
void Voxel::DebugConsole::updateDrawCallsAndVerticesSize()
{
	auto glView = Application::getInstance().getGLView();

	if (glView->doesCountDrawCalls() || glView->doesCountVerticesSize())
	{
		drawCallAndVertCount->setText("Draw calls: " + std::to_string(glView->getTotalDrawCalls()) + ", Vertices: " + std::to_string(glView->getTotalVerticesSize()));
	}
}
*/

#endif