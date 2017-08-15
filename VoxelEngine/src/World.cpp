#include "World.h"

#include <ChunkMap.h>
#include <ChunkLoader.h>
#include <ChunkMeshGenerator.h>
#include <ChunkUtil.h>

#include <InputHandler.h>
#include <Camera.h>

#include <ShaderManager.h>
#include <ProgramManager.h>
#include <Shader.h>
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
{
	Utility::Random::setSeed("ENGINE");

	auto vertexShader = ShaderManager::getInstance().createShader("defaultVert", "shaders/defaultVertexShader.glsl", GL_VERTEX_SHADER);
	auto fragmentShader = ShaderManager::getInstance().createShader("defaultFrag", "shaders/defaultFragmentShader.glsl", GL_FRAGMENT_SHADER);
	program = ProgramManager::getInstance().createProgram("defaultProgram", vertexShader, fragmentShader);

	//initDebugCube();
	initTestChunk();
	initPlayer();

	input->setCursorToCenter();
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
}

void World::initPlayer()
{
	player = new Player();
	player->init(glm::vec3(0, 0, -20.0f));
	player->setRotation(glm::vec3(0, 180.0f, 0));
	player->setFly(true);
}

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

	// Get cube verticies and indicies
	std::vector<float> cubeVerticies = Cube::getVerticies(Cube::Face::ALL, 1.0f, 0.0f, 0.0f);
	std::vector<unsigned int> cubeIndicies = Cube::getIndicies(Cube::Face::ALL, 0);

	// Load cube verticies
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerticies) * cubeVerticies.size(), &cubeVerticies[0], GL_STATIC_DRAW);
	// Enable verticies attrib
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

	// Generate indicies object
	glGenBuffers(1, &ibo);
	// Bind indicies
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	// Load indicies
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndicies) * cubeIndicies.size(), &cubeIndicies[0], GL_STATIC_DRAW);
	// unbind buffer
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	auto flag = Cube::Face::BACK | Cube::Face::FRONT | Cube::Face::RIGHT | Cube::Face::LEFT | Cube::Face::TOP | Cube::Face::BOTTOM;
	std::vector<float> vert = Cube::getVerticies(static_cast<Cube::Face>(flag));
	std::vector<float> color = std::vector<float>();
	for (int i = 0; i < vert.size(); i++)
	{
		color.push_back(1.0f);
	}
	std::vector<unsigned int> indicies = Cube::getIndicies(static_cast<Cube::Face>(flag), 0);

	chunkMesh = new ChunkMesh();
	chunkMesh->initBuffer(vert, color, indicies);
	chunkMesh->initOpenGLObjects();
	//chunkMesh->initTest(cubeVerticies, cubeIndicies);

}

void Voxel::World::initTestChunk()
{
	glm::vec3 playerPosition = glm::vec3(0);
	std::cout << "[World] Player is at (" << playerPosition.x << ", " << playerPosition.y << ", " << playerPosition.z << ")" << std::endl;

	// Only need player x and z to find which chunk that player is in. This is world position
	int chunkX = static_cast<int>(playerPosition.x) / Constant::CHUNK_SECTION_WIDTH;
	int chunkZ = static_cast<int>(playerPosition.z) / Constant::CHUNK_SECTION_LENGTH;

	std::cout << "[World] Player is in chunk (" << chunkX << ", " << chunkZ << ")" << std::endl;

	// init chunk map
	chunkMap = new ChunkMap();
	chunkMap->init();

	// init loader
	chunkLoader = new ChunkLoader();
	chunkLoader->init(glm::vec3(0), chunkMap, 1);

	// init mesh generator
	chunkMeshGenerator = new ChunkMeshGenerator();
	chunkMeshGenerator->generateChunkMesh(chunkLoader, chunkMap);
}

void World::update(const float delta)
{
	// Keyboard
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

	// Mouse
	{
		
		double x, y;
		input->getMousePosition(x, y);

		double dx = x - mouseX;
		double dy = y - mouseY;
		mouseX = x;
		mouseY = y;

		//std::cout << "Cursor pos (" << xf << ", " << yf << ")" << std::endl;

		if (dx != 0)
		{
			player->addRotationY(delta * static_cast<float>(dx));
		}

		if (dy != 0)
		{
			player->addRotationX(delta * static_cast<float>(dy));
		}
	}

	// Controller
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

	/*
	if (input->getKeyDown(GLFW_KEY_Q))
	{
		Camera::mainCamera->addAngle(glm::vec3(0, 15.0f * delta, 0));
	}
	else if (input->getKeyDown(GLFW_KEY_E))
	{
		Camera::mainCamera->addAngle(glm::vec3(0, -15.0f * delta, 0));
	}
	*/

	if (input->getKeyDown(GLFW_KEY_C))
	{
		//Camera::mainCamera->setAngle(glm::vec3(0, 180.0f, 0));
		//Camera::mainCamera->setPosition(glm::vec3(0, 0, -20.0f));

		player->setPosition(glm::vec3(0));
		player->setRotation(glm::vec3(0, 180.0f, 0));
	}
}

void World::render(const float delta)
{
	auto skyboxColor = Color::SKYBOX;
	glClearColor(skyboxColor.x, skyboxColor.y, skyboxColor.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program->getObject());

	auto VPMatrix = player->getVP(Camera::mainCamera->getProjection());

	program->setUniformMat4("cameraMat", VPMatrix);

	/*
	chunkMesh->bind();
	chunkMesh->render();
	chunkMesh->unbind();
	*/

	chunkMap->render();

	/*
	float speed = 0.3f;

	if (InputHandler::getInstance().getMouseDown(GLFW_MOUSE_BUTTON_LEFT))
	{
		//tempRotation = glm::rotate(tempRotation, speed * static_cast<float>(elapsed), vec3(1, 0, 0));
		tempRotation = glm::rotate(tempRotation, speed * delta, vec3(0, 1, 0));
		//tempRotation = glm::rotate(tempRotation, speed * static_cast<float>(elapsed), vec3(0, 0, 1));
	}
	if (InputHandler::getInstance().getMouseDown(GLFW_MOUSE_BUTTON_RIGHT))
	{
		tempTralsnate = glm::translate(tempTralsnate, vec3(30.0f * delta, 0, 0));
	}

	program->setUniformMat4("cameraMat", Camera::mainCamera->getMatrix());
	program->setUniformMat4("modelMat", tempTralsnate * tempRotation);

	glBindVertexArray(vao);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	*/

	glUseProgram(0);

}