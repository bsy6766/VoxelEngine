#include "World.h"

#include <ChunkMap.h>
#include <ChunkLoader.h>
#include <ChunkMeshGenerator.h>
#include <ChunkSection.h>
#include <Chunk.h>
#include <ChunkUtil.h>
#include <ChunkWorkManager.h>
#include <Block.h>
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

World::World()
	: chunkMap(nullptr)
	, chunkLoader(nullptr)
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
	, vd(nullptr)
	, renderChunks(true)
	, renderVoronoi(true)
{
	// init instances
	init();
	// After creation, set cursor to center
	input->setCursorToCenter();

	//Camera::mainCamera->initDebugFrustumLines();
}

World::~World()
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

void Voxel::World::init()
{
	// Init random first
	initRandoms();

	// Initialize default program
	defaultProgram = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::SHADER_COLOR);
	defaultProgram->use(true);

	// Init chunks
	chunkMap = new ChunkMap();
	chunkLoader = new ChunkLoader();
	chunkMeshGenerator = new ChunkMeshGenerator();
	chunkWorkManager = new ChunkWorkManager();

	// player
	player = new Player();

	// cube outline
	initCubeOutline();
	// Skybox
	initSkyBox(glm::vec4(Color::NIGHTTIME, 1.0f));

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

	//Camera::mainCamera->setPosition(glm::vec3(0, 130, -100));
	//Camera::mainCamera->setAngle(glm::vec3(25, 140, 0));
	//cameraMode = true;
	//cameraControlMode = true;

	Application::getInstance().getGLView()->setWindowedFullScreen(0);
	defaultCanvas->setSize(glm::vec2(1920, 1080));
	debugConsole->updateResolution(1920, 1080);

	//Testing voronoi
	initVoronoi();
	//chunkMap->initVoronoi();
}

void Voxel::World::release()
{
	std::cout << "[World] Releasing all instances" << std::endl;
	// delete everything
	if (chunkMap) delete chunkMap;
	if (chunkLoader) delete chunkLoader;
	if (chunkMeshGenerator) delete chunkMeshGenerator;
	if (chunkWorkManager) delete chunkWorkManager;

	if (player)	delete player;

	if (skybox) delete skybox;
	if (calendar) delete calendar;

	if (defaultCanvas) delete defaultCanvas;

	if (debugConsole) delete debugConsole;

	if (vd) delete vd;
}

void Voxel::World::initRandoms()
{
	const auto seed = "ENGINE";
	Utility::Random::setSeed(seed);
	Noise::Manager::init(seed);
}

void Voxel::World::initUI()
{
	FontManager::getInstance().addFont("MunroSmall.ttf", 20);
	FontManager::getInstance().addFont("MunroSmall.ttf", 20, 2);
	defaultCanvas = UI::Canvas::create(Application::getInstance().getGLView()->getScreenSize(), glm::vec2(0));

	// Add temporary cross hair
	defaultCanvas->addImage("crossHair", "cross_hair.png", glm::vec2(0), glm::vec4(Color::WHITE, 1.0f));
	// Add time label
	UI::Text* timeLabel = UI::Text::create(calendar->getTimeInStr(false), glm::vec2(-200.0f, -5.0f), 2, UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 10);
	//UI::Text* timeLabel = UI::Text::create(calendar->getTimeInStr(false), glm::vec2(0), 2, UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 10);
	timeLabel->setPivot(glm::vec2(-0.5f, 0.5f));
	timeLabel->setCanvasPivot(glm::vec2(0.5f, 0.5f));
	defaultCanvas->addText("timeLabel", timeLabel, 0);
}

void Voxel::World::initCubeOutline()
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

void Voxel::World::initVoronoi()
{
	// Generate random grid
	std::vector<std::vector<int>> grid;

	const int EMPTY = 0;
	const int MARKED = 1;
	const int OMITTED = 2;
	const int BORDER = 3;

	const int width = 10;
	const int length = 10;

	// Fill with M (1)
	for (int i = 0; i < width; ++i)
	{
		grid.push_back(std::vector<int>());
		for (int j = 0; j < length; ++j)
		{
			grid.back().push_back(MARKED);
		}
	}

	// set edge of grid as border (2)
	for (auto& i : grid.front())
	{
		i = BORDER;
	}

	for (auto& i : grid.back())
	{
		i = BORDER;
	}

	for (int i = 0; i < width; ++i)
	{
		grid.at(i).front() = BORDER;
		grid.at(i).back() = BORDER;
	}

	// print grid
	for (auto i : grid)
	{
		for (auto j : i)
		{
			std::string str;
			switch (j)
			{
			case EMPTY:
				str = "0";
				break;
			case MARKED:
				str = "M";
				break;
			case OMITTED:
				str = "X";
				break;
			case BORDER:
				str = "B";
				break;
			default:
				continue;
				break;
			}
			std::cout << str << " ";
		}
		std::cout << std::endl;
	}

	//based on grid, generate random points

	int xPos = grid.size() / 2;
	int zPos = grid.front().size() / 2;

	const int interval = 1000;
	const int intervalHalf = interval / 2;

	glm::ivec2 pos = (glm::ivec2(xPos, zPos) * interval) - intervalHalf;

	// For marked 
	const int pad = interval / 10;
	const int randMax = (interval - (pad * 2)) / 2;
	const int randMin = randMax * -1;

	// For omiited cell. More controlled
	const int omittedPad = pad * 3;
	const int omittedRandMax = (interval - (omittedPad * 2)) / 2;
	const int omittedRandMin = omittedRandMax * -1;

	std::vector<Voronoi::Site> points;

	for (auto x : grid)
	{
		for (auto z : x)
		{
			glm::vec2 randPos;
			Voronoi::Site::Type type;
			switch (z)
			{
			case MARKED:
			{
				int randX = Utility::Random::randomInt(randMin, randMax);
				int randZ = Utility::Random::randomInt(randMin, randMax);

				randPos = glm::ivec2(randX, randZ);

				type = Voronoi::Site::Type::MARKED;
				//std::cout << "Marked" << std::endl;
			}
				break;
			case OMITTED:
			{
				//int randX = Utility::Random::randomInt(omittedRandMin, omittedRandMax);
				//int randZ = Utility::Random::randomInt(omittedRandMin, omittedRandMax);

				//randPos = glm::ivec2(randX, randZ);

				int randX = Utility::Random::randomInt(randMin, randMax);
				int randZ = Utility::Random::randomInt(randMin, randMax);

				randPos = glm::ivec2(randX, randZ);

				type = Voronoi::Site::Type::OMITTED;
				//std::cout << "Omitted" << std::endl;
			}
				break;
			case BORDER:
			{
				randPos = glm::vec2(0);
				type = Voronoi::Site::Type::BORDER;
				//std::cout << "Border" << std::endl;
			}
				break;
			case EMPTY:
			default:
				type = Voronoi::Site::Type::NONE;
				continue;
				break;
			}

			randPos += pos;
			//std::cout << "RandPoint = " << Utility::Log::vec2ToStr(randPos) << std::endl;

			points.push_back(Voronoi::Site(randPos, type));

			pos.y/*z*/ -= interval;
		}

		pos.y/*z*/ = (zPos * interval) - intervalHalf;
		pos.x -= interval;
	}

	vd = new Voronoi::Diagram();
	vd->construct(points);

	const float minBound = static_cast<float>(xPos * interval * -1);
	const float maxBound = static_cast<float>(xPos * interval);

	vd->buildCells(minBound, maxBound);
	vd->buildGraph(width, length);
	vd->randomizeCells(width, length);
	vd->initDebugDiagram();
}

void Voxel::World::initSkyBox(const glm::vec4 & skyColor)
{
	skybox = new Skybox();
	// Always set skybox with max render distance
	skybox->init(skyColor, Setting::getInstance().getRenderDistance());

	defaultProgram->setUniformVec3("playerPosition", player->getPosition());
	defaultProgram->setUniformFloat("fogDistance", skybox->getFogDistance());
	defaultProgram->setUniformVec4("fogColor", skybox->getColor());
}

void Voxel::World::initMeshBuilderThread()
{
	// run first, create thread later
	unsigned int concurentThreadsSupported = std::thread::hardware_concurrency();
	std::cout << "Number of supporting threads: " << concurentThreadsSupported << std::endl;

	chunkWorkManager->run();
	chunkWorkManager->createThreads(chunkMap, chunkMeshGenerator, concurentThreadsSupported);
}

void Voxel::World::createNew(const std::string & worldName)
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

	// Then based init chunks
	createChunkMap();

	// Threads
	initMeshBuilderThread();

	// initialize chunk loader.
	loadChunkLoader();

	auto end = Utility::Time::now();
	std::cout << "New world creation took " << Utility::Time::toMilliSecondString(start, end) << std::endl;
}

void World::createPlayer()
{
	// Initialize player. Pick random spot in region (0, 0). 
	// Range is 150 ~ 300, we don't want player to spawn in edge of region. 
	float randX = static_cast<float>(Utility::Random::randomInt(150, 300)) + 0.5f;
	float randZ = static_cast<float>(Utility::Random::randomInt(150, 300)) + 0.5f;
	// For now, set 0 to 0. Todo: Make topY() function that finds hieghts y that player can stand.
	player->init(glm::vec3(randX, 90.0f, randZ));
	player->setPosition(glm::vec3(8, 100, 8));
	player->setRotation(glm::vec3(-90, 0, 0));
	// Todo: load player's last direction

	// Todo: set this to false. For now, set ture for debug
	player->setFly(true);
	// Update matrix
	player->updateViewMatrix();
	// Based on player's matrix, update frustum
	Camera::mainCamera->updateFrustum(player->getPosition(), player->getOrientation(), 16);

	// for debug
	//player->initYLine();
	//player->initRayLine();
}

void World::createChunkMap()
{
	auto playerPosition = player->getPosition();
	std::cout << "[ChunkMap] Player is at (" << playerPosition.x << ", " << playerPosition.y << ", " << playerPosition.z << ")" << std::endl;

	// Only need player x and z to find which chunk that player is in. This is world position
	int chunkX = static_cast<int>(playerPosition.x) / Constant::CHUNK_SECTION_WIDTH;
	int chunkZ = static_cast<int>(playerPosition.z) / Constant::CHUNK_SECTION_LENGTH;

	std::cout << "[ChunkMap] Player is in chunk (" << chunkX << ", " << chunkZ << ")" << std::endl;

	auto start = Utility::Time::now();

	// create chunks for region -1 ~ 1.
	// For now, test with 0, 0
	//chunkMap->generateRegion(glm::ivec2(0, 0));
	chunkMap->initChunkNearPlayer(playerPosition, Setting::getInstance().getRenderDistance());
	FileSystem::getInstance().createRegionFile(0, 0);

	auto end = Utility::Time::now();
	std::cout << "[ChunkMap] ElapsedTime: " << Utility::Time::toMilliSecondString(start, end) << std::endl;
}

void World::loadChunkLoader()
{
	auto start = Utility::Time::now();

	// Load visible chunk based on player's render distance
	// Todo: load render distance from player settings

	auto chunkCoordinates = chunkLoader->init(player->getPosition(), chunkMap, Setting::getInstance().getRenderDistance(), glfwGetTime());

	// Sorts chunk that needs mesh to be generated by closer to player's position.
	// Todo, let chunks that are visible to have higher priority while sort.
	glm::vec2 p = chunkCoordinates.front();
	std::sort(chunkCoordinates.begin(), chunkCoordinates.end(), [p](const glm::vec2& lhs, const glm::vec2& rhs) { return glm::distance(p, lhs) < glm::distance(p, rhs); });

	for (auto xz : chunkCoordinates)
	{
		chunkWorkManager->addLoad(xz);
	}

	auto end = Utility::Time::now();
	std::cout << "[ChunkLoader] ElapsedTime: " << Utility::Time::toMilliSecondString(start, end) << std::endl;
}

void World::loadChunkMesh()
{
	auto start = Utility::Time::now();

	// Load visible chunk based on player's render distance
	// Todo: load render distance from player settings	
	chunkMeshGenerator->generateAllChunkMesh(chunkLoader, chunkMap);

	auto end = Utility::Time::now();
	std::cout << "[ChunkMeshGenerator] ElapsedTime: " << Utility::Time::toMilliSecondString(start, end) << std::endl;
}

void World::update(const float delta)
{
	checkUnloadedChunks();

	bool playerMoved = player->didMoveThisFrame();
	bool playerRotated = player->didRotateThisFrame();

	// After updating frustum, run frustum culling to find visible chunk
	int totalVisible = chunkLoader->findVisibleChunk();

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
		updateChunks();
		debugConsole->updatePlayerPosition(player->getPosition());

		defaultProgram->use(true);
		defaultProgram->setUniformVec3("playerPosition", player->getPosition());
	}

	debugConsole->updateChunkNumbers(totalVisible, chunkLoader->getActiveChunksCount(), chunkMap->getSize());

	player->update();
	skybox->update(delta);

	calendar->update(delta);
	defaultCanvas->getText("timeLabel")->setText(calendar->getTimeInStr(false));
}

void Voxel::World::updateInput(const float delta)
{
	updateKeyboardInput(delta);
	updateMouseMoveInput(delta);
	updateMouseClickInput();
	updateControllerInput(delta);
}

glm::vec3 Voxel::World::getMovedDistByKeyInput(const float angleMod, const glm::vec3 axis, float distance)
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

void Voxel::World::updateKeyboardInput(const float delta)
{
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
		// Stop input while opening console
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

		if (vd)
		{
			delete vd;
		}

		initVoronoi();
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
		float cameraMovementSpeed = 15.0f;

		if (input->getKeyDown(GLFW_KEY_W))
		{
			Camera::mainCamera->addPosition(getMovedDistByKeyInput(-180.0f, glm::vec3(0, 1, 0), cameraMovementSpeed * delta));
		}
		else if (input->getKeyDown(GLFW_KEY_S))
		{
			Camera::mainCamera->addPosition(getMovedDistByKeyInput(0, glm::vec3(0, 1, 0), cameraMovementSpeed * delta));
		}

		if (input->getKeyDown(GLFW_KEY_A))
		{
			Camera::mainCamera->addPosition(getMovedDistByKeyInput(90.0f, glm::vec3(0, 1, 0), cameraMovementSpeed * delta));
		}
		else if (input->getKeyDown(GLFW_KEY_D))
		{
			Camera::mainCamera->addPosition(getMovedDistByKeyInput(-90.0f, glm::vec3(0, 1, 0), cameraMovementSpeed * delta));
		}

		if (input->getKeyDown(GLFW_KEY_SPACE))
		{
			Camera::mainCamera->addPosition(glm::vec3(0, cameraMovementSpeed * delta, 0));
		}
		else if (input->getKeyDown(GLFW_KEY_LEFT_SHIFT))
		{
			Camera::mainCamera->addPosition(glm::vec3(0, -cameraMovementSpeed * delta, 0));
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

void Voxel::World::updateMouseMoveInput(const float delta)
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

void Voxel::World::updateMouseClickInput()
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

void Voxel::World::updateControllerInput(const float delta)
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

void Voxel::World::updateChunks()
{
	// Update chunk.
	// Based on player position, check if player moved to new chunk
	// If so, we need to load new chunks. 
	// Else, player reamains on same chunk as now.
	bool updated = chunkLoader->update(player->getPosition(), chunkMap, chunkWorkManager, glfwGetTime());
}

void Voxel::World::updatePlayerRaycast()
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

void Voxel::World::checkUnloadedChunks()
{
	// Check if there is any chunk to unload
	glm::ivec2 chunkXZ;
	bool result  = chunkWorkManager->getFinishedFront(chunkXZ);
	if (result)
	{
		chunkMap->releaseChunk(chunkXZ);

		chunkWorkManager->popFinishedAndNotify();
	}
}

void World::render(const float delta)
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

	//defaultProgram->setUniformBool("fogEnabled", true);
	defaultProgram->setUniformFloat("fogDistance", skybox->getFogDistance());

	defaultProgram->setUniformVec3("pointLights[0].lightPosition", player->getPosition());

	chunkLoader->render();
	vd->render();

	defaultProgram->setUniformBool("fogEnabled", false);

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

	defaultCanvas->render();
	debugConsole->render();

	glBindVertexArray(0);
	glUseProgram(0);

}