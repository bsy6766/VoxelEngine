#include "UI.h"

#include <Texture2D.h>
#include <sstream>
#include <iostream>
#include <Quad.h>
#include <ProgramManager.h>
#include <Program.h>
#include <FontManager.h>
#include <Font.h>
#include <Limits.h>
#include <Camera.h>
#include <SpriteSheet.h>
#include <Application.h>
#include <glm/gtx/transform.hpp>
#include <Utility.h>

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
		modelMat = parent->getModelMatrix() * glm::translate(glm::mat4(1.0f), glm::vec3(parent->getContentSize() * getCoordinateOrigin(), 0.0f)) * getModelMatrix();
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

//============================================================== Canvas ==============================================================

Canvas::Canvas(const glm::vec2& size, const glm::vec2& centerPosition)
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

void Voxel::UI::Canvas::updateMouseClick(const glm::vec2 & mousePosition, const int button)
{
	for (auto& e : children)
	{
		(e.second)->updateMouseClick(mousePosition, button);
	}
}

void Voxel::UI::Canvas::updateMouseRelease(const glm::vec2 & mousePosition, const int button)
{
	for (auto& e : children)
	{
		(e.second)->updateMouseRelease(mousePosition, button);
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

//====================================================================================================================================

//============================================================== Image ===============================================================

Voxel::UI::Image::Image(const std::string& name)
	: RenderNode(name)
{}

Voxel::UI::Image::~Image()
{
	//std::cout << "~Image()\n";
}

Image * Voxel::UI::Image::create(const std::string & name, std::string & imageFileName)
{
	auto newImage = new Image(name);

	if (newImage->init(imageFileName))
	{
		return newImage;
	}
	else
	{
		delete newImage;
		return nullptr;
	}
}

Image * Voxel::UI::Image::createFromSpriteSheet(const std::string & name, const std::string & spriteSheetName, const std::string & imageFileName)
{
	auto& ssm = SpriteSheetManager::getInstance();

	auto ss = ssm.getSpriteSheet(spriteSheetName);

	if (ss)
	{
		auto newImage = new Image(name);

		if (newImage->initFromSpriteSheet(ss, imageFileName))
		{
			return newImage;
		}
		else
		{
			delete newImage;
			return nullptr;
		}
	}
	else
	{
		return nullptr;
	}
}

bool Voxel::UI::Image::init(const std::string& textureName)
{
	texture = Texture2D::create(textureName, GL_TEXTURE_2D);

	if (texture == nullptr)
	{
		return false;
	}

	texture->setLocationOnProgram(ProgramManager::PROGRAM_NAME::UI_TEXTURE_SHADER);

	auto size = glm::vec2(texture->getTextureSize());

	std::array<float, 12> vertices = { 0.0f };

	float widthHalf = size.x * 0.5f;
	float heightHalf = size.y * 0.5f;

	// Add vertices from 0 to 4
	// 0
	vertices.at(0) = -widthHalf;
	vertices.at(1) = -heightHalf;
	vertices.at(2) = 0.0f;

	//1
	vertices.at(3) = -widthHalf;
	vertices.at(4) = heightHalf;
	vertices.at(5) = 0.0f;

	//2
	vertices.at(6) = widthHalf;
	vertices.at(7) = -heightHalf;
	vertices.at(8) = 0.0f;

	//3
	vertices.at(9) = widthHalf;
	vertices.at(10) = heightHalf;
	vertices.at(11) = 0.0f;

	boundingBox.center = position;
	boundingBox.size = size;

	contentSize = size;

	build(vertices, Quad::uv, Quad::indices);

	return true;
}

bool Voxel::UI::Image::initFromSpriteSheet(SpriteSheet* ss, const std::string& textureName)
{
	texture = ss->getTexture();

	if (texture == nullptr)
	{
		return false;
	}

	texture->setLocationOnProgram(ProgramManager::PROGRAM_NAME::UI_TEXTURE_SHADER);
	
	auto imageEntry = ss->getImageEntry(textureName);

	auto size = glm::vec2(imageEntry->width, imageEntry->height);

	std::array<float, 12> vertices = { 0.0f };

	float widthHalf = size.x * 0.5f;
	float heightHalf = size.y * 0.5f;

	// Add vertices from 0 to 4
	// 0
	vertices.at(0) = -widthHalf;
	vertices.at(1) = -heightHalf;
	vertices.at(2) = 0.0f;

	//1
	vertices.at(3) = -widthHalf;
	vertices.at(4) = heightHalf;
	vertices.at(5) = 0.0f;

	//2
	vertices.at(6) = widthHalf;
	vertices.at(7) = -heightHalf;
	vertices.at(8) = 0.0f;

	//3
	vertices.at(9) = widthHalf;
	vertices.at(10) = heightHalf;
	vertices.at(11) = 0.0f;

	auto& uvOrigin = imageEntry->uvOrigin;
	auto& uvEnd = imageEntry->uvEnd;

	std::array<float, 8> uvs = { 0.0f };

	uvs.at(0) = uvOrigin.x;
	uvs.at(1) = uvOrigin.y;
	uvs.at(2) = uvOrigin.x;
	uvs.at(3) = uvEnd.y;
	uvs.at(4) = uvEnd.x;
	uvs.at(5) = uvOrigin.y;
	uvs.at(6) = uvEnd.x;
	uvs.at(7) = uvEnd.y;

	boundingBox.center = position;
	boundingBox.size = size;

	contentSize = size;
	
	build(vertices, uvs, Quad::indices);

	return true;
}

void Voxel::UI::Image::build(const std::array<float, 12>& vertices, const std::array<float, 8>& uvs, const std::array<unsigned int, 6>& indices)
{
	if (vao)
	{
		// Delte array
		glDeleteVertexArrays(1, &vao);
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	program = ProgramManager::getInstance().getProgram(ProgramManager::PROGRAM_NAME::UI_TEXTURE_SHADER);

	GLint vertLoc = program->getAttribLocation("vert");

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLint uvVertLoc = program->getAttribLocation("uvVert");

	GLuint uvbo;
	glGenBuffers(1, &uvbo);
	glBindBuffer(GL_ARRAY_BUFFER, uvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * uvs.size(), &uvs.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(uvVertLoc);
	glVertexAttribPointer(uvVertLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//============= find error here. error count: 14. Only during using sprite sheet
	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices.front(), GL_STATIC_DRAW);
	//========================

	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &uvbo);
	glDeleteBuffers(1, &ibo);

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX
	createDebugBoundingBoxLine();
#endif
}

void Voxel::UI::Image::renderSelf()
{
	// only render self
	if (texture == nullptr) return;
	if (!visibility) return;
	if (program == nullptr) return;

	program->use(true);
	program->setUniformMat4("modelMat", modelMat);
	program->setUniformFloat("opacity", opacity);
	program->setUniformVec3("color", color);

	texture->activate(GL_TEXTURE0);
	texture->bind();

	if (vao)
	{
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX
	if (bbVao)
	{
		auto lineProgram = ProgramManager::getInstance().getProgram(Voxel::ProgramManager::PROGRAM_NAME::LINE_SHADER);
		lineProgram->use(true);
		lineProgram->setUniformMat4("modelMat", modelMat);
		lineProgram->setUniformMat4("viewMat", glm::mat4(1.0f));

		glBindVertexArray(bbVao);
		glDrawArrays(GL_LINES, 0, 8);
	}
#endif
}

//====================================================================================================================================

//===================================================== Animated Image ===============================================================

Voxel::UI::AnimatedImage::AnimatedImage(const std::string & name)
	: RenderNode(name)
	, frameSize(0)
	, interval(0.0f)
	, elapsedTime(0.0f)
	, currentFrameIndex(0)
	, currentIndex(0)
	, repeat(false)
	, stopped(false)
	, paused(false)
{}

AnimatedImage * Voxel::UI::AnimatedImage::create(const std::string & name, const std::string & spriteSheetName, const std::string& frameName, const int frameSize, const float interval, const bool repeat)
{
	auto newAnimatedImage = new AnimatedImage(name);

	auto& ssm = SpriteSheetManager::getInstance();

	auto ss = ssm.getSpriteSheet(spriteSheetName);

	if (ss)
	{
		if (newAnimatedImage->init(ss, frameName, frameSize, interval, repeat))
		{
			return newAnimatedImage;
		}
	}

	delete newAnimatedImage;
	return nullptr;
}

bool Voxel::UI::AnimatedImage::init(SpriteSheet* ss, const std::string& frameName, const int frameSize, const float interval, const bool repeat)
{
	texture = ss->getTexture();

	if (texture == nullptr)
	{
		return false;
	}

	this->frameSize = frameSize;
	this->interval = interval;
	this->repeat = repeat;

	texture->setLocationOnProgram(ProgramManager::PROGRAM_NAME::UI_TEXTURE_SHADER);

	std::vector<float> vertices;
	std::vector<float> uvs;
	std::vector<unsigned int> indices;

	std::string fileName;
	std::string fileExt;

	Utility::String::fileNameToNameAndExt(frameName, fileName, fileExt);
	
	auto quadIndices = Quad::indices;
	
	for (int i = 0; i < frameSize; i++)
	{
		std::string currentFrameName = fileName + "_" + std::to_string(i) + fileExt;

		auto imageEntry = ss->getImageEntry(currentFrameName);
		
		if (imageEntry)
		{
			auto size = glm::vec2(imageEntry->width, imageEntry->height);
			auto curVertices = Quad::getVertices(size);

			vertices.insert(vertices.end(), curVertices.begin(), curVertices.end());
			
			auto& uvOrigin = imageEntry->uvOrigin;
			auto& uvEnd = imageEntry->uvEnd;

			uvs.push_back(uvOrigin.x);
			uvs.push_back(uvOrigin.y); 
			uvs.push_back(uvOrigin.x);
			uvs.push_back(uvEnd.y);
			uvs.push_back(uvEnd.x);
			uvs.push_back(uvOrigin.y);
			uvs.push_back(uvEnd.x);
			uvs.push_back(uvEnd.y);

			for (auto index : quadIndices)
			{
				indices.push_back(index + (4 * i));
			}

			frameSizes.push_back(size);
		}
		else
		{
			return false;
		}
	}

	boundingBox.center = position;
	boundingBox.size = frameSizes.front();

	contentSize = boundingBox.size;

	build(vertices, uvs, indices);

	return true;
}

void Voxel::UI::AnimatedImage::build(const std::vector<float>& vertices, const std::vector<float>& uvs, const std::vector<unsigned int>& indices)
{
	if (vao)
	{
		// Delte array
		glDeleteVertexArrays(1, &vao);
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	program = ProgramManager::getInstance().getProgram(ProgramManager::PROGRAM_NAME::UI_TEXTURE_SHADER);

	GLint vertLoc = program->getAttribLocation("vert");

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLint uvVertLoc = program->getAttribLocation("uvVert");

	GLuint uvbo;
	glGenBuffers(1, &uvbo);
	glBindBuffer(GL_ARRAY_BUFFER, uvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * uvs.size(), &uvs.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(uvVertLoc);
	glVertexAttribPointer(uvVertLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//============= find error here. error count: 14. Only during using sprite sheet
	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices.front(), GL_STATIC_DRAW);
	//========================

	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &uvbo);
	glDeleteBuffers(1, &ibo);

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX
#endif
}

void Voxel::UI::AnimatedImage::start()
{
	currentFrameIndex = 0;
	elapsedTime = 0.0f;

	paused = false;
	stopped = false;
}

void Voxel::UI::AnimatedImage::pause()
{
	paused = true;
}

void Voxel::UI::AnimatedImage::resume()
{
	paused = false;
}

void Voxel::UI::AnimatedImage::stop()
{
	stopped = true;
}

void Voxel::UI::AnimatedImage::setInterval(const float interval)
{
	this->interval = glm::max(interval, 0.0f);
}

void Voxel::UI::AnimatedImage::update(const float delta)
{
	// If animation is stopped, don't update
	if (stopped) return;

	// If animation is paused, don't update
	if (paused) return;


	elapsedTime += delta;

	bool updated = false;

	while (elapsedTime >= interval)
	{
		elapsedTime -= interval;

		currentFrameIndex++;
		currentIndex += 6;

		updated = true;

		if (currentFrameIndex >= frameSize)
		{
			if (repeat)
			{
				currentFrameIndex = 0;
				currentIndex = 0;
			}
			else
			{
				stopped = true;
				currentIndex -= 6;

				updated = false;
			}
		}
	}

	if (updated)
	{
		boundingBox.size = frameSizes.at(currentFrameIndex);

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX
		createDebugBoundingBoxLine();
#endif
	}
}

void Voxel::UI::AnimatedImage::renderSelf()
{
	if (texture == nullptr) return;
	if (!visibility) return;
	if (program == nullptr) return;

	program->use(true);
	program->setUniformMat4("modelMat", modelMat);
	program->setUniformFloat("opacity", opacity);
	program->setUniformVec3("color", color);

	texture->activate(GL_TEXTURE0);
	texture->bind();

	if (vao)
	{
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(currentIndex * sizeof(GLuint)));
	}

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX
	if (bbVao)
	{
		auto lineProgram = ProgramManager::getInstance().getProgram(Voxel::ProgramManager::PROGRAM_NAME::LINE_SHADER);
		lineProgram->use(true);
		lineProgram->setUniformMat4("modelMat", modelMat);
		lineProgram->setUniformMat4("viewMat", glm::mat4(1.0f));

		glBindVertexArray(bbVao);
		glDrawArrays(GL_LINES, 0, 8);
	}
#endif
}

//====================================================================================================================================

//========================================================= Text =====================================================================

Voxel::UI::Text::Text(const std::string& name)
	: RenderNode(name)
	, text("")
	, align(ALIGN::LEFT)
	, indicesSize(0)
	, outlined(false)
	, color(1.0f)
	, outlineColor(0.0f)
	, vbo(0)
	, cbo(0)
	, uvbo(0)
	, ibo(0)
{}

Voxel::UI::Text::~Text()
{
	clear();
}

Text * Voxel::UI::Text::create(const std::string & name, const std::string & text, const int fontID, const ALIGN align)
{
	Text* newText = new Text(name);

	newText->font = FontManager::getInstance().getFont(fontID);

	if (newText->font != nullptr)
	{
		if (newText->init(text, align))
		{
			return newText;
		}
	}

	delete newText;
	newText = nullptr;

	return nullptr;
}

Text * Voxel::UI::Text::createWithOutline(const std::string & name, const std::string & text, const int fontID, const glm::vec3 & outlineColor, const ALIGN align)
{
	Text* newText = new Text(name);

	newText->font = FontManager::getInstance().getFont(fontID);

	if (newText->font != nullptr)
	{
		// Check if font supports outline
		if (newText->font->isOutlineEnabled())
		{
			if (newText->initWithOutline(text, outlineColor, align))
			{
				return newText;
			}
		}
	}

	delete newText;
	newText = nullptr;

	return nullptr;
}

bool Voxel::UI::Text::init(const std::string & text, const ALIGN align)
{
	this->text = text;
	this->align = align;

	updateModelMatrix();
	
	return buildMesh(true);
}

bool Voxel::UI::Text::initWithOutline(const std::string & text, const glm::vec3 & outlineColor, ALIGN align)
{
	this->text = text;
	this->align = align;
	this->outlineColor = outlineColor;

	this->outlined = (this->font->getOutlineSize() > 0);

	updateModelMatrix();

	return buildMesh(true);
}

void Voxel::UI::Text::setText(const std::string & text)
{
	// Check if text is empty
	if (text.empty())
	{
		// It's empty. Clear the text.
		clear();
	}
	else
	{
		// Not empty. Check if it's different.
		if (text != this->text)
		{
			bool update = false;

			// Check if new text is longer than current text. if so, we have to reallocate the buffer. Else, do nothing
			if (text.size() > this->text.size())
			{
				// new text is larger than current text
				update = true;
			}
			// Else, it's same length or shorter.

			// Different. rebuild text
			this->text = text;

			buildMesh(update);
		}
	}
}

std::string Voxel::UI::Text::getText() const
{
	return text;
}

bool Voxel::UI::Text::isOutlined() const
{
	return outlined;
}

void Voxel::UI::Text::setColor(const glm::vec4 & color)
{
	this->color = color;

	// Todo: rebuild color buffer
}

void Voxel::UI::Text::setOutlineColor(const glm::vec4 & color)
{
	outlineColor = glm::clamp(color, 0.0f, 1.0f);
}

glm::vec3 Voxel::UI::Text::getOutlineColor() const
{
	return outlineColor;
}

void Voxel::UI::Text::clear()
{
	// Delte buffers
	if (vbo)
		glDeleteBuffers(1, &vbo);
	if (cbo)
		glDeleteBuffers(1, &cbo);
	if (ibo)
		glDeleteBuffers(1, &ibo);
	if (uvbo)
		glDeleteBuffers(1, &uvbo);

	vbo = 0;
	cbo = 0;
	ibo = 0;
	uvbo = 0;

	// Delte array
	if (vao)
		glDeleteVertexArrays(1, &vao);

	vao = 0;

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX
	if (bbVao)
	{
		// Delte array
		glDeleteVertexArrays(1, &bbVao);
		bbVao = 0;
	}
#endif
}

bool Voxel::UI::Text::buildMesh(const bool update)
{
	// Check font
	if (font)
	{
		// Check text
		if (text.empty())
		{
			return true;
		}

		// set texture
		texture = font->getTexture();

		// Split text label by line
		std::vector<std::string> split;

		// Turn the string into a stream.
		std::stringstream ss(text); 

		// token (line)
		std::string tok;

		// iterate and get line by new line char
		while (getline(ss, tok, '\n'))
		{
			split.push_back(tok);
		}

		// local struct. Defines line size
		struct LineSize
		{
			// Width of line
			int width;
			// max bearing of line
			int maxBearingY;
			// max bot of line
			int maxBotY;
		};

		// This is where we store each line's size so we can properly reposition all character based on align type
		std::vector<LineSize> lineSizes;

		int maxWidth = 0;
		int totalHeight = 0;

		const float outlineSize = static_cast<float>(font->getOutlineSize());

		int lineGap = 0;
		int lineGapHeight = 0 + static_cast<int>(outlineSize);
		int lineSpace = font->getLineSpace();

		// Iterate per line. Find the maximum width and height
		for (auto& line : split)
		{
			lineSizes.push_back(LineSize());

			int totalWidth = 0;
			int maxBearingY = 0;
			int maxBotY = 0;

			unsigned int len = line.size();
			for (unsigned int i = 0; i < len; i++)
			{
				const char c = line[i];
				Glyph* glyph = font->getCharGlyph(c);

				// Advance value is the distance between pen position of each character in horizontal layout
				// Advance includes bearing x + width + extra space for next character.
				// We don't have to add extra space for last char because line ends.
				if (i == (len - 1))
				{
					totalWidth += (glyph->bearingX + glyph->width);
				}
				else
				{
					totalWidth += glyph->advance;
				}

				// Find max bearing Y. BearingY is the upper height of character from pen position.
				if (maxBearingY < glyph->bearingY)
				{
					maxBearingY = glyph->bearingY;
				}

				// Find max botY. BotY is my defined value, which is the lower height of character from pen position
				if (maxBotY < glyph->botY)
				{
					maxBotY = glyph->botY;
				}
			}

			// Make max width and height even number because this is UI(?)
			// Note: The reason why I added this is because when pen position x had decimal points due to width being odd number, entire text rendering becaome weird (extended in x axis. slightly)
			// This fixed the issue.
			if (totalWidth % 2 == 1)
			{
				totalWidth++;
			}

			// total width is sum of glyph's advance, which includes extra space for next character. Remove it
			maxWidth = glm::max(totalWidth, maxWidth);

			if (line.empty())
			{
				line = " ";
				// quick hack here. Add whitespace to emptyline to treat as line
				lineSizes.back().width = 0;
				lineSizes.back().maxBearingY = font->getCharGlyph(' ')->height;
				lineSizes.back().maxBotY = 0;
			}
			else
			{
				lineSizes.back().width = totalWidth;
				lineSizes.back().maxBearingY = maxBearingY;
				lineSizes.back().maxBotY = maxBotY;
			}
		}

		auto lineSize = split.size() - 1;
		totalHeight = (lineSize * lineSpace) + font->getSize();
		lineGap = lineSize * lineGapHeight;

		/*
		if (totalHeight % 2 == 1)
		{
			totalHeight++;
		}
		*/

		// Set size of Text object. Ignore last line's line gap
		auto boxMin = glm::vec2(static_cast<float>(maxWidth) * -0.5f, static_cast<float>(totalHeight + (lineGap)) * -0.5f);
		auto boxMax = glm::vec2(static_cast<float>(maxWidth) * 0.5f, static_cast<float>(totalHeight + (lineGap)) * 0.5f);

		boundingBox.center = position;
		boundingBox.size = glm::vec2(boxMax.x - boxMin.x, boxMax.y - boxMin.y);

		contentSize = boundingBox.size;
		
		// Pen position. Also called as origin or base line in y pos.
		std::vector<glm::vec2> penPositions;

		// Current Y. Because we are using horizontal layout, we advance y in negative direction (Down), starting from height point, which is half of max height
		float curY = boxMax.y;

		//curY -= lineSizes.front().maxBearingY;
		curY -= font->getSize();

		// Iterate line sizes to find out each line's pen position from origin
		for (auto lineSize : lineSizes)
		{
			glm::vec2 penPos = glm::vec2(0);
			if (align == ALIGN::LEFT)
			{
				// Align text to left. x is always the most left pos of longest line
				penPos.x = static_cast<float>(maxWidth) * -0.5f;
			}
			else if (align == ALIGN::RIGHT)
			{
				// Aling text to right.
				penPos.x = (static_cast<float>(maxWidth) * 0.5f) - static_cast<float>(lineSize.width);
			}
			else // center
			{
				// Aling text to center.
				penPos.x = static_cast<float>(lineSize.width) * -0.5f;
			}

			// Y works same for same
			//penPos.y = curY - lineSize.maxBearingY;
			penPos.y = curY;
			//penPos.y = curY - lineSpace;
			// add pen position
			penPositions.push_back(penPos);
			// Update y to next line
			// I might advance to next line with linespace value, but I have to modify the size of line then. TODO: Consider this
			curY -= (lineSpace + lineGapHeight);
			// As I said, customizing for MunroSmall
			//curY -= (lineSize.maxBearingY + lineSize.maxBotY + lineGapHeight);
		}

		// We have pen position for each line. Iterate line and build vertices based on 
		int penPosIndex = 0;
		//std::vector<std::vector<float>> lineVertices;
		std::vector<float> vertices;
		// colors, indices and uv doesn't have to be tralsated later.
		std::vector<float> colors;
		std::vector<unsigned int> indices;
		std::vector<float> uvVertices;
		unsigned int indicesIndex = 0;

		for (auto& line : split)
		{
			if (line != " ")
			{
				//lineVertices.push_back(std::vector<float>());
				// start x from pen position x
				float curX = penPositions.at(penPosIndex).x;

				unsigned int len = line.size();
				for (unsigned int i = 0; i < len; i++)
				{
					// Build quad for each character
					const char c = line[i];
					Glyph* glyph = font->getCharGlyph(c);
					// Empty pos. p1 = left bottom, p2 = right top. z == 0
					glm::vec2 leftBottom(0);
					glm::vec2 rightTop(0);
					float x = curX;

					// Advance x for bearing x
					x += static_cast<float>(glyph->bearingX);

					// outline
					leftBottom.x = (x - outlineSize);

					// Advance x again for width
					x += static_cast<float>(glyph->width);
					rightTop.x = (x + outlineSize);

					// Calculate Y based on pen position
					leftBottom.y = penPositions.at(penPosIndex).y - static_cast<float>(glyph->botY) - outlineSize;
					rightTop.y = penPositions.at(penPosIndex).y + static_cast<float>(glyph->bearingY) + outlineSize;

					// Advnace pen pos x to next char
					curX += glyph->advance;

					// Store left bttom and right top vertices pos
					vertices.push_back(leftBottom.x); vertices.push_back(leftBottom.y); vertices.push_back(0);	// left bottom
					vertices.push_back(leftBottom.x); vertices.push_back(rightTop.y); vertices.push_back(0);	// left top
					vertices.push_back(rightTop.x); vertices.push_back(leftBottom.y); vertices.push_back(0);		// right bottom
					vertices.push_back(rightTop.x); vertices.push_back(rightTop.y); vertices.push_back(0);		// right top


					for (int j = 0; j < 4; j++)
					{
						colors.push_back(color.r);
						colors.push_back(color.g);
						colors.push_back(color.b);
						//colors.push_back(color.a);
					}

					// uv.
					uvVertices.push_back(glyph->uvTopLeft.x); uvVertices.push_back(glyph->uvBotRight.y);	// Left bottom
					uvVertices.push_back(glyph->uvTopLeft.x); uvVertices.push_back(glyph->uvTopLeft.y);		// Left top
					uvVertices.push_back(glyph->uvBotRight.x); uvVertices.push_back(glyph->uvBotRight.y);	// right bottom
					uvVertices.push_back(glyph->uvBotRight.x); uvVertices.push_back(glyph->uvTopLeft.y);	// right top

																											// indices. range of 4 per quad.
					indices.push_back(indicesIndex * 4);
					indices.push_back(indicesIndex * 4 + 1);
					indices.push_back(indicesIndex * 4 + 2);
					indices.push_back(indicesIndex * 4 + 1);
					indices.push_back(indicesIndex * 4 + 2);
					indices.push_back(indicesIndex * 4 + 3);

					// inc index
					indicesIndex++;
				}
			}

			penPosIndex++;
		}

		updateModelMatrix();

		// load buffer
		loadBuffers(vertices, colors, uvVertices, indices, update);
		
		return true;
	}
	else
	{
		std::cout << "[Text] Error: Font is nullptr\n";
		return false;
	}
}

void Voxel::UI::Text::loadBuffers(const std::vector<float>& vertices, const std::vector<float>& colors, const std::vector<float>& uvs, const std::vector<unsigned int>& indices, const bool update)
{
	auto textSize = text.size();

	if (update)
	{
		// reallocate buffer
		clear();

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		program = ProgramManager::getInstance().getProgram(ProgramManager::PROGRAM_NAME::UI_TEXT_SHADER);
		GLint vertLoc = program->getAttribLocation("vert");

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		// Allocate empty buffer for max length. 12 vertices(4 vec3) per char * max length
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * textSize * 12, nullptr, GL_DYNAMIC_DRAW);
		// fill buffer
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * vertices.size(), &vertices.front());

		glEnableVertexAttribArray(vertLoc);
		glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		GLint colorLoc = program->getAttribLocation("color");

		glGenBuffers(1, &cbo);
		glBindBuffer(GL_ARRAY_BUFFER, cbo);

		// Allocate empty buffer for max length. 12 vertices(4 vec3) per char * max length
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * textSize * 12, nullptr, GL_DYNAMIC_DRAW);
		// fill buffer
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * colors.size(), &colors.front());

		glEnableVertexAttribArray(colorLoc);
		glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		GLint uvVertLoc = program->getAttribLocation("uvVert");

		glGenBuffers(1, &uvbo);
		glBindBuffer(GL_ARRAY_BUFFER, uvbo);

		// Allocate empty buffer for max length. 8 verticies (4 vec2) per char * max len
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * textSize * 8, nullptr, GL_DYNAMIC_DRAW);
		// fill buffer
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * uvs.size(), &uvs.front());

		glEnableVertexAttribArray(uvVertLoc);
		glVertexAttribPointer(uvVertLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

		// Allocate empty buffer for max length. 6 indices ( 2 tri) per char * max len
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * textSize * 8, nullptr, GL_DYNAMIC_DRAW);
		// fill buffer
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned int) * indices.size(), &indices.front());

		indicesSize = indices.size();

		glBindVertexArray(0);
	}
	else
	{
		// No need to reallocate buffer.

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * vertices.size(), &vertices.front());

		glBindBuffer(GL_ARRAY_BUFFER, cbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * colors.size(), &colors.front());

		glBindBuffer(GL_ARRAY_BUFFER, uvbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * uvs.size(), &uvs.front());

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(float) * indices.size(), &indices.front());

		indicesSize = indices.size();

		glBindVertexArray(0);
	}

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX
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
#endif
}

std::vector<glm::vec2> Voxel::UI::Text::computeOrigins(Font * font, const std::vector<std::string>& split)
{
	std::vector<glm::vec2> originList;

	// vars
	int w = 0;
	int h = 0;
	int totalHeight = 0;
	int maxWidth = 0;
	int maxBearingY = 0;
	int maxBotY = 0;

	//std::vector<int> bearingYList;
	//std::vector<int> botYList;
	std::vector<int> widthList;

	// iterate text that is separted by new line
	for (auto& it : split)
	{
		// reset width and height
		w = 0;
		h = 0;
		// iterate each char in line
		auto len = it.length();
		const char* cStr = it.c_str();

		for (unsigned int i = 0; i < len; i++)
		{
			// Get char
			const char c = cStr[i];

			// Get glyph data
			Glyph* glyph = font->getCharGlyph(c);
			if (glyph == nullptr)
			{
				std::cout << "[Text] Failed to find glyph for char: " << c << std::endl;
				continue;
			}

			// sum up all char's width
			w += (glyph->metrics.horiAdvance >> 6);

			// get height of character
			int newHeight = static_cast<int>(glyph->metrics.height >> 6);
			// Store highest height in the line
			if (newHeight >= h)
			{
				h = newHeight;
			}

			// get bearing y  for char
			int newBearingY = static_cast<int>(glyph->metrics.horiBearingY >> 6);
			// store highest bot y (difference from total glyph's height and bearing y)
			int botY = newHeight - newBearingY;

			// Check the max botY
			if (botY > maxBotY)
			{
				maxBotY = botY;
			}

			// Also save max bearing y
			if (newBearingY > maxBearingY)
			{
				maxBearingY = newBearingY;
			}
		}

		// Check if this line has max width
		if (w > maxWidth)
		{
			maxWidth = w;
		}

		// sum total height with highest height
		totalHeight += h;

		// create origin. 
		glm::vec2 origin = glm::vec2(0);
		originList.push_back(origin);

		// save offset and width
		//bearingYList.push_back(maxBearingY);
		//botYList.push_back(maxBotY);
		widthList.push_back(w);
	}

	// get center of height
	float centerY = totalHeight * 0.5f;
	float newY = 0 - maxBearingY + centerY;

	int originIndex = 0;

	// update origin list
	auto len = widthList.size();
	auto mw = static_cast<float>(maxWidth);

	for (unsigned int i = 0; i < len; i++)
	{
		if (align == ALIGN::RIGHT)
		{
			// Not supported yet
			originList.at(originIndex).x = static_cast<float>((-1) * (maxWidth / 2)) + (maxWidth - widthList.at(originIndex));
		}
		else if (align == ALIGN::LEFT)
		{
			originList.at(originIndex).x = (mw * -0.5f);
		}
		else
		{
			// Center.
			originList.at(originIndex).x = static_cast<float>(widthList.at(originIndex) / 2 * -1);
		}

		//set the y position of origin
		originList.at(originIndex).y = newY;
		// move down the y position
		// Todo: check if offsetY is correct.
		//newY -= (bearingYList.at(i) + botYList.at(i));
		newY -= static_cast<float>(font->getLineSpace());

		// inc index
		originIndex++;
	}

	return originList;
}

void Voxel::UI::Text::renderSelf()
{
	if (indicesSize == 0) return;
	if (!visibility) return;
	if (text.empty()) return;
	if (font == nullptr) return;
	if (vao == 0) return;
	if (program == nullptr) return;

	texture->activate(GL_TEXTURE0);
	texture->bind();

	program->use(true);
	program->setUniformMat4("modelMat", modelMat);
	program->setUniformFloat("opacity", opacity);
	program->setUniformVec3("color", color);

	if (outlined)
	{
		program->setUniformBool("outlined", true);
		program->setUniformInt("outlineSize", 2);
		program->setUniformVec3("outlineColor", outlineColor);
	}
	else
	{
		program->setUniformBool("outlined", false);
	}

	if (vao)
	{
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);
	}

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX
	if (bbVao)
	{
		auto lineProgram = ProgramManager::getInstance().getProgram(Voxel::ProgramManager::PROGRAM_NAME::LINE_SHADER);
		lineProgram->use(true);
		lineProgram->setUniformMat4("modelMat", modelMat);
		lineProgram->setUniformMat4("viewMat", glm::mat4(1.0f));

		glBindVertexArray(bbVao);
		glDrawArrays(GL_LINES, 0, 8);

	}
#endif
}

//====================================================================================================================================

//=================================================== Button =========================================================================

Voxel::UI::Button::Button(const std::string & name)
	: RenderNode(name)
	, buttonState(State::IDLE)
	, currentIndex(0)
{}

Voxel::UI::Button* Voxel::UI::Button::create(const std::string & name, const std::string & spriteSheetName, const std::string & buttonImageFileName)
{
	auto newButton = new Voxel::UI::Button(name);

	auto& ssm = SpriteSheetManager::getInstance();

	auto ss = ssm.getSpriteSheet(spriteSheetName);

	if (ss)
	{
		if (newButton->init(ss, buttonImageFileName))
		{
			return newButton;
		}
	}

	delete newButton;
	return nullptr;
}

bool Voxel::UI::Button::init(SpriteSheet * ss, const std::string & buttonImageFileName)
{
	texture = ss->getTexture();

	if (texture == nullptr)
	{
		return false;
	}
	
	texture->setLocationOnProgram(ProgramManager::PROGRAM_NAME::UI_TEXTURE_SHADER);

	std::vector<float> vertices;
	std::vector<float> uvs;
	std::vector<unsigned int> indices;

	std::string fileName;
	std::string fileExt;

	Utility::String::fileNameToNameAndExt(buttonImageFileName, fileName, fileExt);

	auto quadIndices = Quad::indices;

	std::array<std::string, 4> fileNames = { fileName + "_idle" + fileExt, fileName + "_hovered" + fileExt, fileName + "_clicked" + fileExt, fileName + "_disabled" + fileExt };

	for (unsigned int i = 0; i < fileNames.size(); i++)
	{
		auto imageEntry = ss->getImageEntry(fileNames.at(i));

		if (imageEntry)
		{
			auto size = glm::vec2(imageEntry->width, imageEntry->height);
			auto curVertices = Quad::getVertices(size);

			vertices.insert(vertices.end(), curVertices.begin(), curVertices.end());
			
			auto& uvOrigin = imageEntry->uvOrigin;
			auto& uvEnd = imageEntry->uvEnd;

			uvs.push_back(uvOrigin.x);
			uvs.push_back(uvOrigin.y);
			uvs.push_back(uvOrigin.x);
			uvs.push_back(uvEnd.y);
			uvs.push_back(uvEnd.x);
			uvs.push_back(uvOrigin.y);
			uvs.push_back(uvEnd.x);
			uvs.push_back(uvEnd.y);

			for (auto index : quadIndices)
			{
				indices.push_back(index + (4 * i));
			}

			frameSizes.at(i) = size;
		}
		else
		{
			return false;
		}
	}

	boundingBox.center = position;
	boundingBox.size = frameSizes.front();

	contentSize = boundingBox.size;

	build(vertices, uvs, indices);

	return true;
}

void Voxel::UI::Button::build(const std::vector<float>& vertices, const std::vector<float>& uvs, const std::vector<unsigned int>& indices)
{
	if (vao)
	{
		// Delte array
		glDeleteVertexArrays(1, &vao);
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	program = ProgramManager::getInstance().getProgram(ProgramManager::PROGRAM_NAME::UI_TEXTURE_SHADER);

	GLint vertLoc = program->getAttribLocation("vert");

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLint uvVertLoc = program->getAttribLocation("uvVert");

	GLuint uvbo;
	glGenBuffers(1, &uvbo);
	glBindBuffer(GL_ARRAY_BUFFER, uvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * uvs.size(), &uvs.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(uvVertLoc);
	glVertexAttribPointer(uvVertLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//============= find error here. error count: 14. Only during using sprite sheet
	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices.front(), GL_STATIC_DRAW);
	//========================

	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &uvbo);
	glDeleteBuffers(1, &ibo);

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX
	createDebugBoundingBoxLine();
#endif
}

void Voxel::UI::Button::enable()
{
	buttonState = State::IDLE;
	currentIndex = 0;
}

void Voxel::UI::Button::disable()
{
	buttonState = State::DISABLED;
	currentIndex = 18;
}

void Voxel::UI::Button::updateMouseMove(const glm::vec2 & mousePosition)
{
	if (buttonState == State::DISABLED)
	{
		return;
	}
	else
	{
		if (buttonState == State::IDLE)
		{
			if (boundingBox.containsPoint(mousePosition))
			{
				buttonState = State::HOVERED;
				currentIndex = 6;
			}
		}
		else if (buttonState == State::HOVERED || buttonState == State::CLICKED)
		{
			if (!boundingBox.containsPoint(mousePosition))
			{
				buttonState = State::IDLE;
				currentIndex = 0;
			}
		}
	}
}

void Voxel::UI::Button::updateMouseClick(const glm::vec2 & mousePosition, const int button)
{
	if (button == GLFW_MOUSE_BUTTON_1)
	{
		if (buttonState == State::DISABLED)
		{
			return;
		}
		else
		{
			if (buttonState == State::HOVERED)
			{
				if (boundingBox.containsPoint(mousePosition))
				{
					buttonState = State::CLICKED;
					currentIndex = 12;
				}
			}
		}
	}
}

void Voxel::UI::Button::updateMouseRelease(const glm::vec2 & mousePosition, const int button)
{
	if (button == GLFW_MOUSE_BUTTON_1)
	{
		if (buttonState == State::DISABLED)
		{
			return;
		}
		else
		{
			if (buttonState == State::CLICKED)
			{
				if (boundingBox.containsPoint(mousePosition))
				{
					buttonState = State::IDLE;
					currentIndex = 0;

					// button clicked!
					std::cout << "Button " << name << " clicked\n";
				}
			}
		}
	}
}

void Voxel::UI::Button::renderSelf()
{
	if (texture == nullptr) return;
	if (!visibility) return;
	if (program == nullptr) return;

	program->use(true);
	program->setUniformMat4("modelMat", modelMat);
	program->setUniformFloat("opacity", opacity);
	program->setUniformVec3("color", color);

	texture->activate(GL_TEXTURE0);
	texture->bind();

	if (vao)
	{
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(currentIndex * sizeof(GLuint)));
	}

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX
	if (bbVao)
	{
		auto lineProgram = ProgramManager::getInstance().getProgram(Voxel::ProgramManager::PROGRAM_NAME::LINE_SHADER);
		lineProgram->use(true);
		lineProgram->setUniformMat4("modelMat", modelMat);
		lineProgram->setUniformMat4("viewMat", glm::mat4(1.0f));

		glBindVertexArray(bbVao);
		glDrawArrays(GL_LINES, 0, 8);
	}
#endif
}

//====================================================================================================================================

//============================================================= Check box ============================================================

Voxel::UI::CheckBox::CheckBox(const std::string& name)
	: RenderNode(name)
	, currentIndex(0)
	, checkBoxState(State::DESELECTED)
{}

CheckBox * Voxel::UI::CheckBox::create(const std::string & name, const std::string & spriteSheetName, const std::string & checkBoxImageFileName)
{
	auto newCheckBox = new Voxel::UI::CheckBox(name);

	auto& ssm = SpriteSheetManager::getInstance();

	auto ss = ssm.getSpriteSheet(spriteSheetName);

	if (ss)
	{
		if (newCheckBox->init(ss, checkBoxImageFileName))
		{
			return newCheckBox;
		}
	}

	delete newCheckBox;
	return nullptr;
}

bool Voxel::UI::CheckBox::init(SpriteSheet * ss, const std::string & checkBoxImageFileName)
{
	texture = ss->getTexture();

	if (texture == nullptr)
	{
		return false;
	}

	texture->setLocationOnProgram(ProgramManager::PROGRAM_NAME::UI_TEXTURE_SHADER);
	
	std::string fileName;
	std::string fileExt;

	Utility::String::fileNameToNameAndExt(checkBoxImageFileName, fileName, fileExt);

	std::vector<float> vertices;
	std::vector<float> uvs;
	std::vector<unsigned int> indices;
	auto quadIndices = Quad::indices;

	std::array<std::string, 7> fileNames =
	{ 
		fileName + "_deselected" + fileExt, 
		fileName + "_hovered" + fileExt, 
		fileName + "_clicked" + fileExt,
		fileName + "_selected" + fileExt,
		fileName + "_hoveredSelected" + fileExt,
		fileName + "_clickedSelected" + fileExt,
		fileName + "_disabled" + fileExt 
	};

	for (unsigned int i = 0; i < fileNames.size(); i++)
	{
		auto imageEntry = ss->getImageEntry(fileNames.at(i));

		if (imageEntry)
		{
			auto size = glm::vec2(imageEntry->width, imageEntry->height);
			auto curVertices = Quad::getVertices(size);

			vertices.insert(vertices.end(), curVertices.begin(), curVertices.end());
			
			auto& uvOrigin = imageEntry->uvOrigin;
			auto& uvEnd = imageEntry->uvEnd;

			uvs.push_back(uvOrigin.x);
			uvs.push_back(uvOrigin.y);
			uvs.push_back(uvOrigin.x);
			uvs.push_back(uvEnd.y);
			uvs.push_back(uvEnd.x);
			uvs.push_back(uvOrigin.y);
			uvs.push_back(uvEnd.x);
			uvs.push_back(uvEnd.y);

			for (auto index : quadIndices)
			{
				indices.push_back(index + (4 * i));
			}

			//frameSizes.at(i) = size;
		}
		else
		{
			return false;
		}
	}

	boundingBox.center = position;
	auto size = ss->getImageEntry(fileNames.at(0));
	boundingBox.size = glm::vec2(size->width, size->height);

	contentSize = boundingBox.size;

	build(vertices, uvs, indices);

	return true;
}

void Voxel::UI::CheckBox::build(const std::vector<float>& vertices, const std::vector<float>& uvs, const std::vector<unsigned int>& indices)
{
	if (vao)
	{
		// Delte array
		glDeleteVertexArrays(1, &vao);
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	program = ProgramManager::getInstance().getProgram(ProgramManager::PROGRAM_NAME::UI_TEXTURE_SHADER);

	GLint vertLoc = program->getAttribLocation("vert");

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLint uvVertLoc = program->getAttribLocation("uvVert");

	GLuint uvbo;
	glGenBuffers(1, &uvbo);
	glBindBuffer(GL_ARRAY_BUFFER, uvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * uvs.size(), &uvs.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(uvVertLoc);
	glVertexAttribPointer(uvVertLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//============= find error here. error count: 14. Only during using sprite sheet
	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices.front(), GL_STATIC_DRAW);
	//========================

	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &uvbo);
	glDeleteBuffers(1, &ibo);

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX
	createDebugBoundingBoxLine();
#endif
}

void Voxel::UI::CheckBox::updateCurrentIndex()
{
	switch (checkBoxState)
	{
	case Voxel::UI::CheckBox::State::DESELECTED:
		currentIndex = 0;
		break;
	case Voxel::UI::CheckBox::State::HOVERED:
		currentIndex = 6;
		break;
	case Voxel::UI::CheckBox::State::CLICKED:
		currentIndex = 12;
		break;
	case Voxel::UI::CheckBox::State::SELECTED:
		currentIndex = 18;
		break;
	case Voxel::UI::CheckBox::State::HOVERED_SELECTED:
		currentIndex = 24;
		break;
	case Voxel::UI::CheckBox::State::CLICKED_SELECTED:
		currentIndex = 30;
		break;
	case Voxel::UI::CheckBox::State::DISABLED:
		currentIndex = 36;
		break;
	default:
		currentIndex = 0;
		break;
	}
}

void Voxel::UI::CheckBox::enable()
{
	if (checkBoxState == State::DISABLED)
	{
		return;
	}
	else
	{
		checkBoxState = prevCheckBoxState;
		
		updateCurrentIndex();
	}
}

void Voxel::UI::CheckBox::disable()
{
	prevCheckBoxState = checkBoxState;
	checkBoxState = State::DISABLED;

	updateCurrentIndex();
}

void Voxel::UI::CheckBox::select()
{
	if (checkBoxState == State::DISABLED)
	{
		return;
	}
	else
	{
		checkBoxState = State::SELECTED;

		updateCurrentIndex();
	}
}

void Voxel::UI::CheckBox::deselect()
{
	if (checkBoxState == State::DISABLED)
	{
		return;
	}
	else
	{
		checkBoxState = State::DESELECTED;

		updateCurrentIndex();
	}
}

void Voxel::UI::CheckBox::updateMouseMove(const glm::vec2 & mousePosition)
{
	// mouse moved
	if (checkBoxState == State::DISABLED)
	{
		// check box is disabled. do nothing
		return;
	}
	else
	{
		// Check if mouse is in check box
		if (boundingBox.containsPoint(mousePosition))
		{
			// Mouse is in check box
			if (checkBoxState == State::DESELECTED)
			{
				// check box was deselected. hover it
				checkBoxState = State::HOVERED;
			}
			else if (checkBoxState == State::SELECTED)
			{
				// check box was selected. hover it
				checkBoxState = State::HOVERED_SELECTED;
			}
		}
		else
		{
			// Mouse is not in check box
			if (checkBoxState == State::HOVERED)
			{
				// was hovering check box
				checkBoxState = State::DESELECTED;
			}
			else if (checkBoxState == State::HOVERED_SELECTED)
			{
				// was hovering selected check box
				checkBoxState = State::SELECTED;
			}
			else if (checkBoxState == State::CLICKED)
			{
				// was clikcing box. cancel
				checkBoxState = State::DESELECTED;
			}
			else if (checkBoxState == State::CLICKED_SELECTED)
			{
				// was clicking selected box. cancel
				checkBoxState = State::SELECTED;
			}
		}

		updateCurrentIndex();
	}
}

void Voxel::UI::CheckBox::updateMouseClick(const glm::vec2 & mousePosition, const int button)
{
	// mouse clicked
	if (checkBoxState == State::DISABLED)
	{
		// check box is disabled. do nothing
		return;
	}
	else
	{
		// Check if mouse is in check box
		if (boundingBox.containsPoint(mousePosition))
		{
			// Clicked the check box
			if (checkBoxState == State::HOVERED)
			{
				// Was hovering check box. click
				checkBoxState = State::CLICKED;
			}
			else if (checkBoxState == State::HOVERED_SELECTED)
			{
				// was hovering selected box. click
				checkBoxState = State::CLICKED_SELECTED;
			}
		}

		updateCurrentIndex();
	}
}

void Voxel::UI::CheckBox::updateMouseRelease(const glm::vec2 & mousePosition, const int button)
{
	// mouse release
	if (checkBoxState == State::DISABLED)
	{
		// check box is disabled. do nothing
		return;
	}
	else
	{
		// Check if mouse is in check box
		if (boundingBox.containsPoint(mousePosition))
		{
			// Released in the check box
			if (checkBoxState == State::CLICKED)
			{
				// was clicking. select
				checkBoxState = State::SELECTED;
			}
			else if (checkBoxState == State::CLICKED_SELECTED)
			{
				// was clikcing selected. deslect
				checkBoxState = State::DESELECTED;
			}
		}

		updateCurrentIndex();
	}
}

void Voxel::UI::CheckBox::renderSelf()
{
	if (texture == nullptr) return;
	if (!visibility) return;
	if (program == nullptr) return;

	program->use(true);
	program->setUniformMat4("modelMat", modelMat);
	program->setUniformFloat("opacity", opacity);

	texture->activate(GL_TEXTURE0);
	texture->bind();

	if (vao)
	{
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(currentIndex * sizeof(GLuint)));
	}
}

//====================================================================================================================================

//======================================================== Progress Timer ============================================================

Voxel::UI::ProgressTimer::ProgressTimer(const std::string & name)
	: RenderNode(name)
	, percentage(100)
	, hasBackgroundImage(false)
{}

ProgressTimer * Voxel::UI::ProgressTimer::create(const std::string & name, const std::string & spriteSheetName, const std::string & progressTimerImageFileName, const std::string & progressTimerBgImageName, const Type type, const Direction direction)
{
	auto newProgressTimer = new Voxel::UI::ProgressTimer(name);

	auto& ssm = SpriteSheetManager::getInstance();

	auto ss = ssm.getSpriteSheet(spriteSheetName);

	if (ss)
	{
		if (newProgressTimer->init(ss, progressTimerImageFileName, progressTimerBgImageName, type, direction))
		{
			return newProgressTimer;
		}
	}

	delete newProgressTimer;
	return nullptr;
}

bool Voxel::UI::ProgressTimer::init(SpriteSheet * ss, const std::string & progressTimerImageFileName, const std::string & progressTimerBgImageName, const Type type, const Direction direction)
{
	texture = ss->getTexture();

	if (texture == nullptr)
	{
		return false;
	}

	texture->setLocationOnProgram(ProgramManager::PROGRAM_NAME::UI_TEXTURE_SHADER);

	std::vector<float> vertices;
	std::vector<float> uvs;
	std::vector<unsigned int> indices;
	auto quadIndices = Quad::indices;

	// Try to load background image
	if (progressTimerBgImageName.empty())
	{
		hasBackgroundImage = false;
	}
	else
	{
		auto imageEntry = ss->getImageEntry(progressTimerBgImageName);

		if (imageEntry)
		{
			auto size = glm::vec2(imageEntry->width, imageEntry->height);
			auto curVertices = Quad::getVertices(size);

			vertices.insert(vertices.end(), curVertices.begin(), curVertices.end());

			auto& uvOrigin = imageEntry->uvOrigin;
			auto& uvEnd = imageEntry->uvEnd;

			uvs.push_back(uvOrigin.x);
			uvs.push_back(uvOrigin.y);
			uvs.push_back(uvOrigin.x);
			uvs.push_back(uvEnd.y);
			uvs.push_back(uvEnd.x);
			uvs.push_back(uvOrigin.y);
			uvs.push_back(uvEnd.x);
			uvs.push_back(uvEnd.y);

			for (auto index : quadIndices)
			{
				indices.push_back(index);
			}

			//frameSizes.at(i) = size;
		}
		else
		{
			return false;
		}

		hasBackgroundImage = true;
	}

	auto imageEntry = ss->getImageEntry(progressTimerImageFileName);

	if (imageEntry)
	{
		auto size = glm::vec2(imageEntry->width, imageEntry->height);
		auto curVertcies = Quad::getVertices(size);
		
		auto& uvOrigin = imageEntry->uvOrigin;
		auto& uvEnd = imageEntry->uvEnd;

		std::vector<float> curUvs;
		curUvs.push_back(uvOrigin.x);
		curUvs.push_back(uvOrigin.y);
		curUvs.push_back(uvOrigin.x);
		curUvs.push_back(uvEnd.y);
		curUvs.push_back(uvEnd.x);
		curUvs.push_back(uvOrigin.y);
		curUvs.push_back(uvEnd.x);
		curUvs.push_back(uvEnd.y);

		//frameSizes.at(i) = size;

		buildMesh(curVertcies, curUvs, vertices, uvs, indices, type, direction);

		build(vertices, uvs, indices);

		return true;
	}
	else
	{
		return false;
	}
}

void Voxel::UI::ProgressTimer::buildMesh(const std::vector<float>& quadVertices, const std::vector<float>& quadUvs, std::vector<float>& vertices, std::vector<float>& uvs, std::vector<unsigned int>& indices, const Type type, const Direction direction)
{
	int indexOffset = hasBackgroundImage ? 4 : 0;
	
	if (type == Type::HORIZONTAL)
	{
		// Horizontal bar type

		float stepX = (quadVertices.at(6) * 2.0f) * 0.01f;

		float startX = 0.0f;
		const float yTop = quadVertices.at(4);
		const float yBot = quadVertices.at(1);

		float uvStepX = (quadUvs.at(4) - quadUvs.at(0)) * 0.01f;

		float uvStartX = 0.0f;
		const float uvOriginY = quadUvs.at(1);
		const float uvEndY = quadUvs.at(3);

		auto quadIndices = Quad::indices;

		if (direction == Direction::CLOCK_WISE)
		{
			// Progress fills from left to right
			startX = quadVertices.at(0);
			uvStartX = quadUvs.at(0);
		}
		else
		{
			// Progress fills from right to left
			startX = quadVertices.at(6);
			uvStartX = quadUvs.at(4);

			stepX *= -1.0f;
			uvStepX *= -1.0f;
		}

		for (int i = 0; i <= 100; i++)
		{
			// 0
			vertices.push_back(startX);
			vertices.push_back(yBot);
			vertices.push_back(0);

			//1
			vertices.push_back(startX);
			vertices.push_back(yTop);
			vertices.push_back(0);

			startX += stepX;
			
			uvs.push_back(uvStartX);
			uvs.push_back(uvOriginY);

			uvs.push_back(uvStartX);
			uvs.push_back(uvEndY);

			uvStartX += uvStepX;

			for (auto index : quadIndices)
			{
				indices.push_back(index + (2 * i) + indexOffset);
			}
		}
	}
	else if (type == Type::VERTICAL)
	{
		// Vertical bar type

		float stepY = (quadVertices.at(4) * 2.0f) * 0.01f;

		float startY = 0.0f;
		const float xLeft = quadVertices.at(0);
		const float xRight = quadVertices.at(6);

		float uvStepY = (quadUvs.at(1) - quadUvs.at(3)) * 0.01f;

		float uvStartY = 0.0f;
		const float uvOriginX = quadUvs.at(0);
		const float uvEndX = quadUvs.at(4);

		auto quadIndices = std::array<unsigned int, 6>{0, 2, 1, 2, 1, 3};

		if (direction == Direction::CLOCK_WISE)
		{
			// Progress fills from bottom to top
			startY = quadVertices.at(1);
			uvStartY = quadUvs.at(1);

			uvStepY *= -1.0f;
		}
		else
		{
			// Progress fills from top to bottom
			startY = quadVertices.at(4);
			uvStartY = quadUvs.at(3);

			stepY *= -1.0f;
		}

		for (int i = 0; i <= 100; i++)
		{
			// 0
			vertices.push_back(xLeft);
			vertices.push_back(startY);
			vertices.push_back(0);

			//1
			vertices.push_back(xRight);
			vertices.push_back(startY);
			vertices.push_back(0);
			
			startY += stepY;

			uvs.push_back(uvOriginX);
			uvs.push_back(uvStartY);

			uvs.push_back(uvEndX);
			uvs.push_back(uvStartY);
			
			uvStartY += uvStepY;

			for (auto index : quadIndices)
			{
				indices.push_back(index + (2 * i) + indexOffset);
			}
		}
	}
	else if (type == Type::RADIAL)
	{
		// Radial bar type.

		// Radial type is different compared to bar type.
		// We have to divide quad to 100 triangles in radial.
		// However, dividing quad in to 100 triangles with 3.6 degrees each will have problems in corner.

		/*
			
			  c4   7*      0   c1
				*------*------*
				|      |     /|
			  6 |      |   /  | 1*
				|      | /    |
				*      *      *
				|             |
			 5* |             | 2
				|             |
				*------*------*
			  c3   4     3*   c2
			
		*/
	}
}

void Voxel::UI::ProgressTimer::build(const std::vector<float>& vertices, const std::vector<float>& uvs, const std::vector<unsigned int>& indices)
{
	if (vao)
	{
		// Delte array
		glDeleteVertexArrays(1, &vao);
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	program = ProgramManager::getInstance().getProgram(ProgramManager::PROGRAM_NAME::UI_TEXTURE_SHADER);

	GLint vertLoc = program->getAttribLocation("vert");

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLint uvVertLoc = program->getAttribLocation("uvVert");

	GLuint uvbo;
	glGenBuffers(1, &uvbo);
	glBindBuffer(GL_ARRAY_BUFFER, uvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * uvs.size(), &uvs.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(uvVertLoc);
	glVertexAttribPointer(uvVertLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//============= find error here. error count: 14. Only during using sprite sheet
	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices.front(), GL_STATIC_DRAW);
	//========================

	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &uvbo);
	glDeleteBuffers(1, &ibo);

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX
	createDebugBoundingBoxLine();
#endif
}

void Voxel::UI::ProgressTimer::setPercentage(const int percentage)
{
	this->percentage = glm::clamp(percentage, 0, 100);
}

int Voxel::UI::ProgressTimer::getPercentage() const
{
	return percentage;
}

void Voxel::UI::ProgressTimer::renderSelf()
{
	if (texture == nullptr) return;
	if (!visibility) return;
	if (program == nullptr) return;

	program->use(true);
	program->setUniformMat4("modelMat", modelMat);
	program->setUniformFloat("opacity", opacity);

	texture->activate(GL_TEXTURE0);
	texture->bind();

	if (vao)
	{
		glBindVertexArray(vao);
		if (hasBackgroundImage)
		{
			glDrawElements(GL_TRIANGLES, (6 * percentage) + 6, GL_UNSIGNED_INT, 0);
		}
		else
		{
			glDrawElements(GL_TRIANGLES, 6 * percentage, GL_UNSIGNED_INT, 0);
		}
	}
}

//====================================================================================================================================

//=================================================== Cursor =========================================================================

Voxel::UI::Cursor::Cursor()
	: vao(0)
	, visible(false)
	, position(0)
{
}

Voxel::UI::Cursor::~Cursor()
{
	if (uvbo)
	{
		glDeleteBuffers(1, &uvbo);
	}

	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
	}
}

bool Voxel::UI::Cursor::init()
{
	// Initialize cursors
	auto ss = SpriteSheetManager::getInstance().getSpriteSheet("CursorSpriteSheet");

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

Cursor * Voxel::UI::Cursor::create()
{
	Cursor* newCursor = new Cursor();
	if (newCursor->init())
	{
		return newCursor;
	}

	delete newCursor;
	return nullptr;
}

void Voxel::UI::Cursor::addPosition(const glm::vec2 & distance)
{
	this->position += distance;

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

void Voxel::UI::Cursor::updateBoundary()
{
	auto size = glm::vec2(Application::getInstance().getGLView()->getScreenSize());

	minScreenBoundary = size * -0.5f;
	maxScreenBoundary = size * 0.5f;
}

void Voxel::UI::Cursor::setCursorType(const CursorType cursorType)
{
	auto ss = SpriteSheetManager::getInstance().getSpriteSheet("CursorSpriteSheet");

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

void Voxel::UI::Cursor::setVisibility(const bool visibility)
{
	visible = visibility;
}

glm::vec2 Voxel::UI::Cursor::getPosition() const
{
	return position;
}

void Voxel::UI::Cursor::render()
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

			glBindVertexArray(vao);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
	}
}

//====================================================================================================================================