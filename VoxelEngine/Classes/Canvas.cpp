#include "Canvas.h"

// cpp
#include <iostream>

// Voxel
#include "Camera.h"

Voxel::UI::Canvas::Canvas(const glm::vec2& size, const glm::vec2& centerPosition)
	: TransformNode("")
	, size(size)
	, centerPosition(centerPosition)
	, visibility(true)
{
	std::cout << "[Canvas] Creating new canvas\n";
	std::cout << "[Canvas] Size (" << size.x << ", " << size.y << ")\n";
	std::cout << "[Canvas] Center (" << centerPosition.x << ", " << centerPosition.y << ")\n";

	contentSize = size;

	updateModelMatrix();
}

void Voxel::UI::Canvas::setVisibility(const bool visibility)
{
	this->visibility = visibility;
}

bool Voxel::UI::Canvas::getVisibility() const
{
	return visibility;
}

void Voxel::UI::Canvas::setSize(const glm::vec2 & size)
{
	this->size = size;
}

void Voxel::UI::Canvas::updateModelMatrix()
{
	modelMat = Camera::mainCamera->getScreenSpaceMatrix() * Voxel::UI::TransformNode::getModelMatrix();
}

glm::mat4 Voxel::UI::Canvas::getModelMatrix()
{
	return modelMat;
}

void Voxel::UI::Canvas::update(const float delta)
{
	for (auto& e : children)
	{
		(e.second)->update(delta);
	}
}

void Voxel::UI::Canvas::updateMouseMove(const glm::vec2 & mousePosition)
{
	for (auto& e : children)
	{
		(e.second)->updateMouseMove(mousePosition);
	}
}

bool Voxel::UI::Canvas::updateMouseClick(const glm::vec2 & mousePosition, const int button)
{
	bool clicked = false;
	for (auto& e : children)
	{
		bool result = (e.second)->updateMouseClick(mousePosition, button);
		if (result)
		{
			clicked = true;
		}
	}

	return clicked;
}

bool Voxel::UI::Canvas::updateMouseRelease(const glm::vec2 & mousePosition, const int button)
{
	bool released = false;
	for (auto& e : children)
	{
		bool result = (e.second)->updateMouseRelease(mousePosition, button);
		if (result)
		{
			released = true;
		}
	}

	return released;
}

void Voxel::UI::Canvas::render()
{
	if (!visibility) return;

	// Iterate children
	for (auto& e : children)
	{
		// Multiply model matrix with screen space
		(e.second)->render();
	}
}

void Voxel::UI::Canvas::print(const int tab)
{
	std::cout << "[Canvas] Canvas info...\n";
	std::cout << "Size (" << size.x << ", " << size.y << ")\n";
	std::cout << "Pos  (" << position.x << ", " << position.y << ")\n";

	std::cout << "UIs\n";

	for (auto& e : children)
	{
		std::cout << "ZOrder: (" << e.first.globalZOrder << ", " << e.first.localZOrder << "), Name: " << e.second->getName() << "\n";
		if (e.second->hasChildren())
		{
			e.second->printChildren(1);
		}
	}

	std::cout << "\n";
}