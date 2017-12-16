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
	contentSize = size;

	setInteractable();

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

bool Voxel::UI::Canvas::updateMouseMove(const glm::vec2 & mousePosition, const glm::vec2& mouseDelta)
{
	if (!isInteractable()) return false;

	if (visibility)
	{
		bool moved = false;
		for (auto& e : children)
		{
			bool result = (e.second)->updateMouseMove(mousePosition, mouseDelta);
			if (result)
			{
				moved = true;
			}
		}

		return moved;
	}
	else
	{
		return false;
	}
}

bool Voxel::UI::Canvas::updateMousePress(const glm::vec2 & mousePosition, const int button)
{
	if (!isInteractable()) return false;

	if (visibility)
	{
		bool clicked = false;
		for (auto& e : children)
		{
			bool result = (e.second)->updateMousePress(mousePosition, button);
			if (result)
			{
				clicked = true;
			}
		}

		return clicked;
	}
	else
	{
		return false;
	}
}

bool Voxel::UI::Canvas::updateMouseRelease(const glm::vec2 & mousePosition, const int button)
{
	if (!isInteractable()) return false;

	if (visibility)
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
	else
	{
		return false;
	}
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