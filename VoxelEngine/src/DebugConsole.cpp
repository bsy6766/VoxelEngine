#include "DebugConsole.h"
#include <UI.h>
#include <Application.h>
#include <sstream>
#include <ChunkUtil.h>
#include <iomanip>
#include <InputHandler.h>
#include <Player.h>
#include <Game.h>
#include <World.h>
#include <algorithm>
#include <Camera.h>
#include <ChunkMap.h>

using namespace Voxel;

DebugConsole::DebugConsole()
	: openingConsole(false)
	, debugCanvas(nullptr)
	//, staticLabels(nullptr)
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

	commandInputField = UI::Image::createFromSpriteSheet("UISpriteSheet", "2x2.png", glm::vec2(0), glm::vec4(1, 1, 1, 0.225f));
	commandInputField->setPivot(glm::vec2(0, -0.5f));
	commandInputField->setCanvasPivot(glm::vec2(0, -0.5f));
	commandInputField->setScale(glm::vec2(resolution.x * 0.5f, 10.0f));
	commandInputField->setVisibility(false);

	debugCanvas->addImage("cmdInputField", commandInputField, 0);

	commandHistoryBg = UI::Image::createFromSpriteSheet("UISpriteSheet", "2x2.png", glm::vec2(0), glm::vec4(1, 1, 1, 0.45f));
	commandHistoryBg->setCanvasPivot(glm::vec2(0, -0.5f));
	commandHistoryBg->setPivot(glm::vec2(0, -0.5f));
	commandHistoryBg->setScale(glm::vec2(resolution.x * 0.5f, 85.0f));
	commandHistoryBg->setVisibility(false);

	debugCanvas->addImage("commandHistoryBg", commandHistoryBg, 0);

	command = UI::Text::create(DefaultCommandInputText, glm::vec2(5.0f, 15.0f), glm::vec4(1.0f), 1, Voxel::UI::Text::ALIGN::LEFT, Voxel::UI::Text::TYPE::DYNAMIC, 128);
	command->setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	command->setPivot(glm::vec2(-0.5f, 0.5f));
	command->setCanvasPivot(glm::vec2(-0.5f, -0.5f));
	command->setVisibility(false);

	debugCanvas->addText("command", command, 0);

	commandHistorys = UI::Text::create("_", glm::vec2(5.0f, 23.0f), glm::vec4(1.0f), 1, Voxel::UI::Text::ALIGN::LEFT, Voxel::UI::Text::TYPE::DYNAMIC, 1024);
	commandHistorys->setText("");
	commandHistorys->setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	commandHistorys->setPivot(glm::vec2(-0.5f, -0.5f));
	commandHistorys->setCanvasPivot(glm::vec2(-0.5f, -0.5f));
	commandHistorys->setVisibility(false);

	debugCanvas->addText("commandHistorys", commandHistorys, 0);

	auto glview = Application::getInstance().getGLView();

	auto CPUName = glview->getCPUName();
	auto GLVersion = glview->getGLVersion();
	auto GPUVendor = glview->getGPUVendor();
	auto GPURenderer = glview->getGPURenderer();

	glm::vec4 color = glm::vec4(1.0f);
	glm::vec4 outlineColor = glm::vec4(0, 0, 0, 1.0f);
	const int fontID = 2;

	/*
	staticLabels = UI::Text::createWithOutline("fps:\nresolution:\nvsync:\n\nCPU:" + CPUName + "\n" + GPUVendor + "\n" + GPURenderer + "\n" + GLVersion + "\n\nplayer position:\nplayer chunk position:\nplayer looking at:\n\nChunk:\n\nBiome:\n\nRegion:", glm::vec2(5.0f, -5.0f), fontID, color, outlineColor);
	staticLabels->setVisibility(false);
	staticLabels->setPivot(glm::vec2(-0.5f, 0.5f));
	staticLabels->setCanvasPivot(glm::vec2(-0.5f, 0.5f));

	debugCanvas->addText("staticLabels", staticLabels, 0);
	*/

	//defaultCanvas->addText("FPSLabel", "FPS: ", glm::vec2(-50, 70), 1, UI::Text::ALIGN::LEFT, UI::Text::TYPE::STATIC);
	fpsNumber = UI::Text::createWithOutline("fps: 00000", glm::vec2(5.0f, -5.0f), fontID, color, outlineColor, UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 16);
	fpsNumber->setPivot(glm::vec2(-0.5f, 0.5f));
	fpsNumber->setCanvasPivot(glm::vec2(-0.5f, 0.5f));
	fpsNumber->setVisibility(false);
	debugCanvas->addText("fpsNumber", fpsNumber, 0);

	resolutionNumber = UI::Text::createWithOutline("resolution: 00000 x 00000", glm::vec2(5.0f, -19.0f), fontID, color, outlineColor, UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 32);
	resolutionNumber->setPivot(glm::vec2(-0.5f, 0.5f));
	resolutionNumber->setCanvasPivot(glm::vec2(-0.5f, 0.5f));
	resolutionNumber->setVisibility(false);
	debugCanvas->addText("resolutionNumber", resolutionNumber, 0);

	auto vsync = Application::getInstance().getGLView()->isVsyncEnabled();
	vsyncMode = UI::Text::createWithOutline("vsync: false", glm::vec2(5.0f, -33.0f), fontID, color, outlineColor, UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 16);
	vsyncMode->setPivot(glm::vec2(-0.5f, 0.5f));
	vsyncMode->setCanvasPivot(glm::vec2(-0.5f, 0.5f));
	vsyncMode->setVisibility(false);
	debugCanvas->addText("vsyncMode", vsyncMode, 0);

	hardwardInfo = UI::Text::createWithOutline("CPU:" + CPUName + "\n" + GPUVendor + "\n" + GPURenderer + "\n" + GLVersion, glm::vec2(5.0f, -61.0f), fontID, color, outlineColor, UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 128);
	hardwardInfo->setPivot(glm::vec2(-0.5f, 0.5f));
	hardwardInfo->setCanvasPivot(glm::vec2(-0.5f, 0.5f));
	hardwardInfo->setVisibility(false);
	debugCanvas->addText("hardwardInfo", hardwardInfo, 0);

	playerPosition = UI::Text::createWithOutline("player position: 00000.00, 00000.00, 00000.00", glm::vec2(5.0f, -131.0f), fontID, color, outlineColor, UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 64);
	playerPosition->setPivot(glm::vec2(-0.5f, 0.5f));
	playerPosition->setCanvasPivot(glm::vec2(-0.5f, 0.5f));
	playerPosition->setVisibility(false);
	debugCanvas->addText("playerPosition", playerPosition, 0);

	playerRotation = UI::Text::createWithOutline("player rotation: 00000.00, 00000.00, 00000.00 (Facing north)", glm::vec2(5.0f, -145.0f), fontID, color, outlineColor, UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 128);
	playerRotation->setPivot(glm::vec2(-0.5f, 0.5f));
	playerRotation->setCanvasPivot(glm::vec2(-0.5f, 0.5f));
	playerRotation->setVisibility(false);
	debugCanvas->addText("playerRotation", playerRotation, 0);

	playerChunkPosition = UI::Text::createWithOutline("player chunk position: 00000.00, 00000.00, 00000.00", glm::vec2(5.0f, -159.0f), fontID, color, outlineColor, UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 64);
	playerChunkPosition->setPivot(glm::vec2(-0.5f, 0.5f));
	playerChunkPosition->setCanvasPivot(glm::vec2(-0.5f, 0.5f));
	playerChunkPosition->setVisibility(false);
	debugCanvas->addText("playerChunkPosition", playerChunkPosition, 0);

	playerLookingAt = UI::Text::createWithOutline("player looking at: 000000, 000000, 000000 Face (front)", glm::vec2(5.0f, -173.0f), fontID, color, outlineColor, UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 64);
	playerLookingAt->setPivot(glm::vec2(-0.5f, 0.5f));
	playerLookingAt->setCanvasPivot(glm::vec2(-0.5f, 0.5f));
	playerLookingAt->setVisibility(false);
	debugCanvas->addText("playerLookingAt", playerLookingAt, 0);

	chunkNumbers = UI::Text::createWithOutline("Chunks: 00000 / 00000 / 00000", glm::vec2(5.0f, -201.f), fontID, color, outlineColor, UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 32);
	chunkNumbers->setPivot(glm::vec2(-0.5f, 0.5f));
	chunkNumbers->setCanvasPivot(glm::vec2(-0.5f, 0.5f));
	chunkNumbers->setVisibility(false);
	debugCanvas->addText("chunkNumbers", chunkNumbers, 0);

	biome = UI::Text::createWithOutline("biome: type / 00.00 / 00.00", glm::vec2(5.0f, -215.0f), fontID, color, outlineColor, UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 128);
	biome->setPivot(glm::vec2(-0.5f, 0.5f));
	biome->setCanvasPivot(glm::vec2(-0.5f, 0.5f));
	biome->setVisibility(false);
	debugCanvas->addText("biome", biome, 0);

	region = UI::Text::createWithOutline("region: 000", glm::vec2(5.0f, -229.0f), fontID, color, outlineColor, UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 16);
	region->setPivot(glm::vec2(-0.5f, 0.5f));
	region->setCanvasPivot(glm::vec2(-0.5f, 0.5f));
	region->setVisibility(false);
	debugCanvas->addText("region", region, 0);
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
						curText = lastCommand;

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
								std::cout << "Success." << std::endl;
								updateCommandHistory();
							}
							else
							{
								std::cout << "Fail." << std::endl;
							}
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
							lastCommand = command;
							return true;
						}
						else
						{
							return false;
						}
					}
				}
				if (size == 4)
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
								lastCommand = command;
								return true;
							}
							catch (...)
							{
								if (split.at(2) == "default")
								{
									player->setMovementSpeed(15.0f);
									executedCommandHistory.push_back("Set player speed to default (15)");
									lastCommand = command;
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
								lastCommand = command;
								return true;
							}
							catch (...)
							{
								// do nothing
							}
						}
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

							player->setPosition(glm::vec3(x, y, z));
							executedCommandHistory.push_back("Set player position to (" + split.at(2) + ", " + split.at(3) + ", " + split.at(4) + ")");
							lastCommand = command;
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

							player->setRotation(glm::vec3(x, y, z));
							executedCommandHistory.push_back("Set player rotation to (" + split.at(2) + ", " + split.at(3) + ", " + split.at(4) + ")");
							lastCommand = command;
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

							player->setPosition(glm::vec3(x, y, z) + player->getPosition());
							executedCommandHistory.push_back("Added player position by (" + split.at(3) + ", " + split.at(4) + ", " + split.at(5) + ")");
							lastCommand = command;
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

							player->setRotation(glm::vec3(x, y, z) + player->getRotation());
							executedCommandHistory.push_back("Added player rotation by (" + split.at(3) + ", " + split.at(4) + ", " + split.at(5) + ")");
							lastCommand = command;
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
						lastCommand = command;
						return true;
					}
				}
			}
			else if (commandStr == "world")
			{
				if (size == 3)
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
							lastCommand = command;
							return true;
						}
					}
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
				}
				else if (size == 3)
				{
					auto arg1 = split.at(1);
					auto arg2 = split.at(2);

					bool arg2Bool = arg2 == "true" ? true : false;

					if (arg1 == "update" || arg1 == "u")
					{
						chunkMap->setUpdateChunkMapMode(arg2Bool);
						if (arg2Bool)
						{
							executedCommandHistory.push_back("Enabled chunk update");
						}
						else
						{
							executedCommandHistory.push_back("Disabled chunk update");
						}
						lastCommand = command;
						return true;
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
							lastCommand = command;
							return true;
						}
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
							lastCommand = command;
							return true;
						}
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

						int y = 0;
						try
						{
							y = std::stoi(split.at(3));
						}
						catch (...)
						{
							return false;
						}

						//chunkMap->printChunk(x, z);
					}
				}
			}
			else if (commandStr == "camera")
			{
				if (size == 3)
				{
					// camera arg1 arg2
					auto arg1 = split.at(1);

					if (arg1 == "speed")
					{
						try
						{
							float speed = std::stof(split.at(2));
							Camera::mainCamera->setSpeed(speed);
							executedCommandHistory.push_back("Set camera speed to " + split.at(2));
							lastCommand = command;
							return true;
						}
						catch (...)
						{
							if (split.at(2) == "default")
							{
								Camera::mainCamera->setSpeed(15.0f);
								executedCommandHistory.push_back("Set camera speed to default (15)");
								lastCommand = command;
								return true;
							}
						}
					}
					else if (arg1 == "fovy")
					{
						try
						{
							float fovy = std::stof(split.at(2));
							Camera::mainCamera->setFovy(fovy);
							executedCommandHistory.push_back("Set camera fovy to " + split.at(2));
							lastCommand = command;
							return true;
						}
						catch (...)
						{
							if (split.at(2) == "default")
							{
								Camera::mainCamera->setFovy(70.0f);
								executedCommandHistory.push_back("Set camera fovy to default (70)");
								lastCommand = command;
								return true;
							}
						}
					}
				}
			}
		}
	}

	return false;
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
		if (playerLookingAt->isVisible())
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
	biome->setVisibility(debugOutputVisibility);
	region->setVisibility(debugOutputVisibility);
}

void Voxel::DebugConsole::onFPSUpdate(int fps)
{
	fpsNumber->setText("fps: " + std::to_string(fps));
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

void Voxel::DebugConsole::updateChunkNumbers(const int visible, const int active, const int total)
{
	chunkNumbers->setText("chunks: " + std::to_string(visible) + " / " + std::to_string(active) + " / " + std::to_string(total));
}

void Voxel::DebugConsole::updateBiome(const std::string & biomeType, const std::string& terrainType, const float t, const float m)
{
	std::stringstream temp, moist;
	temp << std::fixed << std::showpoint << std::setprecision(2) << t;
	moist << std::fixed << std::showpoint << std::setprecision(2) << m;

	biome->setText("biome: " + biomeType + " / " + terrainType + " / " + temp.str() + " / " + moist.str());
}

void Voxel::DebugConsole::updateRegion(const unsigned int regionID)
{
	region->setText("region: " + std::to_string(regionID));
}
