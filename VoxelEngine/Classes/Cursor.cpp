// pch
#include "PreCompiled.h"

#include "Cursor.h"

// voxel
#include "Application.h"
#include "SpriteSheet.h"
#include "Texture2D.h"
#include "Quad.h"
#include "ProgramManager.h"
#include "Program.h"
#include "Camera.h"

Voxel::Cursor::Cursor()
	: vao(0)
	, visible(false)
	, position(0)
{}

Voxel::Cursor::~Cursor()
{
	release();
}

bool Voxel::Cursor::init()
{
	// Initialize cursors
	auto ss = SpriteSheetManager::getInstance().getSpriteSheetByKey("CursorSpriteSheet");

	// pointer
	this->texture = ss->getTexture();

	this->texture->setLocationOnProgram(ProgramManager::PROGRAM_NAME::UI_TEXTURE_SHADER);

	auto size = glm::vec2(Application::getInstance().getGLView()->getScreenSize());

	minScreenBoundary = size * -0.5f;
	maxScreenBoundary = size * 0.5f;

	auto imageEntry = ss->getImageEntry("pointer.png");

	auto vertices = Quad::getVertices(glm::vec2(imageEntry->width, imageEntry->height));
	auto indices = Quad::indices;

	auto& uvOrigin = imageEntry->uvOrigin;
	auto& uvEnd = imageEntry->uvEnd;

	std::vector<float> uv =
	{
		uvOrigin.x, uvOrigin.y,
		uvOrigin.x, uvEnd.y,
		uvEnd.x, uvOrigin.y,
		uvEnd.x, uvEnd.y
	};

	pivot = glm::vec2(-15.0f / 16.0f, 15.0f / 16.0f) * 0.5f;

	release();

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	auto program = ProgramManager::getInstance().getProgram(ProgramManager::PROGRAM_NAME::UI_TEXTURE_SHADER);
	GLint vertLoc = program->getAttribLocation("vert");

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(vertLoc);	// error count 2
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLint uvVertLoc = program->getAttribLocation("uvVert");

	glGenBuffers(1, &uvbo);
	glBindBuffer(GL_ARRAY_BUFFER, uvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * uv.size(), &uv.front(), GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(uvVertLoc);
	glVertexAttribPointer(uvVertLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices.front(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);

	return true;
}

void Voxel::Cursor::release()
{
	if (uvbo)
	{
		glDeleteBuffers(1, &uvbo);
		uvbo = 0;
	}

	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}
}

void Voxel::Cursor::checkBoundary()
{

	if (this->position.x > maxScreenBoundary.x)
	{
		this->position.x = maxScreenBoundary.x;
	}
	else if (this->position.x < minScreenBoundary.x)
	{
		this->position.x = minScreenBoundary.x;
	}

	if (this->position.y > maxScreenBoundary.y)
	{
		this->position.y = maxScreenBoundary.y;
	}
	else if (this->position.y < minScreenBoundary.y)
	{
		this->position.y = minScreenBoundary.y;
	}
}

void Voxel::Cursor::addPosition(const glm::vec2 & distance)
{
	position.x += distance.x;
	position.y -= distance.y;

	/*
	if (distance.x != 0 || distance.y != 0)
	{
		std::cout << "Cursor moved (" << distance.x << ", " << distance.y << ")\n";
	}
	*/

	checkBoundary();
}

void Voxel::Cursor::setPosition(const glm::vec2 & position)
{
	this->position.x = position.x;
	this->position.y = -position.y;

	checkBoundary();
}

void Voxel::Cursor::updateBoundary()
{
	auto size = glm::vec2(Application::getInstance().getGLView()->getScreenSize());

	minScreenBoundary = size * -0.5f;
	maxScreenBoundary = size * 0.5f;
}

void Voxel::Cursor::setCursorType(const CursorType cursorType)
{
	auto ss = SpriteSheetManager::getInstance().getSpriteSheetByKey("CursorSpriteSheet");

	const ImageEntry* imageEntry = nullptr;

	switch (cursorType)
	{
	case Cursor::CursorType::POINTER:
		imageEntry = ss->getImageEntry("pointer.png");

		pivot = glm::vec2(-14.0f / 16.0f, 14.0f / 16.0f) * 0.5f;
		break;
	case Cursor::CursorType::FINGER:
		imageEntry = ss->getImageEntry("finger.png");

		pivot = glm::vec2(-8.0f / 16.0f, 14.0f / 16.0f) * 0.5f;
		break;
	default:
		return;
		break;
	}

	auto& uvOrigin = imageEntry->uvOrigin;
	auto& uvEnd = imageEntry->uvEnd;

	std::vector<float> uv =
	{
		uvOrigin.x, uvOrigin.y,
		uvOrigin.x, uvEnd.y,
		uvEnd.x, uvOrigin.y,
		uvEnd.x, uvEnd.y
	};

	glBindBuffer(GL_ARRAY_BUFFER, uvbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * uv.size(), &uv.front());
}

void Voxel::Cursor::setVisibility(const bool visibility)
{
	visible = visibility;
}

bool Voxel::Cursor::isVisible() const
{
	return visible;
}

glm::vec2 Voxel::Cursor::getPosition() const
{
	return position;
}

void Voxel::Cursor::render()
{
	if (visible)
	{
		if (vao)
		{
			auto program = ProgramManager::getInstance().getProgram(ProgramManager::PROGRAM_NAME::UI_TEXTURE_SHADER);
			program->use(true);
			program->setUniformMat4("projMat", Camera::mainCamera->getProjection(Camera::UIFovy));

			auto uiMat = glm::translate(glm::translate(Camera::mainCamera->getScreenSpaceMatrix(), glm::vec3(position.x, position.y, 0)), glm::vec3(-pivot.x * size.x, -pivot.y * size.y, 0));

			texture->activate(GL_TEXTURE0);
			texture->bind();

			program->setUniformMat4("modelMat", uiMat);
			program->setUniformFloat("opacity", 1.0f);
			program->setUniformVec3("color", glm::vec3(1.0f));

			glBindVertexArray(vao);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
	}
}