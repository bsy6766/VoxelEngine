#include "World.h"

#include <ChunkMap.h>
#include <ChunkLoader.h>
#include <ChunkMeshGenerator.h>
#include <ChunkUtil.h>
#include <Block.h>

#include <InputHandler.h>
#include <Camera.h>

#include <UI.h>
#include <FontManager.h>

#include <ProgramManager.h>
#include <Program.h>
#include <glm\gtx\transform.hpp>
#include <Cube.h>
#include <Utility.h>
#include <Color.h>
#include <Player.h>

#include <GLFW\glfw3.h>

#include <Application.h>
#include <GLView.h>

using namespace Voxel;

World::World()
	: chunkMap(nullptr)
	, chunkLoader(nullptr)
	, chunkMeshGenerator(nullptr)
	, input(&InputHandler::getInstance())
	, player(nullptr)
	, mouseX(0)
	, mouseY(0)
	, cameraMode(false)
	, keyCDown(false)
	, cameraControlMode(false)
	, keyXDown(false)
	//, debugPlayerCube(nullptr)
	, defaultProgram(nullptr)
	, defaultCanvas(nullptr)
	, fpsNumber(nullptr)
{
	defaultProgram = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::SHADER_COLOR);

	initUI();

	Application::getInstance().getGLView()->setClearColor(Color::SKYBOX);
	GLView::onFPSCounted = std::bind(&World::onFPSCounted, this, std::placeholders::_1);

	//initDebugCube();
	initPlayer();
	initChunk();

	input->setCursorToCenter();
	
	/*
	threadRunning = true;

	unsigned concurentThreadsSupported = std::thread::hardware_concurrency();

	std::cout << "Number of supporting threads: " << concurentThreadsSupported << std::endl;
	chunkElapsedTime = 0;
	for (int i = 0; i < concurentThreadsSupported - 1; i++)
	{
		testThreads.push_back(std::thread(&World::testThreadFunc, this));
		std::cout << "spawning test thread #" << testThreads.back().get_id() << std::endl;
	}
	*/
}

World::~World()
{
	// delete everything
	delete chunkMap;
	delete chunkLoader;
	delete chunkMeshGenerator;

	//delete chunkMesh;

	if (player)
		delete player;

	if (defaultCanvas)
	{
		delete defaultCanvas;
	}

	FontManager::getInstance().clear();

	/*
	int threadCount = testThreads.size();

	while (threadCount > 0)
	{
		for (auto& thread : testThreads)
		{
			if (thread.joinable())
			{
				thread.join();
				std::cout << "joining test thread #" << thread.get_id() << std::endl;
			}
		}
	}
	*/

	/*
	{
		// wait unilt lock is free
		std::unique_lock<std::mutex> lock(chunkQueueMutex);
		for (auto& thread : testThreads)
		{
			if (thread.joinable())
			{
				thread.join();
				std::cout << "joining test thread #" << thread.get_id() << std::endl;
			}
		}
	}
	*/
}

void World::testThreadFunc()
{
	while (threadRunning)
	{
		{
			std::unique_lock<std::mutex> lock(chunkQueueMutex);
			while (chunkQueue.empty() && threadRunning)
			{
				cv.wait(lock);
			}

			if (threadRunning == false)
			{
				break;
			}

			std::cout << "Thread #" << std::this_thread::get_id() << " has " << chunkQueue.front() << std::endl;
			chunkQueue.pop_front();
		}
		/*

		//chunkQueueMutex.lock();
		if (chunkQueue.empty() == false)
		{
			//std::cout << "Thread #" << std::this_thread::get_id() << " has " << chunkQueue.front() << std::endl;
			chunkQueue.pop_front();
		}
		//chunkQueueMutex.unlock();
		*/
	}

	std::cout << "Thread #" << std::this_thread::get_id() << " joining" << std::endl;
}

void World::initPlayer()
{
	player = new Player();
	player->init(glm::vec3(0));
	//player->setRotation(glm::vec3(0, 180.0f, 0));
	player->setFly(true);
	player->updateViewMatrix();

	//initDebugPlayerCube();
	initDebugCamerafrustum();

	Camera::mainCamera->updateFrustum(player->getPosition(), player->getOrientation());


	// Generate vertex array object
	glGenVertexArrays(1, &pvao);
	// Bind it
	glBindVertexArray(pvao);

	// Generate buffer object
	glGenBuffers(1, &pvbo);
	// Bind it
	glBindBuffer(GL_ARRAY_BUFFER, pvbo);

	auto playerPos = player->getPosition();
	auto playerRayEnd = player->getRayEnd();

	GLfloat lines[] = {
		playerPos.x, -100.0f, playerPos.z, 1, 0, 0,
		playerPos.x, 300.0f, playerPos.z, 1, 0, 0,
	};

	// Load cube vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(lines), lines, GL_STATIC_DRAW);
	// Enable vertices attrib
	GLint vertLoc = defaultProgram->getAttribLocation("vert");
	GLint colorLoc = defaultProgram->getAttribLocation("color");
	// vert
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
	// color
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);



	// Generate vertex array object
	glGenVertexArrays(1, &rvao);
	// Bind it
	glBindVertexArray(rvao);

	// Generate buffer object
	glGenBuffers(1, &rvbo);
	// Bind it
	glBindBuffer(GL_ARRAY_BUFFER, rvbo);

	GLfloat ray[] = {
		playerPos.x, playerPos.y, playerPos.z, 1, 0, 0,
		playerRayEnd.x, playerRayEnd.y, playerRayEnd.z, 1, 0, 0
	};

	// Load cube vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(ray), ray, GL_STATIC_DRAW);
	// Enable vertices attrib
	// vert
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
	// color
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);
}

void Voxel::World::initUI()
{
	FontManager::getInstance().addFont("Crazy Pixel.ttf", 120);

	if (defaultCanvas)
	{
		delete defaultCanvas;
	}

	defaultCanvas = UI::Canvas::create(Application::getInstance().getGLView()->getScreenSize(), glm::vec2(0));

	defaultCanvas->addImage("crossHair", "cross_hair.png", glm::vec2(0));
	defaultCanvas->addText("FPSLabel", "FPS: ", glm::vec2(0, 100), 1, UI::Text::ALIGN::LEFT, UI::Text::TYPE::STATIC);
	fpsNumber = UI::Text::create(" ", glm::vec2(150, 100), 1, UI::Text::ALIGN::LEFT, UI::Text::TYPE::DYNAMIC, 20);
	defaultCanvas->addText("FPSNumber", fpsNumber, 0);
}

/*
void Voxel::World::initDebugCube()
{

	// Generate vertex array object
	glGenVertexArrays(1, &vao);
	// Bind it
	glBindVertexArray(vao);

	// Generate buffer object
	glGenBuffers(1, &vbo);
	// Bind it
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// Get cube vertices and indices
	std::vector<float> cubeVertices = Cube::getVertices(Cube::Face::ALL, 1.0f, 0.0f, 0.0f);
	std::vector<unsigned int> cubeIndices = Cube::getIndices(Cube::Face::ALL, 0);

	// Load cube vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices) * cubeVertices.size(), &cubeVertices[0], GL_STATIC_DRAW);
	// Enable vertices attrib
	GLint vertLoc = program->getAttribLocation("vert");
	GLint colorLoc = program->getAttribLocation("color");
	// vert
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
	// color
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));
	// unbind buffer
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Generate indices object
	glGenBuffers(1, &ibo);
	// Bind indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	// Load indices
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices) * cubeIndices.size(), &cubeIndices[0], GL_STATIC_DRAW);
	// unbind buffer
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	auto flag = Cube::Face::BACK | Cube::Face::FRONT | Cube::Face::RIGHT | Cube::Face::LEFT | Cube::Face::TOP | Cube::Face::BOTTOM;
	std::vector<float> vert = Cube::getVertices(static_cast<Cube::Face>(flag));
	std::vector<float> color = std::vector<float>();
	for (int i = 0; i < vert.size(); i++)
	{
		color.push_back(1.0f);
	}
	std::vector<unsigned int> indices = Cube::getIndices(static_cast<Cube::Face>(flag), 0);

	chunkMesh = new ChunkMesh();
	chunkMesh->initBuffer(vert, color, indices);
	chunkMesh->initOpenGLObjects();
	//chunkMesh->initTest(cubeVertices, cubeIndices);

}
*/

void Voxel::World::initChunk()
{
	glm::vec3 playerPosition = glm::vec3(0);
	std::cout << "[World] Player is at (" << playerPosition.x << ", " << playerPosition.y << ", " << playerPosition.z << ")" << std::endl;

	// Only need player x and z to find which chunk that player is in. This is world position
	int chunkX = static_cast<int>(playerPosition.x) / Constant::CHUNK_SECTION_WIDTH;
	int chunkZ = static_cast<int>(playerPosition.z) / Constant::CHUNK_SECTION_LENGTH;

	std::cout << "[World] Player is in chunk (" << chunkX << ", " << chunkZ << ")" << std::endl;

	int renderDistnace = 4;

	auto start = Utility::Time::now();
	// init chunk map
	auto mapStart = Utility::Time::now();
	chunkMap = new ChunkMap();
	chunkMap->initSpawnChunk();
	chunkMap->initChunkNearPlayer(player->getPosition(), renderDistnace);
	auto mapEnd = Utility::Time::now();
	std::cout << "[ChunkMap] ElapsedTime: " << Utility::Time::toMilliSecondString(mapStart, mapEnd) << std::endl;


	// init loader
	auto loaderStart = Utility::Time::now();
	chunkLoader = new ChunkLoader();
	chunkLoader->init(glm::vec3(0), chunkMap, renderDistnace);
	auto loaderEnd = Utility::Time::now();
	std::cout << "[ChunkLoader] ElapsedTime: " << Utility::Time::toMilliSecondString(loaderStart, loaderEnd) << std::endl;

	// init mesh generator
	auto genStart = Utility::Time::now();
	chunkMeshGenerator = new ChunkMeshGenerator();
	chunkMeshGenerator->generateAllChunkMesh(chunkLoader, chunkMap);
	auto getEnd = Utility::Time::now();
	std::cout << "[ChunkMeshGenerator] ElapsedTime: " << Utility::Time::toMilliSecondString(genStart, getEnd) << std::endl;

	auto end = Utility::Time::now();
	std::cout << "Total ElapsedTime: " << Utility::Time::toMilliSecondString(start, end) << std::endl;

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
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f
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
	glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
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

void World::update(const float delta)
{
	updateKeyboardInput(delta);
	updateMouseMoveInput(delta);
	updateMouseClickInput();
	updateControllerInput(delta);

	bool playerMoved = player->didMoveThisFrame();
	bool playerRotated = player->didRotateThisFrame();

	if (playerMoved || playerRotated)
	{
		// If player either move or rotated, update frustum
		Camera::mainCamera->updateFrustum(player->getPosition(), player->getOrientation());
		// After updating frustum, run frustum culling to find visible chunk
		chunkLoader->findVisibleChunk();
		// Also update raycast
		Block* hit = chunkMap->raycastBlock(player->getPosition(), player->getDirection(), player->getRange());
		if (hit)
		{
			player->setLookingBlock(hit);
		}
		else
		{
			player->setLookingBlock(nullptr);
		}
	}

	if (playerMoved)
	{
		// if player moved, update chunk
		updateChunks();
	}
	
	/*
	if (input->getKeyDown(GLFW_KEY_R))
	{
		std::cout << "raycasting = origin(" << rayOrigin.x << ", " << rayOrigin.y << ", " << rayOrigin.z << ")" << std::endl;
		std::cout << "raycasting = direction(" << rayDirection.x << ", " << rayDirection.y << ", " << rayDirection.z << ")" << std::endl;
		std::cout << "raycasting = end(" << rayEnd.x << ", " << rayEnd.y << ", " << rayEnd.z << ")" << std::endl;
	}
	*/

	/*
	chunkElapsedTime += delta;
	if (chunkElapsedTime > 0.5f)
	{
		// Add number to queue every 0.5f seconds
		//chunkQueueMutex.lock();
		{
			std::unique_lock<std::mutex> lock(chunkQueueMutex);
			chunkQueue.push_back(Utility::Random::randomInt(0, 10));
			std::cout << "Main thread added " << chunkQueue.back() << std::endl;
		}

		cv.notify_one();
		//chunkQueueMutex.unlock();
		chunkElapsedTime -= 0.5f;
	}
	*/
	/*
	Camera::mainCamera->updateFrustumPlane(player->getPosition(), player->getRotation());
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	auto playerY = player->getPosition().y - 10.0f;
	playerY = 30.0f;

	auto mainCamera = Camera::mainCamera;

	GLfloat frustumVertex[] = {
		mainCamera->nearPlane.x, playerY, mainCamera->nearPlane.y, 1, 0, 0, 0.5f,
		mainCamera->nearPlane.z, playerY, mainCamera->nearPlane.w,  1, 0, 0, 0.5f,
		mainCamera->farPlane.x, playerY, mainCamera->farPlane.y, 1, 0, 0, 0.5f,
		mainCamera->nearPlane.z, playerY, mainCamera->nearPlane.w, 1, 0, 0, 0.5f,
		mainCamera->farPlane.x, playerY, mainCamera->farPlane.y, 1, 0, 0, 0.5f,
		mainCamera->farPlane.z, playerY, mainCamera->farPlane.w, 1, 0, 0, 0.5f,
	};

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(frustumVertex), frustumVertex);


	glBindVertexArray(0);
	*/


	player->update();

	// Wipe input data for current frame
	input->postUpdate();
}
/*
void Voxel::World::initDebugPlayerCube()
{
	//debugPlayerCube = new ChunkMesh();
	
	//debugPlayerCube->initBuffer();
	
	// Generate vertex array object
	glGenVertexArrays(1, &vao);
	// Bind it
	glBindVertexArray(vao);

	// Generate buffer object
	glGenBuffers(1, &vbo);
	// Bind it
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// Get cube vertices and indices
	float line[] = {
		-100, 100, 0,
		100, 100, 0
	};

	float lineWithColor[] = {
		-100, 100, 0,  1, 0, 0,
		100, 100, 0,  1, 0, 0,
	};
	
	GLfloat triangle[] = {
		//  X     Y     Z
		0.0f, 100.0f, 0.0f,
		-100.0f,-100.0f, 0.0f,
		100.0f,-100.0f, 0.0f,
	};

	GLfloat triangleWithColor[] = {
		//  X     Y     Z
		0.0f, 30.0f, 0.0f,      1, 0, 0,
		-30.0f,-30.0f, 0.0f,   1, 0, 0,
		30.0f,-30.0f, 0.0f,    1, 0, 0
	};

	// Load cube vertices
	//glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineWithColor), lineWithColor, GL_STATIC_DRAW);

	float color[] = {
		1, 0, 0,
		1, 0, 0
	};


	float color2[] = {
		1, 0, 0,
		1, 0, 0,
		1, 0, 0
	};

	// Enable vertices attrib
	GLint vertLoc = defaultProgram->getAttribLocation("vert");
	GLint colorLoc = defaultProgram->getAttribLocation("color");

	// vert
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
	// color
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));

	glBindVertexArray(0);
}
*/

void Voxel::World::initDebugCamerafrustum()
{
	// Generate vertex array object
	glGenVertexArrays(1, &vao);
	// Bind it
	glBindVertexArray(vao);

	// Generate buffer object
	glGenBuffers(1, &vbo);
	// Bind it
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	auto mainCamera = Camera::mainCamera;

	auto playerY = player->getPosition().y - 10.0f;
	playerY = 30.0f;

	GLfloat frustumVertex[] = {
		mainCamera->nearPlane.x, playerY, mainCamera->nearPlane.y, 1, 0, 0, 0.5f,
		mainCamera->nearPlane.z, playerY, mainCamera->nearPlane.w,  1, 0, 0, 0.5f,
		mainCamera->farPlane.x, playerY, mainCamera->farPlane.y, 1, 0, 0, 0.5f,
		mainCamera->nearPlane.z, playerY, mainCamera->nearPlane.w, 1, 0, 0, 0.5f,
		mainCamera->farPlane.x, playerY, mainCamera->farPlane.y, 1, 0, 0, 0.5f,
		mainCamera->farPlane.z, playerY, mainCamera->farPlane.w, 1, 0, 0, 0.5f,
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(frustumVertex), frustumVertex, GL_STATIC_DRAW);

	GLint vertLoc = defaultProgram->getAttribLocation("vert");
	GLint colorLoc = defaultProgram->getAttribLocation("color");

	// vert
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), nullptr);
	// color
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));
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
	if (input->getKeyDown(GLFW_KEY_P, true))
	{
		Camera::mainCamera->print();
		auto playerPos = player->getPosition();
		auto playerRot = player->getRotation();
		std::cout << "Player is at (" << playerPos.x << ", " << playerPos.y << ", " << playerPos.z << "), rotated (" << playerRot.x << ", " << playerRot.y << ", " << playerRot.z << ")" << std::endl;
	}

	if (input->getKeyDown(GLFW_KEY_M, true) && !input->getKeyDown(GLFW_KEY_LEFT_CONTROL))
	{
		Application::getInstance().getGLView()->setWindowedFullScreen(1);
	}	
	else if (input->getKeyDown(GLFW_KEY_M, true) && input->getKeyDown(GLFW_KEY_LEFT_CONTROL))
	{
		Application::getInstance().getGLView()->setWindowed(1280, 720);
		Application::getInstance().getGLView()->setWindowPosition(100, 100);
	}

	if (input->getKeyDown(GLFW_KEY_V, true) && !input->getKeyDown(GLFW_KEY_LEFT_CONTROL))
	{
		Application::getInstance().getGLView()->setVsync(true);
	}
	else if (input->getKeyDown(GLFW_KEY_V, true) && input->getKeyDown(GLFW_KEY_LEFT_CONTROL))
	{
		Application::getInstance().getGLView()->setVsync(false);
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
	if (!keyCDown && input->getKeyDown(GLFW_KEY_C))
	{
		keyCDown = true;
	}
	else if (keyCDown && input->getKeyUp(GLFW_KEY_C))
	{
		keyCDown = false;
		cameraMode = !cameraMode;
		std::cout << "[World] Camera mode " << std::string(cameraMode ? "enabled" : "disabled") << std::endl;
	}

	if (!keyXDown && input->getKeyDown(GLFW_KEY_X))
	{
		keyXDown = true;
	}
	else if (keyXDown && input->getKeyUp(GLFW_KEY_X))
	{
		keyXDown = false;
		cameraControlMode = !cameraControlMode;
		std::cout << "[World] Camera control mode " << std::string(cameraControlMode ? "enabled" : "disabled") << std::endl;
	}

	if (input->getKeyDown(GLFW_KEY_1))
	{
		player->setPosition(glm::vec3(0));
	}

	if (input->getKeyDown(GLFW_KEY_GRAVE_ACCENT))
	{
		player->setPosition(glm::vec3(8.0, 0, 8.0));
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
		player->setRotation(glm::vec3(90, 180, 0));
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

	//std::cout << "prev pos (" << mouseX << ", " << mouseY << ")" << std::endl;
	//std::cout << "Cursor pos (" << x << ", " << y << ")" << std::endl;

	double dx = x - mouseX;
	double dy = y - mouseY;
	mouseX = x;
	mouseY = y;

	if (cameraControlMode)
	{
		if (dx != 0)
		{
			Camera::mainCamera->addAngle(vec3(0, dx * delta * 15.0f, 0));
		}

		if (dy != 0)
		{
			Camera::mainCamera->addAngle(vec3(dy * delta * 15.0f, 0, 0));
		}
	}
	else
	{
		if (dx != 0.0)
		{
			//std::cout << "dx = " << dx << std::endl;
			player->addRotationY(delta * static_cast<float>(dx));
		}

		if (dy != 0.0)
		{
			//std::cout << "dy = " << dx << std::endl;
			player->addRotationX(delta * static_cast<float>(-dy));
		}
	}
}

void Voxel::World::updateMouseClickInput()
{
	if (input->getMouseDown(GLFW_MOUSE_BUTTON_1, true))
	{
	}
	else if (input->getMouseUp(GLFW_MOUSE_BUTTON_1, true))
	{
	}

	if (input->getMouseDown(GLFW_MOUSE_BUTTON_2, true))
	{
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
			player->addRotationY(delta * valueRightAxisX * 3.0f);
		}

		auto valueRightAxisY = input->getAxisValue(IO::XBOX_360::AXIS::R_AXIS_Y);
		if (valueRightAxisY != 0.0f)
		{
			player->addRotationX(delta * -valueRightAxisY * 3.0f);
		}
	}
}

void Voxel::World::updateChunks()
{
	glm::ivec2 mod = glm::ivec2(0);

	bool updated = chunkLoader->update(player->getPosition(), chunkMap, mod);
	if (updated)
	{
		int totalChunks = chunkMap->getSize();
		std::cout << "[World] Total chunks in map = " << totalChunks << std::endl;
		// Generate new mesh
		chunkMeshGenerator->generateNewChunkMesh(chunkLoader, chunkMap, mod);
	}
}

void World::render(const float delta)
{
	glm::mat4 mat = glm::mat4(1.0f);
	if (cameraMode)
	{
		mat = Camera::mainCamera->getMatrix();
	}
	else
	{
		mat = player->getVP(Camera::mainCamera->getProjection());
	}

	ProgramManager::getInstance().useDefaultProgram(ProgramManager::PROGRAM_NAME::SHADER_COLOR);

	defaultProgram->setUniformMat4("cameraMat", mat);
	defaultProgram->setUniformMat4("modelMat", glm::mat4(1.0f));

	chunkLoader->render();
	/*
	{
		glBindVertexArray(pvao);

		glm::mat4 lineMat = mat4(1.0f);
		lineMat = glm::translate(lineMat, player->getPosition());
		lineMat = glm::rotate(lineMat, glm::radians(-player->getRotation().y), glm::vec3(0, 1, 0));
		//lineMat = glm::rotate(lineMat, glm::radians(-player->getRotation().x), glm::vec3(1, 0, 0));
		//lineMat = glm::rotate(lineMat, glm::radians(-player->getRotation().z), glm::vec3(0, 0, 1));

		defaultProgram->setUniformMat4("modelMat", lineMat);
		glDrawArrays(GL_LINES, 0, 2);
	}
	*/

	/*
	{
		glBindVertexArray(rvao);


		glm::mat4 rayMat = mat4(1.0f);
		rayMat = glm::translate(rayMat, player->getPosition());
		rayMat = glm::rotate(rayMat, glm::radians(-player->getRotation().y), glm::vec3(0, 1, 0));
		rayMat = glm::rotate(rayMat, glm::radians(player->getRotation().x), glm::vec3(1, 0, 0));
		rayMat = glm::rotate(rayMat, glm::radians(-player->getRotation().z), glm::vec3(0, 0, 1));

		defaultProgram->setUniformMat4("modelMat", rayMat);
		glDrawArrays(GL_LINES, 0, 2);
	}

	*/

	/*
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	//glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
	*/

	if (player->isLookingAtBlock())
	{
		glBindVertexArray(cvao);

		glm::mat4 cubeMat = glm::translate(glm::mat4(1.0f), player->getLookingBlock()->getWorldPosition());
		defaultProgram->setUniformMat4("modelMat", cubeMat);

		glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
	}

	// Render UI
	//glm::mat4 bMat = mat4(1.0f);
	//bMat = glm::rotate(bMat, glm::radians(-player->getRotation().x), glm::vec3(1, 0, 0));
	//bMat = glm::rotate(bMat, glm::radians(-player->getRotation().y), glm::vec3(0, 1, 0));
	
	/*
	auto screenSpacePos = (Camera::mainCamera->getScreenSpacePos() * -1.0f);
	//screenSpacePos = glm::vec3(0);
	auto UIModelMat = glm::translate(player->getDirMatrix(), screenSpacePos);

	auto testMat = glm::translate(glm::mat4(1.0f), screenSpacePos);

	//defaultProgram->setUniformMat4("cameraMat", glm::mat4(1.0f));
	defaultProgram->setUniformMat4("modelMat", UIModelMat);
	*/


	glClear(GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_ALWAYS);

	defaultCanvas->render();

	glBindVertexArray(0);
	glUseProgram(0);

}

void Voxel::World::onFPSCounted(int fps)
{
	fpsNumber->setText(std::to_string(fps));
}
