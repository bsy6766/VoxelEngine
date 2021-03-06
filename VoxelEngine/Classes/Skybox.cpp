// pch
#include "PreCompiled.h"

#include "Skybox.h"

// voxel
#include "Cube.h"
#include "ProgramManager.h"
#include "Program.h"
#include "ChunkUtil.h"
#include "Utility.h"
#include "Application.h"
#include "Color.h"

using namespace Voxel;

Skybox::Skybox()
	: vao(0)
	, indicesSize(0)
	, fogState(FOG_STATE::IDLE)
	, skycolorState(SKYCOLOR_STATE::IDLE)
	, fogDistance(0)
	, fogLength(0)
	, curFogDistance(0)
	, fogAnimationSpeed(0.25f)
	, fogEnabled(true)
	, skyboxProgram(nullptr)
	, size(0)
	, midBlend(0)
{
}

Skybox::~Skybox()
{
	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
	}
}

void Voxel::Skybox::init(const int renderDistance)
{
	// 3 times than render distance. making sure it renders everthing.
	size = static_cast<float>(renderDistance * 4) * Constant::CHUNK_BORDER_SIZE;

	setFogLength(2);
	setFogDistanceByRenderDistance(renderDistance, false);

	std::cout << "[Skybox] Fog distance = " << fogDistance << std::endl;

	initSkybox();

	initSun();
}

void Voxel::Skybox::initSkybox()
{
	const float topDivider = 0.3f;
	const float botDivider = 0.3f;

	std::vector<float> vertices =
	{
		-0.5f, -0.5f, -0.5f,
		-0.5f, -botDivider, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -botDivider, -0.5f,
		-0.5f, -0.5f, 0.5f,
		-0.5f, -botDivider, 0.5f,
		0.5f, -0.5f, 0.5f,
		0.5f, -botDivider, 0.5f,

		-0.5f, topDivider, -0.5f,
		-0.5f, 0.5f, -0.5f,
		0.5f, topDivider, -0.5f,
		0.5f, 0.5f, -0.5f,
		-0.5f, topDivider, 0.5f,
		-0.5f, 0.5f, 0.5f,
		0.5f, topDivider, 0.5f,
		0.5f, 0.5f, 0.5f,
	};
	//std::vector<float> vertices = Cube::getVertices();

	std::vector<unsigned int> indices =
	{
		0, 1, 2, 1, 2, 3,
		2, 3, 6, 3, 6, 7,
		6, 7, 4, 7, 4, 5,
		4, 5, 0, 5, 0, 1,
		0, 4, 2, 4, 2, 6,

		1, 8, 3, 8, 3, 10,
		3, 10, 7, 10, 7, 14,
		7, 14, 5, 14, 5, 12,
		5, 12, 1, 12, 1, 8,

		8, 9, 10, 9, 10, 11,
		10, 11, 14, 11, 14, 15,
		14, 15, 12, 15, 12, 13,
		12, 13, 8, 13, 8, 9,
		9, 13, 11, 13, 11, 15
	};

	//std::vector<unsigned int> indices = Cube::getIndices(Cube::Face::ALL, 0);

	indicesSize = static_cast<int>(indices.size());
	// 1. VAO
	// Generate vertex array object
	glGenVertexArrays(1, &vao);
	// Bind it
	glBindVertexArray(vao);

	// 2. VBO
	GLuint vbo;
	// Generate buffer object
	glGenBuffers(1, &vbo);
	// Bind it
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Load cube vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices.front(), GL_STATIC_DRAW);

	// Get program
	skyboxProgram = ProgramManager::getInstance().getProgram(ProgramManager::PROGRAM_NAME::SKYBOX_SHADER);

	// Enable vertices attrib
	GLint vertLoc = skyboxProgram->getAttribLocation("vert");

	// vert
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// 4. IBO
	GLuint ibo;
	// Generate indices object
	glGenBuffers(1, &ibo);
	// Bind indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	// Load indices
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices.front(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	// Delte buffers
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
}

void Voxel::Skybox::initSun()
{

}

void Voxel::Skybox::initMoonPhases()
{
}

void Voxel::Skybox::update(const float delta)
{
	if (fogState == FOG_STATE::ANIMATING)
	{
		curFogDistance = Utility::Math::lerp(curFogDistance, fogDistance, delta * fogAnimationSpeed);
		if (abs(curFogDistance - fogDistance) < 2.0f)
		{
			fogState = FOG_STATE::IDLE;
			fogDistance = curFogDistance;
		}
	}

	if (skycolorState == SKYCOLOR_STATE::ANIMATING)
	{

	}
}

void Voxel::Skybox::updateColor(const int hour, const int minute, const float seconds)
{
	float minuteRatio = ((static_cast<float>(minute) + seconds) * 0.0166666666666667f);
		
	glm::vec3 topColor;
	glm::vec3 bottomColor;

	switch (hour)
	{
	case 0:
	{
		// 12:00 am
		topColor = glm::mix(Color::SC_12_00_AM.topColor, Color::SC_1_00_AM.topColor, minuteRatio);
		bottomColor = glm::mix(Color::SC_12_00_AM.bottomColor, Color::SC_1_00_AM.bottomColor, minuteRatio);
	}
	break;
	case 1:
	{
		// 1:00 am
		topColor = glm::mix(Color::SC_1_00_AM.topColor, Color::SC_2_00_AM.topColor, minuteRatio);
		bottomColor = glm::mix(Color::SC_1_00_AM.bottomColor, Color::SC_2_00_AM.bottomColor, minuteRatio);
	}
	break;
	case 2:
	{
		// 2:00 am
		topColor = glm::mix(Color::SC_2_00_AM.topColor, Color::SC_3_00_AM.topColor, minuteRatio);
		bottomColor = glm::mix(Color::SC_2_00_AM.bottomColor, Color::SC_3_00_AM.bottomColor, minuteRatio);
	}
	break;
	case 3:
	{
		// 3:00 am
		topColor = glm::mix(Color::SC_3_00_AM.topColor, Color::SC_4_00_AM.topColor, minuteRatio);
		bottomColor = glm::mix(Color::SC_3_00_AM.bottomColor, Color::SC_4_00_AM.bottomColor, minuteRatio);
	}
	break;
	case 4:
	{
		// 4:00 am
		topColor = glm::mix(Color::SC_4_00_AM.topColor, Color::SC_5_00_AM.topColor, minuteRatio);
		bottomColor = glm::mix(Color::SC_4_00_AM.bottomColor, Color::SC_5_00_AM.bottomColor, minuteRatio);
	}
	break;
	case 5:
	{
		// 5:00 am
		topColor = glm::mix(Color::SC_5_00_AM.topColor, Color::SC_6_00_AM.topColor, minuteRatio);
		bottomColor = glm::mix(Color::SC_5_00_AM.bottomColor, Color::SC_6_00_AM.bottomColor, minuteRatio);
	}
	break;
	case 6:
	{
		// 6:00 am
		topColor = glm::mix(Color::SC_6_00_AM.topColor, Color::SC_7_00_AM.topColor, minuteRatio);
		bottomColor = glm::mix(Color::SC_6_00_AM.bottomColor, Color::SC_7_00_AM.bottomColor, minuteRatio);
	}
	break;
	case 7:
	{
		// 7:00 am
		topColor = glm::mix(Color::SC_7_00_AM.topColor, Color::SC_8_00_AM.topColor, minuteRatio);
		bottomColor = glm::mix(Color::SC_7_00_AM.bottomColor, Color::SC_8_00_AM.bottomColor, minuteRatio);
	}
	break;
	case 8:
	{
		// 8:00 am
		topColor = glm::mix(Color::SC_8_00_AM.topColor, Color::SC_9_00_AM.topColor, minuteRatio);
		bottomColor = glm::mix(Color::SC_8_00_AM.bottomColor, Color::SC_9_00_AM.bottomColor, minuteRatio);
	}
	break;
	case 9:
	{
		// 9:00 am
		topColor = glm::mix(Color::SC_9_00_AM.topColor, Color::SC_10_00_AM.topColor, minuteRatio);
		bottomColor = glm::mix(Color::SC_9_00_AM.bottomColor, Color::SC_10_00_AM.bottomColor, minuteRatio);
	}
	break;
	case 10:
	{
		// 10:00 am
		topColor = glm::mix(Color::SC_10_00_AM.topColor, Color::SC_11_00_AM.topColor, minuteRatio);
		bottomColor = glm::mix(Color::SC_10_00_AM.bottomColor, Color::SC_11_00_AM.bottomColor, minuteRatio);
	}
	break;
	case 11:
	{
		// 11:00 am
		topColor = glm::mix(Color::SC_11_00_AM.topColor, Color::SC_12_00_PM.topColor, minuteRatio);
		bottomColor = glm::mix(Color::SC_11_00_AM.bottomColor, Color::SC_12_00_PM.bottomColor, minuteRatio);
	}
	break;
	case 12:
	{
		// 12:00 pm
		topColor = glm::mix(Color::SC_12_00_PM.topColor, Color::SC_1_00_PM.topColor, minuteRatio);
		bottomColor = glm::mix(Color::SC_12_00_PM.bottomColor, Color::SC_1_00_PM.bottomColor, minuteRatio);
	}
	break;
	case 13:
	{
		// 1:00 pm
		topColor = glm::mix(Color::SC_1_00_PM.topColor, Color::SC_2_00_PM.topColor, minuteRatio);
		bottomColor = glm::mix(Color::SC_1_00_PM.bottomColor, Color::SC_2_00_PM.bottomColor, minuteRatio);
	}
	break;
	case 14:
	{
		// 2:00 pm
		topColor = glm::mix(Color::SC_2_00_PM.topColor, Color::SC_3_00_PM.topColor, minuteRatio);
		bottomColor = glm::mix(Color::SC_2_00_PM.bottomColor, Color::SC_3_00_PM.bottomColor, minuteRatio);
	}
	break;
	case 15:
	{
		// 3:00 pm
		topColor = glm::mix(Color::SC_3_00_PM.topColor, Color::SC_4_00_PM.topColor, minuteRatio);
		bottomColor = glm::mix(Color::SC_3_00_PM.bottomColor, Color::SC_4_00_PM.bottomColor, minuteRatio);
	}
	break;
	case 16:
	{
		// 4:00 pm
		topColor = glm::mix(Color::SC_4_00_PM.topColor, Color::SC_5_00_PM.topColor, minuteRatio);
		bottomColor = glm::mix(Color::SC_4_00_PM.bottomColor, Color::SC_5_00_PM.bottomColor, minuteRatio);
	}
	break;
	case 17:
	{
		// 5:00 pm
		topColor = glm::mix(Color::SC_5_00_PM.topColor, Color::SC_6_00_PM.topColor, minuteRatio);
		bottomColor = glm::mix(Color::SC_5_00_PM.bottomColor, Color::SC_6_00_PM.bottomColor, minuteRatio);
	}
	break;
	case 18:
	{
		// 6:00 pm
		topColor = glm::mix(Color::SC_6_00_PM.topColor, Color::SC_7_00_PM.topColor, minuteRatio);
		bottomColor = glm::mix(Color::SC_6_00_PM.bottomColor, Color::SC_7_00_PM.bottomColor, minuteRatio);
	}
	break;
	case 19:
	{
		// 7:00 pm
		topColor = glm::mix(Color::SC_7_00_PM.topColor, Color::SC_8_00_PM.topColor, minuteRatio);
		bottomColor = glm::mix(Color::SC_7_00_PM.bottomColor, Color::SC_8_00_PM.bottomColor, minuteRatio);
	}
	break;
	case 20:
	{
		// 8:00 pm
		topColor = glm::mix(Color::SC_8_00_PM.topColor, Color::SC_9_00_PM.topColor, minuteRatio);
		bottomColor = glm::mix(Color::SC_8_00_PM.bottomColor, Color::SC_9_00_PM.bottomColor, minuteRatio);
	}
	break;
	case 21:
	{
		// 9:00 pm
		topColor = glm::mix(Color::SC_9_00_PM.topColor, Color::SC_10_00_PM.topColor, minuteRatio);
		bottomColor = glm::mix(Color::SC_9_00_PM.bottomColor, Color::SC_10_00_PM.bottomColor, minuteRatio);
	}
	break;
	case 22:
	{
		// 10:00 pm
		topColor = glm::mix(Color::SC_10_00_PM.topColor, Color::SC_11_00_PM.topColor, minuteRatio);
		bottomColor = glm::mix(Color::SC_10_00_PM.bottomColor, Color::SC_11_00_PM.bottomColor, minuteRatio);
	}
	break;
	case 23:
	{
		// 11:00 pm
		topColor = glm::mix(Color::SC_11_00_PM.topColor, Color::SC_12_00_AM.topColor, minuteRatio);
		bottomColor = glm::mix(Color::SC_11_00_PM.bottomColor, Color::SC_12_00_AM.bottomColor, minuteRatio);
	}
	break;
	default:
		break;
	}

	skyboxProgram->use(true);
	
	skyboxProgram->setUniformVec3("topColor", topColor);
	skyboxProgram->setUniformVec3("bottomColor", bottomColor);

	midBlend = glm::mix(topColor, bottomColor, 0.5f);
	//midBlend = bottomColor;
}

void Voxel::Skybox::updateMatrix(const glm::mat4 & mat)
{
	MVP_Matrix = mat * glm::scale(glm::mat4(1.0f), glm::vec3(size, size * 0.5f, size * 0.75f));
}

void Voxel::Skybox::render()
{
	skyboxProgram->use(true);
	skyboxProgram->setUniformMat4("MVP_Matrix", MVP_Matrix);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);

	// For debug
	auto glView = Application::getInstance().getGLView();

	if (glView->doesCountDrawCalls())
	{
		glView->incrementDrawCall();
	}

	if (glView->doesCountVerticesSize())
	{
		glView->addVerticesSize(indicesSize);
	}
}

float Voxel::Skybox::getFogDistance()
{
	if (fogState == FOG_STATE::IDLE)
	{
		return fogDistance;
	}
	else
	{
		return curFogDistance;
	}
}

float Voxel::Skybox::getFogLength()
{
	return fogLength * Constant::CHUNK_BORDER_SIZE;
}

void Voxel::Skybox::setFogLength(const int chunkSize)
{
	fogLength = static_cast<float>(chunkSize);
}

void Voxel::Skybox::setFogDistanceByRenderDistance(const int renderDistance, const bool animate)
{
	setFogDistance((static_cast<float>(renderDistance) - fogLength) * Constant::CHUNK_BORDER_SIZE, animate);
}

void Voxel::Skybox::setFogDistance(const float distance, const bool animate)
{
	this->curFogDistance = this->fogDistance;
	this->fogDistance = distance;

	if (animate)
	{
		fogState = FOG_STATE::ANIMATING;
	}
	else
	{
		fogState = FOG_STATE::IDLE;
	}
}

bool Voxel::Skybox::isFogEnabled()
{
	return fogEnabled;
}

void Voxel::Skybox::setFogEnabled(const bool enabled)
{
	this->fogEnabled = enabled;
}

glm::vec3 Voxel::Skybox::getMidBlendColor()
{
	return midBlend;
}

float Voxel::Skybox::getAmbientColor(const int hour, const int minute, const float second)
{
	// Max ambient light (12:00pm) is glm::vec3(1.0f)
	// Min ambient light (12:00am) is glm::vec3(0.3f)
	// So we get value between 0.3f and 1.0f based on time.

	float ratio = (static_cast<float>(minute) + second) + static_cast<float>(hour * 60);

	float value = 1.0f;

	if (ratio < 720.0f)
	{
		// 12am to 12pm (0 ~ 719)
	}
	else
	{
		// 12pm to 12am (720 ~ 1439)
		// convert to 0 ~ 719
		ratio -= 720.0f;
		// flip to 719 ~ 0
		ratio = 719.0f - ratio;
	}

	value = glm::mix(0.15f, 1.0f, ratio * 0.0013888888888889f); /* div by 720.0f*/

	//std::cout << "Time = " << hour << ":" << minute << ":" << second << ", r = " << ratio << ", v = " << value << "\n";

	return value;
}
