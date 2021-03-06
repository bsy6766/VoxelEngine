// pch
#include "Precompiled.h"

#include "TransformNode.h"

// voxel
#include "Texture2D.h"
#include "ProgramManager.h"
#include "Program.h"
#include "FontManager.h"
#include "Font.h"
#include "Camera.h"
#include "SpriteSheet.h"
#include "Application.h"
#include "Utility.h"
#include "Canvas.h"
#include "UIActions.h"
#include "Logger.h"

Voxel::UI::TransformNode::TransformNode(const std::string & name)
	: BaseNode(name)
	, visibility(true)
	, opacity(1.0f)
	, position(0.0f)
	, angle(0.0f)
	, scale(1.0f)
	, pivot(0.0f)
	, coordinateOrigin(0.0f)
	, contentSize(0.0f)
	, modelMat(1.0f)
	, zOrder()
	, boundingBox(glm::vec2(0.0), glm::vec2(0.0f))
	, needToUpdateModelMat(false)
	, parent(nullptr)
	, interaction(0)
	, actionPaused(false)
	, onMousePressed(nullptr)
	, onMouseReleased(nullptr)
	, onMouseEnter(nullptr)
	, onMouseExit(nullptr)
	, onMouseMove(nullptr)
#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX
	, bbVao(0)
#endif
{}

Voxel::UI::TransformNode::~TransformNode()
{
	std::cout << "~TransformNode() " << name << "\n";

	for (auto action : actions)
	{
		if (action)
		{
			delete action;
		}
	}

	actions.clear();

	for (auto& e : children)
	{
		if (e.second)
		{
			delete e.second;
		}
	}
}

void Voxel::UI::TransformNode::setVisibility(const bool visibility)
{
	this->visibility = visibility;
}

bool Voxel::UI::TransformNode::getVisibility() const
{
	return visibility;
}

void Voxel::UI::TransformNode::setOpacity(const float opacity)
{
	this->opacity = glm::clamp(opacity, 0.0f, 1.0f);
}

float Voxel::UI::TransformNode::getOpacity() const
{
	return opacity;
}

void Voxel::UI::TransformNode::setPosition(const float x, const float y)
{
	position.x = x;
	position.y = y;

	needToUpdateModelMat = true;
}

void Voxel::UI::TransformNode::setPosition(const float x, const float y, const bool updateModelMatirx)
{
	position.x = x;
	position.y = y;

	needToUpdateModelMat = updateModelMatirx;
}

void Voxel::UI::TransformNode::setPosition(const glm::vec2 & position)
{
	setPosition(position.x, position.y);
}

void Voxel::UI::TransformNode::setPosition(const glm::vec2 & position, const bool updateModelMatirx)
{
	setPosition(position.x, position.y, updateModelMatirx);
}

void Voxel::UI::TransformNode::addPosition(const glm::vec2 & delta)
{
	position += delta;

	needToUpdateModelMat = true;
}

glm::vec2 Voxel::UI::TransformNode::getPosition() const
{
	return position;
}

void Voxel::UI::TransformNode::setAngle(const float angle)
{
	/*
	float newAngle = angle;

	if (newAngle > 360.0f)
	{
	while (newAngle > 360.0f)
	{
	newAngle -= 360.0f;
	}
	}
	else if (newAngle <= -360.0f)
	{
	while (newAngle <= -360.0f)
	{
	newAngle += 360.0f;
	}
	}
	*/

	this->angle = angle;

	needToUpdateModelMat = true;
}

float Voxel::UI::TransformNode::getAngle() const
{
	return angle;
}

void Voxel::UI::TransformNode::setScale(const float scale)
{
	setScale(scale, scale);
}

void Voxel::UI::TransformNode::setScaleX(const float scaleX)
{
	setScale(glm::vec2(scaleX, scale.y));
}

void Voxel::UI::TransformNode::setScaleY(const float scaleY)
{
	setScale(glm::vec2(scale.x, scaleY));
}

void Voxel::UI::TransformNode::setScale(const glm::vec2 & scale)
{
	setScale(scale.x, scale.y);
}

void Voxel::UI::TransformNode::setScale(const float x, const float y)
{
	scale.x = x;
	scale.y = y;

	needToUpdateModelMat = true;
}

glm::vec2 Voxel::UI::TransformNode::getScale() const
{
	return scale;
}

void Voxel::UI::TransformNode::setPivot(const glm::vec2 & pivot)
{
	setPivot(pivot.x, pivot.y);
}

void Voxel::UI::TransformNode::setPivot(const float x, const float y)
{
	this->pivot.x = glm::clamp(x, -0.5f, 0.5f);
	this->pivot.y = glm::clamp(y, -0.5f, 0.5f);

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

void Voxel::UI::TransformNode::updateBoundingBox()
{
	if (parent)
	{
		updateBoundingBox(getParentMatrix());
	}
}

void Voxel::UI::TransformNode::updateBoundingBox(const glm::mat4 & parentMatrix)
{
	// todo: optimize? Also fix bounding box for rotating object, especially nested uis.
	// For now, disable rotation. Because, I don't think here is a case to check UI bounding box that is rotated or rotating.
	auto screenPos = glm::vec2(parentMatrix * glm::vec4(position, 1.0f, 1.0f));
	auto shiftPos = screenPos + (pivot * contentSize * scale * -1.0f);

	auto r = glm::radians(angle);
	auto cos = glm::cos(r);
	auto sin = glm::sin(r);

	/*
	auto rotPos = glm::vec2(0);

	rotPos.x = screenPos.x + ((shiftPos.x - screenPos.x) * cos) + ((shiftPos.y - screenPos.y) * sin);
	rotPos.y = screenPos.y + ((shiftPos.x - screenPos.x) * sin) + ((shiftPos.y - screenPos.y) * cos);

	boundingBox.center = rotPos;

	auto originalBB = Voxel::Shape::Rect(rotPos, contentSize * scale);
	*/

	boundingBox.center = shiftPos;

	auto originalBB = Voxel::Shape::Rect(shiftPos, contentSize * scale);

	auto p1 = originalBB.getMin();
	auto p4 = originalBB.getMax();
	auto p2 = glm::vec2(p1.x, p4.y);
	auto p3 = glm::vec2(p4.x, p1.y);

	std::array<glm::vec2, 4> transformed;

	transformed.at(0).x = ((p1.x - shiftPos.x) * cos) + ((p1.y - shiftPos.y) * sin);
	transformed.at(0).y = ((p1.x - shiftPos.x) * sin) + ((p1.y - shiftPos.y) * cos);

	transformed.at(1).x = ((p2.x - shiftPos.x) * cos) + ((p2.y - shiftPos.y) * sin);
	transformed.at(1).y = ((p2.x - shiftPos.x) * sin) + ((p2.y - shiftPos.y) * cos);

	transformed.at(2).x = ((p3.x - shiftPos.x) * cos) + ((p3.y - shiftPos.y) * sin);
	transformed.at(2).y = ((p3.x - shiftPos.x) * sin) + ((p3.y - shiftPos.y) * cos);

	transformed.at(3).x = ((p4.x - shiftPos.x) * cos) + ((p4.y - shiftPos.y) * sin);
	transformed.at(3).y = ((p4.x - shiftPos.x) * sin) + ((p4.y - shiftPos.y) * cos);

	glm::vec2 min(std::numeric_limits<float>::max()), max(std::numeric_limits<float>::min());

	for (auto& t : transformed)
	{
		min.x = glm::min(t.x, min.x);
		min.y = glm::min(t.y, min.y);

		max.x = glm::max(t.x, max.x);
		max.y = glm::max(t.y, max.y);
	}

	auto size = max - min;

	boundingBox.size = size;

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX
	createDebugBoundingBoxLine();
#endif
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

void Voxel::UI::TransformNode::setInteractable()
{
	interaction |= InteractionFlag::INTERACTABLE;
}

void Voxel::UI::TransformNode::setNonInteractable()
{
	interaction &= ~(InteractionFlag::INTERACTABLE);
}

void Voxel::UI::TransformNode::setDraggable()
{
	interaction |= InteractionFlag::DRAGGABLE;
}

void Voxel::UI::TransformNode::setUndraggable()
{
	interaction &= ~(InteractionFlag::DRAGGABLE);
}

bool Voxel::UI::TransformNode::isInteractable() const
{
	return interaction & InteractionFlag::INTERACTABLE;
}

bool Voxel::UI::TransformNode::isDraggable() const
{
	return interaction & InteractionFlag::DRAGGABLE;
}

bool Voxel::UI::TransformNode::equals(TransformNode * other)
{
	if (other)
	{
		return (id == other->getID() && name == other->getName());
	}

	return false;
}

bool Voxel::UI::TransformNode::addChild(TransformNode * child)
{
	if (child == nullptr) return false;
	if (equals(child)) return false;

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
	if (child == nullptr) return false;
	if (equals(child)) return false;

	if (auto canvasChild = dynamic_cast<Voxel::UI::Canvas*>(child))
	{
		return false;
	}

	return addChild(child, ZOrder(zOrder));
}

bool Voxel::UI::TransformNode::addChild(Voxel::UI::TransformNode * child, Voxel::ZOrder& zOrder)
{
	if (child == nullptr) return false;
	if (equals(child)) return false;

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
		children.insert(std::make_pair(zOrder, child));

		child->parent = this;

		// New child added. Update model matrix based on parent's model matrix
		child->updateModelMatrix();
		child->updateBoundingBox();

		return true;
	}
}

bool Voxel::UI::TransformNode::removeChild(const std::string & name, const bool releaseChild)
{
	auto it = children.begin();
	for (; it != children.end();)
	{
		if ((it->second)->getName() == name)
		{
			if (releaseChild)
			{
				if (it->second)
				{
					delete (it->second);
				}
			}

			children.erase(it);

			return true;
		}
	}

	return false;
}

bool Voxel::UI::TransformNode::removeChild(const unsigned int id, const bool releaseChild)
{
	auto it = children.begin();
	for (; it != children.end();)
	{
		if ((it->second)->getID() == id)
		{
			if (releaseChild)
			{
				if (it->second)
				{
					delete (it->second);
				}
			}

			children.erase(it);

			return true;
		}
	}

	return false;
}

bool Voxel::UI::TransformNode::removeChild(TransformNode * child, const bool releaseChild)
{
	auto zOrder = child->getZOrder();

	auto find_it = children.find(zOrder);
	if (find_it == children.end())
	{
		return removeChild(child->getID());
	}
	else
	{
		if (releaseChild)
		{
			if ((find_it->second))
			{
				delete (find_it->second);
			}
		}

		children.erase(find_it);

		return true;
	}
}

void Voxel::UI::TransformNode::setParent(TransformNode * parent)
{
	if (parent)
	{
		// set parent
		if (this->parent)
		{
			// parent exists. remove from children
			this->parent->removeChild(this);
			// set to nullptr
			this->parent = nullptr;
		}

		// add to new parent
		parent->addChild(this);
	}
}

void Voxel::UI::TransformNode::removeParent()
{
	// remove parent.
	if (this->parent)
	{
		// parent exists. remove from children
		this->parent->removeChild(this);
		// set to nullptr
		this->parent = nullptr;
	}
}

bool Voxel::UI::TransformNode::getNextZOrder(Voxel::ZOrder & curZOrder)
{
	auto it = children.begin();

	bool hasUIWithSameGlobalZorder = false;

	for (; it != children.end();)
	{
		if ((it->first).getGlobalZOrder() == curZOrder.getGlobalZOrder())
		{
			// there is ui with same global z order
			hasUIWithSameGlobalZorder = true;
			break;
		}

		it++;
	}

	if (hasUIWithSameGlobalZorder)
	{
		int targetZOrder = (it->first).getGlobalZOrder();
		int curLocalZOrder = (it->first).getLocalZOrder();

		it++;

		for (; it != children.end();)
		{
			if ((it->first).getGlobalZOrder() > targetZOrder)
			{
				break;
			}

			curLocalZOrder = (it->first).getLocalZOrder();

			it++;
		}

		curZOrder.globalZOrder = targetZOrder;
		curZOrder.localZOrder = curLocalZOrder + 1;

		return true;
	}
	else
	{
		// there wasn't any ui with same z order
		curZOrder.localZOrder = 0;
		return true;
	}
}

Voxel::UI::TransformNode * Voxel::UI::TransformNode::getChild(const std::string & name)
{
	for (auto& e : children)
	{
		if ((e.second)->name == name)
		{
			return (e.second);
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
					(e.second)->getAllChildrenInVector(negativesOrder, (e.second));
				}
				else
				{
					negativesOrder.push_back((e.second));
				}
			}
			else
			{
				if ((e.second)->hasChildren())
				{
					(e.second)->getAllChildrenInVector(positiveOrder, (e.second));
				}
				else
				{
					positiveOrder.push_back((e.second));
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

void Voxel::UI::TransformNode::stopAllActions()
{
	for (auto action : actions)
	{
		if (action)
		{
			delete action;
		}
	}

	actions.clear();
}

glm::vec2 Voxel::UI::TransformNode::getContentSize() const
{
	return contentSize;
}

glm::mat4 Voxel::UI::TransformNode::getParentMatrix() const
{
	if (parent)
	{
		return parent->modelMat * glm::translate(glm::mat4(1.0f), glm::vec3(parent->getContentSize() * parent->getScale() * getCoordinateOrigin(), 0.0f));
	}
	else
	{
		return glm::mat4(1.0f);
	}
}

glm::mat4 Voxel::UI::TransformNode::getModelMatrix()
{
	auto mat = glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(position, 0)), glm::radians(angle), glm::vec3(0.0f, 0.0f, -1.0f));

	if (pivot.x != 0.0f || pivot.y != 0.0f)
	{
		mat = glm::translate(mat, glm::vec3(pivot * getContentSize() * scale * -1.0f, 0));
	}

	return mat;
}

void Voxel::UI::TransformNode::updateMouseMoveFalse()
{
	if (visibility)
	{
		if (!children.empty())
		{
			bool moved = false;
			for (auto& child : children)
			{
				(child.second)->updateMouseMoveFalse();
			}
		}
	}
}

void Voxel::UI::TransformNode::updateModelMatrix()
{
	// Update model matrix
	if (parent)
	{
		modelMat = getParentMatrix() * getModelMatrix();
	}
	else
	{
		modelMat = getModelMatrix();
	}

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

void Voxel::UI::TransformNode::updateModelMatrix(const glm::mat4 & parentMatrix)
{
	modelMat = parentMatrix * getModelMatrix();

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
	if (!actionPaused)
	{
		if (!actions.empty())
		{
			auto it = actions.begin();
			for (; it != actions.end();)
			{
				if ((*it)->isDone())
				{
					// action is done. remove it
					delete (*it);
					it = actions.erase(it);
				}
				else
				{
					// update action
					(*it)->update(delta);

					// check time
					auto et = (*it)->getExceededTime();

					int threshold = 10;

					while (et > 0.0f)
					{
						(*it)->update(et);
						et = (*it)->getExceededTime();

						threshold--;

						if (threshold <= 0)
						{
							et = 0.0f;
						}
					}

					++it;
				}
			}
		}
	}

	if (needToUpdateModelMat)
	{
		updateModelMatrix();
		updateBoundingBox();
		needToUpdateModelMat = false;
	}

	for (auto& e : children)
	{
		(e.second)->update(delta);
	}
}

bool Voxel::UI::TransformNode::updateKeyboardInput(const std::string & str)
{
	if (visibility)
	{
		if (!children.empty())
		{
			bool moved = false;
			for (auto& child : children)
			{
				bool result = (child.second)->updateKeyboardInput(str);
				if (result)
				{
					moved = true;
				}
			}

			return moved;
		}
	}

	return false;
}

bool Voxel::UI::TransformNode::updateMouseMove(const glm::vec2 & mousePosition, const glm::vec2& mouseDelta)
{
	if (visibility)
	{
		if (!children.empty())
		{
			bool moved = false;
			for (auto& child : children)
			{
				bool result = (child.second)->updateMouseMove(mousePosition, mouseDelta);
				if (result)
				{
					moved = true;
				}
			}

			return moved;
		}
	}

	return false;
}

bool Voxel::UI::TransformNode::updateMousePress(const glm::vec2 & mousePosition, const int button)
{
	if (visibility)
	{
		if (!children.empty())
		{
			bool clicked = false;
			for (auto& child : children)
			{
				bool result = (child.second)->updateMousePress(mousePosition, button);
				if (result)
				{
					clicked = true;
				}
			}

			return clicked;
		}
	}

	return false;
}

bool Voxel::UI::TransformNode::updateMouseRelease(const glm::vec2 & mousePosition, const int button)
{
	if (visibility)
	{
		if (!children.empty())
		{
			bool clicked = false;
			for (auto& child : children)
			{
				bool result = (child.second)->updateMouseRelease(mousePosition, button);
				if (result)
				{
					clicked = true;
				}
			}

			return clicked;
		}
	}

	return false;
}

void Voxel::UI::TransformNode::updateBoundary(const Voxel::Shape::Rect& canvasBoundary)
{
	if (boundingBox.doesIntersectsWith(canvasBoundary))
	{
		auto iRect = boundingBox.getIntersectingRect(canvasBoundary);

		if (boundingBox.size.x > iRect.size.x)
		{
			//if(boundingBox.cent)
		}
	}
	else
	{
		// UI completely out of canvas boundary. Move to neareast position.
	}
}

void Voxel::UI::TransformNode::setOnMousePressedCallback(const std::function<void(TransformNode*, const int)>& func)
{
	onMousePressed = func;
}

void Voxel::UI::TransformNode::setOnMouseReleasedCallback(const std::function<void(TransformNode*, const int)>& func)
{
	onMouseReleased = func;
}

void Voxel::UI::TransformNode::setOnMouseEnterCallback(const std::function<void(TransformNode*)>& func)
{
	onMouseEnter = func;
}

void Voxel::UI::TransformNode::setOnMouseExitCallback(const std::function<void(TransformNode*)>& func)
{
	onMouseExit = func;
}

void Voxel::UI::TransformNode::setOnMouseMoveCallback(const std::function<void(TransformNode*)>& func)
{
	onMouseMove = func;
}

void Voxel::UI::TransformNode::runAction(Voxel::UI::Action * action)
{
	if (action)
	{
		actions.push_back(action);
		actions.back()->setTarget(this);
	}
}

void Voxel::UI::TransformNode::pauseAction()
{
	actionPaused = true;
}

void Voxel::UI::TransformNode::resumeAction()
{
	actionPaused = false;
}

void Voxel::UI::TransformNode::restartAllActions()
{
	for (auto action : actions)
	{
		if (action)
		{
			action->reset();
		}
	}
}

#if V_DEBUG
#if V_DEBUG_PRINT
void Voxel::UI::TransformNode::printChildren(const int tab)
{
#if V_DEBUG_LOG_CONSOLE
	auto logger = &Voxel::Logger::getInstance();

	std::string tabStr = "";

	for (int i = 0; i < tab; i++)
	{
		tabStr += "\t";
	}

	for (auto& e : children)
	{
		logger->consoleInfo(tabStr + "ZOrder (" + std::to_string(e.first.globalZOrder) + ", " + std::to_string(e.first.localZOrder) + "), Name: " + e.second->getName());

		e.second->printChildren(tab + 1);
	}
#endif
}
#endif
#if V_DEBUG_DRAW_UI_BOUNDING_BOX
void Voxel::UI::TransformNode::createDebugBoundingBoxLine()
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
#endif