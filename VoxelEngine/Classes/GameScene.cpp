#include "GameScene.h"

// cpp
#include <algorithm>

// glm
#include <glm\gtx\transform.hpp>

// voxel
#include "World.h"
#include "Region.h"
#include "WorldMap.h"
#include "ChunkMap.h"
#include "ChunkMeshGenerator.h"
#include "ChunkSection.h"
#include "Chunk.h"
#include "ChunkUtil.h"
#include "ChunkWorkManager.h"
#include "Block.h"
#include "Biome.h"
#include "Terrain.h"

#include "Physics.h"

#include "Setting.h"

#include "InputHandler.h"
#include "Camera.h"
#include "Frustum.h"

#include "SpriteSheet.h"

#include "UI.h"
#include "UIActions.h"
#include "Cursor.h"
#include "GameMenu.h"
#include "FontManager.h"

#include "ProgramManager.h"
#include "Program.h"
#include "Cube.h"
#include "Utility.h"
#include "Random.h"
#include "SimplexNoise.h"
#include "Color.h"
#include "Player.h"
#include "Skybox.h"
#include "Calendar.h"

#include "Application.h"
#include "GLView.h"

#include "DebugConsole.h"

#include "FileSystem.h"

#include "EarClip.h"

using namespace Voxel;

GameScene::GameScene()
	: world(nullptr)
	, chunkMap(nullptr)
	, chunkMeshGenerator(nullptr)
	, input(&InputHandler::getInstance())
	, player(nullptr)
	, prevMouseCursorPos(0)
	, chunkWorkManager(nullptr)
	, staticCanvas(nullptr)
	, dynamicCanvas(nullptr)
	, timeLabel(nullptr)
	, loadingCanvas(nullptr)
	, skybox(nullptr)
	, calendar(nullptr)
	, settingPtr(nullptr)
	, worldMap(nullptr)
	, loadingState(LoadingState::INITIALIZING)
	, reloadState(ReloadState::NONE)
	, gameState(GameState::IDLE)
	, skipUpdate(false)
#if V_DEBUG
#if V_DEBUG_CONSOLE
	, debugConsole(nullptr)
#endif
#if V_DEBUG_CAMERA_MODE
	, cameraMode(false)
	, cameraControlMode(false)
#endif
#endif
{
	// After creation, set cursor to center
	input->setCursorToCenter();
	prevMouseCursorPos = input->getMousePosition();
	//Camera::mainCamera->initDebugFrustumLines();
}

GameScene::~GameScene()
{
	// Stop running mesh building before releasing
	chunkWorkManager->stop();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	chunkWorkManager->joinThread();

	// Release all instances
	release();

#if V_DEBUG && V_DEBUG_CONSOLE
	releaseDebugConsole();
#endif

	// Release fonts
	FontManager::getInstance().clear();
}

void Voxel::GameScene::init()
{
	// Init random first
	initRandoms();

	settingPtr = &Setting::getInstance();

	// init spritesheet
	initSpriteSheets();
	
	// Init world
	world = new World();

	// Init world map
	worldMap = new WorldMap();
	// Call init just once.
	worldMap->init();
	//worldMap->initDebugCenterLine();

	// Init chunks
	chunkMap = new ChunkMap();
	chunkMeshGenerator = new ChunkMeshGenerator();
	chunkWorkManager = new ChunkWorkManager();

	// init physics
	physics = new Physics();

	// player
	player = new Player();

	// program
	auto program = ProgramManager::getInstance().getProgram(ProgramManager::PROGRAM_NAME::BLOCK_SHADER);
	// use it
	program->use(true);

	// Lights
	program->setUniformVec4("ambientColor", glm::vec4(1.0f));
	program->setUniformFloat("pointLights[0].lightIntensity", 5.0f);
	program->setUniformVec4("pointLights[0].lightColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	// Skybox
	initSkyBox();

	// Initialize fog uniforms
	program->setUniformVec3("playerPosition", player->getPosition());
	program->setUniformFloat("fogDistance", skybox->getFogDistance());
	program->setUniformBool("fogEnabled", skybox->isFogEnabled());
	program->setUniformFloat("fogLength", skybox->getFogLength());

	// stop use
	program->use(false);

	// Calendar
	calendar = new Calendar();
	calendar->init();

	// UI & font
	initUI();

#if V_DEBUG && V_DEBUG_CONSOLE
	initDebugConsole();
#endif

	TextureManager::getInstance().print();

	//Application::getInstance().getGLView()->setWindowedFullScreen(1);
	//defaultCanvas->setSize(glm::vec2(1920, 1080));
	//debugConsole->updateResolution(1920, 1080);
}

void Voxel::GameScene::onEnter()
{}

void Voxel::GameScene::onEnterFinished()
{
	createNew("New world");
}

void Voxel::GameScene::onExit()
{}

void Voxel::GameScene::initSpriteSheets()
{
	auto& ssm = SpriteSheetManager::getInstance();

	ssm.addSpriteSheet("UISpriteSheet.json");
	ssm.addSpriteSheet("EnvironmentSpriteSheet.json");

#if V_DEBUG
	ssm.addSpriteSheet("DebugSpriteSheet.json");
#endif
}

void Voxel::GameScene::initRandoms()
{
	globalSeed = "ENGINE";
	Utility::Random::setSeed(globalSeed);
	Noise::Manager::init(globalSeed);

	auto& rand = Random::getInstance();
	rand.init(globalSeed);
}

void Voxel::GameScene::initUI()
{
	FontManager::getInstance().addFont("Pixel.ttf", 10);
	FontManager::getInstance().addFont("Pixel.ttf", 10, 2);

	initCursor();

	initDefaultCanvas();

	initLoadingScreen();

	initGameMenu();
}

void Voxel::GameScene::initLoadingScreen()
{
	auto resolution = Application::getInstance().getGLView()->getScreenSize();
	
	// loading canvas
	loadingCanvas = new Voxel::UI::Canvas(resolution, glm::vec2(0));

	// Add bg
	auto bg = UI::Image::createFromSpriteSheet("bg", "GlobalSpriteSheet", "1x1_black.png");
	bg->setPosition(glm::vec2(0.0f));
	bg->setScale(resolution);

	loadingCanvas->addChild(bg, 0);

	// Add loading label
	auto loadingLabel = UI::Image::createFromSpriteSheet("loadingLabel", "UISpriteSheet", "loading_label.png");
	loadingLabel->setPosition(glm::vec2(-20.0f, 20.0f));
	loadingLabel->setPivot(glm::vec2(0.5f, -0.5f));
	loadingLabel->setCoordinateOrigin(glm::vec2(0.5f, -0.5f));

	loadingCanvas->addChild(loadingLabel, 1);
}

void Voxel::GameScene::initDefaultCanvas()
{
	auto resolution = Application::getInstance().getGLView()->getScreenSize(); 
	
	staticCanvas = new Voxel::UI::Canvas(resolution, glm::vec2(0));

	// Add temporary cross hair
	auto crossHairImage = UI::Image::createFromSpriteSheet("crossHair", "UISpriteSheet", "cross_hair.png");
	crossHairImage->setScale(glm::vec2(2.0f));
	staticCanvas->addChild(crossHairImage, 0);

	/*
	auto temp = Voxel::UI::NinePatchImage::create("tmp", "UISpriteSheet", "game_menu_bg.png", 12, 12, 12, 12, glm::vec2(50.0f, 100.0f));
	temp->setPosition(100, 0);
	temp->setScale(3.0f);
	temp->setDraggable();
	staticCanvas->addChild(temp, 100);
	*/

	// Add time label
	timeLabel = UI::Text::createWithOutline("timeLabel", calendar->getTimeInStr(false), 2, glm::vec4(0, 0, 0, 1), UI::Text::ALIGN::LEFT);
	timeLabel->setPosition(-200.0f, -5.0f);
	timeLabel->setPivot(glm::vec2(-0.5f, 0.5f));
	timeLabel->setCoordinateOrigin(glm::vec2(0.5f, 0.5f));

	staticCanvas->addChild(timeLabel, 0);

}

void Voxel::GameScene::initCursor()
{
	// cursor
	cursor = UI::Cursor::create();
}

void Voxel::GameScene::initGameMenu()
{
	// game menu
	gameMenu = new GameMenu();
	gameMenu->init(this);
}

void Voxel::GameScene::initSkyBox()
{
	skybox = new Skybox();
	// Always set skybox with max render distance
	skybox->init(settingPtr->getRenderDistance());
}

void Voxel::GameScene::initWorldMap()
{
	worldMap->buildMesh(world);
}

void Voxel::GameScene::initMeshBuilderThread()
{
	// run first, create thread later
	unsigned int concurentThreadsSupported = std::thread::hardware_concurrency();
	std::cout << "Number of supporting threads: " << concurentThreadsSupported << std::endl;

	chunkWorkManager->run();
	chunkWorkManager->createThreads(chunkMap, chunkMeshGenerator, world, concurentThreadsSupported);
}

void Voxel::GameScene::release()
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

	if (staticCanvas) delete staticCanvas;
	if (dynamicCanvas) delete dynamicCanvas;
	if (gameMenu) delete gameMenu;

	if (worldMap) delete worldMap;

	if (world) delete world;
}

void Voxel::GameScene::createNew(const std::string & worldName)
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

	// Init worldMap after creating the world
	initWorldMap();

	// Then based init chunks
	createChunkMap();

#if V_DEBUG && V_DEBUG_CONSOLE
	Region* curRegion = world->getCurrentRegion();
	Biome biomeType = curRegion->getBiomeType();
	debugConsole->updateRegion(curRegion->getID());
	debugConsole->updateBiome(Biome::biomeTypeToString(biomeType), Terrain::terrainTypeToString(curRegion->getTerrainType()), biomeType.getTemperature(), biomeType.getMoisture());
#endif
	
	auto end = Utility::Time::now();
	std::cout << "New world creation took " << Utility::Time::toMilliSecondString(start, end) << std::endl;
}

void GameScene::createPlayer()
{
	// For now, set 0 to 0. Todo: Make topY() function that finds hieghts y that player can stand.
	player->init();

	// Todo: set this to false. For now, set ture for debug
	player->setFly(true);
	player->setViewMode(1);
	// Update matrix
	player->updateViewMatrix();
	// Based on player's matrix, update frustum
	Camera::mainCamera->getFrustum()->updateFrustumPlanes(player->getViewMatrix() * player->getWorldMatrix());
	Camera::mainCamera->setSpeed(100.0f);
}

void GameScene::createChunkMap()
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
	auto program = ProgramManager::getInstance().getProgram(ProgramManager::PROGRAM_NAME::LINE_SHADER);

	// block outline. this is not debug. Required for building.
	chunkMap->initBlockOutline(program);

#if V_DEBUG && V_DEBUG_CHUNK_BORDER_LINE
	chunkMap->initChunkBorderDebug(program);
#endif

	// End measure
	auto end = Utility::Time::now();
	std::cout << "[ChunkMap] ElapsedTime: " << Utility::Time::toMilliSecondString(start, end) << std::endl;
}

void Voxel::GameScene::createWorld()
{
	world->setTemperature(0.5f, 1.5f);
	world->setMoisture(0.5f, 1.5f);
	world->init(10, 10, 0, globalSeed);

	auto startingRegionSitePos = world->getCurrentRegion()->getSitePosition();
	//startingRegionSitePos = world->getRegion(31)->getSitePosition();

	startingRegionSitePos = glm::vec2(glm::ivec2(startingRegionSitePos)) + 0.5f;

	player->setPosition(glm::vec3(startingRegionSitePos.x, -1.0f, startingRegionSitePos.y), false);

	auto pp = player->getPosition();
	std::cout << "Placing player to starting region #: " << world->getCurrentRegion()->getID() << ", at (" << pp.x << ", " << pp.z << ")\n";
}

void Voxel::GameScene::teleportPlayer(const glm::vec3 & position)
{
	std::cout << "Teleporint to " << Utility::Log::vec3ToStr(position) << "\n";

	if (gameState == GameState::VIEWING_WORLD_MAP)
	{
		closeWorldMap();
	}

	// move player
	player->setPosition(position, false);
	player->setLookingBlock(nullptr, Cube::Face::NONE);
	player->setRotation(glm::vec3(0), false);

	// clear chunk work manager
	chunkWorkManager->clear();
	chunkWorkManager->notify();

	loadingState = LoadingState::RELOADING;
	reloadState = ReloadState::CHUNK_MAP;

	skipUpdate = true;
}

void GameScene::update(const float delta)
{
	if (loadingState == LoadingState::INITIALIZING)
	{
		loadingCanvas->update(delta);

		// Game is initializing
		if (chunkWorkManager->isFirstInitDone())
		{
			// First initialization is done.
			loadingState = LoadingState::FINISHED;

			// get top y at player position
			replacePlayerToTopY();
		}
	}
	else if (loadingState == LoadingState::RELOADING)
	{
		loadingCanvas->update(delta);

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
			chunkWorkManager->notify();

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
				// Chunk generation is done. Replace player if player teleported
				replacePlayerToTopY();

				// Reset states
				loadingState = LoadingState::FINISHED;
				reloadState = ReloadState::NONE;
			}
		}
	}
	else
	{
		// update key board input
		updateKeyboardInput(delta);
		// Update mouse move input
		updateMouseMoveInput(delta);
		// Update mouse click input
		updateMouseClickInput();
		// Update mouse scroll input
		updateMouseScrollInput(delta);
		// Update controller input
		updateControllerInput(delta);

		// Check if we need to skip update. Inputs might trigger this.
		if (skipUpdate)
		{
			// Reset flag and return.
			skipUpdate = false;
			return;
		}
		// Else, not skipping update. 

		// Update world map
		worldMap->update(delta);

		// Check if there is a chunk to unload on main thread
		checkUnloadedChunks();

		// Update physics
		updatePhysics(delta);

		// Resolve collision in game
		//updateCollisionResolution();

		bool playerMoved = player->didMoveThisFrame();
		bool playerRotated = player->didRotateThisFrame();

		// After resolving collision and updating physics, update player's movement
		player->updateMovement(delta);

		// First check visible chunk
		Camera::mainCamera->getFrustum()->updateFrustumPlanes(player->getViewMatrix() * player->getWorldMatrix());

		// After updating frustum, run frustum culling to find visible chunk
		int totalVisible = chunkMap->findVisibleChunk(settingPtr->getRenderDistance());

		if (playerMoved || playerRotated)
		{
			auto playerPos = player->getPosition();
			// Also update raycast
			updatePlayerRaycast();

#if V_DEBUG && V_DEBUG_CONSOLE
			debugConsole->updatePlayerPosition(playerPos);
			debugConsole->updatePlayerRotation(player->getRotation());
#endif
		}

		updateChunks();

		if (playerMoved)
		{
			auto playerPos = player->getPosition();

#if V_DEBUG && V_DEBUG_CONSOLE
			bool regionChanged = world->updatePlayerPos(playerPos);
			if (regionChanged)
			{
				Region* curRegion = world->getCurrentRegion();
				Biome biomeType = curRegion->getBiomeType();
				debugConsole->updateRegion(curRegion->getID());
				debugConsole->updateBiome(Biome::biomeTypeToString(biomeType), Terrain::terrainTypeToString(curRegion->getTerrainType()), biomeType.getTemperature(), biomeType.getMoisture());
			}
#else
			world->updatePlayerPos(playerPos);
#endif

			auto program = ProgramManager::getInstance().getProgram(ProgramManager::PROGRAM_NAME::BLOCK_SHADER);
			program->use(true);
			program->setUniformVec3("playerPosition", playerPos);
		}

		player->update(delta);

		if (player->isOnTPViewMode())
		{
			updatePlayerCameraCollision();
		}
		
		player->updateCameraDistanceZ(delta);

		calendar->update(delta);

		skybox->update(delta);
		skybox->updateColor(calendar->getHour(), calendar->getMinutes(), calendar->getSeconds());

		timeLabel->setText(calendar->getTimeInStr(false));

#if V_DEBUG && V_DEBUG_CONSOLE
		debugConsole->updateChunkNumbers(totalVisible, chunkMap->getActiveChunksCount(), chunkMap->getSize(), chunkWorkManager->getDebugOutput());
		debugConsole->update(delta);
#endif
	}
}

glm::vec3 Voxel::GameScene::getMovedDistByKeyInput(const float angleMod, const glm::vec3 axis, float distance)
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

void Voxel::GameScene::updateKeyboardInput(const float delta)
{
#if V_DEBUG && V_DEBUG_CONSOLE
	// Handle Escape key. 
	if (input->getKeyDown(Voxel::InputHandler::KEY_INPUT::GLOBAL_ESCAPE, true))
	{
		// check if debug console is opened
		if (debugConsole->isConsoleOpened())
		{
			// opened. close it
			debugConsole->closeConsole();
			toggleCursorMode(false);

			// end key update
			return;
		}
	}

	// F3
	if (input->getKeyDown(GLFW_KEY_F3, true))
	{
		// togle debug console visibility
		debugConsole->toggleVisibility();
		return;
	}

	// ~
	if (input->getKeyDown(GLFW_KEY_GRAVE_ACCENT, true))
	{
		// check if console is opened
		if (debugConsole->isConsoleOpened())
		{
			// close it
			debugConsole->closeConsole();
			toggleCursorMode(false);
		}
		else
		{
			// open it
			debugConsole->openConsole();
			toggleCursorMode(true);
		}

		// end key update
		return;
	}

	if (debugConsole->isConsoleOpened())
	{
		// While console is opened, block all input in world
		debugConsole->updateConsoleInputText(input->getBuffer());
		return;
	}
#endif
	// If reach here, console is not enabled or nothing to do with debug console

	// Handle escape key first
	if (input->getKeyDown(Voxel::InputHandler::KEY_INPUT::GLOBAL_ESCAPE, true))
	{
		if (gameState == GameState::IDLE)
		{
			// idle
		}
		else if (gameState == GameState::VIEWING_WORLD_MAP)
		{
			// was viewing world map.
			closeWorldMap();

			// end key update
			return;
		}
		else if (gameState == GameState::VIEWING_GAME_MENU)
		{
			if (gameMenu->isOpened())
			{
				gameMenu->close();
				gameState = GameState::IDLE;
				toggleCursorMode(false);

				// end key update
				return;
			}
		}
	}

	// Handle game menu next
	if (input->getKeyDown(Voxel::InputHandler::KEY_INPUT::TOGGLE_GAME_MENU, true))
	{
		if (gameState == GameState::IDLE)
		{
			if (gameMenu->isClosed())
			{
				gameMenu->open();
				gameState = GameState::VIEWING_GAME_MENU;
				toggleCursorMode(true);

				// end key update
				return;
			}
		}
	}

	// handle input.
	
	if(input->getKeyDown(GLFW_KEY_T, true))
	{
	}
		
	if (input->getKeyDown(GLFW_KEY_P, true))
	{
		auto playerPos = player->getPosition();
		auto playerRot = player->getRotation();
		std::cout << "Player is at (" << playerPos.x << ", " << playerPos.y << ", " << playerPos.z << "), rotated (" << playerRot.x << ", " << playerRot.y << ", " << playerRot.z << ")\n";
	}
	else if (input->getKeyDown(GLFW_KEY_O, true))
	{
		if (gameState == GameState::VIEWING_WORLD_MAP)
		{
			worldMap->print();
		}
		else
		{
			Camera::mainCamera->print();
		}
	}

	if (input->getKeyDown(GLFW_KEY_N, true) && input->getMods() == 0)
	{
		Application::getInstance().getGLView()->setWindowedFullScreen(1);
		staticCanvas->setSize(glm::vec2(1920, 1080));
#if V_DEBUG && V_DEBUG_CONSOLE
		debugConsole->updateResolution(1920, 1080);
#endif
		cursor->updateBoundary();
	}
	else if (input->getKeyDown(GLFW_KEY_N, true) && input->getMods() == GLFW_MOD_CONTROL)
	{
		Application::getInstance().getGLView()->setWindowed(1280, 720);
		Application::getInstance().getGLView()->setWindowPosition(100, 100);
		staticCanvas->setSize(glm::vec2(1280, 720));
#if V_DEBUG && V_DEBUG_CONSOLE
		debugConsole->updateResolution(1280, 720);
#endif
		cursor->updateBoundary();
	}
	else if (input->getKeyDown(GLFW_KEY_N, true) && input->getMods() == (GLFW_MOD_CONTROL | GLFW_MOD_SHIFT))
	{
		Application::getInstance().getGLView()->setWindowedFullScreen(0);
		staticCanvas->setSize(glm::vec2(1920, 1080));
#if V_DEBUG && V_DEBUG_CONSOLE
		debugConsole->updateResolution(1920, 1080);
#endif
		cursor->updateBoundary();
	}

	if (input->getKeyDown(GLFW_KEY_V, true) && !input->getKeyDown(GLFW_KEY_LEFT_CONTROL))
	{
		Application::getInstance().getGLView()->setVsync(true);
#if V_DEBUG && V_DEBUG_CONSOLE
		debugConsole->updateVsync(true);
#endif
	}
	else if (input->getKeyDown(GLFW_KEY_V, true) && input->getKeyDown(GLFW_KEY_LEFT_CONTROL))
	{
		Application::getInstance().getGLView()->setVsync(false);
#if V_DEBUG && V_DEBUG_CONSOLE
		debugConsole->updateVsync(false);
#endif
	}

	if (input->getKeyDown(Voxel::InputHandler::KEY_INPUT::TOGGLE_WORLD_MAP, true))
	{
		if (gameState == GameState::IDLE)
		{
			openWorldMap();
		}
		else if (gameState == GameState::VIEWING_WORLD_MAP)
		{
			closeWorldMap();
		}
	}

#if V_DEBUG && V_DEBUG_CAMERA_MODE
	if (input->getKeyDown(GLFW_KEY_C, true))
	{
		cameraMode = !cameraMode;
		std::cout << "[World] Camera mode " << std::string(cameraMode ? "enabled" : "disabled") << std::endl;

		if (!cameraMode)
		{
			worldMap->updateViewMatrix();
		}

		return;
	}
	
	if (input->getKeyDown(GLFW_KEY_X, true))
	{
		cameraControlMode = !cameraControlMode;
		std::cout << "[World] Camera control mode " << std::string(cameraControlMode ? "enabled" : "disabled") << std::endl;

		return;
	}
	
	if (input->getKeyDown(GLFW_KEY_Z, true))
	{
		Camera::mainCamera->setPosition(player->getPosition());

		return;
	}

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

		return;
	}
#endif
	if (!cursor->isVisible())
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

	if (input->getKeyDown(GLFW_KEY_3, true))
	{
		player->setRotation(glm::vec3(0, 0, 0), false);
	}
	if (input->getKeyDown(GLFW_KEY_4, true))
	{
		player->setRotation(glm::vec3(0, 180, 0), false);
	}
}

void Voxel::GameScene::updateMouseMoveInput(const float delta)
{
	// Get current mouse pos
	auto curMousePos = input->getMousePosition();

	// Calculate how far did cursor moved
	float dx = curMousePos.x - prevMouseCursorPos.x;
	float dy = curMousePos.y - prevMouseCursorPos.y;
	// Store cursor position
	prevMouseCursorPos.x = curMousePos.x;
	prevMouseCursorPos.y = curMousePos.y;

	// Check if cursor is visible
	if (cursor->isVisible())
	{
		// Update cursor position only if it's visible
		cursor->addPosition(glm::vec2(dx, -dy));
	}

	// Update UI
	if (gameMenu->isOpened())
	{
		gameMenu->updateMouseMove(cursor->getPosition());
	}

#if V_DEBUG && V_DEBUG_CONSOLE
	// update debug console
	debugConsole->updateMouseMove(cursor->getPosition(), glm::vec2(dx, -dy));

	if (debugConsole->isConsoleOpened())
	{
		// Stop input while opening console
		return;
	}
#endif

	// Todo: Find better way? focing elapsed time in mouse pos update to 1/60 if exceeds 1/60
	// This is because, mouse position updates more quicker in higher frame, which means smaller changes from
	// previous position. However because of frequent change, cumulitive change of angle seemed to be same with 
	// 60 fps. So if if delta is bigger than 1/60, force 1/60
	float newDelta = delta;

	if (delta < 0.0166666667f)
	{
		newDelta = 0.01666666666667f;
	}

#if V_DEBUG && V_DEBUG_CAMERA_MODE
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

		return;
	}
#endif

	if (gameState == GameState::IDLE)
	{
		if (dx != 0.0)
		{
			player->addRotationY(newDelta * dx);
		}

		if (dy != 0.0)
		{
			player->addRotationX(newDelta * -dy);
		}
	}
	else if (gameState == GameState::VIEWING_WORLD_MAP)
	{
		glm::vec2 delta(0.0f);

		if (dx != 0)
		{
			delta.x = newDelta * static_cast<float>(dx);
		}

		if (dy != 0)
		{
			delta.y = newDelta * static_cast<float>(dy);
		}

		worldMap->updateMouseMove(delta, cursor->getPosition());
	}
}

void Voxel::GameScene::updateMouseClickInput()
{
	int button = -1;
	bool clicked = false;

	if (input->getMouseDown(GLFW_MOUSE_BUTTON_1, true))
	{
		button = 0;
		clicked = true;
	}
	else if (input->getMouseDown(GLFW_MOUSE_BUTTON_2, true))
	{
		button = 1;
		clicked = true;
	}
	else if (input->getMouseDown(GLFW_MOUSE_BUTTON_3, true))
	{
		button = 2;
		clicked = true;
	}
	else if (input->getMouseUp(GLFW_MOUSE_BUTTON_1, true))
	{
		button = 0;
		clicked = false;
	}
	else if (input->getMouseUp(GLFW_MOUSE_BUTTON_2, true))
	{
		button = 1;
		clicked = false;
	}
	else if (input->getMouseUp(GLFW_MOUSE_BUTTON_3, true))
	{
		button = 2;
		clicked = false;
	}

#if V_DEBUG && V_DEBUG_CONSOLE
	if (button != -1)
	{
		if (clicked)
		{
			debugConsole->updateMousePress(cursor->getPosition(), button);
		}
		else
		{
			debugConsole->updateMouseRelease(cursor->getPosition(), button);
		}
	}

	if (debugConsole->isConsoleOpened())
	{
		// Stop input while opening console
		return;
	}
#endif

	if (cursor->isVisible())
	{
		// cursor is vibile
		if (gameState == GameState::VIEWING_WORLD_MAP)
		{
			worldMap->updateMouseClick(button, clicked, cursor->getPosition());
		}
		else if (gameState == GameState::VIEWING_GAME_MENU)
		{
			// Update UI
			if (gameMenu->isOpened())
			{
				if (clicked)
				{
					gameMenu->updateMousePress(cursor->getPosition(), button);
				}
				else
				{
					gameMenu->updateMouseReleased(cursor->getPosition(), button);
				}
			}
		}
	}
	else
	{
		// cursor is hidden.
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
}

void Voxel::GameScene::updateMouseScrollInput(const float delta)
{
	auto mouseScroll = input->getMouseScrollValue();
	if (gameState == GameState::IDLE)
	{
		if (mouseScroll == 1)
		{
			player->zoomInCamera();
		}
		else if (mouseScroll == -1)
		{
			player->zoomOutCamera();
		}
		// Else, mouse scroll didn't move
	}
	else if (gameState == GameState::VIEWING_WORLD_MAP)
	{
		if (mouseScroll == 1)
		{
			worldMap->zoomIn();
		}
		else if (mouseScroll == -1)
		{
			worldMap->zoomOut();
		}
	}
}

void Voxel::GameScene::updateControllerInput(const float delta)
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
					// Todo: Controller manager doesn't have feature to check if button was pressed only current frame.
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

void Voxel::GameScene::updatePhysics(const float delta)
{
	// Don't update if player is flying
	if (player->isFlying()) return;

	// Update player's jump force
	bool jumping = physics->updatePlayerJumpForce(player, delta);

	/*
	// debug
	auto state = player->getJumpState();

	switch (state)
	{
	case Voxel::Player::JumpState::IDLE:
		std::cout << "JS: IDLE\n";
		break;
	case Voxel::Player::JumpState::JUMPING:
		std::cout << "JS: JUMPING\n";
		break;
	case Voxel::Player::JumpState::FALLING:
		std::cout << "JS: FALLING\n";
		break;
	default:
		break;
	}
	*/

	if (!jumping)
	{
		// Only apply when player is not jumping
		physics->applyGravity(player, delta);
	}

	auto start = Utility::Time::now();

	std::vector<Block*> collidableBlocks;

	bool autoJumped = false;

	// Check if player can auto jump
	if (player->isOnGround())
	{
		// Can auto jump only if player is on ground

		// Query near by block in XZ axis
		chunkMap->queryNearByCollidableBlocksInXZ(player->getNextPosition(), collidableBlocks);

		// resolve auto jump
		autoJumped = physics->resolveAutoJump(player, collidableBlocks);

		if (autoJumped)
		{
			// If auto jump, it means there isn't anything that collides player. 
			return;
		}
		else
		{
			// Didn't auto jumped. Don't need to query blocks again
			
			// Reoslve XZ
			physics->resolvePlayerAndBlockCollisionInXZAxis(player, collidableBlocks);
		}
	}
	else
	{
		// Either jumping or falling. Both need to resolve collision between in axis XZ first

		// Query near by block in XZ axis
		chunkMap->queryNearByCollidableBlocksInXZ(player->getNextPosition(), collidableBlocks);

		// Reoslve XZ
		physics->resolvePlayerAndBlockCollisionInXZAxis(player, collidableBlocks);

		// clear list
		collidableBlocks.clear();

		if (player->isJumping())
		{
			// query again in negative Y 
			chunkMap->queryTopCollidableBlocksInY(player->getNextPosition(), collidableBlocks);

			// At this moment, player won't have any blocks that are colliding in XZ direction. Check bottom y. If so, player hit the ground.
			physics->resolvePlayerTopCollision(player, collidableBlocks);
		}
		else if (player->isFalling())
		{
			// query again in negative Y 
			chunkMap->queryBottomCollidableBlocksInY(player->getNextPosition(), collidableBlocks);

			// At this moment, player won't have any blocks that are colliding in XZ direction. Check bottom y. If so, player hit the ground.
			physics->resolvePlayerBottomCollision(player, collidableBlocks);
		}
	}

	// Keep track if player is on ground or not. If player is jumping, skip

	if (!jumping)
	{
		// clear list
		collidableBlocks.clear();

		// query again in negative Y 
		auto pos = player->getPosition();
		//pos.y = player->getNextPosition().y;
		chunkMap->queryBottomCollidableBlocksInY(pos, collidableBlocks);

		// At this moment, player won't have any blocks that are colliding in XZ direction. Check bottom y. If so, player hit the ground.
		physics->checkIfPlayerIsFalling(player, collidableBlocks);
	}

	auto end = Utility::Time::now();
	//std::cout << "Player vs blocks collision resolution took: " << Utility::Time::toMicroSecondString(start, end) << std::endl;
}

void Voxel::GameScene::updateChunks()
{
	// Update chunk.
	// Based on player position, check if player moved to new chunk
	// If so, we need to load new chunks. 
	// Else, player reamains on same chunk as now.

	auto playerPos = player->getPosition();
	auto prevChunkXZ = chunkMap->getCurrentChunkXZ();
	bool updated = chunkMap->updateCurrentChunkPos(playerPos);

	if (updated)
	{
		// moved to new chunk
		auto curChunkXz = chunkMap->getCurrentChunkXZ();
		glm::ivec2 absDist = glm::abs(curChunkXz - prevChunkXZ);

		auto rd = settingPtr->getRenderDistance();

		if (absDist.x >= rd || absDist.y >= rd)
		{
			// player moved more than render distance at once. consider this as teleport.
			teleportPlayer(player->getPosition());
		}
		else
		{
			// player moved less than render distance. Normally load chunk map
			chunkMap->update(curChunkXz - prevChunkXZ, chunkWorkManager);
		}
	}

	// Debug
#if V_DEBUG && V_DEBUG_CHUNK_BORDER_LINE
	chunkMap->updateChunkBorderDebugLineModelMat();
#endif
}

void Voxel::GameScene::updatePlayerRaycast()
{
	auto result = chunkMap->raycastBlock(player->getEyePosition(), player->getDirection(), player->getRange());
	player->setLookingBlock(result.block, result.face);

#if V_DEBUG && V_DEBUG_CONSOLE
	if (result.block != nullptr)
	{
		debugConsole->setPlayerLookingAtVisibility(true);
		debugConsole->updatePlayerLookingAt(result.block->getWorldCoordinate(), result.face);
	}
	else
	{
		debugConsole->setPlayerLookingAtVisibility(false);
		player->setLookingBlock(nullptr, Cube::Face::NONE);
	}
#endif
}

void Voxel::GameScene::updatePlayerCameraCollision()
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

	glm::vec3 camPos = rayStart + (-playerDir * minCamDist);

	while (minCamDist >= 0)
	{
		std::vector<Block*> nearByBlock;
		chunkMap->queryNearByBlocks(camPos, nearByBlock);
		bool result = physics->checkSphereCollisionWithBlocks(Shape::Sphere(0.5f, camPos), nearByBlock);

		if (result == false)
		{
			break;
		}

		minCamDist -= 0.01f;
		camPos = rayStart + (-playerDir * minCamDist);
	}

	if (minCamDist < 0)
	{
		minCamDist = 0;
	}

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

void Voxel::GameScene::openWorldMap()
{
	std::cout << "[Game] Opening world map\n";
	gameState = GameState::VIEWING_WORLD_MAP;
	toggleCursorMode(true);
}

void Voxel::GameScene::closeWorldMap()
{
	std::cout << "[Game] Closing world map\n";
	gameState = GameState::IDLE;
	toggleCursorMode(false);
	worldMap->resetPosAndRot();
	worldMap->updateViewMatrix();
	worldMap->updateModelMatrix();
}

void Voxel::GameScene::replacePlayerToTopY()
{
	auto playerPosition = player->getPosition();
	if (playerPosition.y == -1)
	{
		playerPosition.y = static_cast<float>(chunkMap->getTopYAt(glm::vec2(playerPosition.x, playerPosition.z))) + 1.1f;
	}

	std::cout << "Replacing player to top y. Pos: (" << playerPosition.x << ", " << playerPosition.y << ", " << playerPosition.z << ")\n";

	player->setPosition(playerPosition, false);
}

void Voxel::GameScene::checkUnloadedChunks()
{
	// Iterate until queue is empty
	while (true)
	{
		glm::ivec2 chunkXZ(0);

		bool hasChunk = chunkWorkManager->getAndPopFirstUnloadFinishedQueue(chunkXZ);
		if (hasChunk)
		{
			// Succesfully got the chunk coordinate. Release chunk.
			chunkMap->releaseChunk(chunkXZ);

			// Check if releasing is finished. If so, notify
			bool empty = chunkWorkManager->isUnloadFinishedQueueEmpty();

			if (empty)
			{
				chunkWorkManager->notify();
			}
		}
		else
		{
			break;
		}
	}
}

void Voxel::GameScene::toggleCursorMode(const bool mode)
{
	if (mode)
	{
		cursor->setVisibility(true);
	}
	else
	{
		cursor->setVisibility(false);
	}
}

void Voxel::GameScene::onReturnToGameClicked()
{
	gameMenu->close();
	gameState = GameState::IDLE;
	toggleCursorMode(false);
}

void Voxel::GameScene::refreshChunkMap()
{
	std::cout << "Refreshing all chunk meshes" << std::endl;

	player->setLookingBlock(nullptr, Cube::Face::NONE);
	player->setRotation(glm::vec3(0), false);

	chunkWorkManager->clear();
	chunkWorkManager->notify();

	loadingState = LoadingState::RELOADING;
	reloadState = ReloadState::CHUNK_MESH;
}

void Voxel::GameScene::rebuildChunkMap()
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

void Voxel::GameScene::rebuildWorld()
{
	std::cout << "Rebuiling the world\n";

	player->setLookingBlock(nullptr, Cube::Face::NONE);
	player->setRotation(glm::vec3(0), false);

	chunkWorkManager->clear();
	chunkWorkManager->notify();

	loadingState = LoadingState::RELOADING;
	reloadState = ReloadState::WORLD;
}

void GameScene::render()
{
	if (loadingState == LoadingState::INITIALIZING || loadingState == LoadingState::RELOADING)
	{
		renderLoadingScreen();
	}
	else if (loadingState == LoadingState::FADING)
	{
		renderLoadingScreen();
		renderGame();
	}
	else
	{
		renderGame();
	}
}

void Voxel::GameScene::renderGame()
{
	if (gameState == GameState::IDLE)
	{
		renderWorld();
		renderUI();
	}
	else if (gameState == GameState::VIEWING_WORLD_MAP)
	{
		renderWorldMap();
	}
	else if (gameState == GameState::VIEWING_GAME_MENU)
	{
		renderWorld();
		renderUI();
	}

	// Clear depth buffer and render above current buffer
	glClear(GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_ALWAYS);

#if V_DEBUG && V_DEBUG_CONSOLE
	renderDebugConsole();
#endif

	// Render cursor. Cursor has highest Z Order among other UIs. Must be always visible.
	cursor->render();
}

void Voxel::GameScene::renderWorld()
{
	// Render world.

	// Get program manager
	auto& pm = ProgramManager::getInstance();

	// Get block shader porgram
	auto program = pm.getProgram(ProgramManager::PROGRAM_NAME::BLOCK_SHADER);
	// use it
	program->use(true);

	// Projection matrix is already set.

	// Get view and world matrix
	glm::mat4 viewMat = glm::mat4(1.0f);
	glm::mat4 worldMat = glm::mat4(1.0f);

	// Center position for chunk map
	glm::vec3 centerPos = glm::vec3(0.0f);

	// ------------------------------ Render world ------------------------------------------

	// Get world view matrix based on mode
#if V_DEBUG && V_DEBUG_CAMERA_MODE
	if (cameraMode)
	{
		// Camera's view matrix based on rotation
		viewMat = Camera::mainCamera->getViewMat();
		// Camera's world matrix based on camera's position
		worldMat = Camera::mainCamera->getWorldMat();
		// Camera's position
		centerPos = Camera::mainCamera->getPosition();
		// While camera mode, apply world amtrix
	}
	else
	{
		// Player's view matrix based on rotation
		viewMat = player->getViewMatrix();
		// Player's world matrix is idendity matrix
		worldMat = player->getWorldMatrix();
		// Player's position
		centerPos = player->getPosition();
	}
#else
	// Player's view matrix based on rotation
	viewMat = player->getViewMatrix();
	// Player's world matrix is idendity matrix
	worldMat = player->getWorldMatrix();
	// Player's position
	centerPos = player->getPosition();
#endif

	// set view matrix.
	program->setUniformMat4("viewMat", viewMat);
	
	// Update light
	float ambientValue = skybox->getAmbientColor(calendar->getHour(), calendar->getMinutes(), calendar->getSeconds());
	program->setUniformVec4("ambientColor", glm::vec4(ambientValue, ambientValue, ambientValue, 1.0f));
	program->setUniformVec3("pointLights[0].lightPosition", player->getEyePosition());

	// Check if fog is enabled. Todo: Make fog always on.
	if (skybox->isFogEnabled())
	{
		// Enable fog
		program->setUniformBool("fogEnabled", true);
		// Set current fog color
		program->setUniformVec4("fogColor", glm::vec4(skybox->getMidBlendColor(), 1.0f));
	}

	// Render chunk map. Uses block shader. Updates each chunk's model matrix based on distance between player and chunk's world position.
	chunkMap->render(centerPos);

	// Update skybox's matrix. 
	skybox->updateMatrix(Camera::mainCamera->getProjection() * viewMat * worldMat * player->getSkyboxMat(true));
	// Render skybox. 
	skybox->render();
	// --------------------------------------------------------------------------------------

	// ------------------------------ Render Lines ------------------------------------------
	auto lineProgram = pm.getProgram(ProgramManager::PROGRAM_NAME::LINE_SHADER);
	lineProgram->use(true);	

	// Use viewmat first
	lineProgram->setUniformMat4("viewMat", viewMat);
	
#if V_DEBUG
	// Debug mode
#if V_DEBUG_CHUNK_BORDER_LINE
	// render chunk border.
	chunkMap->renderChunkBorder(lineProgram);

#if V_DEBUG_CAMERA_MODE
	// Camera mode enabled
	if (cameraMode)
	{
		// If camera mode, render chunk border for camea again.
		chunkMap->renderCameraChunkBorder(lineProgram, centerPos);
	}
#endif

#endif
#if V_DEBUG_CAMERA_MODE
	if (cameraMode)
	{
		// Change view mat for other lines
		lineProgram->setUniformMat4("viewMat", viewMat * worldMat * glm::inverse(player->getWorldMatrix()));

		Camera::mainCamera->getFrustum()->render(player->getFrustumViewMatrix(), lineProgram);
	}
#endif
#endif

	// render player. Need model matrix
	player->renderDebugLines(lineProgram);

	if (player->isLookingAtBlock())
	{
		chunkMap->renderBlockOutline(lineProgram, player->getLookingBlock()->getWorldPosition() - player->getPosition());
	}

#if V_DEBUG && V_DEBUG_VORONOI_LINE
	//glClear(GL_DEPTH_BUFFER_BIT);
	//glDepthFunc(GL_ALWAYS);
	// Render voronoi diagram
	//world->renderVoronoi(lineProgram);
#endif

	// --------------------------------------------------------------------------------------
}

void Voxel::GameScene::renderWorldMap()
{
	// ---------------------------- Render world Map ----------------------------------------
	glm::mat4 viewMat = worldMap->getViewMatrix();

#if V_DEBUG && V_DEBUG_CAMERA_MODE
	if (cameraMode)
	{
		viewMat = Camera::mainCamera->getViewMat();
	}
#endif

	worldMap->updateWithCamViewMatrix(viewMat);

	worldMap->render();
	// --------------------------------------------------------------------------------------
	
	// ------------------------------ Render Lines ------------------------------------------
	auto lineProgram = ProgramManager::getInstance().getProgram(ProgramManager::PROGRAM_NAME::LINE_SHADER);
	lineProgram->use(true);

	lineProgram->setUniformMat4("viewMat", viewMat);
	lineProgram->setUniformMat4("modelMat", glm::mat4(1.0f));

	worldMap->renderCenterLine();
	worldMap->renderRay();
	// --------------------------------------------------------------------------------------
}

void Voxel::GameScene::renderLoadingScreen()
{
	glClear(GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_ALWAYS);

	// Render UIs
	loadingCanvas->render();
}

void Voxel::GameScene::renderUI()
{
	// Clear depth buffer and render above current buffer
	glClear(GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_ALWAYS);

	// --------------------------------- Render UI ------------------------------------------
	// Render UIs
	staticCanvas->render();

	if (gameMenu->isOpened())
	{
		gameMenu->render();
	}
	// --------------------------------------------------------------------------------------
}

void Voxel::GameScene::setFogEnabled(const bool enabled)
{
	skybox->setFogEnabled(enabled);

	auto program = ProgramManager::getInstance().getProgram(ProgramManager::PROGRAM_NAME::BLOCK_SHADER);
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

#if V_DEBUG && V_DEBUG_CONSOLE
void Voxel::GameScene::initDebugConsole()
{
	auto resolution = Application::getInstance().getGLView()->getScreenSize();

	// Debug. This creates all the debug UI components
	debugConsole = new DebugConsole();
	debugConsole->toggleVisibility();

	// for debugging
	debugConsole->player = player;
	debugConsole->game = this;
	debugConsole->chunkMap = chunkMap;
	debugConsole->world = world;
	debugConsole->calendar = calendar;
}

void Voxel::GameScene::releaseDebugConsole()
{
	if (debugConsole) delete debugConsole;
}

void Voxel::GameScene::renderDebugConsole()
{
	debugConsole->render();
	//debugConsole->updateDrawCallsAndVerticesSize();
}
#endif
