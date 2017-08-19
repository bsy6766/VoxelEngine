#include "UI.h"

#include <Texture2D.h>
#include <iostream>
#include <Quad.h>
#include <ProgramManager.h>
#include <Program.h>

using namespace Voxel::UI;

Image::Image()
	: visible(true)
	, texture(nullptr)
	, position(0)
{

}

Image::~Image()
{
	if (texture)
	{
		delete texture;
	}

	// Delte buffers
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &cbo);
	glDeleteBuffers(1, &ibo);
	// Delte array
	glDeleteVertexArrays(1, &vao);
}

Image* Image::create(const std::string& textureName, const glm::vec2& screenPosition)
{
	auto newImage = new Image();

	if (newImage->init(textureName, screenPosition))
	{
		return newImage;
	}
	else
	{
		delete newImage;
		return nullptr;
	}
}

void Voxel::UI::Image::render()
{
	texture->activate(GL_TEXTURE0);
	texture->bind();

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	//glBindVertexArray(0);
}

bool Voxel::UI::Image::init(const std::string& textureName, const glm::vec2& screenPosition)
{
	texture = Texture2D::create(textureName, GL_TEXTURE_2D);

	if (texture == nullptr)
	{
		return false;
	}

	position = screenPosition;

	auto size = texture->getTextureSize();

	auto vertices = Quad::getVertices(glm::vec2(size), screenPosition);
	auto indices = Quad::indices;
	auto colors = Quad::defaultColors;
	auto uv = Quad::uv;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	auto program = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM::SHADER_TEXTURE_COLOR);
	GLint vertLoc = program->getAttribLocation("vert");

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * vertices.size(), &vertices.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLint colorLoc = program->getAttribLocation("color");

	glGenBuffers(1, &cbo);
	glBindBuffer(GL_ARRAY_BUFFER, cbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors) * colors.size(), &colors.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLint uvVertLoc = program->getAttribLocation("uvVert");

	glGenBuffers(1, &uvbo);
	glBindBuffer(GL_ARRAY_BUFFER, uvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uv) * uv.size(), &uv.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(uvVertLoc);
	glVertexAttribPointer(uvVertLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) * indices.size(), &indices.front(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	return true;
}



Canvas::Canvas()
	: screenSize(0)
	, centerPosition(0)
{

}

Canvas::~Canvas()
{
	// Release all images
	for (auto image : images)
	{
		if (image.second)
		{
			delete image.second;
		}
	}

	images.clear();
}

Canvas* Canvas::create(const glm::vec2& screenSize, const glm::vec2& centerPosition)
{
	auto newCanvas = new Canvas();
	
	newCanvas->screenSize = screenSize;
	newCanvas->centerPosition = centerPosition;

	std::cout << "[Canvas] Creating new canvas" << std::endl;
	std::cout << "[Canvas] Size (" << screenSize.x << ", " << screenSize.y << ")" << std::endl;
	std::cout << "[Canvas] Center (" << centerPosition.x << ", " << centerPosition.y << ")" << std::endl;

	return newCanvas;
}

void Voxel::UI::Canvas::addImage(const std::string & name, const std::string & textureName, const glm::vec2& position)
{
	auto newImage = Image::create(textureName, position);
	if (newImage)
	{
		addImage(name, newImage, 0);
	}
}

void Voxel::UI::Canvas::addImage(const std::string & name, Image * image, const int z)
{
	if (image)
	{
		auto find_it = images.find(name);
		if (find_it == images.end())
		{
			images.emplace(name, image);
		}
		else
		{
			// Same image exists
			return;
		}
	}
}

void Voxel::UI::Canvas::render()
{
	for (auto image : images)
	{
		(image.second)->render();
	}
}
