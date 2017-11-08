#include "Game.h"

#include <World.h>
#include <Region.h>
#include <ChunkMap.h>
#include <ChunkMeshGenerator.h>
#include <ChunkSection.h>
#include <Chunk.h>
#include <ChunkUtil.h>
#include <ChunkWorkManager.h>
#include <Block.h>
#include <Biome.h>
#include <Terrain.h>

#include <Physics.h>

#include <Setting.h>

#include <InputHandler.h>
#include <Camera.h>
#include <Frustum.h>

#include <SpriteSheet.h>

#include <UI.h>
#include <NewUI.h>
#include <FontManager.h>

#include <ProgramManager.h>
#include <Program.h>
#include <glm\gtx\transform.hpp>
#include <Cube.h>
#include <Utility.h>
#include <Random.h>
#include <SimplexNoise.h>
#include <Color.h>
#include <Player.h>
#include <Skybox.h>
#include <Calendar.h>

#include <Application.h>
#include <GLView.h>

#include <DebugConsole.h>

#include <FileSystem.h>
#include <algorithm>

using namespace Voxel;

// Temporary. 
// Todo: Move this to game settings

Game::Game()
	: world(nullptr)
	, chunkMap(nullptr)
	, chunkMeshGenerator(nullptr)
	, input(&InputHandler::getInstance())
	, player(nullptr)
	, mouseX(0)
	, mouseY(0)
	, cameraMode(false)
	, cameraControlMode(false)
	, chunkWorkManager(nullptr)
	, defaultCanvas(nullptr)
	, loadingCanvas(nullptr)
	, debugConsole(nullptr)
	, skybox(nullptr)
	, calendar(nullptr)
	, settingPtr(nullptr)
	, gameState(GameState::IDLE)
	, loadingState(LoadingState::INITIALIZING)
	, reloadState(ReloadState::NONE)
{
	// init instances
	init();
	// After creation, set cursor to center
	input->setCursorToCenter();

	//Camera::mainCamera->initDebugFrustumLines();
}

Game::~Game()
{
	// Stop running mesh building before releasing
	chunkWorkManager->stop();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	chunkWorkManager->joinThread();

	// Release all instances
	release();

	// Release fonts
	FontManager::getInstance().clear();
}

void Voxel::Game::init()
{
	// Init random first
	initRandoms();

	settingPtr = &Setting::getInstance();

	// init spritesheet
	initSpriteSheets();

	// program
	auto program = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::COLOR_SHADER);
	// use it
	program->use(true);

	// Init world
	world = new World();

	// Init chunks
	chunkMap = new ChunkMap();
	chunkMeshGenerator = new ChunkMeshGenerator();
	chunkWorkManager = new ChunkWorkManager();

	// init physics
	physics = new Physics();

	// player
	player = new Player();

	// Lights
	program->setUniformVec4("ambientColor", glm::vec4(1.0f));
	program->setUniformFloat("pointLights[0].lightIntensity", 20.0f);
	program->setUniformVec4("pointLights[0].lightColor", glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

	// Skybox
	initSkyBox();

	// Initialize fog uniforms
	program->setUniformVec3("playerPosition", player->getPosition());
	program->setUniformFloat("fogDistance", skybox->getFogDistance());
	program->setUniformBool("fogEnabled", skybox->isFogEnabled());
	program->setUniformFloat("chunkBorderSize", Constant::CHUNK_BORDER_SIZE);

	// stop use
	program->use(false);

	// Calendar
	calendar = new Calendar();
	calendar->init();

	// UI & font
	initUI();

	TextureManager::getInstance().print();

	//Application::getInstance().getGLView()->setWindowedFullScreen(1);
	//defaultCanvas->setSize(glm::vec2(1920, 1080));
	//debugConsole->updateResolution(1920, 1080);
}

void Voxel::Game::release()
{
	std::cout << "[World] Releasing all instances\n";

	SpriteSheetManager::getInstance().releaseAll();
	TextureManager::getInstance().releaseAll();

	// delete everything
	if (chunkMap) delete chunkMap;
	if (chunkMeshGenerator) delete chunkMeshGenerator;
	if (chunkWorkManager) delete chunkWorkManager;

	if (physics) delete physics;

	if (player)	delete player;

	if (skybox) delete skybox;
	if (calendar) delete calendar;

	if (cursor) delete cursor;

	if (defaultCanvas) delete defaultCanvas;

	if (debugConsole) delete debugConsole;

	if (world) delete world;
}

void Voxel::Game::initSpriteSheets()
{
	auto& ssm = SpriteSheetManager::getInstance();

	ssm.addSpriteSheet("UISpriteSheet.json");
	ssm.addSpriteSheet("CursorSpriteSheet.json");
	ssm.addSpriteSheet("EnvironmentSpriteSheet.json");
}

void Voxel::Game::initRandoms()
{
	globalSeed = "ENGINE";
	Utility::Random::setSeed(globalSeed);
	Noise::Manager::init(globalSeed);

	auto& rand = Random::getInstance();
	rand.init(globalSeed);
}

void Voxel::Game::initUI()
{
	FontManager::getInstance().addFont("Pixel.ttf", 10);
	FontManager::getInstance().addFont("Pixel.ttf", 10, 2);

	initCursor();

	initDefaultCanvas();

	initLoadingScreen();

	initDebugConsole();
}

void Voxel::Game::initLoadingScreen()
{
	auto resolution = Application::getInstance().getGLView()->getScreenSize();
	
	// loading canvas
	loadingCanvas = UI::Canvas::create(resolution, glm::vec2(0));

	// Add bg
	auto bg = UI::Image::createFromSpriteSheet("UISpriteSheet", "1x1.png", glm::vec2(0), glm::vec4(1));
	bg->setScale(resolution);
	loadingCanvas->addImage("bg", bg, 0);

	// Add loading label
	auto loadingLabel = UI::Image::createFromSpriteSheet("UISpriteSheet", "loading_label.png", glm::vec2(-20.0f, 20.0f), glm::vec4(1));
	loadingLabel->setPivot(glm::vec2(0.5f, -0.5f));
	loadingLabel->setCanvasPivot(glm::vec2(0.5f, -0.5f));
	loadingCanvas->addImage("loadingLabel", loadingLabel, 1);
}

void Voxel::Game::initDefaultCanvas()
{
	auto resolution = Application::getInstance().getGLView()->getScreenSize(); 
	
	defaultCanvas = UI::Canvas::create(resolution, glm::vec2(0));

	// Add temporary cross hair
	auto crossHairImage = UI::Image::createFromSpriteSheet("UISpriteSheet", "cross_hair.png", glm::vec2(0), glm::vec4(1.0f));
	defaultCanvas->addImage("crossHair", crossHairImage, 0);

	// Add time label
	UI::Text* timeLabel = UI::Text::createWithOutline(calendar->getTimeInStr(false), glm::vec2(-200.0f, -5.0f), 2, glm::vec4(1.0f), glm::vec4(0, 0, 0, 1), UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 10);
	timeLabel->setPivot(glm::vec2(-0.5f, 0.5f));
	timeLabel->setCanvasPivot(glm::vec2(0.5f, 0.5f));
	defaultCanvas->addText("timeLabel", timeLabel, 0);

}

void Voxel::Game::initDebugConsole()
{
	auto resolution = Application::getInstance().getGLView()->getScreenSize();

	// Debug. This creates all the debug UI components
	debugConsole = new DebugConsole();
	debugConsole->toggleDubugOutputs();

	// for debugging
	debugConsole->player = player;
	debugConsole->game = this;
	debugConsole->chunkMap = chunkMap;
	debugConsole->world = world;
	debugConsole->calendar = calendar;
}

void Voxel::Game::initCursor()
{
	// cursor
	cursor = UI::Cursor::create();
}

void Voxel::Game::initSkyBox()
{
	skybox = new Skybox();
	// Always set skybox with max render distance
	skybox->init(settingPtr->getRenderDistance());
}

void Voxel::Game::initMeshBuilderThread()
{
	// run first, create thread later
	unsigned int concurentThreadsSupported = std::thread::hardware_concurrency();
	std::cout << "Number of supporting threads: " << concurentThreadsSupported << std::endl;

	chunkWorkManager->run();
	chunkWorkManager->createThreads(chunkMap, chunkMeshGenerator, world, concurentThreadsSupported);
}

void Voxel::Game::createNew(const std::string & worldName)
{
	auto start = Utility::Time::now();
	// Create folder
	//FileSystem& fs = FileSystem::getInstance();
	//fs.init();
	//fs.createNewWorldSave("New World");

	// Creates new world. 
	// First, create player.
	// Todo: Load player from menu screen. 
	createPlayer();

	// Threads
	initMeshBuilderThread();

	// Create world
	createWorld();

	// Then based init chunks
	createChunkMap();

	Region* curRegion = world->getCurrentRegion();
	debugConsole->updateRegion(curRegion->getID());
	Biome biomeType = curRegion->getBiomeType();
	debugConsole->updateBiome(Biome::biomeTypeToString(biomeType), Terrain::terrainTypeToString(curRegion->getTerrainType()), biomeType.getTemperature(), biomeType.getMoisture());
	
	auto end = Utility::Time::now();
	std::cout << "New world creation took " << Utility::Time::toMilliSecondString(start, end) << std::endl;
}

World * Voxel::Game::getWorld()
{
	return world;
}

void Game::createPlayer()
{
	// For now, set 0 to 0. Todo: Make topY() function that finds hieghts y that player can stand.
	player->init();

	// Todo: set this to false. For now, set ture for debug
	player->setFly(true);
	player->setViewMode(1);
	// Update matrix
	player->updateViewMatrix();
	// Based on player's matrix, update frustum
	Camera::mainCamera->getFrustum()->update(player->getPosition(), player->getOrientation());
	Camera::mainCamera->setSpeed(100.0f);

	// for debug
	//player->initYLine();
	player->initRayLine(); 
	player->initBoundingBoxLine();
}

void Game::createChunkMap()
{
	// Debug: measure time
	auto start = Utility::Time::now();
	
	// Initilize chunks near player based on render distance
	auto chunkCoordinates = chunkMap->initChunkNearPlayer(player->getPosition(), settingPtr->getRenderDistance());

	// Initilize active chunks.
	chunkMap->initActiveChunks();
	
	// Sort chunk coordinates closer to player
	glm::vec2 p = chunkCoordinates.front();
	std::sort(chunkCoordinates.begin(), chunkCoordinates.end(), [p](const glm::vec2& lhs, const glm::vec2& rhs) { return glm::distance(p, lhs) < glm::distance(p, rhs); });

	for (auto xz : chunkCoordinates)
	{
		chunkWorkManager->addPreGenerateWork(glm::ivec2(xz));
	}

	// Get line program for debug
	auto program = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::LINE_SHADER);

	// for debug
	chunkMap->initChunkBorderDebug(program);

	// block outline. this is not debug. Required for building.
	chunkMap->initBlockOutline(program);

	// End measure
	auto end = Utility::Time::now();
	std::cout << "[ChunkMap] ElapsedTime: " << Utility::Time::toMilliSecondString(start, end) << std::endl;
}

void Voxel::Game::createWorld()
{
	world->setTemperature(0.5f, 1.5f);
	world->setMoisture(0.5f, 1.5f);
	world->init(10, 10, 0, globalSeed);

	auto startingRegionSitePos = world->getCurrentRegion()->getSitePosition();
	auto& rand = Random::getInstance();
	auto randX = rand.getRandomFloat(startingRegionSitePos.x - 100.0f, startingRegionSitePos.x + 100.0f);
	auto randZ = rand.getRandomFloat(startingRegionSitePos.y - 100.0f, startingRegionSitePos.y + 100.0f);

	player->setPosition(glm::vec3(randX, 0, randZ), false);
}

void Voxel::Game::teleportPlayer(const glm::vec3 & position)
{
	std::cout << "Teleporint to " << Utility::Log::vec3ToStr(position) << "\n";
	// move player
	player->setPosition(position, false);
	player->setLookingBlock(nullptr, Cube::Face::NONE);
	player->setRotation(glm::vec3(0), false);

	// clear chunk work manager
	chunkWorkManager->clear();
	chunkWorkManager->notify();

	loadingState = LoadingState::RELOADING;
	reloadState = ReloadState::CHUNK_MAP;
}

void Game::update(const float delta)
{
	if (loadingState == LoadingState::INITIALIZING)
	{
		if (chunkWorkManager->isFirstInitDone())
		{
			loadingState = LoadingState::FINISHED;

			// get top y at player position
			auto playerPosition = player->getPosition();
			float topY = static_cast<float>(chunkMap->getTopYAt(glm::vec2(playerPosition.x, playerPosition.z))) + 1.5f;
			playerPosition.y = topY;
			player->setPosition(playerPosition, false);
		}
	}
	else if (loadingState == LoadingState::RELOADING)
	{
		if (chunkWorkManager->isClearing())
		{
			// chunk work manager is clearing. don't update stuffs
			return;
		}
		else if (chunkWorkManager->isWaitingMainThread())
		{
			std::cout << "Chunk work manager is waiting for main thread\n.";

			Utility::Random::resetGenerator();
			Random::getInstance().resetAll();

			if (reloadState == ReloadState::CHUNK_MAP)
			{
				chunkMap->clear();

				std::cout << "Regenerating chunk map and get chunk work manager back to work\n";

				createChunkMap();
			}
			else if (reloadState == ReloadState::CHUNK_MESH)
			{
				chunkMap->clearAllMeshes();

				chunkMap->rebuildAllMeshes(chunkWorkManager);

				std::cout << "Regenerating meshes and get chunk work manager back to work\n";
			}
			else if (reloadState == ReloadState::WORLD)
			{
				world->rebuildWorldMap();
				chunkMap->clear();
				createChunkMap();
			}

			chunkWorkManager->resumeWork();

			return;
		}
		else
		{
			if (chunkWorkManager->isGeneratingChunks())
			{
				return;
			}
			else
			{
				loadingState = LoadingState::FINISHED;
				reloadState = ReloadState::NONE;
			}
		}
	}
	else
	{
		// update input
		updateKeyboardInput(delta);
		updateMouseMoveInput(delta);
		updateMouseClickInput();
		updateMouseScrollInput(delta);
		updateControllerInput(delta);

		checkUnloadedChunks();

		// Update physics
		updatePhysics(delta);

		// Resolve collision in game
		updateCollisionResolution();

		bool playerMoved = player->didMoveThisFrame();
		bool playerRotated = player->didRotateThisFrame();

		// After resolving collision and updating physics, update player's movement
		player->updateMovement(delta);

		// First check visible chunk
		Camera::mainCamera->getFrustum()->update(player->getViewMatrix());

		// After updating frustum, run frustum culling to find visible chunk
		//std::vector<glm::ivec2> visibleChunks;
		//std::unordered_set<glm::ivec2, KeyFuncs, KeyFuncs> visibleChunks;
		//int totalVisible = chunkMap->findVisibleChunk(visibleChunks);
		int totalVisible = chunkMap->findVisibleChunk();

		/*
		auto sortStart = Utility::Time::now();
		chunkWorkManager->sortBuildMeshQueue(chunkMap->getCurrentChunkXZ(), visibleChunks);
		auto sortEnd = Utility::Time::now();
		std::cout << "sort: " << Utility::Time::toMicroSecondString(sortStart, sortEnd) << std::endl;
		*/

		if (playerMoved || playerRotated)
		{
			auto playerPos = player->getPosition();
			// Also update raycast
			updatePlayerRaycast();

			debugConsole->updatePlayerPosition(playerPos);
			debugConsole->updatePlayerRotation(player->getRotation());
		}

		if (playerMoved)
		{
			// if player moved, update chunk only if chunk work manager is not aborting
			updateChunks();

			auto playerPos = player->getPosition();

			bool regionChanged = world->updatePlayerPos(playerPos);

			if (regionChanged)
			{
				Region* curRegion = world->getCurrentRegion();
				debugConsole->updateRegion(curRegion->getID());
				Biome biomeType = curRegion->getBiomeType();
				debugConsole->updateBiome(Biome::biomeTypeToString(biomeType), Terrain::terrainTypeToString(curRegion->getTerrainType()), biomeType.getTemperature(), biomeType.getMoisture());
			}

			auto program = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::COLOR_SHADER);
			program->use(true);
			program->setUniformVec3("playerPosition", playerPos);
		}

		debugConsole->updateChunkNumbers(totalVisible, chunkMap->getActiveChunksCount(), chunkMap->getSize(), chunkWorkManager->getDebugOutput());

		player->update(delta);

		if (player->isOnTPViewMode())
		{
			updatePlayerCameraCollision();
		}
		
		player->updateCameraDistanceZ(delta);

		calendar->update(delta);

		skybox->update(delta);
		skybox->updateColor(calendar->getHour(), calendar->getMinutes(), calendar->getSeconds());

		defaultCanvas->getText("timeLabel")->setText(calendar->getTimeInStr(false));
	}
}

void Voxel::Game::updateInput(const float delta)
{
	if (loadingState == LoadingState::FINISHED)
	{
	}
}

glm::vec3 Voxel::Game::getMovedDistByKeyInput(const float angleMod, const glm::vec3 axis, float distance)
{
	float angle = 0;
	if (axis.y == 1.0f)
	{
		angle = Camera::mainCamera->getAngleY();
	}

	angle += angleMod;

	if (angle < 0) angle += 360.0f;
	else if (angle >= 360.0f) angle -= 360.0f;

	auto rotateMat = glm::rotate(mat4(1.0f), glm::radians(angle), axis);
	auto movedDist = glm::inverse(rotateMat) * glm::vec4(0, 0, distance, 1);

	return movedDist;
}

void Voxel::Game::updateKeyboardInput(const float delta)
{
	if (input->getKeyDown(GLFW_KEY_ESCAPE, true))
	{
		if (debugConsole->isConsoleOpened())
		{
			debugConsole->closeConsole();
		}
		else
		{
			Application::getInstance().getGLView()->close();
		}
		return;
	}

	if (input->getKeyDown(GLFW_KEY_F3, true))
	{
		debugConsole->toggleDubugOutputs();
		return;
	}

	if (input->getKeyDown(GLFW_KEY_GRAVE_ACCENT, true))
	{
		if (debugConsole->isConsoleOpened())
		{
			debugConsole->closeConsole();
		}
		else
		{
			debugConsole->openConsole();
		}
		return;
	}

	if (debugConsole->isConsoleOpened())
	{
		// While console is opened, block all input in world
		debugConsole->updateConsoleInputText(input->getBuffer());
		return;
	}


	if (input->getKeyDown(GLFW_KEY_LEFT_ALT, true))
	{
		gameState = GameState::CURSOR_MODE;
		cursor->setVisibility(true);
	}
	else if (input->getKeyUp(GLFW_KEY_LEFT_ALT, true))
	{
		gameState = GameState::IDLE;
		cursor->setVisibility(false);
	}

	if(input->getKeyDown(GLFW_KEY_T, true))
	{
		/*
		auto testPos = glm::ivec3(-24, -40, -1);
		auto testLocal = glm::ivec3(0);
		auto testChunk = glm::ivec3(0);
		chunkMap->blockWorldCoordinateToLocalAndChunkSectionCoordinate(testPos, testLocal, testChunk);
		std::cout << "testPos: " << Utility::Log::vec3ToStr(testPos) << std::endl;
		std::cout << "testLocal: " << Utility::Log::vec3ToStr(testLocal) << std::endl;
		std::cout << "testChunk: " << Utility::Log::vec3ToStr(testChunk) << std::endl;
		*/

		/*
		std::vector<Block*> collidableBlocks;
		//chunkMap->queryBottomCollidableBlocksInY(player->getNextPosition(), collidableBlocks);
		chunkMap->queryTopCollidableBlocksInY(player->getPosition(), collidableBlocks);

		std::cout << "Queried " << collidableBlocks.size() << " blocks\n";
		for (auto block : collidableBlocks)
		{
			std::cout << "Block pos = " << Utility::Log::vec3ToStr(block->getWorldCoordinate()) << std::endl;
		}
		*/
	}

	if (input->getKeyDown(GLFW_KEY_Y, true))
	{
		/*
		std::mt19937 generator;
		std::binomial_distribution<int> dist(9, 0.5);

		std::cout << "max = " << dist.max() << std::endl;
		std::cout << "min = " << dist.min() << std::endl;

		std::vector<int> freq(10, 0);

		for (int i = 0; i < 1000; i++)
		{
			freq.at(dist(generator))++;
		}

		for (int i = 0; i < 10; i++)
		{
			std::cout << i << ": ";
			int len = freq.at(i) / 50;
			for (int j = 0; j < len; j++)
			{
				std::cout << "*";
			}
			std::cout << "   (" << freq.at(i) << ")\n";
		}
		*/
	}
	
	if (input->getKeyDown(GLFW_KEY_P, true))
	{
		Camera::mainCamera->print();
		auto playerPos = player->getPosition();
		auto playerRot = player->getRotation();
		std::cout << "Player is at (" << playerPos.x << ", " << playerPos.y << ", " << playerPos.z << "), rotated (" << playerRot.x << ", " << playerRot.y << ", " << playerRot.z << ")\n";
	}

	if (input->getKeyDown(GLFW_KEY_N, true) && input->getMods() == 0)
	{
		Application::getInstance().getGLView()->setWindowedFullScreen(1);
		defaultCanvas->setSize(glm::vec2(1920, 1080));
		debugConsole->updateResolution(1920, 1080);
	}
	else if (input->getKeyDown(GLFW_KEY_N, true) && input->getMods() == GLFW_MOD_CONTROL)
	{
		Application::getInstance().getGLView()->setWindowed(1280, 720);
		Application::getInstance().getGLView()->setWindowPosition(100, 100);
		defaultCanvas->setSize(glm::vec2(1280, 720));
		debugConsole->updateResolution(1280, 720);
	}
	else if (input->getKeyDown(GLFW_KEY_N, true) && input->getMods() == (GLFW_MOD_CONTROL | GLFW_MOD_SHIFT))
	{
		Application::getInstance().getGLView()->setWindowedFullScreen(0);
		defaultCanvas->setSize(glm::vec2(1920, 1080));
		debugConsole->updateResolution(1920, 1080);
	}

	if (input->getKeyDown(GLFW_KEY_V, true) && !input->getKeyDown(GLFW_KEY_LEFT_CONTROL))
	{
		Application::getInstance().getGLView()->setVsync(true);
		debugConsole->updateVsync(true);
	}
	else if (input->getKeyDown(GLFW_KEY_V, true) && input->getKeyDown(GLFW_KEY_LEFT_CONTROL))
	{
		Application::getInstance().getGLView()->setVsync(false);
		debugConsole->updateVsync(false);
	}

	// Keyboard
	if (cameraControlMode)
	{
		if (input->getKeyDown(InputHandler::KEY_INPUT::MOVE_FOWARD))
		{
			Camera::mainCamera->addPosition(getMovedDistByKeyInput(-180.0f, glm::vec3(0, 1, 0), delta));
		}
		else if (input->getKeyDown(InputHandler::KEY_INPUT::MOVE_BACKWARD))
		{
			Camera::mainCamera->addPosition(getMovedDistByKeyInput(0, glm::vec3(0, 1, 0), delta));
		}

		if (input->getKeyDown(InputHandler::KEY_INPUT::MOVE_LEFT))
		{
			Camera::mainCamera->addPosition(getMovedDistByKeyInput(90.0f, glm::vec3(0, 1, 0), delta));
		}
		else if (input->getKeyDown(InputHandler::KEY_INPUT::MOVE_RIGHT))
		{
			Camera::mainCamera->addPosition(getMovedDistByKeyInput(-90.0f, glm::vec3(0, 1, 0), delta));
		}

		if (input->getKeyDown(InputHandler::KEY_INPUT::MOVE_UP))
		{
			Camera::mainCamera->addPosition(glm::vec3(0, delta, 0));
		}
		else if (input->getKeyDown(InputHandler::KEY_INPUT::MOVE_DOWN))
		{
			Camera::mainCamera->addPosition(glm::vec3(0, -delta, 0));
		}
	}
	else
	{
		if (gameState == GameState::IDLE)
		{
			if (input->getKeyDown(InputHandler::KEY_INPUT::MOVE_FOWARD))
			{
				player->moveFoward(delta);
			}
			else if (input->getKeyDown(InputHandler::KEY_INPUT::MOVE_BACKWARD))
			{
				player->moveBackward(delta);
			}

			if (input->getKeyDown(InputHandler::KEY_INPUT::MOVE_LEFT))
			{
				player->moveLeft(delta);
			}
			else if (input->getKeyDown(InputHandler::KEY_INPUT::MOVE_RIGHT))
			{
				player->moveRight(delta);
			}

			if (player->isFlying())
			{
				if (input->getKeyDown(InputHandler::KEY_INPUT::MOVE_UP))
				{
					player->moveUp(delta);
				}
				else if (input->getKeyDown(InputHandler::KEY_INPUT::MOVE_DOWN))
				{
					player->moveDown(delta);
				}
			}
			else
			{
				if (player->isOnGround())
				{
					if (input->getKeyDown(InputHandler::KEY_INPUT::JUMP, true))
					{
						player->jump();
						physics->applyJumpForceToPlayer(glm::vec3(0, 2.2f, 0));
					}
				}
			}
		}
	}

	// For debug
	if (input->getKeyDown(GLFW_KEY_BACKSPACE))
	{
		Camera::mainCamera->print();
	}

	if (input->getKeyDown(GLFW_KEY_C, true))
	{
		cameraMode = !cameraMode;
		std::cout << "[World] Camera mode " << std::string(cameraMode ? "enabled" : "disabled") << std::endl;
	}

	if (input->getKeyDown(GLFW_KEY_X, true))
	{
		cameraControlMode = !cameraControlMode;
		std::cout << "[World] Camera control mode " << std::string(cameraControlMode ? "enabled" : "disabled") << std::endl;
	}

	if (input->getKeyDown(GLFW_KEY_1, true))
	{
		cursor->setCursorType(UI::Cursor::CursorType::POINTER);
	}

	if (input->getKeyDown(GLFW_KEY_2, true))
	{
		cursor->setCursorType(UI::Cursor::CursorType::FINGER);
	}

	if (input->getKeyDown(GLFW_KEY_3, true))
	{
		player->setRotation(glm::vec3(0, 0, 0), false);
	}
	if (input->getKeyDown(GLFW_KEY_4, true))
	{
		player->setRotation(glm::vec3(0, 180, 0), false);
	}

	if (input->getKeyDown(GLFW_KEY_5, true))
	{
		player->setPosition(glm::vec3(365.244, 68, -117.754), false);
		player->setRotation(glm::vec3(291.5, 302.25, 0), false);
	}

	if (input->getKeyDown(GLFW_KEY_6, true))
	{
		Camera::mainCamera->setPosition(player->getPosition());
	}
}

void Voxel::Game::updateMouseMoveInput(const float delta)
{
	double x, y;
	input->getMousePosition(x, y);

	double dx = x - mouseX;
	double dy = y - mouseY;
	mouseX = x;
	mouseY = y;

	if (gameState == GameState::IDLE)
	{
		// Todo: Find better way? focing elapsed time in mouse pos update to 1/60 if exceeds 1/60
		// This is because, mouse position updates more quicker in higher frame, which means smaller changes from
		// previous position. However because of frequent change, cumulitive change of angle seemed to be same with 
		// 60 fps. So if if delta is bigger than 1/60, force 1/60

		float newDelta = delta;

		if (delta < 0.0166666667f)
		{
			newDelta = 0.01666666666667f;
		}

		if (debugConsole->isConsoleOpened())
		{
			// Stop input while opening console
			return;
		}

		if (cameraControlMode)
		{
			if (dx != 0)
			{
				Camera::mainCamera->addAngle(vec3(0, dx * newDelta * 15.0f, 0));
			}

			if (dy != 0)
			{
				Camera::mainCamera->addAngle(vec3(dy * newDelta * 15.0f, 0, 0));
			}
		}
		else
		{
			if (dx != 0.0)
			{
				player->addRotationY(newDelta * static_cast<float>(dx));
			}

			if (dy != 0.0)
			{
				player->addRotationX(newDelta * static_cast<float>(-dy));
			}
		}
	}
	else if (gameState == GameState::CURSOR_MODE)
	{
		cursor->addPosition(glm::vec2(dx, -dy));
	}
}

void Voxel::Game::updateMouseClickInput()
{
	if (debugConsole->isConsoleOpened())
	{
		// Stop input while opening console
		return;
	}

	if (gameState == GameState::IDLE)
	{
		if (input->getMouseDown(GLFW_MOUSE_BUTTON_1, true))
		{
			if (player->isLookingAtBlock())
			{
				auto lookingBlock = player->getLookingBlock();
				auto blockPos = player->getLookingBlock()->getWorldCoordinate();
				chunkMap->removeBlockAt(blockPos, chunkWorkManager);
				updatePlayerRaycast();
			}
		}
		else if (input->getMouseDown(GLFW_MOUSE_BUTTON_2, true))
		{
			if (player->isLookingAtBlock())
			{
				auto lookingBlock = player->getLookingBlock();
				auto blockPos = player->getLookingBlock()->getWorldCoordinate();
				chunkMap->placeBlockFromFace(blockPos, Block::BLOCK_ID::GRASS, player->getLookingFace(), chunkWorkManager);
				updatePlayerRaycast();
			}
		}
	}
	else if (gameState == GameState::CURSOR_MODE)
	{

	}
}

void Voxel::Game::updateMouseScrollInput(const float delta)
{
	auto mouseScroll = input->getMouseScrollValue();
	if (mouseScroll == 1)
	{
		player->zoomInCamera();
	}
	else if(mouseScroll == -1)
	{
		player->zoomOutCamera();
	}
	// Else, mouse scroll didn't move
}

void Voxel::Game::updateControllerInput(const float delta)
{
	if (input->hasController())
	{
		auto valueLeftAxisX = input->getAxisValue(IO::XBOX_360::AXIS::L_AXIS_X);
		if (valueLeftAxisX  > 0.0f)
		{
			// Right
			player->moveRight(delta);
		}
		else if (valueLeftAxisX < 0.0f)
		{
			player->moveLeft(delta);
		}
		// Else, didn't move

		auto valueLeftAxisY = input->getAxisValue(IO::XBOX_360::AXIS::L_AXIS_Y);
		if (valueLeftAxisY > 0.0f)
		{
			//foward
			player->moveFoward(delta);
		}
		else if (valueLeftAxisY < 0.0f)
		{
			// back
			player->moveBackward(delta);
		}

		if (player->isFlying())
		{
			auto valueLeftTrigger = input->getAxisValue(IO::XBOX_360::AXIS::LT);
			if (valueLeftTrigger > 0.0f)
			{
				player->moveUp(delta);
			}

			auto valueRightTrigger = input->getAxisValue(IO::XBOX_360::AXIS::RT);
			if (valueRightTrigger > 0.0f)
			{
				player->moveDown(delta);
			}
		}
		else
		{
			if (player->isOnGround())
			{
				if (input->isControllerButtonDown(Voxel::IO::XBOX_360::BUTTON::B))
				{
					player->jump();
					physics->applyJumpForceToPlayer(glm::vec3(0, 2.2f, 0));
				}
			}
		}

		auto valueRightAxisX = input->getAxisValue(IO::XBOX_360::AXIS::R_AXIS_X);
		if (valueRightAxisX != 0.0f)
		{
			//std::cout << "V = " << valueRightAxisX << std::endl;
			//std::cout << "d = " << delta << std::endl;
			player->addRotationY(delta * valueRightAxisX * 10.0f);
		}

		auto valueRightAxisY = input->getAxisValue(IO::XBOX_360::AXIS::R_AXIS_Y);
		if (valueRightAxisY != 0.0f)
		{
			player->addRotationX(delta * valueRightAxisY * 10.0f);
		}
	}
}

void Voxel::Game::updateCollisionResolution()
{
	if (player->isFlying()) return;

	auto start = Utility::Time::now();

	if (player->isOnGround())
	{
		// Player is on ground. First check if player auto jumped. Then, resolve XZ.
		std::vector<Block*> collidableBlocks;
		// Query near by block in XZ axis
		chunkMap->queryNearByCollidableBlocksInXZ(player->getNextPosition(), collidableBlocks);

		bool autoJumped = physics->resolveAutoJump(player, collidableBlocks);

		if (!autoJumped)
		{
			// clear list
			//collidableBlocks.clear();

			// Query near by block in XZ axis
			//chunkMap->queryNearByCollidableBlocksInXZ(player->getNextPosition(), collidableBlocks);

			// Reoslve XZ
			physics->resolvePlayerAndBlockCollisionInXZAxis(player, collidableBlocks);

			// clear list
			collidableBlocks.clear();

			// query again in negative Y 
			chunkMap->queryBottomCollidableBlocksInY(player->getPosition(), collidableBlocks);

			// At this moment, player won't have any blocks that are colliding in XZ direction. Check bottom y. If so, player hit the ground.
			physics->checkIfPlayerIsFalling(player, collidableBlocks);
		}
		else
		{
			return;
		}
	}
	else
	{
		// Either jumping or falling
		if (player->isJumping())
		{
			// player is jumping. Check if player collided on top. Then, resolve XZ.
			std::vector<Block*> collidableBlocks;

			// Query near by block in XZ axis
			chunkMap->queryNearByCollidableBlocksInXZ(player->getNextPosition(), collidableBlocks);

			// Reoslve XZ
			physics->resolvePlayerAndBlockCollisionInXZAxis(player, collidableBlocks);

			// clear list
			collidableBlocks.clear();

			// query again in negative Y 
			chunkMap->queryTopCollidableBlocksInY(player->getNextPosition(), collidableBlocks);

			// At this moment, player won't have any blocks that are colliding in XZ direction. Check bottom y. If so, player hit the ground.
			physics->resolvePlayerTopCollision(player, collidableBlocks);
		}
		else if (player->isFalling())
		{
			// Player is falling. Check if player collided on bottom. Then, resolve XZ.
			std::vector<Block*> collidableBlocks;

			// Query near by block in XZ axis
			chunkMap->queryNearByCollidableBlocksInXZ(player->getNextPosition(), collidableBlocks);

			// Reoslve XZ
   			physics->resolvePlayerAndBlockCollisionInXZAxis(player, collidableBlocks);

			// clear list
			collidableBlocks.clear();

			// query again in negative Y 
			chunkMap->queryBottomCollidableBlocksInY(player->getNextPosition(), collidableBlocks);

			// At this moment, player won't have any blocks that are colliding in XZ direction. Check bottom y. If so, player hit the ground.
			physics->resolvePlayerBottomCollision(player, collidableBlocks);
		}
	}

	auto end = Utility::Time::now();
	//std::cout << "Player vs blocks collision resolution took: " << Utility::Time::toMicroSecondString(start, end) << std::endl;
}

void Voxel::Game::updatePhysics(const float delta)
{
	bool jumpForceApplied = physics->updatePlayerJumpForce(player, delta);

	if (!jumpForceApplied)
	{
		physics->applyGravity(player, delta);
	}
}

void Voxel::Game::updateChunks()
{
	// Update chunk.
	// Based on player position, check if player moved to new chunk
	// If so, we need to load new chunks. 
	// Else, player reamains on same chunk as now.
	glm::ivec2 newChunkXZ = chunkMap->checkPlayerChunkPos(player->getPosition());
	glm::ivec2 curChunkXZ = chunkMap->getCurrentChunkXZ();

	if (newChunkXZ == curChunkXZ)
	{
		// still on same chunk. do nothing
		return;
	}
	else
	{
		// moved to new chunk
		glm::ivec2 absDist = glm::abs(newChunkXZ - curChunkXZ);

		auto rd = settingPtr->getRenderDistance();

		if (absDist.x >= rd || absDist.y >= rd)
		{
			// player moved more than render distance at once. consider this as teleport.
			teleportPlayer(player->getPosition());
		}
		else
		{
			// player moved less than render distance. Normally load chunk map
			bool updated = chunkMap->update(newChunkXZ, chunkWorkManager, glfwGetTime());

			/*
			//Whenever player moved, sort the load queue again
			if (updated)
			{
			chunkWorkManager->sortBuildMeshQueue(player->getPosition());
			}
			*/
		}
	}
}

void Voxel::Game::updatePlayerRaycast()
{
	auto result = chunkMap->raycastBlock(player->getEyePosition(), player->getDirection(), player->getRange());
	if (result.block != nullptr)
	{
		player->setLookingBlock(result.block, result.face);
		debugConsole->setPlayerLookingAtVisibility(true);
		debugConsole->updatePlayerLookingAt(result.block->getWorldCoordinate(), result.face);
	}
	else
	{
		debugConsole->setPlayerLookingAtVisibility(false);
		player->setLookingBlock(nullptr, Cube::Face::NONE);
	}
}

void Voxel::Game::updatePlayerCameraCollision()
{
	// auto start = Utility::Time::now();

	// update camera ray
	auto playerEyePos = player->getNextEyePosition();
	//auto playerEyePos = player->getEyePosition();
	//auto playerDir = player->getNextDirection();
	auto playerDir = player->getDirection();

	auto rayStart = playerEyePos;
	auto rayEnd = rayStart + (-playerDir * player->getCameraDistanceZ());

	auto camYOffset = Player::EyeHeight - player->getCameraY();

	rayStart.y -= camYOffset;
	rayEnd.y -= camYOffset;

	/*
	std::vector<glm::vec3> rayEnds;
	float pad = 0.75f;
	float padY = 0.5f;
	//rayEnds.push_back(glm::vec3(rayEnd.x + pad, rayEnd.y + pad, rayEnd.z));
	rayEnds.push_back(glm::vec3(rayEnd.x, rayEnd.y + padY, rayEnd.z));
	//rayEnds.push_back(glm::vec3(rayEnd.x - pad, rayEnd.y + pad, rayEnd.z));
	//rayEnds.push_back(glm::vec3(rayEnd.x + pad, rayEnd.y, rayEnd.z));
	rayEnds.push_back(rayEnd);
	//rayEnds.push_back(glm::vec3(rayEnd.x - pad, rayEnd.y, rayEnd.z));
	//rayEnds.push_back(glm::vec3(rayEnd.x + pad, rayEnd.y - pad, rayEnd.z));
	rayEnds.push_back(glm::vec3(rayEnd.x, rayEnd.y - padY, rayEnd.z));
	//rayEnds.push_back(glm::vec3(rayEnd.x - pad, rayEnd.y - pad, rayEnd.z));

	auto minCamDist = player->getCameraDistanceZ();

	for (auto& re : rayEnds)
	{
		float dist = chunkMap->raycastCamera(rayStart, re, player->getCameraDistanceZ());
		minCamDist = std::min(minCamDist, dist);
	}
	*/

	/*
	//float minCamDist = chunkMap->raycastCamera(rayStart, rayEnd, player->getCameraDistanceZ());

	std::vector<Block*> nearByBlock;
	//auto camPos = rayStart + (-playerDir * minCamDist);
	//chunkMap->queryNearByBlocks(camPos, nearByBlock);

	bool result = false;
	float curCamDist = 0;
	auto camPos = rayStart + (-playerDir * curCamDist);
	chunkMap->queryNearByBlocks(camPos, nearByBlock);
	float maxDist = player->getCameraDistanceZ();
	
	while (result == false && curCamDist < maxDist)
	{
		camPos = rayStart + (-playerDir * curCamDist);
		//result = physics->checkCollisionWithBlocks(Geometry::AABB(camPos, glm::vec3(0.5f)), nearByBlock);
		result = physics->checkSphereCollisionWithBlocks(Geometry::Sphere(0.5f, camPos), nearByBlock);

		if (result)
		{
			//minCamDist -= 0.05f;
		}

		if (result == false)
		{
			curCamDist += 0.25f;
			nearByBlock.clear();
			chunkMap->queryNearByBlocks(camPos, nearByBlock);
		}
	}

	if (result == false)
	{
		curCamDist = maxDist;
	}


	// raycastCamera takes 10 micro seconds for single ray.
	//std::cout << "dist = " << minCamDist << std::endl;

	if (curCamDist >= maxDist)
	{
		player->setCameraColliding(false);
	}
	else
	{
		player->setCameraColliding(true);
	}
	*/

	// get max cam dist
	float maxDist = player->getMaxCameraDistanceZ();
	// using max dist, get closest position where ray hits
	float minCamDist = chunkMap->raycastCamera(rayStart, rayEnd, maxDist);

	/*
	if (minCamDist != maxDist)
	{
		// Get near by blocks from the point where ray hit
		std::vector<Block*> nearByBlock;
		auto camPos = rayStart + (-playerDir * minCamDist);
		chunkMap->queryNearByBlocks(camPos, nearByBlock);

		bool result = true;
		// repeat until there is no collision or cam dist gets less than 0
		while (result && minCamDist > 0)
		{
			// recalculate pos
			camPos = rayStart + (-playerDir * minCamDist);
			// Check collision
			result = physics->checkSphereCollisionWithBlocks(Geometry::Sphere(0.5f, camPos), nearByBlock);

			// If collided, update
			if (result)
			{
				minCamDist -= 0.25f;
				nearByBlock.clear();
				camPos = rayStart + (-playerDir * minCamDist);
				chunkMap->queryNearByBlocks(camPos, nearByBlock);
			}
		}

		// Can't be negative
		if (minCamDist < 0)
		{
			minCamDist = 0;
		}
	}
	*/

	if (minCamDist >= maxDist)
	{
		player->setCameraColliding(false);
	}
	else
	{
		player->setCameraColliding(true);
	}

	player->setResolvedCameraDistanceZ(minCamDist);

	//auto end = Utility::Time::now();
	//std::cout << "t: " << Utility::Time::toMicroSecondString(start, end) << "\n";
}

void Voxel::Game::checkUnloadedChunks()
{
	bool result = true;
	while (result)
	{
		// Check if there is any chunk to unload
		glm::ivec2 chunkXZ;
		result = chunkWorkManager->getUnloadFinishedQueueFront(chunkXZ);
		if (result)
		{
			chunkMap->releaseChunk(chunkXZ);

			chunkWorkManager->popFinishedAndNotify();
		}
		else
		{
			break;
		}
	}
}

void Voxel::Game::refreshChunkMap()
{
	std::cout << "Refreshing all chunk meshes" << std::endl;

	player->setLookingBlock(nullptr, Cube::Face::NONE);
	player->setRotation(glm::vec3(0), false);

	chunkWorkManager->clear();
	chunkWorkManager->notify();

	loadingState = LoadingState::RELOADING;
	reloadState = ReloadState::CHUNK_MESH;
}

void Voxel::Game::rebuildChunkMap()
{
	// First, we need to clear chunk work manager. Then, wait till it clears all the work. Once it's done, it will wait for main thread to clear chunk map.
	std::cout << "Rebuilding chunk map\n";

	player->setLookingBlock(nullptr, Cube::Face::NONE);
	player->setRotation(glm::vec3(0), false);

	chunkWorkManager->clear();
	chunkWorkManager->notify();

	loadingState = LoadingState::RELOADING;
	reloadState = ReloadState::CHUNK_MAP;
}

void Voxel::Game::rebuildWorld()
{
	std::cout << "Rebuiling the world\n";

	player->setLookingBlock(nullptr, Cube::Face::NONE);
	player->setRotation(glm::vec3(0), false);

	chunkWorkManager->clear();
	chunkWorkManager->notify();

	loadingState = LoadingState::RELOADING;
	reloadState = ReloadState::WORLD;
}

void Game::render(const float delta)
{
	if (loadingState == LoadingState::INITIALIZING || loadingState == LoadingState::RELOADING)
	{
		renderLoadingScreen(delta);
	}
	else if (loadingState == LoadingState::FADING)
	{
		renderLoadingScreen(delta);
		renderGameWorld(delta);
	}
	else
	{
		renderGameWorld(delta);
	}
	
	glBindVertexArray(0);
	glUseProgram(0);
}

void Voxel::Game::renderGameWorld(const float delta)
{
	auto& pm = ProgramManager::getInstance();

	auto program = pm.getDefaultProgram(ProgramManager::PROGRAM_NAME::COLOR_SHADER);
	program->use(true);

	glm::mat4 worldMat = glm::mat4(1.0f);

	// ------------------------------ Render world ------------------------------------------
	// Get world view matrix based on mode
	if (cameraMode)
	{
		worldMat = Camera::mainCamera->getView();

		/*
		glm::mat4 rayMat = mat4(1.0f);
		rayMat = glm::translate(rayMat, player->getPosition());
		auto playerRotation = player->getRotation();
		rayMat = glm::rotate(rayMat, glm::radians(-playerRotation.y), glm::vec3(0, 1, 0));
		rayMat = glm::rotate(rayMat, glm::radians(playerRotation.x), glm::vec3(1, 0, 0));
		rayMat = glm::rotate(rayMat, glm::radians(-playerRotation.z), glm::vec3(0, 0, 1));

		defaultProgram->setUniformMat4("modelMat", rayMat);
		Camera::mainCamera->getFrustum()->render(rayMat, defaultProgram);
		*/
	}
	else
	{
		worldMat = player->getViewMatrix();
	}

	program->setUniformMat4("worldMat", worldMat);

	// Model mat is identity matrix
	program->setUniformMat4("modelMat", glm::mat4(1.0f));

	// Light
	program->setUniformVec4("ambientColor", glm::vec4(skybox->getAmbientColor(calendar->getHour(), calendar->getMinutes(), calendar->getSeconds())));
	program->setUniformVec3("pointLights[0].lightPosition", player->getEyePosition());

	// fog
	if (skybox->isFogEnabled())
	{
		program->setUniformBool("fogEnabled", true);
		program->setUniformVec4("fogColor", glm::vec4(skybox->getMidBlendColor(), 1.0f));
	}

	// Render chunk. Doesn't need molde matrix for each chunk. All vertices are translated.
	chunkMap->render();

	// render skybox
	skybox->updateMatrix(Camera::mainCamera->getProjection() * worldMat * player->getTranslationXZMat());
	skybox->render();

	// Render skybox
	//program->setUniformMat4("modelMat", player->getTranslationMat());

	// turn off the fog with sky box
	//program->setUniformBool("fogEnabled", false);
	// --------------------------------------------------------------------------------------

	// ------------------------------ Render Lines ------------------------------------------
	auto lineProgram = pm.getDefaultProgram(ProgramManager::PROGRAM_NAME::LINE_SHADER);
	lineProgram->use(true);

	lineProgram->setUniformMat4("worldMat", worldMat);

	// render chunk border. Need model matrix
	chunkMap->renderChunkBorder(lineProgram);

	// render player. Need model matrix
	player->renderDebugLines(lineProgram);

	if (player->isLookingAtBlock())	// Error stack: 3
	{
		chunkMap->renderBlockOutline(lineProgram, player->getLookingBlock()->getWorldPosition());
	}

	// Clear depth buffer and render above current buffer
	glClear(GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_ALWAYS);

	// Render voronoi diagram
	world->renderVoronoi(lineProgram);
	// --------------------------------------------------------------------------------------

	// --------------------------------- Render UI ------------------------------------------
	// Render UIs
	defaultCanvas->render();
	debugConsole->render();

	cursor->render();

	debugConsole->updateDrawCallsAndVerticesSize();
	// --------------------------------------------------------------------------------------
}

void Voxel::Game::renderLoadingScreen(const float delta)
{
	glClear(GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_ALWAYS);

	// Render UIs
	loadingCanvas->render();
}

void Voxel::Game::setFogEnabled(const bool enabled)
{
	skybox->setFogEnabled(enabled);

	auto program = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::COLOR_SHADER);
	program->use(true);

	// fog
	if (skybox->isFogEnabled())
	{
		program->setUniformBool("fogEnabled", true);
		program->setUniformFloat("fogDistance", skybox->getFogDistance());
	}
	else
	{
		program->setUniformBool("fogEnabled", false);
	}
	program->use(false);
}
