#include "Text.h"

// cpp
#include <iostream>
#include <string>
#include <sstream>

// voxel
#include "FontManager.h"
#include "Font.h"
#include "ProgramManager.h"
#include "Program.h"
#include "Texture2D.h"

// glm
#include <glm/gtx/transform.hpp>

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

Voxel::UI::Text * Voxel::UI::Text::create(const std::string & name, const std::string & text, const int fontID, const ALIGN align)
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

Voxel::UI::Text * Voxel::UI::Text::createWithOutline(const std::string & name, const std::string & text, const int fontID, const glm::vec3 & outlineColor, const ALIGN align)
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
	// Must bind vertex array to 0. If it's bind to other, it might deleter other buffer(?). This fixes bug.
	glBindVertexArray(0);

	// Delte buffers
	if (vbo)
	{
		glDeleteBuffers(1, &vbo);
		vbo = 0;
	}

	if (cbo)
	{
		glDeleteBuffers(1, &cbo);
		cbo = 0;
	}

	if (uvbo)
	{
		glDeleteBuffers(1, &uvbo);
		ibo = 0;
	}

	if (ibo)
	{
		glDeleteBuffers(1, &ibo);
		uvbo = 0;
	}
	
	// vao is released on RenderNode

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX
	if (bbVao)
	{
		// Delte array
		glDeleteVertexArrays(1, &bbVao);
		bbVao = 0;
	}
#endif
}

bool Voxel::UI::Text::buildMesh(const bool reallocate)
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
		loadBuffers(vertices, colors, uvVertices, indices, reallocate);

		return true;
	}
	else
	{
		std::cout << "[Text] Error: Font is nullptr\n";
		return false;
	}
}

void Voxel::UI::Text::loadBuffers(const std::vector<float>& vertices, const std::vector<float>& colors, const std::vector<float>& uvs, const std::vector<unsigned int>& indices, const bool reallocate)
{
	auto textSize = text.size();

	if (reallocate)
	{
		// reallocate buffer
		//clear();

		// check program
		if (program == nullptr)
		{
			// assign
			program = ProgramManager::getInstance().getProgram(ProgramManager::PROGRAM_NAME::UI_TEXT_SHADER);
		}

		// Get vert location
		GLint vertLoc = program->getAttribLocation("vert");

		// if vao is 0, gen.
		if (vao == 0)
		{
			glGenVertexArrays(1, &vao);
			std::cout << "Text() " << name << ", vao = " << vao << "\n";
		}

		// bind
		glBindVertexArray(vao);

		// if vbo is 0, gen
		if (vbo == 0)
		{
			glGenBuffers(1, &vbo);
			std::cout << "Text() " << name << ", vbo = " << vbo << "\n";
		}

		// bind
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		// Allocate empty buffer for max length. 12 vertices(4 vec3) per char * max length
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * textSize * 12, nullptr, GL_DYNAMIC_DRAW);
		// fill buffer
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * vertices.size(), &vertices.front());
		// enable location
		glEnableVertexAttribArray(vertLoc);
		// set
		glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		// get color location
		GLint colorLoc = program->getAttribLocation("color");

		// if cbo is 0, gen
		if (cbo == 0)
		{
			glGenBuffers(1, &cbo);
			std::cout << "Text() " << name << ", cbo = " << cbo << "\n";
		}

		// bind
		glBindBuffer(GL_ARRAY_BUFFER, cbo);

		// Allocate empty buffer for max length. 12 vertices(4 vec3) per char * max length
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * textSize * 12, nullptr, GL_DYNAMIC_DRAW);
		// fill buffer
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * colors.size(), &colors.front());
		// enable location
		glEnableVertexAttribArray(colorLoc);
		// set
		glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		// get uvVert location
		GLint uvVertLoc = program->getAttribLocation("uvVert");

		// if uvbo is 0, gen
		if (uvbo == 0)
		{
			glGenBuffers(1, &uvbo);
			std::cout << "Text() " << name << ", uvbo = " << uvbo << "\n";
		}

		// bind
		glBindBuffer(GL_ARRAY_BUFFER, uvbo);

		// Allocate empty buffer for max length. 8 verticies (4 vec2) per char * max len
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * textSize * 8, nullptr, GL_DYNAMIC_DRAW);
		// fill buffer
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * uvs.size(), &uvs.front());
		// enable
		glEnableVertexAttribArray(uvVertLoc);
		// set
		glVertexAttribPointer(uvVertLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

		// if ibo is 0, gen
		if (ibo == 0)
		{
			glGenBuffers(1, &ibo);
			std::cout << "Text() " << name << ", ibo = " << ibo << "\n";
		}

		// bind
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

		// Allocate empty buffer for max length. 6 indices ( 2 tri) per char * max len
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * textSize * 8, nullptr, GL_DYNAMIC_DRAW);
		// fill buffer
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned int) * indices.size(), &indices.front());
		// indices don't have to enable and set.

		// get size
		indicesSize = indices.size();

		// unbind
		glBindVertexArray(0);

		/*
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &cbo);
		glDeleteBuffers(1, &uvbo);
		glDeleteBuffers(1, &ibo);

		vbo = 0;
		cbo = 0;
		ibo = 0;
		uvbo = 0;
		*/
	}
	else
	{
		// No need to reallocate buffer.

		// bind vao
		glBindVertexArray(vao);

		// bind vbo
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		// update data
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * vertices.size(), &vertices.front());

		glBindBuffer(GL_ARRAY_BUFFER, cbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * colors.size(), &colors.front());

		glBindBuffer(GL_ARRAY_BUFFER, uvbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * uvs.size(), &uvs.front());

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(float) * indices.size(), &indices.front());

		// update size
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
	if (text.empty()) return;
	if (font == nullptr) return;
	if (vao == 0) return;
	if (program == nullptr) return;

	texture->activate(GL_TEXTURE0);
	texture->bind();

	program->use(true);
	program->setUniformMat4("modelMat", glm::scale(modelMat, glm::vec3(scale, 1)));
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