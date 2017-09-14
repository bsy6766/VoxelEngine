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

#include <Setting.h>

#include <InputHandler.h>
#include <Camera.h>
#include <Frustum.h>

#include <UI.h>
#include <FontManager.h>

#include <ProgramManager.h>
#include <Program.h>
#include <glm\gtx\transform.hpp>
#include <Cube.h>
#include <Utility.h>
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
	, defaultProgram(nullptr)
	, defaultCanvas(nullptr)
	, debugConsole(nullptr)
	, skybox(nullptr)
	, calendar(nullptr)
	, renderChunks(true)
	, renderVoronoi(true)
	, updateChunkMap(true)
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

	// Initialize default program
	defaultProgram = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::SHADER_COLOR);
	defaultProgram->use(true);

	// Init world
	world = new World();

	// Init chunks
	chunkMap = new ChunkMap();
	chunkMeshGenerator = new ChunkMeshGenerator();
	chunkWorkManager = new ChunkWorkManager();

	// player
	player = new Player();

	// cube outline
	initCubeOutline();
	// Skybox
	initSkyBox(glm::vec4(Color::DAYTIME, 1.0f));

	// Calendar
	calendar = new Calendar();
	calendar->init();

	defaultProgram->setUniformVec4("ambientColor", glm::vec4(1.0f));
	defaultProgram->setUniformFloat("pointLights[0].lightIntensity", 20.0f);
	defaultProgram->setUniformVec4("pointLights[0].lightColor", glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

	// UI & font
	initUI();
	// Debug. This creates all the debug UI components
	debugConsole = new DebugConsole();
	debugConsole->toggleDubugOutputs();
	// for debugging
	debugConsole->player = player;
	debugConsole->game = this;

	//Camera::mainCamera->setPosition(glm::vec3(0, 130, -100));
	//Camera::mainCamera->setAngle(glm::vec3(25, 140, 0));
	//cameraMode = true;
	//cameraControlMode = true;

	Application::getInstance().getGLView()->setWindowedFullScreen(0);
	defaultCanvas->setSize(glm::vec2(1920, 1080));
	debugConsole->updateResolution(1920, 1080);
}

void Voxel::Game::release()
{
	std::cout << "[World] Releasing all instances" << std::endl;
	// delete everything
	if (chunkMap) delete chunkMap;
	if (chunkMeshGenerator) delete chunkMeshGenerator;
	if (chunkWorkManager) delete chunkWorkManager;

	if (player)	delete player;

	if (skybox) delete skybox;
	if (calendar) delete calendar;

	if (defaultCanvas) delete defaultCanvas;

	if (debugConsole) delete debugConsole;

	if (world) delete world;
}

void Voxel::Game::initRandoms()
{
	const auto seed = "ENGINE";
	Utility::Random::setSeed(seed);
	Noise::Manager::init(seed);
}

void Voxel::Game::initUI()
{
	FontManager::getInstance().addFont("MunroSmall.ttf", 20);
	FontManager::getInstance().addFont("MunroSmall.ttf", 20, 2);
	defaultCanvas = UI::Canvas::create(Application::getInstance().getGLView()->getScreenSize(), glm::vec2(0));

	// Add temporary cross hair
	auto crossHairImage = UI::Image::create("cross_hair.png", glm::vec2(0), glm::vec4(1.0f));
	defaultCanvas->addImage("crossHair", crossHairImage, 0);

	// Add time label
	UI::Text* timeLabel = UI::Text::createWithOutline(calendar->getTimeInStr(false), glm::vec2(-200.0f, -5.0f), 2, glm::vec4(1.0f), glm::vec4(0, 0, 0, 1), UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 10);
	timeLabel->setPivot(glm::vec2(-0.5f, 0.5f));
	timeLabel->setCanvasPivot(glm::vec2(0.5f, 0.5f));
	defaultCanvas->addText("timeLabel", timeLabel, 0);
}

void Voxel::Game::initCubeOutline()
{

	// Create debug chunk box
	// Generate vertex array object
	glGenVertexArrays(1, &cvao);
	// Bind it
	glBindVertexArray(cvao);

	// Enable vertices attrib
	GLint vertLoc = defaultProgram->getAttribLocation("vert");
	GLint colorLoc = defaultProgram->getAttribLocation("color");

	// Generate buffer object
	glGenBuffers(1, &cvbo);
	// Bind it
	glBindBuffer(GL_ARRAY_BUFFER, cvbo);

	GLfloat cube[] = {
		// x, y, z
		0.502f, -0.502f, -0.502f,
		-0.502f, -0.502f, -0.502f,
		-0.502f, -0.502f, 0.502f,
		0.502f, -0.502f, 0.502f,
		0.502f, 0.502f, -0.502f,
		-0.502f, 0.502f, -0.502f,
		-0.502f, 0.502f, 0.502f,
		0.502f, 0.502f, 0.502f,
	};

	GLfloat color[] = {
		// x, y, z
		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};

	unsigned int indices[] = {
		0, 1, 1, 2, 2, 3, 3, 0,
		4, 5, 5, 6, 6, 7, 7, 4,
		0, 4, 1, 5, 2, 6, 3, 7
	};

	// Load cube vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

	// vert
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Generate buffer object
	glGenBuffers(1, &ccbo);
	// Bind it
	glBindBuffer(GL_ARRAY_BUFFER, ccbo);

	// Load cube vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
	// color
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
	// unbind buffer
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Generate indices object
	glGenBuffers(1, &cibo);
	// Bind indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cibo);
	// Load indices
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	// unbind buffer
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void Voxel::Game::initSkyBox(const glm::vec4 & skyColor)
{
	skybox = new Skybox();
	// Always set skybox with max render distance
	skybox->init(skyColor, Setting::getInstance().getRenderDistance());

	defaultProgram->setUniformVec3("playerPosition", player->getPosition());
	defaultProgram->setUniformFloat("fogDistance", skybox->getFogDistance());
	defaultProgram->setUniformVec4("fogColor", skybox->getColor());
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
	
	auto end = Utility::Time::now();
	std::cout << "New world creation took " << Utility::Time::toMilliSecondString(start, end) << std::endl;
}

World * Voxel::Game::getWorld()
{
	return world;
}

void Game::createPlayer()
{
	// Initialize player. Pick random spot in region (0, 0). 
	// Range is 150 ~ 300, we don't want player to spawn in edge of region. 
	float randX = static_cast<float>(Utility::Random::randomInt(150, 300)) + 0.5f;
	float randZ = static_cast<float>(Utility::Random::randomInt(150, 300)) + 0.5f;
	// For now, set 0 to 0. Todo: Make topY() function that finds hieghts y that player can stand.
	player->init(glm::vec3(randX, 90.0f, randZ));
	player->setPosition(glm::vec3(0, 100, 0));
	//player->setPosition(glm::vec3(-300, 500, 0));
	player->setRotation(glm::vec3(-90, 0, 0));
	// Todo: load player's last direction

	// Todo: set this to false. For now, set ture for debug
	player->setFly(true);
	// Update matrix
	player->updateViewMatrix();
	// Based on player's matrix, update frustum
	Camera::mainCamera->updateFrustum(player->getPosition(), player->getOrientation(), 16);
	Camera::mainCamera->setSpeed(100.0f);

	// for debug
	//player->initYLine();
	//player->initRayLine();
}

void Game::createChunkMap()
{
	auto playerPosition = player->getPosition();
	auto start = Utility::Time::now();

	// create chunks for region -1 ~ 1.
	// For now, test with 0, 0
	//chunkMap->generateRegion(glm::ivec2(0, 0));
	auto rd = Setting::getInstance().getRenderDistance();
	chunkMap->initChunkNearPlayer(playerPosition, rd);
	auto chunkCoordinates = chunkMap->initActiveChunks(rd);

	glm::vec2 p = chunkCoordinates.front();
	std::sort(chunkCoordinates.begin(), chunkCoordinates.end(), [p](const glm::vec2& lhs, const glm::vec2& rhs) { return glm::distance(p, lhs) < glm::distance(p, rhs); });

	for (auto xz : chunkCoordinates)
	{
		chunkWorkManager->addLoad(xz);
	}

	auto end = Utility::Time::now();
	std::cout << "[ChunkMap] ElapsedTime: " << Utility::Time::toMilliSecondString(start, end) << std::endl;
}

void Voxel::Game::createWorld()
{
	world->setTemperature(0.5f, 1.5f);
	world->setMoisture(0.5f, 1.5f);
	world->init(10, 10);
}

void Game::update(const float delta)
{
	checkUnloadedChunks();

	bool playerMoved = player->didMoveThisFrame();
	bool playerRotated = player->didRotateThisFrame();

	// After updating frustum, run frustum culling to find visible chunk
	int totalVisible = chunkMap->findVisibleChunk();

	if (playerMoved || playerRotated)
	{
		// If player either move or rotated, update frustum
		Camera::mainCamera->updateFrustum(player->getPosition(), player->getOrientation(), 16);
		// Also update raycast
		updatePlayerRaycast();
	}

	if (playerMoved)
	{
		// if player moved, update chunk
		if (updateChunkMap)
		{
			updateChunks();
		}

		auto playerPos = player->getPosition();

		bool regionChanged = world->updatePlayerPos(playerPos);

		if (regionChanged)
		{
			Region* curRegion = world->getCurrentRegion();
			debugConsole->updateRegion(curRegion->getID());
			Biome biomeType = curRegion->getBiomeType();
			debugConsole->updateBiome(Biome::biomeTypeToString(biomeType), Terrain::terrainTypeToString(curRegion->getTerrainType()), biomeType.getTemperature(), biomeType.getMoisture());
		}

		debugConsole->updatePlayerPosition(playerPos);

		defaultProgram->use(true);
		defaultProgram->setUniformVec3("playerPosition", playerPos);
	}

	debugConsole->updateChunkNumbers(totalVisible, chunkMap->getActiveChunksCount(), chunkMap->getSize());

	player->update();
	skybox->update(delta);

	calendar->update(delta);
	defaultCanvas->getText("timeLabel")->setText(calendar->getTimeInStr(false));
}

void Voxel::Game::updateInput(const float delta)
{
	updateKeyboardInput(delta);
	updateMouseMoveInput(delta);
	updateMouseClickInput();
	updateControllerInput(delta);
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

		world->rebuildWorldMap();
	}

	if (input->getKeyDown(GLFW_KEY_Y, true))
	{
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
			std::cout << "   (" << freq.at(i) << ")" << std::endl;
		}
	}

	if (input->getKeyDown(GLFW_KEY_MINUS, true))
	{
		renderChunks = !renderChunks;
	}
	
	if (input->getKeyDown(GLFW_KEY_EQUAL, true))
	{
		renderVoronoi = !renderVoronoi;
	}

	if (input->getKeyDown(GLFW_KEY_P, true))
	{
		Camera::mainCamera->print();
		auto playerPos = player->getPosition();
		auto playerRot = player->getRotation();
		std::cout << "Player is at (" << playerPos.x << ", " << playerPos.y << ", " << playerPos.z << "), rotated (" << playerRot.x << ", " << playerRot.y << ", " << playerRot.z << ")" << std::endl;
	}

	if (input->getKeyDown(GLFW_KEY_M, true) && input->getMods() == 0)
	{
		Application::getInstance().getGLView()->setWindowedFullScreen(1);
		defaultCanvas->setSize(glm::vec2(1920, 1080));
		debugConsole->updateResolution(1920, 1080);
	}
	else if (input->getKeyDown(GLFW_KEY_M, true) && input->getMods() == GLFW_MOD_CONTROL)
	{
		Application::getInstance().getGLView()->setWindowed(1280, 720);
		Application::getInstance().getGLView()->setWindowPosition(100, 100);
		defaultCanvas->setSize(glm::vec2(1280, 720));
		debugConsole->updateResolution(1280, 720);
	}
	else if (input->getKeyDown(GLFW_KEY_M, true) && input->getMods() == (GLFW_MOD_CONTROL | GLFW_MOD_SHIFT))
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
		if (input->getKeyDown(GLFW_KEY_W))
		{
			Camera::mainCamera->addPosition(getMovedDistByKeyInput(-180.0f, glm::vec3(0, 1, 0), delta));
		}
		else if (input->getKeyDown(GLFW_KEY_S))
		{
			Camera::mainCamera->addPosition(getMovedDistByKeyInput(0, glm::vec3(0, 1, 0), delta));
		}

		if (input->getKeyDown(GLFW_KEY_A))
		{
			Camera::mainCamera->addPosition(getMovedDistByKeyInput(90.0f, glm::vec3(0, 1, 0), delta));
		}
		else if (input->getKeyDown(GLFW_KEY_D))
		{
			Camera::mainCamera->addPosition(getMovedDistByKeyInput(-90.0f, glm::vec3(0, 1, 0), delta));
		}

		if (input->getKeyDown(GLFW_KEY_SPACE))
		{
			Camera::mainCamera->addPosition(glm::vec3(0, delta, 0));
		}
		else if (input->getKeyDown(GLFW_KEY_LEFT_SHIFT))
		{
			Camera::mainCamera->addPosition(glm::vec3(0, -delta, 0));
		}
	}
	else
	{
		if (input->getKeyDown(GLFW_KEY_W))
		{
			player->moveFoward(delta);
			//std::cout << "!" << std::endl;
		}
		else if (input->getKeyDown(GLFW_KEY_S))
		{
			player->moveBackward(delta);
		}

		if (input->getKeyDown(GLFW_KEY_A))
		{
			player->moveLeft(delta);
		}
		else if (input->getKeyDown(GLFW_KEY_D))
		{
			player->moveRight(delta);
		}

		if (input->getKeyDown(GLFW_KEY_SPACE))
		{
			player->moveUp(delta);
		}
		else if (input->getKeyDown(GLFW_KEY_LEFT_SHIFT))
		{
			player->moveDown(delta);
		}

		if (input->getKeyDown(GLFW_KEY_BACKSPACE))
		{
			Camera::mainCamera->print();
		}
	}

	// For debug
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

	if (input->getKeyDown(GLFW_KEY_1))
	{
		player->setPosition(glm::vec3(0));
	}

	if (input->getKeyDown(GLFW_KEY_2))
	{
		player->setRotation(glm::vec3(0, 0, 0));
	}

	if (input->getKeyDown(GLFW_KEY_3))
	{
		player->setRotation(glm::vec3(0, 90, 0));
	}

	if (input->getKeyDown(GLFW_KEY_4))
	{
		player->setRotation(glm::vec3(0, 180, 0));
	}

	if (input->getKeyDown(GLFW_KEY_5))
	{
		player->setRotation(glm::vec3(0, 270, 0));
	}

	if (input->getKeyDown(GLFW_KEY_6))
	{
		Camera::mainCamera->setPosition(player->getPosition());
	}

	if (input->getKeyDown(GLFW_KEY_KP_1, true))
	{
		Camera::mainCamera->setFovy(Camera::mainCamera->getFovy() + 5.0f);
		std::cout << "fovy = " << Camera::mainCamera->getFovy() << std::endl;
	}
	if (input->getKeyDown(GLFW_KEY_KP_2, true))
	{
		Camera::mainCamera->setFovy(Camera::mainCamera->getFovy() - 5.0f);
		std::cout << "fovy = " << Camera::mainCamera->getFovy() << std::endl;
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

void Voxel::Game::updateMouseClickInput()
{
	if (debugConsole->isConsoleOpened())
	{
		// Stop input while opening console
		return;
	}

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
			chunkMap->placeBlockAt(blockPos, player->getLookingFace(), chunkWorkManager);
			updatePlayerRaycast();
		}
	}
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

void Voxel::Game::updateChunks()
{
	// Update chunk.
	// Based on player position, check if player moved to new chunk
	// If so, we need to load new chunks. 
	// Else, player reamains on same chunk as now.
	bool updated = chunkMap->update(player->getPosition(), chunkWorkManager, glfwGetTime());

	//Whenever player moved, sort the load queue again 
	if (updated)
	{
		chunkWorkManager->sortLoadQueue(player->getPosition());
	}
}

void Voxel::Game::updatePlayerRaycast()
{
	auto result = chunkMap->raycastBlock(player->getPosition(), player->getDirection(), player->getRange());
	if (result.block != nullptr)
	{
		player->setLookingBlock(result.block, result.face);
		debugConsole->updatePlayerLookingAt(result.block->getWorldCoordinate(), result.face);
	}
	else
	{
		player->setLookingBlock(nullptr, Cube::Face::NONE);
	}
}

void Voxel::Game::checkUnloadedChunks()
{
	bool result = true;
	while (result)
	{
		// Check if there is any chunk to unload
		glm::ivec2 chunkXZ;
		result = chunkWorkManager->getFinishedFront(chunkXZ);
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

void Game::render(const float delta)
{
	glm::mat4 projMat = Camera::mainCamera->getProjection();
	glm::mat4 worldMat = glm::mat4(1.0f);

	ProgramManager::getInstance().useDefaultProgram(ProgramManager::PROGRAM_NAME::SHADER_COLOR);

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

	defaultProgram->setUniformMat4("projMat", projMat);
	defaultProgram->setUniformMat4("worldMat", worldMat);
	defaultProgram->setUniformMat4("modelMat", glm::mat4(1.0f));

	if (skybox->isFogEnabled())
	{
		defaultProgram->setUniformBool("fogEnabled", true);
		defaultProgram->setUniformFloat("fogDistance", skybox->getFogDistance());
	}
	else
	{
		defaultProgram->setUniformBool("fogEnabled", false);
	}

	defaultProgram->setUniformVec3("pointLights[0].lightPosition", player->getPosition());

	if (renderChunks)
	{
		chunkMap->render();
	}

	player->render(defaultProgram);

	if (player->isLookingAtBlock())
	{
		glBindVertexArray(cvao);

		glm::mat4 cubeMat = glm::translate(glm::mat4(1.0f), player->getLookingBlock()->getWorldPosition());
		defaultProgram->setUniformMat4("modelMat", cubeMat);

		glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
	}

	defaultProgram->setUniformMat4("modelMat", player->getTranslationMat());
	skybox->render();

	glClear(GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_ALWAYS);

	if (renderVoronoi)
	{
		defaultProgram->setUniformMat4("modelMat", glm::mat4(1.0f));
		world->renderVoronoi();
	}



	defaultCanvas->render();
	debugConsole->render();

	glBindVertexArray(0);
	glUseProgram(0);

}

void Voxel::Game::setRenderChunkMode(const bool mode)
{
	renderChunks = mode;
}

void Voxel::Game::setRenderVoronoiMode(const bool mode)
{
	renderVoronoi = mode;
}

void Voxel::Game::setUpdateChunkMapMode(const bool mode)
{
	updateChunkMap = mode;
}

void Voxel::Game::setFogEnabled(const bool enabled)
{
	skybox->setFogEnabled(enabled);
}
