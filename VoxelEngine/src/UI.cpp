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
#include <Utility.h>

using namespace Voxel::UI;

Voxel::UI::UINode::UINode(const std::string& name)
	: pivot(glm::vec2(0))
	, name(name)
	, parentCanvas(nullptr)
	, parentPivot(glm::vec2(0))
	, position(0)
	, scale(1)
	, modelMatrix(1.0f)
	, visible(true)
	, size(0)
	, opacity(1.0f)
	, color(1.0f)
{}

void Voxel::UI::UINode::updateMatrix()
{
	// Move to pos, move by pivot, then scale
	modelMatrix = glm::scale(glm::translate(glm::translate(glm::mat4(1.0f), glm::vec3(position, 0)), glm::vec3(pivot * size * scale * -1.0f, 0)), glm::vec3(scale, 1));
}

void Voxel::UI::UINode::setParentPivot(const glm::vec2 & pivot)
{
	parentPivot = pivot;
}

glm::vec2 Voxel::UI::UINode::getParentPivot()
{
	return parentPivot;
}

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

bool Voxel::UI::UINode::addChild(UINode * child, int zOrder)
{
	return addChild(child, ZOrder(zOrder));
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
		return true;
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

Voxel::Texture2D * Voxel::UI::UINode::getTexture()
{
	return nullptr;
}

void Voxel::UI::UINode::setColor(const glm::vec3 & color)
{
	this->color = color;
}

glm::vec3 Voxel::UI::UINode::getColor()
{
	return color;
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

bool Voxel::UI::UINode::isRenderable()
{
	return false;
}

unsigned int Voxel::UI::UINode::getIndicesOffset()
{
	return 0;
}

void Voxel::UI::UINode::getChildrenInVector(std::vector<UINode*>& nodes, UINode * parent)
{
	if (children.empty())
	{
		return;
	}
	else
	{
		std::vector<UINode*> negativesORder, positiveOrder;

		for (auto& e : children)
		{
			if ((e.first).globalZOrder < 0)
			{
				if ((e.second)->hasChildren())
				{
					(e.second)->getChildrenInVector(negativesORder, (e.second).get());
				}
				else
				{
					negativesORder.push_back((e.second).get());
				}
			}
			else
			{
				if ((e.second)->hasChildren())
				{
					(e.second)->getChildrenInVector(positiveOrder, (e.second).get());
				}
				else
				{
					positiveOrder.push_back((e.second).get());
				}
			}
		}

		if (!negativesORder.empty())
		{
			nodes.insert(nodes.end(), negativesORder.begin(), negativesORder.end());
		}

		nodes.push_back(parent);

		if (!positiveOrder.empty())
		{
			nodes.insert(nodes.end(), positiveOrder.begin(), positiveOrder.end());
		}
	}
}

void Voxel::UI::UINode::printChildren(const int depth)
{
	std::string str = "";

	for (int i = 0; i < depth; i++)
	{
		str += "\t";
	}

	for (auto& e : children)
	{
		std::cout << str << "ZOrder: (" << e.first.globalZOrder << ", " << e.first.localZOrder << "), Name: " << e.second->getName() << "\n";
		e.second->printChildren(depth + 1);
	}
}

//============================================================================================

Voxel::UI::UIBatch::UIBatch()
	: vao(0)
	, indicesSize(0)
	, texture(nullptr)
	, program(nullptr)
{}

Voxel::UI::UIBatch::~UIBatch()
{
	// Delte array
	glDeleteVertexArrays(1, &vao);

	texture = nullptr;
	program = nullptr;
}

void Voxel::UI::UIBatch::load(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, const std::vector<float>& colors, const std::vector<float>& uvs)
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	auto program = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::SHADER_TEXTURE_COLOR);
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
	glEnableVertexAttribArray(colorLoc);	// error stack: 1
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLint uvVertLoc = program->getAttribLocation("uvVert");

	GLuint uvbo;
	glGenBuffers(1, &uvbo);
	glBindBuffer(GL_ARRAY_BUFFER, uvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvs) * uvs.size(), &uvs.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(uvVertLoc);
	glVertexAttribPointer(uvVertLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//============= find error here. error count: 11
	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) * indices.size(), &indices.front(), GL_STATIC_DRAW);
	//========================
	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &cbo);
	glDeleteBuffers(1, &uvbo);
	glDeleteBuffers(1, &ibo);

	indicesSize = indices.size();
}

void Voxel::UI::UIBatch::render()
{
	if (texture == nullptr || program == nullptr) return;

	program->use(true);
	program->setUniformMat4("projMat", Camera::mainCamera->getProjection(Camera::UIFovy));
	
	texture->activate(GL_TEXTURE0);
	texture->bind();

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);
	//glBindVertexArray(0);
}

//============================================================================================

Voxel::UI::UITextBatch::UITextBatch()
	: vao(0)
	, vbo(0)
	, cbo(0)
	, uvbo(0)
	, ibo(0)
	, texture(nullptr)
	, program(nullptr)
	, outlineSize(0)
{}

Voxel::UI::UITextBatch::~UITextBatch()
{
	// Delte buffers
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &cbo);
	glDeleteBuffers(1, &ibo);
	glDeleteBuffers(1, &uvbo);
	// Delte array
	glDeleteVertexArrays(1, &vao);
}

void Voxel::UI::UITextBatch::load(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, const std::vector<float>& colors, const std::vector<float>& uvs, const unsigned int maxTextLength)
{
	// based on data, load data for the first time
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	auto program = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::SHADER_TEXT);
	GLint vertLoc = program->getAttribLocation("vert");

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// Allocate empty buffer for max length. 12 vertices(4 vec3) per char * max length
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * maxTextLength * 12, nullptr, GL_DYNAMIC_DRAW);
	// fill buffer
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * vertices.size(), &vertices.front());

	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLint colorLoc = program->getAttribLocation("color");

	glGenBuffers(1, &cbo);
	glBindBuffer(GL_ARRAY_BUFFER, cbo);

	// Allocate empty buffer for max length. 16 vertices(4 vec4) per char * max length
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * maxTextLength * 16, nullptr, GL_DYNAMIC_DRAW);
	// fill buffer
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * colors.size(), &colors.front());

	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLint uvVertLoc = program->getAttribLocation("uvVert");

	glGenBuffers(1, &uvbo);
	glBindBuffer(GL_ARRAY_BUFFER, uvbo);

	// Allocate empty buffer for max length. 8 verticies (4 vec2) per char * max len
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * maxTextLength * 8, nullptr, GL_DYNAMIC_DRAW);
	// fill buffer
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * uvs.size(), &uvs.front());

	glEnableVertexAttribArray(uvVertLoc);
	glVertexAttribPointer(uvVertLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	// Allocate empty buffer for max length. 6 indices ( 2 tri) per char * max len
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * maxTextLength * 8, nullptr, GL_DYNAMIC_DRAW);
	// fill buffer
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned int) * indices.size(), &indices.front());

	indicesSize = indices.size();

	glBindVertexArray(0);
}

void Voxel::UI::UITextBatch::update(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, const std::vector<float>& colors, const std::vector<float>& uvs)
{
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
}

void Voxel::UI::UITextBatch::render()
{
	if (indicesSize == 0) return;
	if (!loaded) return;

	if (texture == nullptr || program == nullptr) return;

	program->use(true);
	program->setUniformMat4("projMat", Camera::mainCamera->getProjection(Camera::UIFovy));

	if (outlineSize > 0)
	{
		program->setUniformBool("outlined", true);
		program->setUniformInt("outlineSize", 2);
		program->setUniformVec4("outlineColor", outlineColor);
	}
	else
	{
		program->setUniformBool("outlined", false);
		program->setUniformInt("outlineSize", 0);
		program->setUniformVec4("outlineColor", glm::vec4(1.0f));
	}

	texture->activate(GL_TEXTURE0);
	texture->bind();

	//prog->setUniformMat4("modelMat", screenMat * canvasPivotMat * modelMatrix);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);
}

//============================================================================================

Voxel::UI::Image::Image(const std::string& name)
	: UINode(name)
	, texture(nullptr)	
{}

bool Voxel::UI::Image::init(const std::string & textureName, const glm::vec2 & position, const glm::vec3 & color, const float opacity)
{
	// find texture
	auto& tm = TextureManager::getInstance();

	if (tm.hasTexture(textureName))
	{
		this->texture = tm.getTexture(textureName).get();

		this->position = position;
		this->color = color;
		this->opacity = opacity;

		this->size = texture->getTextureSize();

		this->uvs = Quad::uv;

		updateMatrix();
	}
	else
	{
		texture = Texture2D::create(textureName, GL_TEXTURE_2D);

		if (texture == nullptr)
		{
			return false;
		}

		texture->setLocationOnProgram(ProgramManager::PROGRAM_NAME::SHADER_TEXTURE_COLOR);
	}

	auto size = texture->getTextureSize();

	return true;
}

bool Voxel::UI::Image::initWithSpriteSheet(const ImageEntry* imageEntry, Texture2D * texture, const glm::vec2 & position, const glm::vec3 & color, const float opacity)
{
	if (texture)
	{
		this->texture = texture;

		this->position = position;
		this->color = color;
		this->opacity = opacity;
		
		auto& uvOrigin = imageEntry->uvOrigin;
		auto& uvEnd = imageEntry->uvEnd;

		this->uvs =
		{
			uvOrigin.x, uvOrigin.y,
			uvOrigin.x, uvEnd.y,
			uvEnd.x, uvOrigin.y,
			uvEnd.x, uvEnd.y
		};

		this->size = glm::vec2(imageEntry->width, imageEntry->height);

		updateMatrix();

		return true;
	}
	else
	{
		return false;
	}
}

Image * Voxel::UI::Image::create(const std::string & name, const std::string & textureName, const glm::vec2 & position, const glm::vec3 & color, const float opacity)
{
	if (name.empty()) return nullptr;
	if (textureName.empty()) return nullptr;

	auto newImage = new Image(name);

	if (newImage->init(textureName, position, color, opacity))
	{
		return newImage;
	}
	else
	{
		delete newImage;
		return nullptr;
	}
}

Image * Voxel::UI::Image::createWithSpriteSheet(const std::string & name, const std::string & spriteSheetName, const std::string & imageName, const glm::vec2 & position, const glm::vec3 & color, const float opacity)
{
	if (name.empty()) return nullptr;
	if (spriteSheetName.empty()) return nullptr;
	if (imageName.empty()) return nullptr;

	auto spriteSheet = SpriteSheetManager::getInstance().getSpriteSheet(spriteSheetName);

	if (spriteSheet)
	{
		if (spriteSheet->hasImage(imageName))
		{
			auto newImage = new Image(name);

			if (newImage->initWithSpriteSheet(spriteSheet->getImageEntry(imageName), spriteSheet->getTexture(), position, color, opacity))
			{
				return newImage;
			}
			else
			{
				delete newImage;
				return nullptr;
			}
		}
	}

	return nullptr;
}

bool Voxel::UI::Image::buildVertices(std::vector<float>& bVertices, std::vector<unsigned int>& bIndices, std::vector<float>& bColors, std::vector<float>& bUVs, const unsigned int indexStart, const glm::mat4& parentMat)
{
	auto temp = Quad::getVertices(glm::vec2(size));
	std::vector<float> vertices;

	auto uiMat = Camera::mainCamera->getScreenSpaceMatrix();

	for(unsigned int i = 0; i < temp.size(); i+=3)
	{
		auto mv = uiMat * parentMat * modelMatrix * glm::vec4(temp.at(i), temp.at(i + 1), temp.at(i + 2), 1.0f);
		vertices.push_back(mv.x);
		vertices.push_back(mv.y);
		vertices.push_back(mv.z);
	}

	auto indices = Quad::indices;

	for (auto& i : indices)
	{
		i += indexStart;
	}

	auto colors = Quad::getColors(glm::vec4(color, opacity));

	bVertices.insert(bVertices.end(), vertices.begin(), vertices.end());
	bIndices.insert(bIndices.end(), indices.begin(), indices.end());
	bColors.insert(bColors.end(), colors.begin(), colors.end());
	bUVs.insert(bUVs.end(), this->uvs.begin(), this->uvs.end());

	return true;
}

bool Voxel::UI::Image::isRenderable()
{
	return visible;
}

unsigned int Voxel::UI::Image::getIndicesOffset()
{
	return 4;
}

Voxel::Texture2D* Voxel::UI::Image::getTexture()
{
	return this->texture;
}

//============================================================================================

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

	this->texture->setLocationOnProgram(ProgramManager::PROGRAM_NAME::SHADER_TEXTURE_COLOR);

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

	auto program = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::SHADER_TEXTURE_COLOR);
	GLint vertLoc = program->getAttribLocation("vert");

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * vertices.size(), &vertices.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(vertLoc);	// error count 1
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

void Voxel::UI::Cursor::updateBoudnary()
{
	auto size = glm::vec2(Voxel::Application::getInstance().getGLView()->getScreenSize());

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

void Voxel::UI::Cursor::render()
{
	if (visible)
	{
		if (vao)
		{
			auto program = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::SHADER_TEXTURE_COLOR);
			program->use(true);
			program->setUniformMat4("projMat", Camera::mainCamera->getProjection(Camera::UIFovy));

			auto uiMat = glm::translate(glm::translate(Camera::mainCamera->getScreenSpaceMatrix(), glm::vec3(position.x, position.y, 0)), glm::vec3(-pivot.x * size.x, -pivot.y * size.y, 0));

			texture->activate(GL_TEXTURE0);
			texture->bind();
			
			glBindVertexArray(vao);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
	}
}


//============================================================================================

Voxel::UI::Text2::Text2(const std::string& name)
	: UINode(name)
	, text("")
	, align(ALIGN::LEFT)
	, maxTextLength(0)
	, outlineColor(1.0f)
{}

Text2 * Voxel::UI::Text2::create(const std::string& name, const std::string& text, const glm::vec2& position, const glm::vec4& color, const int fontID, ALIGN align, const int maxLength)
{
	Text2* newText = new Text2(name);
	newText->font = FontManager::getInstance().getFont(fontID);

	if (newText->font != nullptr)
	{
		if (newText->init(text, position, color, align, maxLength))
		{
			return newText;
		}
	}

	delete newText;
	newText = nullptr;
	return nullptr;
}

Text2 * Voxel::UI::Text2::createWithOutline(const std::string& name, const std::string & text, const glm::vec2 & position, const int fontID, const glm::vec4 & color, const glm::vec3 & outlineColor, ALIGN align, const int maxLength)
{
	Text2* newText = new Text2(name);
	newText->font = FontManager::getInstance().getFont(fontID);

	if (newText->font != nullptr)
	{
		// Check if font supports outline
		if (newText->font->isOutlineEnabled())
		{
			if (newText->initWithOutline(text, position, color, outlineColor, align, maxLength))
			{
				return newText;
			}
		}
	}

	delete newText;
	newText = nullptr;
	return nullptr;
}

void Voxel::UI::Text2::setText(const std::string & text)
{
	if (!text.empty())
	{
		if (text != this->text)
		{
			// Can modify text. reject larger texts
			if (text.length() >= maxTextLength)
			{
				std::cout << "[Text] Can't not rebuild text over initial maximum size\n";
				return;
			}
			else
			{
				this->text = text;
				/*
				if (vao)
				{
					buildMesh(true);
				}
				else
				{
					buildMesh(false);
				}
				*/
			}
		}
	}
	else
	{
		clear();
	}
}

std::string Voxel::UI::Text2::getText()
{
	return text;
}

bool Voxel::UI::Text2::isOutlined()
{
	return font->getOutlineSize() > 0;
}

void Voxel::UI::Text2::setColor(const glm::vec4 & color)
{
	this->color = color;

	// Todo: rebuild color buffer
}

glm::vec4 Voxel::UI::Text2::getOutlineColor()
{
	return glm::vec4(outlineColor, opacity);
}

unsigned int Voxel::UI::Text2::getMaxTextLength()
{
	return maxTextLength;
}

void Voxel::UI::Text2::clear()
{
	text = "";
}

Voxel::UI::Text2::~Text2()
{
}

bool Voxel::UI::Text2::init(const std::string & text, const glm::vec2& position, const glm::vec4& color, ALIGN align, const int maxLength)
{
	this->text = text;
	this->align = align;
	this->color = color;
	this->position = position;
	this->align = align;
	this->maxTextLength = maxLength;
	//color will be same for all color for now
	// Todo: make char quad to have own separate color

	return true;
}

bool Voxel::UI::Text2::initWithOutline(const std::string & text, const glm::vec2 & position, const glm::vec4 & color, const glm::vec3 & outlineColor, ALIGN align, const int maxLength)
{
	this->text = text;
	this->align = align;
	this->color = color;
	this->outlineColor = outlineColor;
	this->position = position;
	this->align = align;
	this->maxTextLength = maxLength;

	return true;
}

std::vector<glm::vec2> Voxel::UI::Text2::computeOrigins(Font * font, const std::vector<std::string>& split)
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

bool Voxel::UI::Text2::isRenderable()
{
	return visible && !text.empty();
}

int Voxel::UI::Text2::getOutlineSize()
{
	if (font)
	{
		return font->getOutlineSize();
	}
	else
	{
		return 0;
	}
}

Voxel::Texture2D * Voxel::UI::Text2::getTexture()
{
	if (font)
	{
		return font->getTexture();
	}
	else
	{
		return nullptr;
	}
}

bool Voxel::UI::Text2::buildVertices(std::vector<float>& bVertices, std::vector<unsigned int>& bIndices, std::vector<float>& bColors, std::vector<float>& bUVs, const unsigned int indexStart, const glm::mat4 & parentMat)
{
	if (font)
	{
		if (text.empty())
		{
			return true;
		}

		if (text.length() >= this->maxTextLength)
		{
			// Todo: automatically reallocate buffer with new size
			std::cout << "[Text] Can't not rebuild text over initial maximum size\n";
			return false;
		}

		// Split text label by line
		std::vector<std::string> split;
		std::stringstream ss(text); // Turn the string into a stream.
		std::string tok;
		while (getline(ss, tok, '\n'))
		{
			split.push_back(tok);
		}

		struct LineSize
		{
			int width;
			int maxBearingY;
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
		glm::vec2 boxMin = glm::vec2(static_cast<float>(maxWidth) * -0.5f, static_cast<float>(totalHeight + (lineGap)) * -0.5f);
		glm::vec2 boxMax = glm::vec2(static_cast<float>(maxWidth) * 0.5f, static_cast<float>(totalHeight + (lineGap)) * 0.5f);

		this->setSize(glm::vec2(boxMax.x - boxMin.x, boxMax.y - boxMin.y));

		this->updateMatrix();

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

		unsigned int indicesIndex = 0;

		auto uiMat = Camera::mainCamera->getScreenSpaceMatrix();

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

					glm::vec4 lb = uiMat * parentMat * modelMatrix * glm::vec4(leftBottom, 0.0f, 1.0f);
					glm::vec4 rt = uiMat * parentMat * modelMatrix * glm::vec4(rightTop, 0.0f, 1.0f);

					// Store left bttom and right top vertices pos
					//bVertices.push_back(lb.x); bVertices.push_back(lb.y); bVertices.push_back(lb.z);	// left bottom
					//bVertices.push_back(lb.x); bVertices.push_back(rt.y); bVertices.push_back(lb.z);	// left top
					//bVertices.push_back(rt.x); bVertices.push_back(lb.y); bVertices.push_back(lb.z);		// right bottom
					//bVertices.push_back(rt.x); bVertices.push_back(rt.y); bVertices.push_back(rt.z);

					bVertices.push_back(leftBottom.x); bVertices.push_back(leftBottom.y); bVertices.push_back(lb.z);	// left bottom
					bVertices.push_back(leftBottom.x); bVertices.push_back(rightTop.y); bVertices.push_back(lb.z);	// left top
					bVertices.push_back(rightTop.x); bVertices.push_back(leftBottom.y); bVertices.push_back(lb.z);		// right bottom
					bVertices.push_back(rightTop.x); bVertices.push_back(rightTop.y); bVertices.push_back(rt.z);	// right top


					for (int j = 0; j < 4; j++)
					{
						bColors.push_back(color.r);
						bColors.push_back(color.g);
						bColors.push_back(color.b);
						bColors.push_back(opacity);
					}

					// uv.
					bUVs.push_back(glyph->uvTopLeft.x); bUVs.push_back(glyph->uvBotRight.y);	// Left bottom
					bUVs.push_back(glyph->uvTopLeft.x); bUVs.push_back(glyph->uvTopLeft.y);		// Left top
					bUVs.push_back(glyph->uvBotRight.x); bUVs.push_back(glyph->uvBotRight.y);	// right bottom
					bUVs.push_back(glyph->uvBotRight.x); bUVs.push_back(glyph->uvTopLeft.y);	// right top

																											// indices. range of 4 per quad.
					bIndices.push_back(indicesIndex * 4);
					bIndices.push_back(indicesIndex * 4 + 1);
					bIndices.push_back(indicesIndex * 4 + 2);
					bIndices.push_back(indicesIndex * 4 + 1);
					bIndices.push_back(indicesIndex * 4 + 2);
					bIndices.push_back(indicesIndex * 4 + 3);

					// inc index
					indicesIndex++;
				}
			}

			penPosIndex++;
		}

		/*
		// load buffer
		if (update)
		{
			updateBuffer(vertices, colors, uvVertices, indices);
		}
		else
		{
			loadBuffers(vertices, colors, uvVertices, indices);
		}

		this->loaded = true;
		*/

		return true;
	}
	else
	{
		std::cout << "[Text] Error: Font is nullptr\n";
		return false;
	}
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

	for (auto batch : batches)
	{
		delete batch;
	}
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

bool Voxel::UI::Canvas::addNode(UINode * node, int zOrder)
{
	return addNode(node, ZOrder(zOrder));
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
		if (dynamic_cast<UI::Image*>(node))
		{
			needToUpdateBatch = true;
		}

		uiNodes.insert(std::make_pair(zOrder, std::unique_ptr<UINode>(node)));
	}

	return true;
}

glm::vec2 Voxel::UI::Canvas::getSize()
{
	return size;
}

void Voxel::UI::Canvas::setSize(const glm::vec2 & size)
{
	if (size.x < 0 || size.y < 0)
	{
		return;
	}
	else
	{
		this->size = size;
	}
}

bool Voxel::UI::Canvas::getVisibility()
{
	return visible;
}

void Voxel::UI::Canvas::setVisibility(const bool visibility)
{
	this->visible = visibility;
}

void Voxel::UI::Canvas::updateBatch()
{
	if (needToUpdateBatch)
	{
		auto start = Utility::Time::now();

		for (auto batch : batches)
		{
			delete batch;
		}

		batches.clear();
		
		UIBatch* curBatch = new UIBatch();
		batches.push_back(curBatch);

		std::vector<float> vertices;
		std::vector<unsigned int> indices;
		std::vector<float> colors;
		std::vector<float> uvs;

		unsigned int curTextureID = -1;

		unsigned int indexCounter = 0;

		std::vector<UINode*> nodes;

		for (auto& e : uiNodes)
		{
			// skip Text
			if ((e.second)->isRenderable())
			{
				if ((e.second)->hasChildren())
				{
					(e.second)->getChildrenInVector(nodes, (e.second).get());
				}
				else
				{
					nodes.push_back((e.second).get());
				}
			}
		}
		
		for (auto node : nodes)
		{
			if (auto textObj = dynamic_cast<UI::Text2*>(node))
			{
				auto textBatch = new UITextBatch();
				textBatch->texture = textObj->getTexture();
				textBatch->program = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::SHADER_TEXT);

				std::vector<float> tVertices;
				std::vector<unsigned int> tIndices;
				std::vector<float> tColors;
				std::vector<float> tUVs;

				// batch it
				auto parentPivotMat = textObj->getParentPivot();
				parentPivotMat.x *= size.x;
				parentPivotMat.y *= size.y;
				glm::mat4 parentMat = glm::mat4(1.0f);
				parentMat = glm::translate(parentMat, glm::vec3(position, 0.0f));
				parentMat = glm::translate(parentMat, glm::vec3(parentPivotMat, 0.0f));

				textObj->buildVertices(tVertices, tIndices, tColors, tUVs, 0, parentMat);

				textBatch->load(tVertices, tIndices, tColors, tUVs, textObj->getMaxTextLength());

				textBatch->outlineSize = textObj->getOutlineSize();
				textBatch->outlineColor = glm::vec4(textObj->getOutlineColor());

				batches.push_back(textBatch);

				if (vertices.empty() && indices.empty() && colors.empty() && uvs.empty())
				{

				}
				else
				{
					curBatch->load(vertices, indices, colors, uvs);

					vertices.clear();
					indices.clear();
					indexCounter = 0;
					colors.clear();
					uvs.clear();

					// create new batch
					curBatch = new UIBatch();
					batches.push_back(curBatch);

					curTextureID = -1;
				}
			}
			else
			{
				if (node->isRenderable())
				{
					auto texture = node->getTexture();

					if (texture)
					{
						if (curTextureID == -1)
						{
							curTextureID = texture->getID();
							curBatch->texture = texture;
							curBatch->program = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::SHADER_TEXTURE_COLOR);

							// batch it
							auto parentPivotMat = node->getParentPivot();
							parentPivotMat.x *= size.x;
							parentPivotMat.y *= size.y;
							glm::mat4 parentMat = glm::mat4(1.0f);
							parentMat = glm::translate(parentMat, glm::vec3(position, 0.0f));
							parentMat = glm::translate(parentMat, glm::vec3(parentPivotMat, 0.0f));

							node->buildVertices(vertices, indices, colors, uvs, indexCounter, parentMat);

							indexCounter += node->getIndicesOffset();
						}
						else
						{
							if (curTextureID == texture->getID())
							{
								// batch it
								auto parentPivotMat = node->getParentPivot();
								parentPivotMat.x *= size.x;
								parentPivotMat.y *= size.y;
								glm::mat4 parentMat = glm::mat4(1.0f);
								parentMat = glm::translate(parentMat, glm::vec3(position, 0.0f));
								parentMat = glm::translate(parentMat, glm::vec3(parentPivotMat, 0.0f));

								node->buildVertices(vertices, indices, colors, uvs, indexCounter, parentMat);

								indexCounter += node->getIndicesOffset();
							}
							else
							{
								curBatch->load(vertices, indices, colors, uvs);

								vertices.clear();
								indices.clear();
								indexCounter = 0;
								colors.clear();
								uvs.clear();

								// create new batch
								curBatch = new UIBatch();
								batches.push_back(curBatch);

								curTextureID = texture->getID();

								curBatch->program = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::SHADER_TEXTURE_COLOR);
							}
						}
					}
					else
					{
						continue;
					}
				}
			}
		}

		if (!vertices.empty() && !indices.empty() && !colors.empty() && !uvs.empty())
		{
			curBatch->load(vertices, indices, colors, uvs);
		}
		else
		{
			delete curBatch;
			batches.pop_back();
		}

		auto end = Utility::Time::now();

		std::cout << "[Canvas] Total batch: " << batches.size() << ", UI count: " << nodes.size() << ", t: " << Utility::Time::toMicroSecondString(start, end) << "\n";

		needToUpdateBatch = false;
	}
}

void Voxel::UI::Canvas::render()
{
	for (auto e : batches)
	{
		//e->render();
	}

	if (temp)
	{
		auto textShader = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::SHADER_TEXT);
		textShader->use(true);
		textShader->setUniformMat4("projMat", Camera::mainCamera->getProjection(Camera::UIFovy));

		auto uiMat = Camera::mainCamera->getScreenSpaceMatrix();
		bool outlined = temp->isOutlined();

		if (outlined)
		{
			textShader->setUniformBool("outlined", outlined);
			textShader->setUniformInt("outlineSize", 2);
			textShader->setUniformVec4("outlineColor", temp->getOutlineColor());
		}
		else
		{
			textShader->setUniformBool("outlined", outlined);
			textShader->setUniformInt("outlineSize", 0);
			textShader->setUniformVec4("outlineColor", glm::vec4(1.0f));
		}

		auto canvasPivot = temp->getParentPivot();
		canvasPivot.x *= size.x;
		canvasPivot.y *= size.y;
		glm::mat4 canvasMat = glm::mat4(1.0f);
		/*
		canvasMat = glm::translate(canvasMat, glm::vec3(position, 0.0f));
		canvasMat = glm::translate(canvasMat, glm::vec3(canvasPivot, 0.0f));
		*/

		temp->render(uiMat, canvasMat, textShader);
	}
}

void Voxel::UI::Canvas::print()
{
	std::cout << "[Canvas] Canvas info...\n";
	std::cout << "Size (" << size.x << ", " << size.y << ")\n";
	std::cout << "Pos  (" << position.x << ", " << position.y << ")\n";

	std::cout << "UIs\n";

	for (auto& e : uiNodes)
	{
		std::cout << "ZOrder: (" << e.first.globalZOrder << ", " << e.first.localZOrder << "), Name: " << e.second->getName() << "\n";
		if (e.second->hasChildren())
		{
			e.second->printChildren(1);
		}
	}

	std::cout << "\n";
}


































Voxel::UI::Text::Text()
	: UINode("")
	, text("")
	, maxWidth(0)
	, totalHeight(0)
	, align(ALIGN::LEFT)
	, indicesSize(0)
	, type(TYPE::STATIC)
	, maxTextLength(0)
	, outlined(false)
	, loaded(false)
	, color(1.0f)
	, outlineColor(1.0f)
{
}

Text * Voxel::UI::Text::create(const std::string & text, const glm::vec2& position, const glm::vec4& color, const int fontID, ALIGN align, TYPE type, const int maxLength)
{
	Text* newText = new Text();
	newText->font = FontManager::getInstance().getFont(fontID);

	if (newText->font != nullptr)
	{
		if (newText->init(text, position, color, align, type, maxLength))
		{
			return newText;
		}
	}

	delete newText;
	newText = nullptr;
	return nullptr;
}

Text * Voxel::UI::Text::createWithOutline(const std::string & text, const glm::vec2 & position, const int fontID, const glm::vec4 & color, const glm::vec4 & outlineColor, ALIGN align, TYPE type, const int maxLength)
{
	Text* newText = new Text();
	newText->font = FontManager::getInstance().getFont(fontID);

	if (newText->font != nullptr)
	{
		// Check if font supports outline
		if (newText->font->isOutlineEnabled())
		{
			if (newText->initWithOutline(text, position, color, outlineColor, align, type, maxLength))
			{
				return newText;
			}
		}
	}

	delete newText;
	newText = nullptr;
	return nullptr;
}

void Voxel::UI::Text::setText(const std::string & text)
{
	if (!text.empty())
	{
		if (text != this->text)
		{
			if (type == TYPE::STATIC)
			{
				std::cout << "[TEXT] Static text's can't be modified. Use Dynamic Text\n";
			}
			else
			{
				// Can modify text. reject larger texts
				if (text.length() >= maxTextLength)
				{
					std::cout << "[Text] Can't not rebuild text over initial maximum size\n";
					return;
				}
				else
				{
					this->text = text;
					if (vao)
					{
						buildMesh(true);
					}
					else
					{
						buildMesh(false);
					}
				}
			}
		}
	}
	else
	{
		clear();
	}
}

std::string Voxel::UI::Text::getText()
{
	return text;
}

bool Voxel::UI::Text::isOutlined()
{
	return outlined;
}

void Voxel::UI::Text::setColor(const glm::vec4 & color)
{
	this->color = color;

	// Todo: rebuild color buffer
}

glm::vec4 Voxel::UI::Text::getOutlineColor()
{
	return outlineColor;
}

void Voxel::UI::Text::clear()
{
	text = "";
}

Voxel::UI::Text::~Text()
{
	// Delte buffers
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &cbo);
	glDeleteBuffers(1, &ibo);
	glDeleteBuffers(1, &uvbo);
	// Delte array
	glDeleteVertexArrays(1, &vao);
}

bool Voxel::UI::Text::init(const std::string & text, const glm::vec2& position, const glm::vec4& color, ALIGN align, TYPE type, const int maxLength)
{
	this->text = text;
	this->align = align;
	this->color = color;
	this->position = position;
	this->align = align;
	this->type = type;
	this->maxTextLength = maxLength;
	//color will be same for all color for now
	// Todo: make char quad to have own separate color

	return buildMesh(false);
}

bool Voxel::UI::Text::initWithOutline(const std::string & text, const glm::vec2 & position, const glm::vec4 & color, const glm::vec4 & outlineColor, ALIGN align, TYPE type, const int maxLength)
{
	this->text = text;
	this->align = align;
	this->color = color;
	this->outlineColor = outlineColor;
	this->position = position;
	this->align = align;
	this->type = type;
	this->maxTextLength = maxLength;

	return buildMesh(false);
}

bool Voxel::UI::Text::buildMesh(const bool update)
{
	if (font)
	{
		this->outlined = (font->getOutlineSize() > 0);

		if (text.empty())
		{
			return true;
		}

		if (type == TYPE::DYNAMIC)
		{
			if (text.length() >= this->maxTextLength)
			{
				// Todo: automatically reallocate buffer with new size
				std::cout << "[Text] Can't not rebuild text over initial maximum size\n";
				return false;
			}
		}

		// Split text label by line
		std::vector<std::string> split;
		std::stringstream ss(text); // Turn the string into a stream.
		std::string tok;
		while (getline(ss, tok, '\n'))
		{
			split.push_back(tok);
		}

		struct LineSize
		{
			int width;
			int maxBearingY;
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

		this->setSize(glm::vec2(boxMax.x - boxMin.x, boxMax.y - boxMin.y));

		this->updateMatrix();

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
		if (update)
		{
			updateBuffer(vertices, colors, uvVertices, indices);
		}
		else
		{
			loadBuffers(vertices, colors, uvVertices, indices);
		}

		this->loaded = true;

		return true;
	}
	else
	{
		std::cout << "[Text] Error: Font is nullptr\n";
		return false;
	}

	/*
	// Legacy algorithm back in 2013 inmplemented by myself

	std::vector<float> vertices;
	std::vector<float> colors;
	std::vector<unsigned int> indices;
	std::vector<float> uvVertices;


	if (font)
	{
	// List of origin of each character.
	std::vector<glm::vec2> originList;
	// Iterate through each line and compute origin point of each character to compute vertex
	// Think like how human writes in lined paper.
	originList = computeOrigins(font, split);

	glm::vec2 min(10000, 10000);
	glm::vec2 max(-10000, -10000);

	unsigned int indicesIndex = 0;
	int index = 0;
	for (auto& line : split)
	{
	glm::vec2 origin = originList.at(index);
	index++;
	unsigned int len = line.size();
	for (unsigned int i = 0; i < len; i++)
	{
	const char c = line[i];
	Glyph* glyph = font->getCharGlyph(c);
	if (glyph == nullptr)
	{
	std::cout << "[Text] Failed to find glyph for char: " << c << std::endl;
	continue;
	}

	if (glyph->valid == false)
	{
	std::cout << "[Text] Glyph is invalid\n";
	continue;
	}

	// get data from gylph.
	int bearingY = glyph->bearingY;
	int glyphHeight = glyph->height;
	int glyphWidth = glyph->width;

	// compute vertex quad at origin
	// Left bottom
	glm::vec2 p1 = glm::vec2(glyphWidth / -2, -(glyphHeight - bearingY));	// botY
	// right top
	glm::vec2 p2 = glm::vec2(glyphWidth / 2, bearingY);

	// get point where each char ahs to move (center of quad)
	// y just follow's origin's y because that is the guideline for each line
	glm::vec2 fPos = glm::vec2(origin.x + (static_cast<float>(glyphWidth) * 0.5f), origin.y);
	// fPos is also distance from origin
	// So add to vertex to translate
	p1 += fPos;
	p2 += fPos;

	if (p1.x < min.x)
	{
	min.x = p1.x;
	}
	else if (p2.x > max.x)
	{
	max.x = p2.x;
	}

	if (p1.y < min.y)
	{
	min.y = p1.y;
	}
	else if (p2.y > max.y)
	{
	max.y = p2.y;
	}

	// add to vertices
	vertices.push_back(p1.x); vertices.push_back(p1.y); vertices.push_back(0);	// left bottom
	vertices.push_back(p1.x); vertices.push_back(p2.y); vertices.push_back(0);	// left top
	vertices.push_back(p2.x); vertices.push_back(p1.y); vertices.push_back(0);	// right bottom
	vertices.push_back(p2.x); vertices.push_back(p2.y); vertices.push_back(0);	// right top

	// Add global color for now
	for (int j = 0; j < 16; j++)
	{
	colors.push_back(1.0f);
	}

	// compute uv.
	uvVertices.push_back(glyph->uvTopLeft.x); uvVertices.push_back(glyph->uvBotRight.y);	// Left bottom
	uvVertices.push_back(glyph->uvTopLeft.x); uvVertices.push_back(glyph->uvTopLeft.y);	// Left top
	uvVertices.push_back(glyph->uvBotRight.x); uvVertices.push_back(glyph->uvBotRight.y);	// right bottom
	uvVertices.push_back(glyph->uvBotRight.x); uvVertices.push_back(glyph->uvTopLeft.y);	// right top

	// indices.
	indices.push_back(indicesIndex * 4);
	indices.push_back(indicesIndex * 4 + 1);
	indices.push_back(indicesIndex * 4 + 2);
	indices.push_back(indicesIndex * 4 + 1);
	indices.push_back(indicesIndex * 4 + 2);
	indices.push_back(indicesIndex * 4 + 3);

	indicesIndex++;

	// advance origin
	origin.x += (glyph->metrics.horiAdvance >> 6);
	}
	}

	this->boxMin = min;
	this->boxMax = max;

	this->setSize(glm::vec2(boxMax.x - boxMin.x, boxMax.y - boxMin.y));

	this->updateMatrix();



	if (update)
	{
	updateBuffer(vertices, colors, uvVertices, indices);
	}
	else
	{
	loadBuffers(vertices, colors, uvVertices, indices);
	}

	return true;
	}
	else
	{
	std::cout << "[Text] Error: Failed to build mesh with font id: " << fontID << std::endl;
	return false;
	}
	*/
}

void Voxel::UI::Text::loadBuffers(const std::vector<float>& vertices, const std::vector<float>& colors, const std::vector<float>& uvs, const std::vector<unsigned int>& indices)
{
	// based on data, load data for the first time
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	auto program = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::SHADER_TEXT);
	GLint vertLoc = program->getAttribLocation("vert");

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	if (type == TYPE::STATIC)
	{
		// Allocate buffer just enough for the text
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices.front(), GL_STATIC_DRAW);
	}
	else
	{
		// Allocate empty buffer for max length. 12 vertices(4 vec3) per char * max length
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * maxTextLength * 12, nullptr, GL_DYNAMIC_DRAW);
		// fill buffer
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * vertices.size(), &vertices.front());
	}

	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLint colorLoc = program->getAttribLocation("color");

	glGenBuffers(1, &cbo);
	glBindBuffer(GL_ARRAY_BUFFER, cbo);

	if (type == TYPE::STATIC)
	{
		// Allocate buffer just enough for the text
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * colors.size(), &colors.front(), GL_STATIC_DRAW);
	}
	else
	{
		// Allocate empty buffer for max length. 16 vertices(4 vec4) per char * max length
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * maxTextLength * 16, nullptr, GL_DYNAMIC_DRAW);
		// fill buffer
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * colors.size(), &colors.front());
	}

	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLint uvVertLoc = program->getAttribLocation("uvVert");

	glGenBuffers(1, &uvbo);
	glBindBuffer(GL_ARRAY_BUFFER, uvbo);

	if (type == TYPE::STATIC)
	{
		// Allocate buffer just enough for the text
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * uvs.size(), &uvs.front(), GL_STATIC_DRAW);
	}
	else
	{
		// Allocate empty buffer for max length. 8 verticies (4 vec2) per char * max len
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * maxTextLength * 8, nullptr, GL_DYNAMIC_DRAW);
		// fill buffer
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * uvs.size(), &uvs.front());
	}

	glEnableVertexAttribArray(uvVertLoc);
	glVertexAttribPointer(uvVertLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	if (type == TYPE::STATIC)
	{
		// Allocate buffer just enough for the text
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices.front(), GL_STATIC_DRAW);
	}
	else
	{
		// Allocate empty buffer for max length. 6 indices ( 2 tri) per char * max len
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * maxTextLength * 8, nullptr, GL_DYNAMIC_DRAW);
		// fill buffer
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned int) * indices.size(), &indices.front());
	}

	indicesSize = indices.size();

	glBindVertexArray(0);
}

void Voxel::UI::Text::updateBuffer(const std::vector<float>& vertices, const std::vector<float>& colors, const std::vector<float>& uvs, const std::vector<unsigned int>& indices)
{
	if (type == TYPE::DYNAMIC)
	{
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
	}
}

void Voxel::UI::Text::clearBuffer()
{
	if (type == TYPE::DYNAMIC)
	{
		//instead of delete buffer and all, just render nothing
		indicesSize = 0;
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

void Voxel::UI::Text::render(const glm::mat4& screenMat, const glm::mat4& canvasPivotMat, Program* prog)
{
	if (!visible) return;
	if (indicesSize == 0) return;
	if (!loaded) return;

	if (text.empty()) return;

	font->activateTexture(GL_TEXTURE0);
	font->bind();

	prog->setUniformMat4("modelMat", screenMat * canvasPivotMat * modelMatrix);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);
}