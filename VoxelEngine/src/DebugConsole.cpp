#include "DebugConsole.h"
#include <UI.h>
#include <Application.h>
#include <sstream>
#include <ChunkUtil.h>
#include <iomanip>
#include <InputHandler.h>
#include <Player.h>
#include <Game.h>
#include <algorithm>

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
	// debug
	, player(nullptr)
	, game(nullptr)
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
	auto resolution = Application::getInstance().getGLView()->getScreenSize();

	commandInputField = UI::Image::create("2x2.png", glm::vec2(0, 10.0f), glm::vec4(1, 1, 1, 0.45f));
	commandInputField->setCanvasPivot(glm::vec2(0, -0.5f));
	commandInputField->setScale(glm::vec2(resolution.x * 0.5f, 10.0f));
	commandInputField->setVisibility(false);

	debugCanvas->addImage("cmdInputField", commandInputField, 0);

	command = UI::Text::create(DefaultCommandInputText, glm::vec2(5.0f, 5.0f), glm::vec4(1.0f), 1, Voxel::UI::Text::ALIGN::LEFT, Voxel::UI::Text::TYPE::DYNAMIC, 100);
	command->setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	command->setPivot(glm::vec2(-0.5f, -0.5f));
	command->setCanvasPivot(glm::vec2(-0.5f, -0.5f));
	command->setVisibility(false);

	debugCanvas->addText("command", command, 0);

	auto glview = Application::getInstance().getGLView();

	auto CPUName = glview->getCPUName();
	auto GLVersion = glview->getGLVersion();
	auto GPUVendor = glview->getGPUVendor();
	auto GPURenderer = glview->getGPURenderer();

	glm::vec4 color = glm::vec4(1.0f);
	glm::vec4 outlineColor = glm::vec4(0, 0, 0, 1.0f);
	const int fontID = 2;

	staticLabels = UI::Text::createWithOutline("fps:\nresolution:\nvsync:\n\nCPU:" + CPUName + "\n" + GPUVendor + "\n" + GPURenderer + "\n" + GLVersion + "\n\nplayer position:\nplayer chunk position:\nplayer looking at:\n\nChunk:", glm::vec2(5.0f, -5.0f), fontID, color, outlineColor);
	staticLabels->setVisibility(false);
	staticLabels->setPivot(glm::vec2(-0.5f, 0.5f));
	staticLabels->setCanvasPivot(glm::vec2(-0.5f, 0.5f));

	debugCanvas->addText("staticLabels", staticLabels, 0);

	//defaultCanvas->addText("FPSLabel", "FPS: ", glm::vec2(-50, 70), 1, UI::Text::ALIGN::LEFT, UI::Text::TYPE::STATIC);
	fpsNumber = UI::Text::createWithOutline("9999", glm::vec2(41.0f, -5.0f), fontID, color, outlineColor, UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 6);
	fpsNumber->setPivot(glm::vec2(-0.5f, 0.5f));
	fpsNumber->setCanvasPivot(glm::vec2(-0.5f, 0.5f));
	fpsNumber->setVisibility(false);
	debugCanvas->addText("fpsNumber", fpsNumber, 0);

	resolutionNumber = UI::Text::createWithOutline(std::to_string(resolution.x) + ", " + std::to_string(resolution.y), glm::vec2(101.0f, -19.0f), fontID, color, outlineColor, UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 13);
	resolutionNumber->setPivot(glm::vec2(-0.5f, 0.5f));
	resolutionNumber->setCanvasPivot(glm::vec2(-0.5f, 0.5f));
	resolutionNumber->setVisibility(false);
	debugCanvas->addText("resolutionNumber", resolutionNumber, 0);

	auto vsync = Application::getInstance().getGLView()->isVsyncEnabled();
	vsyncMode = UI::Text::createWithOutline("False", glm::vec2(65.0f, -33.0f), fontID, color, outlineColor, UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 6);
	vsyncMode->setPivot(glm::vec2(-0.5f, 0.5f));
	vsyncMode->setCanvasPivot(glm::vec2(-0.5f, 0.5f));
	vsyncMode->setVisibility(false);
	debugCanvas->addText("vsyncMode", vsyncMode, 0);

	playerPosition = UI::Text::createWithOutline("00000.00, 00000.00, 00000.00", glm::vec2(143.0f, -149.0f), fontID, color, outlineColor, UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 30);
	playerPosition->setPivot(glm::vec2(-0.5f, 0.5f));
	playerPosition->setCanvasPivot(glm::vec2(-0.5f, 0.5f));
	playerPosition->setVisibility(false);
	debugCanvas->addText("playerPosition", playerPosition, 0);

	playerChunkPosition = UI::Text::createWithOutline("00000.00, 00000.00, 00000.00", glm::vec2(197.0f, -163.0f), fontID, color, outlineColor, UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 30);
	playerChunkPosition->setPivot(glm::vec2(-0.5f, 0.5f));
	playerChunkPosition->setCanvasPivot(glm::vec2(-0.5f, 0.5f));
	playerChunkPosition->setVisibility(false);
	debugCanvas->addText("playerChunkPosition", playerChunkPosition, 0);

	playerLookingAt = UI::Text::createWithOutline("000000, 000000, 000000 Face (-)", glm::vec2(165.0f, -177.0f), fontID, color, outlineColor, UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 40);
	playerLookingAt->setPivot(glm::vec2(-0.5f, 0.5f));
	playerLookingAt->setCanvasPivot(glm::vec2(-0.5f, 0.5f));
	playerLookingAt->setVisibility(false);
	debugCanvas->addText("playerLookingAt", playerLookingAt, 0);

	chunkNumbers = UI::Text::createWithOutline("0000 / 0000 / 0000", glm::vec2(69.0f, -207.f), fontID, color, outlineColor, UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 19);
	chunkNumbers->setPivot(glm::vec2(-0.5f, 0.5f));
	chunkNumbers->setCanvasPivot(glm::vec2(-0.5f, 0.5f));
	chunkNumbers->setVisibility(false);
	debugCanvas->addText("chunkNumbers", chunkNumbers, 0);
}

void Voxel::DebugConsole::openConsole()
{
	commandInputField->setVisibility(true);
	command->setVisibility(true);
	openingConsole = true;

	InputHandler::getInstance().setBufferMode(true);
}

void Voxel::DebugConsole::closeConsole()
{
	commandInputField->setVisibility(false);
	command->setVisibility(false);
	command->setText(DefaultCommandInputText);
	openingConsole = false;

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
						executeCommand(curText);
						closeConsole();
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
						//std::cout << "Backspace" << std::endl;

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

			// if it wasn't enter or backspace, keep update
			std::string first = strCpy.substr(0, 1);
			curText += first;
			strCpy = strCpy.substr(1);

			//std::cout << "strcpy = " << strCpy << std::endl;
		}


		command->setText(curText);
	}
}

void Voxel::DebugConsole::executeCommand(const std::string & command)
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
					if (arg1 == "speed")
					{
						try
						{
							float speed = std::stof(split.at(2));
							player->setMovementSpeed(speed);
						}
						catch (...)
						{
							if (split.at(2) == "default")
							{
								player->setMovementSpeed(15.0f);
							}
							else
							{
								return;
							}
						}
					}
				}
				else if (size == 5)
				{
					//player position x y z
					auto arg1 = split.at(1);
					if (arg1 == "position" || arg1 == "pos")
					{
						float x = 0;
						try
						{
							x = std::stof(split.at(2));
						}
						catch (...)
						{
							return;
						}

						float y = 0;
						try
						{
							y = std::stof(split.at(3));
						}
						catch (...)
						{
							return;
						}

						float z = 0;
						try
						{
							z = std::stof(split.at(4));
						}
						catch (...)
						{
							return;
						}

						player->setPosition(glm::vec3(x, y, z));
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

					if (arg1 == "renderchunks" || arg1 == "renderchunk" || arg1 == "rc")
					{
						game->setRenderChunkMode(arg2Bool);
					}
					else if (arg1 == "rendervoronoi" || arg1 == "rv")
					{
						game->setRenderVoronoiMode(arg2Bool);
					}
					else if (arg1 == "updatechunkmap" || arg1 == "ucm")
					{
						game->setUpdateChunkMapMode(arg2Bool);
					}
					else if (arg1 == "fogenabled" || arg1 == "fog")
					{
						game->setFogEnabled(arg2Bool);
					}
				}
			}
		}
	}
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
	playerChunkPosition->setVisibility(debugOutputVisibility);
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

	commandInputField->setScale(glm::vec2(static_cast<float>(width) * 0.5f, 10.0f));
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

	auto iPos = glm::ivec3(position);
	int chunkX = iPos.x / Constant::CHUNK_SECTION_WIDTH;
	int chunkY = iPos.y / Constant::CHUNK_SECTION_HEIGHT;
	int chunkZ = iPos.z / Constant::CHUNK_SECTION_LENGTH;

	// Shift by 1 if player is in negative position in XZ axis.
	if (position.x < 0) chunkX -= 1;
	if (position.y < 0) chunkY -= 1;
	if (position.z < 0) chunkZ -= 1;

	playerChunkPosition->setText(std::to_string(chunkX) + ", " + std::to_string(chunkY) + ", " + std::to_string(chunkZ));
}

void Voxel::DebugConsole::updatePlayerLookingAt(const glm::ivec3 & lookingAt, const Cube::Face& face)
{
	playerLookingAt->setText(std::to_string(lookingAt.x) + ", " + std::to_string(lookingAt.y) + ", " + std::to_string(lookingAt.z) + " FACE: " + Cube::faceToString(face));
}

void Voxel::DebugConsole::updateChunkNumbers(const int visible, const int active, const int total)
{
	chunkNumbers->setText(std::to_string(visible) + " / " + std::to_string(active) + " / " + std::to_string(total));
}
