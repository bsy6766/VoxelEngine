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

Voxel::UI::Node::Node()
	: visibility(true)
	, opacity(0.0f)
	, position(0.0f)
	, angle(0.0f)
	, scale(1.0f)
	, pivot(0.0f)
	, coordinateOrigin(0.0f)
	, contentSize(0.0f)
	, modelMat(1.0f)
	, sequence(nullptr)
	, zOrder()
	, program(nullptr)
	, boundingBox(glm::vec2(0.0), glm::vec2(0.0f))
	, needToUpdateModelMat(false)
	, parent(nullptr)
{}

Voxel::UI::Node::Node(const std::string & name)
	: name(name)
	, visibility(true)
	, opacity(0.0f)
	, position(0.0f)
	, angle(0.0f)
	, scale(1.0f)
	, pivot(0.0f)
	, coordinateOrigin(0.0f)
	, contentSize(0.0f)
	, modelMat(1.0f)
	, sequence(nullptr)
	, zOrder()
	, program(nullptr)
	, boundingBox(glm::vec2(0.0), glm::vec2(0.0f))
	, needToUpdateModelMat(false)
	, parent(nullptr)
{}

Voxel::UI::Node::~Node()
{
	if (sequence)
	{
		delete sequence;
	}
}

void Voxel::UI::Node::setOpacity(const float opacity)
{
	this->opacity = glm::clamp(opacity, 0.0f, 1.0f);
}

float Voxel::UI::Node::getOpacity() const
{
	return opacity;
}

void Voxel::UI::Node::setPosition(const float x, const float y)
{
	this->position.x = x;
	this->position.y = y;

	this->boundingBox.center = position;

	needToUpdateModelMat = true;
}

void Voxel::UI::Node::setPosition(const glm::vec2 & position)
{
	setPosition(position.x, position.y);
}

glm::vec2 Voxel::UI::Node::getPosition() const
{
	return position;
}

void Voxel::UI::Node::setAngle(const float angle)
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

float Voxel::UI::Node::getAngle() const
{
	return angle;
}

void Voxel::UI::Node::setScale(const glm::vec2 & scale)
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

glm::vec2 Voxel::UI::Node::getScale() const
{
	return scale;
}

void Voxel::UI::Node::setPivot(const glm::vec2 & pivot)
{
	this->pivot = glm::clamp(pivot, -0.5f, 0.5f);

	needToUpdateModelMat = true;
}

glm::vec2 Voxel::UI::Node::getPivot() const
{
	return pivot;
}

void Voxel::UI::Node::setCoordinateOrigin(const glm::vec2 & coordinateOrigin)
{
	this->coordinateOrigin = glm::clamp(coordinateOrigin, -0.5f, 0.5f);

	needToUpdateModelMat = true;
}

glm::vec2 Voxel::UI::Node::getCoordinateOrigin() const
{
	return coordinateOrigin;
}

void Voxel::UI::Node::setBoundingBox(const glm::vec2 & center, const glm::vec2 & size)
{
	boundingBox.center = center;
	boundingBox.size = size;
}

Voxel::Shape::Rect Voxel::UI::Node::getBoundingBox() const
{
	auto scaled = boundingBox;
	scaled.size *= scale;
	return scaled;
}

void Voxel::UI::Node::setZorder(const ZOrder & zOrder)
{
	this->zOrder = zOrder;
}

Voxel::ZOrder Voxel::UI::Node::getZOrder() const
{
	return zOrder;
}

bool Voxel::UI::Node::addChild(Node * child)
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

bool Voxel::UI::Node::addChild(Voxel::UI::Node * child, int zOrder)
{
	if (auto canvasChild = dynamic_cast<Voxel::UI::Canvas*>(child))
	{
		return false;
	}

	return addChild(child, ZOrder(zOrder));
}

bool Voxel::UI::Node::addChild(Voxel::UI::Node * child, Voxel::ZOrder& zOrder)
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
		children.insert(std::make_pair(zOrder, std::unique_ptr<Voxel::UI::Node>(child)));

		child->parent = this;

		// New child added. Update model matrix based on parent's model matrix
		child->updateModelMatrix();
		
		return true;
	}
}

bool Voxel::UI::Node::getNextZOrder(Voxel::ZOrder & curZOrder)
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

Node * Voxel::UI::Node::getChild(const std::string & name)
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

bool Voxel::UI::Node::hasChildren()
{
	return !(children.empty());
}

void Voxel::UI::Node::getAllChildrenInVector(std::vector<Node*>& nodes, Node * parent)
{
	if (children.empty())
	{
		return;
	}
	else
	{
		std::vector<Node*> negativesOrder, positiveOrder;

		for (auto& e : children)
		{
			if ((e.first).globalZOrder < 0)
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

void Voxel::UI::Node::update(const float delta)
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

void Voxel::UI::Node::runAction(Voxel::UI::Sequence * sequence)
{
	if (sequence)
	{
		delete sequence;
	}

	this->sequence = sequence;
}

void Voxel::UI::Node::render()
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

void Voxel::UI::Node::print(const int tab)
{
	for (int i = 0; i < tab; i++)
	{
		std::cout << "\t";
	}

	std::cout << name << "\n";
}

glm::vec2 Voxel::UI::Node::getContentSize()
{
	glm::vec2 scaled = contentSize;
	scaled.x *= scale.x;
	scaled.y *= scale.y;

	return scaled;
}

glm::mat4 Voxel::UI::Node::getModelMatrix()
{
	return glm::scale(glm::translate(glm::translate(glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 0, 1)), glm::vec3(position, 0)), glm::vec3(pivot * getContentSize() * -1.0f, 0)), glm::vec3(scale, 1));
}

void Voxel::UI::Node::updateModelMatrix()
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
//====================================================================================================================================

//============================================================== Canvas ==============================================================

Canvas::Canvas(const glm::vec2& size, const glm::vec2& centerPosition)
	: Node()
	, size(size)
	, centerPosition(centerPosition)
{
	std::cout << "[Canvas] Creating new canvas\n";
	std::cout << "[Canvas] Size (" << size.x << ", " << size.y << ")\n";
	std::cout << "[Canvas] Center (" << centerPosition.x << ", " << centerPosition.y << ")\n";

	contentSize = size;

	updateModelMatrix();
}

void Voxel::UI::Canvas::setSize(const glm::vec2 & size)
{
	this->size = size;
}

void Voxel::UI::Canvas::updateModelMatrix()
{
	modelMat = Camera::mainCamera->getScreenSpaceMatrix() * Voxel::UI::Node::getModelMatrix();
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

void Voxel::UI::Canvas::renderSelf()
{
	if (!visibility) return;

	// parent matrix is screen space matrix
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

	/*
	auto imageShader = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::TEXTURE_SHADER);
	imageShader->use(true);
	imageShader->setUniformMat4("projMat", Camera::mainCamera->getProjection(Camera::UIFovy));

	auto uiMat = Camera::mainCamera->getScreenSpaceMatrix();

	for (auto image : images)
	{
	auto canvasPivot = (image.second)->getCanvasPivot();
	canvasPivot.x *= size.x;
	canvasPivot.y *= size.y;
	glm::mat4 canvasMat = glm::mat4(1.0f);
	canvasMat = glm::translate(canvasMat, glm::vec3(centerPosition, 0.0f));
	canvasMat = glm::translate(canvasMat, glm::vec3(canvasPivot, 0.0f));
	(image.second)->render(uiMat, canvasMat, imageShader);
	}

	auto textShader = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::TEXT_SHADER);
	textShader->use(true);
	textShader->setUniformMat4("projMat", Camera::mainCamera->getProjection(Camera::UIFovy));

	uiMat = Camera::mainCamera->getScreenSpaceMatrix();

	for (auto& text : texts)
	{
	bool outlined = (text.second)->isOutlined();
	if (outlined)
	{
	textShader->setUniformBool("outlined", outlined);
	textShader->setUniformInt("outlineSize", 2);
	textShader->setUniformVec4("outlineColor", (text.second)->getOutlineColor());
	}
	else
	{
	textShader->setUniformBool("outlined", outlined);
	textShader->setUniformInt("outlineSize", 0);
	textShader->setUniformVec4("outlineColor", glm::vec4(1.0f));
	}

	auto canvasPivot = (text.second)->getCanvasPivot();
	canvasPivot.x *= size.x;
	canvasPivot.y *= size.y;
	glm::mat4 canvasMat = glm::mat4(1.0f);
	canvasMat = glm::translate(canvasMat, glm::vec3(centerPosition, 0.0f));
	canvasMat = glm::translate(canvasMat, glm::vec3(canvasPivot, 0.0f));
	(text.second)->render(uiMat, canvasMat, textShader);
	}
	*/
}

//====================================================================================================================================

//============================================================== Image ===============================================================

Voxel::UI::Image::Image(const std::string& name)
	: Node(name)
	, vao(0)
	, indicesSize(0)
	, texture(nullptr)
#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX
	, bbVao(0)
#endif
{
	program = ProgramManager::getInstance().getProgram(Voxel::ProgramManager::PROGRAM_NAME::UI_TEXTURE_SHADER);
}

Image::~Image()
{
	if (vao)
	{
		// Delte array
		glDeleteVertexArrays(1, &vao);
	}
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

	auto size = texture->getTextureSize();

	auto vertices = Quad::getVertices(glm::vec2(size));

	boundingBox.center = position;
	boundingBox.size = size;

	contentSize = size;

	build(vertices, Quad::getColors(glm::vec4(1.0f)), Quad::uv, Quad::indices);

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
	auto vertices = Quad::getVertices(size);

	auto& uvOrigin = imageEntry->uvOrigin;
	auto& uvEnd = imageEntry->uvEnd;

	std::vector<float> uvs =
	{
		uvOrigin.x, uvOrigin.y,
		uvOrigin.x, uvEnd.y,
		uvEnd.x, uvOrigin.y,
		uvEnd.x, uvEnd.y
	};

	boundingBox.center = position;
	boundingBox.size = size;

	contentSize = size;
	
	build(vertices, Quad::getColors(glm::vec4(1.0f)), uvs, Quad::indices);

	return true;
}

void Voxel::UI::Image::build(const std::vector<float>& vertices, const std::vector<float>& colors, const std::vector<float>& uvs, const std::vector<unsigned int>& indices)
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * vertices.size(), &vertices.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLint colorLoc = program->getAttribLocation("color");

	GLuint cbo;
	glGenBuffers(1, &cbo);
	glBindBuffer(GL_ARRAY_BUFFER, cbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors) * colors.size(), &colors.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLint uvVertLoc = program->getAttribLocation("uvVert");

	GLuint uvbo;
	glGenBuffers(1, &uvbo);
	glBindBuffer(GL_ARRAY_BUFFER, uvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvs) * uvs.size(), &uvs.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(uvVertLoc);
	glVertexAttribPointer(uvVertLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//============= find error here. error count: 14. Only during using sprite sheet
	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) * indices.size(), &indices.front(), GL_STATIC_DRAW);
	//========================

	indicesSize = indices.size();

	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &cbo);
	glDeleteBuffers(1, &uvbo);
	glDeleteBuffers(1, &ibo);


#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX
	glGenVertexArrays(1, &bbVao);
	glBindVertexArray(bbVao);

	auto min = boundingBox.getMin();
	auto max = boundingBox.getMax();

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices) * lineVertices.size(), &lineVertices.front(), GL_STATIC_DRAW);

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

void Voxel::UI::Image::renderSelf()
{
	// only render self
	if (texture == nullptr) return;
	if (indicesSize == 0) return;
	if (!visibility) return;

	program->use(true);
	program->setUniformMat4("modelMat", modelMat);

	texture->activate(GL_TEXTURE0);
	texture->bind();

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX
	auto lineProgram = ProgramManager::getInstance().getProgram(Voxel::ProgramManager::PROGRAM_NAME::LINE_SHADER);
	lineProgram->use(true);
	lineProgram->setUniformMat4("modelMat", modelMat);
	lineProgram->setUniformMat4("viewMat", glm::mat4(1.0f));

	glBindVertexArray(bbVao);
	glDrawArrays(GL_LINES, 0, 8);
#endif
}

//====================================================================================================================================

//========================================================= Text =====================================================================

Voxel::UI::Text::Text(const std::string& name)
	: Node(name)
	, text("")
	, align(ALIGN::LEFT)
	, indicesSize(0)
	, outlined(false)
	, color(1.0f)
	, outlineColor(0.0f)
	, vao(0)
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

Text * Voxel::UI::Text::createWithOutline(const std::string & name, const std::string & text, const int fontID, const glm::vec4 & outlineColor, const ALIGN align)
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

bool Voxel::UI::Text::initWithOutline(const std::string & text, const glm::vec4 & outlineColor, ALIGN align)
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

glm::vec4 Voxel::UI::Text::getOutlineColor() const
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
		int lineGapHeight = 2 + static_cast<int>(outlineSize);
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
				totalWidth += glyph->advance;

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

			if (totalWidth > maxWidth)
			{
				maxWidth = totalWidth;
			}

			if (line.empty())
			{
				line = " ";
				// quick hack here. Add whitespace to emptyline to treat as line
				lineSizes.back().width = 0;
				lineSizes.back().maxBearingY = font->getCharGlyph(' ')->height;
				lineSizes.back().maxBotY = 0;

				// For MunroSmall sized 20, linespace was 34 which was ridiculous. 
				// I'm going to customize just for MunroSmall. 
				//totalHeight += lineSizes.back().maxBearingY;
				lineGap += lineGapHeight;
				totalHeight += lineSpace;
			}
			else
			{
				lineSizes.back().width = totalWidth;
				lineSizes.back().maxBearingY = maxBearingY;
				lineSizes.back().maxBotY = maxBotY;

				// For MunroSmall sized 20, linespace was 34 which was ridiculous. 
				// I'm going to customize just for MunroSmall. 
				//totalHeight += (maxBearingY + maxBotY);
				lineGap += lineGapHeight;
				totalHeight += lineSpace;
			}
		}

		lineGap -= lineGapHeight;

		if (split.size() == 1)
		{
			lineGap = 0;
		}

		// Make max width and height even number because this is UI(?)
		if (maxWidth % 2 == 1)
		{
			maxWidth++;
		}

		if (totalHeight % 2 == 1)
		{
			totalHeight++;
		}

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
						colors.push_back(color.a);
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

		// Allocate empty buffer for max length. 16 vertices(4 vec4) per char * max length
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * textSize * 16, nullptr, GL_DYNAMIC_DRAW);
		// fill buffer
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * colors.size(), &colors.front());

		glEnableVertexAttribArray(colorLoc);
		glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

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

	font->activateTexture(GL_TEXTURE0);
	font->bind();

	program->use(true);
	program->setUniformMat4("modelMat", modelMat);

	if (outlined)
	{
		program->setUniformBool("outlined", true);
		program->setUniformInt("outlineSize", 2);
		program->setUniformVec4("outlineColor", outlineColor);
	}
	else
	{
		program->setUniformBool("outlined", false);
	}

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);
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
	auto colors = Quad::getColors(glm::vec4(1.0f));

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * vertices.size(), &vertices.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(vertLoc);	// error count 2
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLint colorLoc = program->getAttribLocation("color");

	GLuint cbo;
	glGenBuffers(1, &cbo);
	glBindBuffer(GL_ARRAY_BUFFER, cbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors) * colors.size(), &colors.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLint uvVertLoc = program->getAttribLocation("uvVert");

	glGenBuffers(1, &uvbo);
	glBindBuffer(GL_ARRAY_BUFFER, uvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uv) * uv.size(), &uv.front(), GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(uvVertLoc);
	glVertexAttribPointer(uvVertLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) * indices.size(), &indices.front(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &cbo);
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

void Voxel::UI::Cursor::setPosition(const glm::vec2 & position)
{
	this->position = position;

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

glm::vec2 Voxel::UI::Cursor::getPosition()
{
	return position;
}

glm::vec3 Voxel::UI::Cursor::getWorldPosition()
{
	return glm::vec3(Camera::mainCamera->getScreenSpaceMatrix() * glm::vec4(position.x, position.y, 0.0f, 1.0f));
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