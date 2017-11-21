#include "DebugConsole.h"
#include <Utility.h>
#include <Random.h>
#include <UI.h>
#include <Application.h>
#include <sstream>
#include <ChunkUtil.h>
#include <iomanip>
#include <InputHandler.h>
#include <Player.h>
#include <Game.h>
#include <World.h>
#include <Region.h>
#include <algorithm>
#include <Camera.h>
#include <ChunkMap.h>
#include <Setting.h>
#include <Calendar.h>
#include <TreeBuilder.h>

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
	, lastCommandIndex(0)
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
	this->settingPtr = &Setting::getInstance();

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

	resolutionNumber = UI::Text::createWithOutline("resolution: " + std::to_string(resolution.x) + " x " + std::to_string(resolution.y), glm::vec2(5.0f, -19.0f), fontID, color, outlineColor, UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 32);
	resolutionNumber->setPivot(glm::vec2(-0.5f, 0.5f));
	resolutionNumber->setCanvasPivot(glm::vec2(-0.5f, 0.5f));
	resolutionNumber->setVisibility(false);
	debugCanvas->addText("resolutionNumber", resolutionNumber, 0);

	auto vsync = Application::getInstance().getGLView()->isVsyncEnabled();
	vsyncMode = UI::Text::createWithOutline("vsync: " + (vsync ? std::string("true") : std::string("false")), glm::vec2(5.0f, -33.0f), fontID, color, outlineColor, UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 16);
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

	chunkNumbers = UI::Text::createWithOutline("Chunks: 00000 / 00000 / 00000", glm::vec2(5.0f, -201.f), fontID, color, outlineColor, UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 128);
	chunkNumbers->setPivot(glm::vec2(-0.5f, 0.5f));
	chunkNumbers->setCanvasPivot(glm::vec2(-0.5f, 0.5f));
	chunkNumbers->setVisibility(false);
	debugCanvas->addText("chunkNumbers", chunkNumbers, 0);

	biomeAndTerrainInfo = UI::Text::createWithOutline("biome: type / 00.00 / 00.00", glm::vec2(5.0f, -215.0f), fontID, color, outlineColor, UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 128);
	biomeAndTerrainInfo->setPivot(glm::vec2(-0.5f, 0.5f));
	biomeAndTerrainInfo->setCanvasPivot(glm::vec2(-0.5f, 0.5f));
	biomeAndTerrainInfo->setVisibility(false);
	debugCanvas->addText("biome", biomeAndTerrainInfo, 0);

	regionID = UI::Text::createWithOutline("region: 000", glm::vec2(5.0f, -229.0f), fontID, color, outlineColor, UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 16);
	regionID->setPivot(glm::vec2(-0.5f, 0.5f));
	regionID->setCanvasPivot(glm::vec2(-0.5f, 0.5f));
	regionID->setVisibility(false);
	debugCanvas->addText("region", regionID, 0);

	drawCallAndVertCount = UI::Text::createWithOutline("Draw calls: ----, vertices: -------", glm::vec2(5.0f, -257.0f), fontID, color, outlineColor, UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 64);
	drawCallAndVertCount->setPivot(glm::vec2(-0.5f, 0.5f));
	drawCallAndVertCount->setCanvasPivot(glm::vec2(-0.5f, 0.5f));
	drawCallAndVertCount->setVisibility(false);
	debugCanvas->addText("drawCallAndVertCount", drawCallAndVertCount, 0);
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
						executedCommandHistory.push_back("Refreshing chunk map");
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
							addCommandHistory(command);
							return true;
						}
						catch (...)
						{
							if (split.at(2) == "default")
							{
								Camera::mainCamera->setSpeed(15.0f);
								executedCommandHistory.push_back("Set camera speed to default (15)");
								addCommandHistory(command);
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
							addCommandHistory(command);
							return true;
						}
						catch (...)
						{
							if (split.at(2) == "default")
							{
								Camera::mainCamera->setFovy(70.0f);
								executedCommandHistory.push_back("Set camera fovy to default (70)");
								addCommandHistory(command);
								return true;
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
	biomeAndTerrainInfo->setVisibility(debugOutputVisibility);
	regionID->setVisibility(debugOutputVisibility);
	drawCallAndVertCount->setVisibility(debugOutputVisibility);
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

void Voxel::DebugConsole::updateDrawCallsAndVerticesSize()
{
	auto glView = Application::getInstance().getGLView();

	if (glView->doesCountDrawCalls() || glView->doesCountVerticesSize())
	{
		drawCallAndVertCount->setText("Draw calls: " + std::to_string(glView->getTotalDrawCalls()) + ", Vertices: " + std::to_string(glView->getTotalVerticesSize()));
	}
}
