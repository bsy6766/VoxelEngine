#include "DebugConsole.h"
#include <UI.h>
#include <Application.h>
#include <sstream>
#include <iomanip>

using namespace Voxel;

DebugConsole::DebugConsole()
	: openingConsole(false)
	, debugCanvas(nullptr)
	, staticLabels(nullptr)
	, debugOutputVisibility(false)
	, fpsNumber(nullptr)
	, resolutionNumber(nullptr)
	, vsyncMode(nullptr)
	, mousePosition(nullptr)
	, cameraPosition(nullptr)
	, cameraRotation(nullptr)
	, playerPosition(nullptr)
	, playerLookingAt(nullptr)
	, chunkNumbers(nullptr)
{
	auto res = Application::getInstance().getGLView()->getScreenSize();
	debugCanvas = UI::Canvas::create(glm::vec2(res), glm::vec2(0));

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
	commandInputField = UI::Image::create("2x2.png", glm::vec2(0, 10.0f), glm::vec4(1, 1, 1, 0.35f));
	commandInputField->setCanvasPivot(glm::vec2(0, -0.5f));
	commandInputField->setScale(glm::vec2(640.0f, 10.0f));
	commandInputField->setVisibility(false);

	debugCanvas->addImage("cmdInputField", commandInputField, 0);

	//staticLabels = UI::Text::create("fps:\nresolution:\nvsync:\n\nmouse position:\n\ncamera position:\ncamera rotation:\n\nfovy:\nfovx:\n\nplayer position:\nplayer rotation:\n\nchunk:map, loader\nchunk section:total, visible", glm::vec2(5.0f, -5.0f), 2);
	staticLabels = UI::Text::create("fps:\nresolution:\nvsync:\n\nplayer position:\nplayer looking at:\n\nChunk:", glm::vec2(5.0f, -5.0f), 2);
	staticLabels->setVisibility(false);
	staticLabels->setPivot(glm::vec2(-0.5f, 0.5f));
	staticLabels->setCanvasPivot(glm::vec2(-0.5f, 0.5f));

	debugCanvas->addText("staticLabels", staticLabels, 0);

	//defaultCanvas->addText("FPSLabel", "FPS: ", glm::vec2(-50, 70), 1, UI::Text::ALIGN::LEFT, UI::Text::TYPE::STATIC);
	fpsNumber = UI::Text::create("9999", glm::vec2(41.0f, -5.0f), 2, UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 6);
	fpsNumber->setPivot(glm::vec2(-0.5f, 0.5f));
	fpsNumber->setCanvasPivot(glm::vec2(-0.5f, 0.5f));
	fpsNumber->setVisibility(false);
	debugCanvas->addText("fpsNumber", fpsNumber, 0);

	auto resolution = Application::getInstance().getGLView()->getScreenSize();
	resolutionNumber = UI::Text::create(std::to_string(resolution.x) + ", " + std::to_string(resolution.y), glm::vec2(101.0f, -19.0f), 2, UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 13);
	resolutionNumber->setPivot(glm::vec2(-0.5f, 0.5f));
	resolutionNumber->setCanvasPivot(glm::vec2(-0.5f, 0.5f));
	resolutionNumber->setVisibility(false);
	debugCanvas->addText("resolutionNumber", resolutionNumber, 0);

	auto vsync = Application::getInstance().getGLView()->isVsyncEnabled();
	vsyncMode = UI::Text::create("False", glm::vec2(65.0f, -33.0f), 2, UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 6);
	vsyncMode->setPivot(glm::vec2(-0.5f, 0.5f));
	vsyncMode->setCanvasPivot(glm::vec2(-0.5f, 0.5f));
	vsyncMode->setVisibility(false);
	debugCanvas->addText("vsyncMode", vsyncMode, 0);

	playerPosition = UI::Text::create("00000.00, 00000.00, 00000.00", glm::vec2(143.0f, -65.0f), 2, UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 30);
	playerPosition->setPivot(glm::vec2(-0.5f, 0.5f));
	playerPosition->setCanvasPivot(glm::vec2(-0.5f, 0.5f));
	playerPosition->setVisibility(false);
	debugCanvas->addText("playerPosition", playerPosition, 0);

	playerLookingAt = UI::Text::create("000000, 000000, 000000", glm::vec2(165.0f, -79.0f), 2, UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 23);
	playerLookingAt->setPivot(glm::vec2(-0.5f, 0.5f));
	playerLookingAt->setCanvasPivot(glm::vec2(-0.5f, 0.5f));
	playerLookingAt->setVisibility(false);
	debugCanvas->addText("playerLookingAt", playerLookingAt, 0);

	chunkNumbers = UI::Text::create("0000 / 0000 / 0000", glm::vec2(69.0f, -109.0f), 2, UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 19);
	chunkNumbers->setPivot(glm::vec2(-0.5f, 0.5f));
	chunkNumbers->setCanvasPivot(glm::vec2(-0.5f, 0.5f));
	chunkNumbers->setVisibility(false);
	debugCanvas->addText("chunkNumbers", chunkNumbers, 0);
}

void Voxel::DebugConsole::openConsole()
{
	commandInputField->setVisibility(true);
	openingConsole = true;
}

void Voxel::DebugConsole::closeConsole()
{
	commandInputField->setVisibility(false);
	openingConsole = false;
}

bool Voxel::DebugConsole::isConsoleOpened()
{
	return openingConsole;
}

void Voxel::DebugConsole::render()
{
	debugCanvas->render();
}

void Voxel::DebugConsole::toggleDubugOutputs()
{
	debugOutputVisibility = !debugOutputVisibility;

	staticLabels->setVisibility(debugOutputVisibility);
	fpsNumber->setVisibility(debugOutputVisibility);
	resolutionNumber->setVisibility(debugOutputVisibility);
	vsyncMode->setVisibility(debugOutputVisibility);
	playerPosition->setVisibility(debugOutputVisibility);
	playerLookingAt->setVisibility(debugOutputVisibility);
	chunkNumbers->setVisibility(debugOutputVisibility);
}

void Voxel::DebugConsole::onFPSUpdate(int fps)
{
	fpsNumber->setText(std::to_string(fps));
}

void Voxel::DebugConsole::updateResolution(int width, int height)
{
	debugCanvas->setSize(glm::vec2(width, height));

	resolutionNumber->setText(std::to_string(width) + ", " + std::to_string(height));
}

void Voxel::DebugConsole::updateVsync(bool vsync)
{
	if (vsync)
	{
		vsyncMode->setText("True");
	}
	else
	{
		vsyncMode->setText("False");
	}
}

void Voxel::DebugConsole::updatePlayerPosition(const glm::vec3 & position)
{
	std::stringstream x, y, z;
	x << std::fixed << std::showpoint << std::setprecision(2) << position.x;
	y << std::fixed << std::showpoint << std::setprecision(2) << position.y;
	z << std::fixed << std::showpoint << std::setprecision(2) << position.z;

	playerPosition->setText(x.str() + ", " + y.str() + ", " + z.str());
}

void Voxel::DebugConsole::updatePlayerLookingAt(const glm::ivec3 & lookingAt)
{
	playerLookingAt->setText(std::to_string(lookingAt.x) + ", " + std::to_string(lookingAt.y) + ", " + std::to_string(lookingAt.z));
}

void Voxel::DebugConsole::updateChunkNumbers(const int visible, const int active, const int total)
{
	chunkNumbers->setText(std::to_string(visible) + " / " + std::to_string(active) + " / " + std::to_string(total));
}
