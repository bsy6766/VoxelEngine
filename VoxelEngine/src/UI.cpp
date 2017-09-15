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
#include <glm/gtx/transform.hpp>

using namespace Voxel::UI;

Voxel::UI::UINode::UINode()
	: pivot(glm::vec2(0))
	, canvasPivot(glm::vec2(0))
	, position(0)
	, scale(1)
	, modelMatrix(1.0f)
	, visible(true)
	, boxMin(0)
	, boxMax(0)
	, size(0)
{
}

void Voxel::UI::UINode::updateMatrix()
{
	// Move to pos, move by pivot, then scale
	modelMatrix = glm::scale(glm::translate(glm::translate(glm::mat4(1.0f), glm::vec3(position, 0)), glm::vec3(pivot * size * -1.0f, 0)), glm::vec3(scale, 1));
}

void Voxel::UI::UINode::setCanvasPivot(const glm::vec2 & pivot)
{
	canvasPivot = pivot;
}

glm::vec2 Voxel::UI::UINode::getCanvasPivot()
{
	return canvasPivot;
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

glm::mat4 Voxel::UI::UINode::getModelMatrix()
{
	return modelMatrix;
}

glm::vec4 Voxel::UI::UINode::getBoundingBox()
{
	auto min = vec4(boxMin, 1.0f, 1.0f);
	auto max = vec4(boxMax, 1.0f, 1.0f);

	min = modelMatrix * min;
	max = modelMatrix * max;

	return glm::vec4(min.x, min.y, max.x, max.y);
}







Voxel::UI::Text::Text()
	: UINode()
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
				std::cout << "[TEXT] Static text's can't be modified. Use Dynamic Text" << std::endl;
			}
			else
			{
				// Can modify text. reject larger texts
				if (text.length() >= maxTextLength)
				{
					std::cout << "[Text] Can't not rebuild text over initial maximum size" << std::endl;
					return;
				}
				else
				{
					this->text = text;
					buildMesh(true);
				}
			}
		}
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
				std::cout << "[Text] Can't not rebuild text over initial maximum size" << std::endl;
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
				totalHeight += lineSizes.back().maxBearingY;
				lineGap += lineGapHeight;
				//totalHeight += font->getLineSpace();
			}
			else
			{
				lineSizes.back().width = totalWidth;
				lineSizes.back().maxBearingY = maxBearingY;
				lineSizes.back().maxBotY = maxBotY;

				// For MunroSmall sized 20, linespace was 34 which was ridiculous. 
				// I'm going to customize just for MunroSmall. 
				totalHeight += (maxBearingY + maxBotY);
				lineGap += lineGapHeight;
				//totalHeight += font->getLineSpace();
			}
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
		this->boxMin = glm::vec2(static_cast<float>(maxWidth) * -0.5f, static_cast<float>(totalHeight + (lineGap - lineGapHeight)) * -0.5f);
		this->boxMax = glm::vec2(static_cast<float>(maxWidth) * 0.5f, static_cast<float>(totalHeight + (lineGap - lineGapHeight)) * 0.5f);

		this->setSize(glm::vec2(boxMax.x - boxMin.x, boxMax.y - boxMin.y));

		this->updateMatrix();


		// Pen position. Also called as origin or base line in y pos.
		std::vector<glm::vec2> penPositions;
		// Current Y. Because we are using horizontal layout, we advance y in negative direction (Down), starting from height point, which is half of max height
		float curY = static_cast<float>(totalHeight + (lineGap - lineGapHeight)) * 0.5f;
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
			penPos.y = curY - lineSize.maxBearingY;
			// add pen position
			penPositions.push_back(penPos);
			// Update y to next line
			// I might advance to next line with linespace value, but I have to modify the size of line then. TODO: Consider this
			//curY -= font->getLineSpace();
			// As I said, customizing for MunroSmall
			curY -= (lineSize.maxBearingY + lineSize.maxBotY + lineGapHeight);
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
		std::cout << "[Text] Error: Font is nullptr" << std::endl;
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
					std::cout << "[Text] Glyph is invalid" << std::endl;
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

	font->activateTexture(GL_TEXTURE0);
	font->bind();

	prog->setUniformMat4("modelMat", screenMat * canvasPivotMat * modelMatrix);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);
}



Image::Image()
	: UINode()
	, texture(nullptr)
{

}

Image::~Image()
{
	if (texture)
	{
		delete texture;
	}

	// Delte array
	glDeleteVertexArrays(1, &vao);
}

Image* Image::create(const std::string& textureName, const glm::vec2& screenPosition, const glm::vec4& color)
{
	auto newImage = new Image();

	if (newImage->init(textureName, screenPosition, color))
	{
		return newImage;
	}
	else
	{
		delete newImage;
		return nullptr;
	}
}

void Voxel::UI::Image::render(const glm::mat4& screenMat, const glm::mat4& canvasPivotMat, Program* prog)
{
	if (!visible) return;
	if (!texture) return;

	texture->activate(GL_TEXTURE0);
	texture->bind();

	prog->setUniformMat4("modelMat", screenMat * canvasPivotMat * modelMatrix);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	//glBindVertexArray(0);
}

bool Voxel::UI::Image::init(const std::string& textureName, const glm::vec2& screenPosition, const glm::vec4& color)
{
	texture = Texture2D::create(textureName, GL_TEXTURE_2D);

	if (texture == nullptr)
	{
		return false;
	}

	texture->setLocationOnProgram(ProgramManager::PROGRAM_NAME::SHADER_TEXTURE_COLOR);

	position = screenPosition;

	auto size = texture->getTextureSize();

	auto vertices = Quad::getVertices(glm::vec2(size));
	auto indices = Quad::indices;
	auto colors = Quad::getColors(color);
	auto uv = Quad::uv;

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(uv) * uv.size(), &uv.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(uvVertLoc);
	glVertexAttribPointer(uvVertLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) * indices.size(), &indices.front(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	this->boxMin = glm::vec2(vertices.at(6), vertices.at(7));
	this->boxMax = glm::vec2(vertices.at(3), vertices.at(4));

	this->updateMatrix();

	this->setSize(glm::vec2(boxMax.x - boxMin.x, boxMax.y - boxMin.y));

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &cbo);
	glDeleteBuffers(1, &uvbo);
	glDeleteBuffers(1, &ibo);

	return true;
}




Voxel::UI::Cursor::Cursor()
	: vao(0)
	, visible(false)
	, position(0)
{
}

Voxel::UI::Cursor::~Cursor()
{
	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
	}
}


bool Voxel::UI::Cursor::init()
{
	// Initialize cursors

	// pointer
	auto texture = Texture2D::create("cursors/pointer.png", GL_TEXTURE_2D);

	texture->setLocationOnProgram(ProgramManager::PROGRAM_NAME::SHADER_TEXTURE_COLOR);
	
	auto size = texture->getTextureSize();

	auto vertices = Quad::getVertices(glm::vec2(size));
	auto indices = Quad::indices;
	auto colors = Quad::getColors(glm::vec4(1.0f));
	auto uv = Quad::uv;

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(uv) * uv.size(), &uv.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(uvVertLoc);
	glVertexAttribPointer(uvVertLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) * indices.size(), &indices.front(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &cbo);
	glDeleteBuffers(1, &uvbo);
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




const float Canvas::fixedFovy = 70.0f;

Canvas::Canvas()
	: size(0)
	, centerPosition(0)
	, visible(true)
{}

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

Canvas* Canvas::create(const glm::vec2& size, const glm::vec2& centerPosition)
{
	auto newCanvas = new Canvas();
	
	newCanvas->size = size;
	newCanvas->centerPosition = centerPosition;

	std::cout << "[Canvas] Creating new canvas" << std::endl;
	std::cout << "[Canvas] Size (" << size.x << ", " << size.y << ")" << std::endl;
	std::cout << "[Canvas] Center (" << centerPosition.x << ", " << centerPosition.y << ")" << std::endl;

	return newCanvas;
}

/*
bool Voxel::UI::Canvas::addImage(const std::string & name, const std::string & textureName, const glm::vec2& position, const glm::vec4& color)
{
	auto newImage = Image::create(textureName, position, color);
	if (newImage)
	{
		return addImage(name, newImage, 0);
	}
	else
	{
		delete newImage;
		return false;
	}
}
*/

bool Voxel::UI::Canvas::addImage(const std::string & name, Image * image, const int z)
{
	if (image)
	{
		auto find_it = images.find(name);
		if (find_it == images.end())
		{
			images.emplace(name, image);
			return true;
		}
	}
	return false;
}

/*
bool Voxel::UI::Canvas::addText(const std::string & name, const std::string & text, const glm::vec2 & position, const glm::vec4& color, const int fontID, Text::ALIGN align, Text::TYPE type, const int maxLength)
{
	auto newText = Text::create(text, position, color, fontID, align, type, maxLength);
	if (newText)
	{
		return addText(name, newText, 0);
	}
	else
	{
		delete newText;
		return false;
	}
}
*/

bool Voxel::UI::Canvas::addText(const std::string & name, Text * text, const int z)
{
	if (text)
	{
		auto find_it = texts.find(name);
		if (find_it == texts.end())
		{
			texts.emplace(name, text);
			return true;
		}
	}
	return false;
}

void Voxel::UI::Canvas::render()
{
	if (!visible) return;

	auto imageShader = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::SHADER_TEXTURE_COLOR);
	imageShader->use(true);
	imageShader->setUniformMat4("cameraMat", Camera::mainCamera->getProjection(Canvas::fixedFovy));

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

	auto textShader = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::SHADER_TEXT);
	textShader->use(true);
	textShader->setUniformMat4("cameraMat", Camera::mainCamera->getProjection(Canvas::fixedFovy));

	for (auto text : texts)
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
}

Image * Voxel::UI::Canvas::getImage(const std::string & name)
{
	auto find_it = images.find(name);
	if (find_it == images.end())
	{
		return nullptr;
	}
	else
	{
		return find_it->second;
	}
}

Text * Voxel::UI::Canvas::getText(const std::string & name)
{
	auto find_it = texts.find(name);
	if (find_it == texts.end())
	{
		return nullptr;
	}
	else
	{
		return find_it->second;
	}
}

glm::vec2 Voxel::UI::Canvas::getPivotCanvasPos(PIVOT pivot)
{
	auto centerPos = this->centerPosition;

	switch (pivot)
	{
	case Voxel::UI::Canvas::PIVOT::CENTER:
		break;
	case Voxel::UI::Canvas::PIVOT::LEFT:
		centerPos.x -= (size.x * 0.5f);
		break;
	case Voxel::UI::Canvas::PIVOT::RIGHT:
		centerPos.x += (size.x * 0.5f);
		break;
	case Voxel::UI::Canvas::PIVOT::TOP:
		centerPos.y += (size.y * 0.5f);
		break;
	case Voxel::UI::Canvas::PIVOT::BOTTOM:
		centerPos.y -= (size.y * 0.5f);
		break;
	case Voxel::UI::Canvas::PIVOT::LEFT_TOP:
		centerPos.x -= (size.x * 0.5f);
		centerPos.y += (size.y * 0.5f);
		break;
	case Voxel::UI::Canvas::PIVOT::LEFT_BOTTOM:
		centerPos.x -= (size.x * 0.5f);
		centerPos.y -= (size.y * 0.5f);
		break;
	case Voxel::UI::Canvas::PIVOT::RIGHT_TOP:
		centerPos.x += (size.x * 0.5f);
		centerPos.y += (size.y * 0.5f);
		break;
	case Voxel::UI::Canvas::PIVOT::RIGHT_BOTTOM:
		centerPos.x += (size.x * 0.5f);
		centerPos.y -= (size.y * 0.5f);
		break;
	default:
		return glm::vec2(0);
		break;
	}

	return centerPos;
}

void Voxel::UI::Canvas::setSize(const glm::vec2 & size)
{
	this->size = size;
}

void Voxel::UI::Canvas::setVisibility(const bool visibility)
{
	visible = visibility;
}