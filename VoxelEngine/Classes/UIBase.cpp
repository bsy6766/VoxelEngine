#include "UI.h"

// cpp
#include <sstream>
#include <iostream>
#include <limits>

// voxel
#include "Texture2D.h"
#include "Quad.h"
#include "ProgramManager.h"
#include "Program.h"
#include "FontManager.h"
#include "Font.h"
#include "Camera.h"
#include "SpriteSheet.h"
#include "Application.h"
#include "Utility.h"
#include "Canvas.h"

// glm
#include <glm/gtx/transform.hpp>

using namespace Voxel::UI;

//=============================================================== Node ===============================================================

unsigned int Node::idCounter = 0;

Voxel::UI::Node::Node(const std::string & name)
	: name(name)
	, id(++Node::idCounter)
{}

Voxel::UI::Node::~Node()
{
	//std::cout << "~Node()\n";
}

unsigned int Voxel::UI::Node::getID() const
{
	return id;
}

std::string Voxel::UI::Node::getName() const
{
	return name;
}

//====================================================================================================================================

//=========================================================== Transform Node =========================================================

Voxel::UI::TransformNode::TransformNode(const std::string & name)
	: Node(name)
	, position(0.0f)
	, angle(0.0f)
	, scale(1.0f)
	, pivot(0.0f)
	, coordinateOrigin(0.0f)
	, contentSize(0.0f)
	, modelMat(1.0f)
	, sequence(nullptr)
	, zOrder()
	, boundingBox(glm::vec2(0.0), glm::vec2(0.0f))
	, needToUpdateModelMat(false)
	, parent(nullptr)
{}

Voxel::UI::TransformNode::~TransformNode()
{
	if (sequence)
	{
		delete sequence;
	}

	//std::cout << "~TransformNode()\n";
}

void Voxel::UI::TransformNode::setPosition(const float x, const float y)
{
	this->position.x = x;
	this->position.y = y;

	this->boundingBox.center = position;

	needToUpdateModelMat = true;
}

void Voxel::UI::TransformNode::setPosition(const glm::vec2 & position)
{
	setPosition(position.x, position.y);
}

glm::vec2 Voxel::UI::TransformNode::getPosition() const
{
	return position;
}

void Voxel::UI::TransformNode::setAngle(const float angle)
{
	float newAngle = angle;
	if (newAngle < 0.0f)
	{
		while (newAngle < 0.0f)
		{
			newAngle += 360.0f;
		}
	}
	else if (newAngle >= 360.0f)
	{
		while (newAngle < 360.0f)
		{
			newAngle -= 360.0f;
		}
	}

	this->angle = newAngle;

	needToUpdateModelMat = true;
}

float Voxel::UI::TransformNode::getAngle() const
{
	return angle;
}

void Voxel::UI::TransformNode::setScale(const glm::vec2 & scale)
{
	this->scale = scale;

	if (this->scale.x < 0.0f)
	{
		this->scale.x = 0.0f;
	}

	if (this->scale.y < 0.0f)
	{
		this->scale.y = 0.0f;
	}

	needToUpdateModelMat = true;
}

glm::vec2 Voxel::UI::TransformNode::getScale() const
{
	return scale;
}

void Voxel::UI::TransformNode::setPivot(const glm::vec2 & pivot)
{
	this->pivot = glm::clamp(pivot, -0.5f, 0.5f);

	needToUpdateModelMat = true;
}

glm::vec2 Voxel::UI::TransformNode::getPivot() const
{
	return pivot;
}

void Voxel::UI::TransformNode::setCoordinateOrigin(const glm::vec2 & coordinateOrigin)
{
	this->coordinateOrigin = glm::clamp(coordinateOrigin, -0.5f, 0.5f);

	needToUpdateModelMat = true;
}

glm::vec2 Voxel::UI::TransformNode::getCoordinateOrigin() const
{
	return coordinateOrigin;
}

void Voxel::UI::TransformNode::setBoundingBox(const glm::vec2 & center, const glm::vec2 & size)
{
	boundingBox.center = center;
	boundingBox.size = size;
}

Voxel::Shape::Rect Voxel::UI::TransformNode::getBoundingBox() const
{
	auto scaled = boundingBox;
	scaled.size *= scale;
	return scaled;
}

void Voxel::UI::TransformNode::setZorder(const ZOrder & zOrder)
{
	this->zOrder = zOrder;
}

Voxel::ZOrder Voxel::UI::TransformNode::getZOrder() const
{
	return zOrder;
}

bool Voxel::UI::TransformNode::addChild(TransformNode * child)
{
	if (child == nullptr) return false;
	if (auto canvasChild = dynamic_cast<Voxel::UI::Canvas*>(child))
	{
		return false;
	}

	if (children.empty())
	{
		return addChild(child, Voxel::ZOrder(0, 0));
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
			return addChild(child, Voxel::ZOrder(lastZorder.getGlobalZOrder() + 1, 0));
		}
	}

	return true;
}

bool Voxel::UI::TransformNode::addChild(Voxel::UI::TransformNode * child, int zOrder)
{
	if (auto canvasChild = dynamic_cast<Voxel::UI::Canvas*>(child))
	{
		return false;
	}

	return addChild(child, ZOrder(zOrder));
}

bool Voxel::UI::TransformNode::addChild(Voxel::UI::TransformNode * child, Voxel::ZOrder& zOrder)
{
	if (child == nullptr) return false;

	if (auto canvasChild = dynamic_cast<Voxel::UI::Canvas*>(child))
	{
		return false;
	}

	bool result = getNextZOrder(zOrder);

	if (result == false)
	{
		return false;
	}
	else
	{
		children.insert(std::make_pair(zOrder, std::unique_ptr<Voxel::UI::TransformNode>(child)));

		child->parent = this;

		// New child added. Update model matrix based on parent's model matrix
		child->updateModelMatrix();

		return true;
	}
}

bool Voxel::UI::TransformNode::getNextZOrder(Voxel::ZOrder & curZOrder)
{
	bool lookForLocal = false;
	int globalTemp = std::numeric_limits<int>::min();
	int localTemp = globalTemp;
	bool found = false;

	for (auto& e : children)
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

TransformNode * Voxel::UI::TransformNode::getChild(const std::string & name)
{
	for (auto& e : children)
	{
		if ((e.second)->name == name)
		{
			return (e.second).get();
		}
	}

	return nullptr;
}

bool Voxel::UI::TransformNode::hasChildren()
{
	return !(children.empty());
}

void Voxel::UI::TransformNode::getAllChildrenInVector(std::vector<TransformNode*>& nodes, TransformNode * parent)
{
	if (children.empty())
	{
		return;
	}
	else
	{
		std::vector<TransformNode*> negativesOrder, positiveOrder;

		for (auto& e : children)
		{
			if ((e.first).getGlobalZOrder() < 0)
			{
				if ((e.second)->hasChildren())
				{
					(e.second)->getAllChildrenInVector(negativesOrder, (e.second).get());
				}
				else
				{
					negativesOrder.push_back((e.second).get());
				}
			}
			else
			{
				if ((e.second)->hasChildren())
				{
					(e.second)->getAllChildrenInVector(positiveOrder, (e.second).get());
				}
				else
				{
					positiveOrder.push_back((e.second).get());
				}
			}
		}

		if (!negativesOrder.empty())
		{
			nodes.insert(nodes.end(), negativesOrder.begin(), negativesOrder.end());
		}

		nodes.push_back(parent);

		if (!positiveOrder.empty())
		{
			nodes.insert(nodes.end(), positiveOrder.begin(), positiveOrder.end());
		}
	}
}

void Voxel::UI::TransformNode::print(const int tab)
{
	for (int i = 0; i < tab; i++)
	{
		std::cout << "\t";
	}

	std::cout << name << "\n";
}

void Voxel::UI::TransformNode::printChildren(const int tab)
{
	std::string str = "";

	for (int i = 0; i < tab; i++)
	{
		str += "\t";
	}

	for (auto& e : children)
	{
		std::cout << str << "ZOrder: (" << e.first.globalZOrder << ", " << e.first.localZOrder << "), Name: " << e.second->getName() << "\n";
		e.second->printChildren(tab + 1);
	}
}

glm::vec2 Voxel::UI::TransformNode::getContentSize()
{
	glm::vec2 scaled = contentSize;
	scaled.x *= scale.x;
	scaled.y *= scale.y;

	return scaled;
}

glm::mat4 Voxel::UI::TransformNode::getModelMatrix()
{
	auto mat = glm::translate(glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 0, 1)), glm::vec3(position, 0));
	if (pivot.x != 0.0f || pivot.y != 0.0f)
	{
		mat = glm::translate(mat, glm::vec3(pivot * getContentSize() * -1.0f, 0));
	}

	return glm::scale(mat, glm::vec3(scale, 1));
}

void Voxel::UI::TransformNode::updateModelMatrix()
{
	// Update model matrix
	if (parent)
	{
		modelMat = parent->modelMat * glm::translate(glm::mat4(1.0f), glm::vec3(parent->getContentSize() * getCoordinateOrigin(), 0.0f)) * getModelMatrix();
	}
	else
	{
		modelMat = getModelMatrix();
	}

	boundingBox.center = glm::vec2(modelMat * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	// Check if this node has children
	if (hasChildren())
	{
		// Update model matrix for children as well
		for (auto& e : children)
		{
			(e.second)->updateModelMatrix();
		}
	}
}

void Voxel::UI::TransformNode::update(const float delta)
{
	if (sequence)
	{
		needToUpdateModelMat = true;
	}


	/*
	if (needToUpdateModelMat)
	{
	updateModelMatrix();
	needToUpdateModelMat = false;
	}
	*/

	for (auto& e : children)
	{
		(e.second)->update(delta);
	}
}

void Voxel::UI::TransformNode::updateMouseMove(const glm::vec2 & mousePosition)
{

}

void Voxel::UI::TransformNode::updateMouseClick(const glm::vec2 & mousePosition, const int button)
{

}

void Voxel::UI::TransformNode::updateMouseRelease(const glm::vec2 & mousePosition, const int button)
{
}

void Voxel::UI::TransformNode::runAction(Voxel::UI::Sequence * sequence)
{
	if (sequence)
	{
		delete sequence;
	}

	this->sequence = sequence;
}

//====================================================================================================================================

//======================================================= RenderNode =================================================================

Voxel::UI::RenderNode::RenderNode(const std::string & name)
	: TransformNode(name)
	, visibility(true)
	, opacity(1.0f)
	, program(nullptr)
	, vao(0)
	, texture(nullptr)
	, color(1.0f)
#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX
	, bbVao(0)
#endif
{}

Voxel::UI::RenderNode::~RenderNode()
{
	std::cout << "~RenderNode() " + name + "\n";
	if (vao)
	{
		// Delte array
		glDeleteVertexArrays(1, &vao);
	}

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX
	if (bbVao)
	{
		// Delte array
		glDeleteVertexArrays(1, &bbVao);
	}
#endif
}

void Voxel::UI::RenderNode::setVisibility(const bool visibility)
{
	this->visibility = visibility;
}

bool Voxel::UI::RenderNode::getVisibility() const
{
	return visibility;
}

void Voxel::UI::RenderNode::setOpacity(const float opacity)
{
	this->opacity = glm::clamp(opacity, 0.0f, 1.0f);
}

float Voxel::UI::RenderNode::getOpacity() const
{
	return opacity;
}

void Voxel::UI::RenderNode::render()
{
	if (children.empty())
	{
		renderSelf();
	}
	else
	{
		auto children_it = children.begin();
		auto beginZOrder = ((children_it)->first).getGlobalZOrder();

		if (beginZOrder < 0)
		{
			// has negative ordered children
			for (; ((children_it)->first).getGlobalZOrder() < 0; children_it++)
			{
				((children_it)->second)->render();
			}
		}
		// else, doesn't have negative ordered children

		// Render self
		renderSelf();

		// Render positive 
		for (; children_it != children.end(); children_it++)
		{
			((children_it)->second)->render();
		}
	}
}

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX
void Voxel::UI::RenderNode::createDebugBoundingBoxLine()
{
	if (bbVao)
	{
		glDeleteVertexArrays(1, &bbVao);
		bbVao = 0;
	}

	glGenVertexArrays(1, &bbVao);
	glBindVertexArray(bbVao);

	auto min = -boundingBox.size * 0.5f;
	auto max = boundingBox.size * 0.5f;

	std::vector<float> lineVertices =
	{
		min.x, min.y, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		min.x, max.y, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,

		min.x, max.y, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		max.x, max.y, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,

		max.x, max.y, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		max.x, min.y, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,

		max.x, min.y, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		min.x, min.y, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	};

	GLuint lineVbo;
	glGenBuffers(1, &lineVbo);
	glBindBuffer(GL_ARRAY_BUFFER, lineVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * lineVertices.size(), &lineVertices.front(), GL_STATIC_DRAW);

	auto lineProgram = ProgramManager::getInstance().getProgram(Voxel::ProgramManager::PROGRAM_NAME::LINE_SHADER);
	GLint lineVertLoc = lineProgram->getAttribLocation("vert");

	glEnableVertexAttribArray(lineVertLoc);
	glVertexAttribPointer(lineVertLoc, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), nullptr);

	GLint lineColorLoc = lineProgram->getAttribLocation("color");

	glEnableVertexAttribArray(lineColorLoc);
	glVertexAttribPointer(lineColorLoc, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));

	glBindVertexArray(0);

	glDeleteBuffers(1, &lineVbo);
}
#endif
//====================================================================================================================================