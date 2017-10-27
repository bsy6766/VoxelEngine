#include "UI.h"

#include <Texture2D.h>
#include <sstream>
#include <iostream>
#include <Quad.h>
#include <ProgramManager.h>
#include <Program.h>
#include <FontManager.h>
#include <Font.h>
#include <Camera.h>
#include <SpriteSheet.h>
#include <Application.h>
#include <glm/gtx/transform.hpp>

using namespace Voxel::UI;

Voxel::UI::UINode::UINode(const std::string& name)
	: pivot(glm::vec2(0))
	, name(name)
	//, canvasPivot(glm::vec2(0))
	, position(0)
	, scale(1)
	, modelMatrix(1.0f)
	, visible(true)
	, size(0)
	, opacity(0)
{
}

void Voxel::UI::UINode::updateMatrix()
{
	// Move to pos, move by pivot, then scale
	modelMatrix = glm::scale(glm::translate(glm::translate(glm::mat4(1.0f), glm::vec3(position, 0)), glm::vec3(pivot * size * scale * -1.0f, 0)), glm::vec3(scale, 1));
}

/*
void Voxel::UI::UINode::setCanvasPivot(const glm::vec2 & pivot)
{
	canvasPivot = pivot;
}

glm::vec2 Voxel::UI::UINode::getCanvasPivot()
{
	return canvasPivot;
}
*/

bool Voxel::UI::UINode::getNextZOrder(ZOrder & curZOrder)
{
	bool lookForLocal = false;
	int globalTemp = std::numeric_limits<int>::min();
	int localTemp = globalTemp;
	
	for (auto& e : children)
	{
		if (!lookForLocal)
		{
			if (e.first.getGlobalZOrder() == curZOrder.getGlobalZOrder())
			{
				lookForLocal = true;
				globalTemp = e.first.getGlobalZOrder();

				if (e.first.isLocalZOrderMaxInt())
				{
					return false;
				}
				else
				{
					continue;
				}
			}
		}
		else
		{
			localTemp = e.first.getLocalZOrder();

			if (e.first.getGlobalZOrder() != globalTemp)
			{
				if (localTemp == std::numeric_limits<int>::max())
				{
					return false;
				}
				else
				{
					curZOrder.globalZOrder = globalTemp;
					curZOrder.localZOrder = localTemp + 1;
					break;
				}
			}
			else
			{
				continue;
			}
		}
	}

	return true;
	/*
	if (found)
	{
		return true;
	}
	else
	{
		auto& lastZorder = children.rbegin()->first;

		if (lastZorder.isGlobalZOrderMaxInt())
		{
			return false;
		}
		else
		{
			curZOrder.globalZOrder = lastZorder.globalZOrder + 1;
			curZOrder.localZOrder = 0;
			return true;
		}
	}
	*/
}

bool Voxel::UI::UINode::addChild(UINode * child)
{
	if (child == nullptr) return false;

	if (children.empty())
	{
		return addChild(child, ZOrder(0, 0));
	}
	else
	{
		auto& lastZorder = children.rbegin()->first;

		if (lastZorder.isGlobalZOrderMaxInt())
		{
			return false;
		}
		else
		{
			return addChild(child, ZOrder(lastZorder.getGlobalZOrder() + 1, 0));
		}
	}

	return true;
}

bool Voxel::UI::UINode::addChild(UINode * child, ZOrder& zOrder)
{
	if (child == nullptr) return false;

	bool result = getNextZOrder(zOrder);

	if (result == false)
	{
		return false;
	}
	else
	{
		children.insert(std::make_pair(zOrder, std::unique_ptr<UINode>(child)));
	}
}

void Voxel::UI::UINode::setScale(const glm::vec2 & scale)
{
	this->scale = scale;
	updateMatrix();
}

void Voxel::UI::UINode::addScale(const glm::vec2 & scale)
{
	this->scale += scale;
	updateMatrix();
}

void Voxel::UI::UINode::setPosition(const glm::vec2 & position)
{
	this->position = position;
	updateMatrix();
}

void Voxel::UI::UINode::addPosition(const glm::vec2 & position)
{
	this->position += position;
	updateMatrix();
}

glm::vec2 Voxel::UI::UINode::getPosition()
{
	return position;
}

void Voxel::UI::UINode::setPivot(const glm::vec2 & pivot)
{
	this->pivot = pivot;
	updateMatrix();
}

void Voxel::UI::UINode::setVisibility(const bool visibility)
{
	visible = visibility;
}

bool Voxel::UI::UINode::isVisible()
{
	return visible;
}

void Voxel::UI::UINode::setSize(const glm::vec2 & size)
{
	this->size = size;
	this->updateMatrix();
}

glm::vec2 Voxel::UI::UINode::getSize()
{
	return size;
}

void Voxel::UI::UINode::setOpacity(const float opacity)
{
	if (opacity < 0) this->opacity = 0;
	else if (opacity > 1.0f) this->opacity = 1.0f;
	else this->opacity = opacity;
}

float Voxel::UI::UINode::getOpacity()
{
	return opacity;
}

void Voxel::UI::UINode::setName(const std::string & name)
{
	if (!name.empty())
	{
		this->name = name;
	}
}

std::string Voxel::UI::UINode::getName()
{
	return name;
}

glm::mat4 Voxel::UI::UINode::getModelMatrix()
{
	return modelMatrix;
}

Voxel::Geometry::Rect Voxel::UI::UINode::getBoundingBox()
{
	glm::vec4 screenPos = modelMatrix * glm::vec4(position, 1.0f, 1.0f);

	return Geometry::Rect(glm::vec2(screenPos.x, screenPos.y), size);
}

bool Voxel::UI::UINode::hasChildren()
{
	return !children.empty();
}

void Voxel::UI::UINode::printChildren()
{
	std::cout << "UI Node (" << name << ")'s children...";
	for (auto& e : children)
	{
		e.second->printChildren();
	}
}

//============================================================================================

Voxel::UI::UIBatch::UIBatch()
	: vao(0)
	, indicesSize(0)
	, texture(nullptr)
	, program(nullptr)
{}

void Voxel::UI::UIBatch::render()
{
}

//============================================================================================

Canvas::Canvas()
	: size(0)
	, position(0)
	, visible(true)
{}

Voxel::UI::Canvas::~Canvas()
{
	// automatically released
	uiNodes.clear();
}

Canvas * Voxel::UI::Canvas::create(const glm::vec2 & size, const glm::vec2 & position)
{
	auto newCanvas = new Canvas();

	newCanvas->size = size;
	newCanvas->position = position;

	std::cout << "[Canvas] Creating new canvas\n";
	std::cout << "[Canvas] Size (" << size.x << ", " << size.y << ")\n";
	std::cout << "[Canvas] Pos  (" << position.x << ", " << position.y << ")\n";

	return newCanvas;
}

bool Voxel::UI::Canvas::getNextZOrder(ZOrder & curZOrder)
{
	bool lookForLocal = false;
	int globalTemp = std::numeric_limits<int>::min();
	int localTemp = globalTemp;
	bool found = false;
	
	for (auto& e : uiNodes)
	{
		if (!lookForLocal)
		{
			if (e.first.getGlobalZOrder() == curZOrder.getGlobalZOrder())
			{
				lookForLocal = true;
				globalTemp = e.first.getGlobalZOrder();
				localTemp = e.first.getLocalZOrder();

				if (e.first.isLocalZOrderMaxInt())
				{
					return false;
				}
				else
				{
					continue;
				}
			}
		}
		else
		{
			localTemp = e.first.getLocalZOrder();

			if (e.first.getGlobalZOrder() != globalTemp)
			{
				if (localTemp == std::numeric_limits<int>::max())
				{
					return false;
				}
				else
				{
					found = true;
					curZOrder.globalZOrder = globalTemp;
					curZOrder.localZOrder = localTemp + 1;
					break;
				}
			}
			else
			{
				continue;
			}
		}
	}

	if (lookForLocal)
	{
		if (found)
		{
			return true;
		}
		else
		{
			if (localTemp == std::numeric_limits<int>::max())
			{
				return false;
			}
			else
			{
				curZOrder.globalZOrder = globalTemp;
				curZOrder.localZOrder = localTemp + 1;
				return true;
			}
		}
	}

	return true;
}

bool Voxel::UI::Canvas::addNode(UINode * node)
{
	if (node == nullptr) return false;

	if (uiNodes.empty())
	{
		return addNode(node, ZOrder(0, 0));
	}
	else
	{
		auto& lastZorder = uiNodes.rbegin()->first;

		if (lastZorder.isGlobalZOrderMaxInt())
		{
			return false;
		}
		else
		{
			return addNode(node, ZOrder(lastZorder.getGlobalZOrder() + 1, 0));
		}
	}

	return true;
}

bool Voxel::UI::Canvas::addNode(UINode * node, ZOrder zOrder)
{
	if (node == nullptr) return false;

	bool result = getNextZOrder(zOrder);

	if (result == false)
	{
		return false;
	}
	else
	{
		uiNodes.insert(std::make_pair(zOrder, std::unique_ptr<UINode>(node)));
	}

	return true;
}

void Voxel::UI::Canvas::print()
{
	std::cout << "[Canvas] Canvas info...\n";
	std::cout << "\tSize (" << size.x << ", " << size.y << ")\n";
	std::cout << "\tPos  (" << position.x << ", " << position.y << ")\n";

	std::cout << "\tUIs\n";

	for (auto& e : uiNodes)
	{
		std::cout << "ZOrder: (" << e.first.globalZOrder << ", " << e.first.localZOrder << "), Name: " << e.second->getName() << "\n";
		if (e.second->hasChildren())
		{
			e.second->printChildren();
		}
	}

	std::cout << "\n";
}
