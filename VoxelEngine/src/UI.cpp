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
	, parentCanvas(nullptr)
	, parentPivot(glm::vec2(0))
	, position(0)
	, scale(1)
	, modelMatrix(1.0f)
	, visible(true)
	, size(0)
	, opacity(0)
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
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLint uvVertLoc = program->getAttribLocation("uvVert");

	GLuint uvbo;
	glGenBuffers(1, &uvbo);
	glBindBuffer(GL_ARRAY_BUFFER, uvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvs) * uvs.size(), &uvs.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(uvVertLoc);
	glVertexAttribPointer(uvVertLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//============= find error here. error count: 10
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
	auto imageShader = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::SHADER_TEXTURE_COLOR);
	imageShader->use(true);
	imageShader->setUniformMat4("projMat", Camera::mainCamera->getProjection(Camera::UIFovy));
	
	texture->activate(GL_TEXTURE0);
	texture->bind();

	imageShader->setUniformMat4("modelMat", glm::mat4(1.0f));

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);
	//glBindVertexArray(0);
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

void Voxel::UI::Image::buildVertices(std::vector<float>& bVertices, std::vector<unsigned int>& bIndices, std::vector<float>& bColors, std::vector<float>& bUVs, const unsigned int indexStart, const glm::mat4& parentMat)
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

			program->setUniformMat4("modelMat", uiMat);

			glBindVertexArray(vao);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
	}
}


//============================================================================================

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
		for (auto batch : batches)
		{
			delete batch;
		}

		batches.clear();

		batches.push_back(new UIBatch());

		UIBatch* curBatch = batches.front();

		std::vector<float> vertices;
		std::vector<unsigned int> indices;
		std::vector<float> colors;
		std::vector<float> uvs;

		unsigned int curTextureID = -1;

		unsigned int indexCounter = 0;

		for (auto& e : uiNodes)
		{
			// skip Text
			if ((e.second)->isRenderable())
			{
				auto texture = (e.second)->getTexture();

				if (texture)
				{
					if (curTextureID == -1)
					{
						curTextureID = texture->getID();
						curBatch->texture = texture;

						// batch it
						auto parentPivotMat = (e.second)->getParentPivot();
						parentPivotMat.x *= size.x;
						parentPivotMat.y *= size.y;
						glm::mat4 parentMat = glm::mat4(1.0f);
						parentMat = glm::translate(parentMat, glm::vec3(position, 0.0f));
						parentMat = glm::translate(parentMat, glm::vec3(parentPivotMat, 0.0f));

						(e.second)->buildVertices(vertices, indices, colors, uvs, indexCounter, parentMat);

						indexCounter += (e.second)->getIndicesOffset();
					}
					else
					{
						if (curTextureID == texture->getID())
						{
							// batch it
							auto parentPivotMat = (e.second)->getParentPivot();
							parentPivotMat.x *= size.x;
							parentPivotMat.y *= size.y;
							glm::mat4 parentMat = glm::mat4(1.0f);
							parentMat = glm::translate(parentMat, glm::vec3(position, 0.0f));
							parentMat = glm::translate(parentMat, glm::vec3(parentPivotMat, 0.0f));

							(e.second)->buildVertices(vertices, indices, colors, uvs, indexCounter, parentMat);

							indexCounter += (e.second)->getIndicesOffset();
						}
						else
						{
							curBatch->texture = texture;
							curBatch->load(vertices, indices, colors, uvs);

							// create new batch

							batches.push_back(new UIBatch());
							curBatch = batches.back();

							curTextureID = texture->getID();

							vertices.clear();
							indices.clear();
							indexCounter = 0;
							colors.clear();
							uvs.clear();
						}
					}
				}
				else
				{
					continue;
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

		std::cout << "[Canvas] Total batch: " << batches.size() << "\n";

		needToUpdateBatch = false;
	}
}

void Voxel::UI::Canvas::render()
{
	for (auto e : batches)
	{
		e->render();
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