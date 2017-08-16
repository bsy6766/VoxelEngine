#include "World.h"

#include <ChunkMap.h>
#include <ChunkLoader.h>
#include <ChunkMeshGenerator.h>
#include <ChunkUtil.h>

#include <InputHandler.h>
#include <Camera.h>

#include <ProgramManager.h>
#include <Program.h>
#include <glm\gtx\transform.hpp>
#include <Cube.h>
#include <Utility.h>
#include <Color.h>
#include <Player.h>

#include <GLFW\glfw3.h>

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
{
	defaultProgram = ProgramManager::getInstance().getDefaultProgram();

	//initDebugCube();
	initPlayer();
	initChunk();

	input->setCursorToCenter();
	
	threadRunning = true;

	unsigned concurentThreadsSupported = std::thread::hardware_concurrency();

	std::cout << "Number of supporting threads: " << concurentThreadsSupported << std::endl;
	chunkElapsedTime = 0;
	for (int i = 0; i < concurentThreadsSupported - 1; i++)
	{
		testThreads.push_back(std::thread(&World::testThreadFunc, this));
		std::cout << "spawning test thread #" << testThreads.back().get_id() << std::endl;
	}
	
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
	player->init(glm::vec3(0, 60.0f, 0.0f));
	player->setRotation(glm::vec3(0, 180.0f, 0));
	player->setFly(true);
	player->updateViewMatrix();

	//initDebugPlayerCube();
	initDebugCamerafrustum();
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
}

void World::update(const float delta)
{
	updateKeyboardInput(delta);
	updateMouseInput(delta);
	updateControllerInput(delta);
	updateChunks();

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

	if (input->getKeyDown(GLFW_KEY_Y))
	{
		Camera::mainCamera->updateFrustumPlane(player->getPosition(), player->getRotation());
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		GLfloat frustumVertex[] = {
			Camera::mainCamera->nearPlane.x, 1, Camera::mainCamera->nearPlane.y, 1, 0, 0, 0.5f,
			Camera::mainCamera->nearPlane.z, 1, Camera::mainCamera->nearPlane.w,  1, 0, 0, 0.5f,
			Camera::mainCamera->farPlane.x, 1, Camera::mainCamera->farPlane.y, 1, 0, 0, 0.5f,
			Camera::mainCamera->nearPlane.z, 1, Camera::mainCamera->nearPlane.w, 1, 0, 0, 0.5f, 
			Camera::mainCamera->farPlane.x, 1, Camera::mainCamera->farPlane.y, 1, 0, 0, 0.5f,
			Camera::mainCamera->farPlane.z, 1, Camera::mainCamera->farPlane.w, 1, 0, 0, 0.5f,
		};

		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(frustumVertex), frustumVertex);


		glBindVertexArray(0);
	}
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



	GLfloat frustumVertex[] = {
		mainCamera->nearPlane.x, 1, mainCamera->nearPlane.y, 1, 0, 0, 0.5f,
		mainCamera->nearPlane.z, 1, mainCamera->nearPlane.w,  1, 0, 0, 0.5f,
		mainCamera->farPlane.x, 1, mainCamera->farPlane.y, 1, 0, 0, 0.5f,
		mainCamera->nearPlane.z, 1, mainCamera->nearPlane.w, 1, 0, 0, 0.5f,
		mainCamera->farPlane.x, 1, mainCamera->farPlane.y, 1, 0, 0, 0.5f,
		mainCamera->farPlane.z, 1, mainCamera->farPlane.w, 1, 0, 0, 0.5f,
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
	if (input->getKeyDown(GLFW_KEY_P))
	{
		Camera::mainCamera->print();
		auto playerPos = player->getPosition();
		auto playerRot = player->getRotation();
		std::cout << "Player is at (" << playerPos.x << ", " << playerPos.y << ", " << playerPos.z << "), rotated (" << playerRot.x << ", " << playerRot.y << ", " << playerRot.z << ")" << std::endl;
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

	if (input->getKeyDown(GLFW_KEY_V))
	{
		player->setPosition(glm::vec3(0));
		player->setRotation(glm::vec3(0, 180.0f, 0));
	}
}

void Voxel::World::updateMouseInput(const float delta)
{
	double x, y;
	input->getMousePosition(x, y);

	double dx = x - mouseX;
	double dy = y - mouseY;
	mouseX = x;
	mouseY = y;

	//std::cout << "Cursor pos (" << xf << ", " << yf << ")" << std::endl;

	if (cameraControlMode)
	{
		if (dx != 0)
		{
			Camera::mainCamera->addAngle(vec3(0, dx * delta * 100.0f, 0));
		}

		if (dy != 0)
		{
			Camera::mainCamera->addAngle(vec3(dy * delta * 100.0f, 0, 0));
		}
	}
	else
	{
		if (dx != 0)
		{
			player->addRotationY(delta * static_cast<float>(dx));
		}

		if (dy != 0)
		{
			player->addRotationX(delta * static_cast<float>(dy));
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
			player->addRotationY(delta * valueRightAxisX * 0.6f);
		}

		auto valueRightAxisY = input->getAxisValue(IO::XBOX_360::AXIS::R_AXIS_Y);
		if (valueRightAxisY != 0.0f)
		{
			player->addRotationX(delta * -valueRightAxisY * 0.6f);
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

	defaultProgram->setUniformMat4("cameraMat", mat);


	chunkLoader->render();


	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	//glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);


	glUseProgram(0);

}