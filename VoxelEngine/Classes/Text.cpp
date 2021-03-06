// pch
#include "PreCompiled.h"

#include "Text.h"

// voxel
#include "FontManager.h"
#include "Font.h"
#include "ProgramManager.h"
#include "Program.h"
#include "Texture2D.h"

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
	, textSize(0)
	, totalLines(0)
	, state(State::IDLE)
#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX && V_DEBUG_DRAW_TEXT_BOUNDING_BOX
	, lineIndicesSize(0)
#endif
{}

Voxel::UI::Text::~Text()
{
	clear();
}

Voxel::UI::Text * Voxel::UI::Text::create(const std::string & name, const std::string & text, const int fontID, const ALIGN align, const unsigned int lineBreakWidth)
{
	auto newText = new Text(name);

	if (newText->init(text, fontID, align, lineBreakWidth))
	{
		return newText;
	}
	else
	{
		delete newText;
		newText = nullptr;

		return nullptr;
	}
}

Voxel::UI::Text * Voxel::UI::Text::createWithOutline(const std::string & name, const std::string & text, const int fontID, const glm::vec3 & outlineColor, const ALIGN align, const unsigned int lineBreakWidth)
{
	Text* newText = new Text(name);

	newText->font = FontManager::getInstance().getFont(fontID);

	if (newText->font != nullptr)
	{
		// Check if font supports outline
		if (newText->font->isOutlineEnabled())
		{
			if (newText->initWithOutline(text, fontID, outlineColor, align, lineBreakWidth))
			{
				return newText;
			}
		}
	}

	delete newText;
	newText = nullptr;

	return nullptr;
}

bool Voxel::UI::Text::init(const std::string & text, const int fontID, const ALIGN align, const unsigned int lineBreakWidth)
{
	auto fm = &FontManager::getInstance();

	if (fm == nullptr) return false;

	this->font = fm->getFont(fontID);

	if (this->font == nullptr) return false;

	this->text = text;
	this->align = align;
	this->lineBreakWidth = lineBreakWidth;

	updateModelMatrix();

	return buildMesh(true);
}

bool Voxel::UI::Text::initWithOutline(const std::string & text, const int fontID, const glm::vec3 & outlineColor, ALIGN align, const unsigned int lineBreakWidth)
{
	this->text = text;
	this->align = align;
	this->outlineColor = outlineColor;
	this->lineBreakWidth = lineBreakWidth;

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
		//clear();
		this->text = text;
		this->contentSize.x = 0.0f;
		this->contentSize.y = 0.0f;
		this->boundingBox.size.x = 0.0f;
		this->boundingBox.size.y = 0.0f;

		updateModelMatrix();
		updateBoundingBox();

		lineSizes.clear();

		totalLines = 1;
	}
	else
	{
		// Not empty. Check if it's different.
		if (text != this->text)
		{
			bool update = false;

			// Check if new text is longer than current text. if so, we have to reallocate the buffer. Else, do nothing
			if (text.size() > textSize)
			{
				// new text is larger than current text
				update = true;
			}
			// Else, it's same length or shorter.

			// Different. rebuild text
			this->text = text;

			bool result = buildMesh(update);

			if (!result)
			{
				// failed update mesh
				this->text = "";
				this->contentSize.x = 0.0f;
				this->boundingBox.size.x = 0.0f;
			}
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
		uvbo = 0;
	}

	if (ibo)
	{
		glDeleteBuffers(1, &ibo);
		ibo = 0;
	}
	
	// vao is released on RenderNode

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX && V_DEBUG_DRAW_TEXT_BOUNDING_BOX
	if (bbVao)
	{
		// Delte array
		glDeleteVertexArrays(1, &bbVao);
		bbVao = 0;
	}
#endif
}

int Voxel::UI::Text::getCharIndexOnCursor(const glm::vec2 & cursorPosition)
{
	// todo: Implement this. For now, doesn't really need it. It's good to have to make inputfield more functional, but it's really an option to have. No big deal.
	if (visibility)
	{
		// visible
		if (textSize > 0)
		{
			// have atleast 1 char
			float yStart = boundingBox.center.y + (boundingBox.size.y * 0.5f);

			if (totalLines == 1)
			{
				// only have 1 line
			}
			else
			{
				const int lineSpace = font->getLineSpace();
				const int outlineSize = font->getOutlineSize();
			}
		}
	}

	return -1;
}

bool Voxel::UI::Text::updateTextMouseMove(const glm::vec2 & mousePosition, const glm::vec2 & mouseDelta)
{
	// check if progress timer is interactable
	if (isInteractable())
	{
		// check bb
		if (boundingBox.containsPoint(mousePosition))
		{
			if (state == State::IDLE)
			{
				state = State::HOVERED;

				if (onMouseEnter)
				{
					onMouseEnter(this);
				}
			}
			else if (state == State::HOVERED)
			{
				if (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f)
				{
					if (onMouseMove)
					{
						onMouseMove(this);
					}
				}
			}

			return true;
		}
		else
		{
			updateMouseMoveFalse();
		}
	}

	return false;
}

void Voxel::UI::Text::updateMouseMoveFalse()
{
	if (state != State::IDLE)
	{
		state = State::IDLE;

		if (onMouseExit)
		{
			onMouseExit(this);
		}
	}
}

bool Voxel::UI::Text::updateMouseMove(const glm::vec2 & mousePosition, const glm::vec2 & mouseDelta)
{
	if (visibility)
	{
		// visible
		if (children.empty())
		{
			// Has no children. update self
			return updateTextMouseMove(mousePosition, mouseDelta);
		}
		else
		{
			// Has children
			bool childHovered = false;

			// Reverse iterate children because child who has higher global z order gets rendered above other siblings who has lower global z order
			auto rit = children.rbegin();
			for (; rit != children.rend();)
			{
				bool result = (rit->second)->updateMouseMove(mousePosition, mouseDelta);
				if (result)
				{
					// child hovered
					childHovered = true;
					break;
				}

				rit++;
			}

			if (childHovered)
			{
				// There was a child had mouse move event. Iterate remaining children and update

				// Don't forget to increment iterator.
				rit++;

				for (; rit != children.rend(); rit++)
				{
					(rit->second)->updateMouseMoveFalse();
				}

				updateMouseMoveFalse();
			}
			else
			{
				// There was no mouse move event on child
				childHovered = updateTextMouseMove(mousePosition, mouseDelta);
			}

			return childHovered;
		}
	}
	else
	{
		return false;
	}
}

bool Voxel::UI::Text::updateMousePress(const glm::vec2 & mousePosition, const int button)
{
	if (visibility)
	{
		bool pressed = false;

		if (isInteractable())
		{
			if (button == GLFW_MOUSE_BUTTON_1)
			{
				if (boundingBox.containsPoint(mousePosition))
				{
					if (state == State::HOVERED)
					{
						state = State::CLICKED;
						pressed = true;

						if (onMousePressed)
						{
							onMousePressed(this, button);
						}
					}
				}
			}
		}

		if (!pressed)
		{
			pressed = Voxel::UI::TransformNode::updateMousePress(mousePosition, button);
		}

		return pressed;
	}
	else
	{
		return false;
	}
}

bool Voxel::UI::Text::updateMouseRelease(const glm::vec2 & mousePosition, const int button)
{
	if (visibility)
	{
		bool released = false;

		if (isInteractable())
		{
			if (button == GLFW_MOUSE_BUTTON_1)
			{
				if (boundingBox.containsPoint(mousePosition))
				{
					if (state == State::CLICKED)
					{
						state = State::IDLE;

						released = true;

						if (onMouseReleased)
						{
							onMouseReleased(this, button);
						}
					}
				}
			}
		}

		if (!released)
		{
			released = Voxel::UI::TransformNode::updateMouseRelease(mousePosition, button);
		}

		return released;
	}
	else
	{
		return false;
	}
}


bool Voxel::UI::Text::computeLineSizes(std::vector<std::string>& lines, std::vector<LineSize>& lineSizes, int & maxWidth)
{
	// get white space advance
	Glyph* glyph = font->getGlyph(32);

	if (glyph == nullptr) return false;

	const int whiteSpaceAdvnace = glyph->advance;

	// Iterate per line. Find the maximum width and height
	for (auto& line : lines)
	{
		// create line size
		lineSizes.push_back(LineSize());

		// get ref
		LineSize& lineSize = lineSizes.back();

		if (line.empty())
		{
			lineSize.subLineSizes.push_back(SubLineSize());
			lineSize.subLineSizes.back().width = 0;
			lineSize.subLineSizes.back().maxBearingY = font->getGlyph(32/* whitespace */)->height;
			lineSize.subLineSizes.back().maxBotY = 0;
			lineSize.subLineSizes.back().text = "";

			lineSize.maxX = 0;
			lineSize.maxBearingY = lineSize.subLineSizes.back().maxBearingY;
			lineSize.maxBotY = 0;

			maxWidth = glm::max(maxWidth, 0);
		}

		// maximum x in single LineSize.
		int maxX = 0;

		int maxBearingY = 0;
		int maxBotY = 0;

		// get length of line
		unsigned int len = line.size();
		
		// Mesure size of the word.
		unsigned int currentWordWith = 0;

		// Measure size of current line
		unsigned int currentLineWidth = 0;

		// Measure max
		int currentMaxBearingY = 0;
		int currentMaxBotY = 0;

		// sub line text
		std::string subLineText = "";
		std::string currentWord = "";

		for (unsigned int i = 0; i < len; i++)
		{
			const char c = line[i];
			Glyph* glyph = font->getGlyph((int)c);

			// Advance value is the distance between pen position of each character in horizontal layout
			// Advance includes bearing x + width + extra space for next character.
			// We don't have to add extra space for last char because line ends.
			// However, if it's white space, add advance

			int charWidth = 0;
			bool whitespace = (c == ' ');

			if (whitespace)
			{
				// white space. advance
				charWidth = glyph->advance;
			}
			else
			{
				if (i == (len - 1))
				{
					// If character is
					charWidth = (glyph->bearingX + glyph->width);
				}
				else
				{
					// Character is not in the end. advance.
					charWidth = glyph->advance;
				}
			}
			
			if (!whitespace)
			{
				// if char is not whitespace, measure world size
				currentWordWith += charWidth;
				// append char to current word
				currentWord.append(1, c);
			}

			// First, check if word can even fit on line break with. If so, continue. Else, invalid text. If line break width is smaller than single word's width, we can't render anything.
			if (lineBreakWidth > 0 && currentWordWith > lineBreakWidth)
			{
				// word size exceeded line break width. Abort.
				return false;
			}
			// Else, word can fit.

			// add to line width
			currentLineWidth += charWidth;

			// Then, check if line can fit the 
			if (lineBreakWidth > 0 && currentLineWidth > lineBreakWidth)
			{
				// current line exceeded line break width. 
				lineSize.subLineSizes.push_back(SubLineSize());
				SubLineSize& sls = lineSize.subLineSizes.back();

				if (whitespace)
				{
					// last char added was whitespace. This means all words in this sub line fits line break width. 
					sls.width = currentLineWidth - charWidth;
					if (subLineText.empty())
					{
						sls.text = currentWord;
					}
					else
					{
						sls.text = subLineText + " " + currentWord;
					}
					currentWord = "";
					currentLineWidth = 0;
				}
				else
				{
					// last char add wasn't whitesapce. sub line's width is current line with widhotu current word width
					sls.width = currentLineWidth - currentWordWith;
					if (subLineText.empty() == false)
					{
						sls.width -= whiteSpaceAdvnace;
					}
					sls.text = subLineText;
					currentLineWidth = currentWordWith;
				}

				if (sls.width % 2 == 1)
				{
					// Odd size distorts ui
					sls.width++;
				}

				sls.maxBearingY = currentMaxBearingY;
				sls.maxBotY = currentMaxBotY;

				subLineText = "";

				maxX = glm::max(sls.width, maxX);

				currentMaxBearingY = 0;
				currentMaxBotY = 0;
			}
			else
			{
				// current line still fits
			}

			// Find max bearing Y. BearingY is the upper height of character from pen position.
			maxBearingY = glm::max(maxBearingY, glyph->bearingY);
			currentMaxBearingY = glm::max(currentMaxBearingY, glyph->bearingY);

			// Find max botY. BotY is my defined value, which is the lower height of character from pen position
			maxBotY = glm::max(maxBotY, glyph->botY);
			currentMaxBotY = glm::max(currentMaxBotY, glyph->botY);

			// If char was whitespace, reset world size and word str
			if (whitespace)
			{
				charWidth = 0;

				if (subLineText.empty())
				{
					subLineText = currentWord;
				}
				else
				{
					subLineText += (" " + currentWord);
				}

				currentWord = "";
				currentWordWith = 0;
			}
		}
		// loop ends.

		// Add list subline size.
		lineSize.subLineSizes.push_back(SubLineSize());
		SubLineSize& sls = lineSize.subLineSizes.back();

		sls.width = currentLineWidth;

		if (sls.width % 2 == 1)
		{
			// Odd size distorts ui
			sls.width++;
		}

		sls.maxBearingY = currentMaxBearingY;
		sls.maxBotY = currentMaxBotY;

		if (subLineText.empty())
		{
			sls.text = currentWord;
		}
		else
		{
			sls.text = subLineText + " " + currentWord;
		}

		maxX = glm::max(sls.width, maxX);

		if (maxX % 2 == 1)
		{
			// Odd size distorts ui
			maxX++;
		}

		lineSize.maxX = maxX;
		lineSize.maxBearingY = maxBearingY;
		lineSize.maxBotY = maxBotY;

		// Keeping this to remind myself the bug I had.
		// Make max width and height even number because this is UI(?)
		// Note: The reason why I added this is because when pen position x had decimal points due to width being odd number, entire text rendering becaome weird (extended in x axis. slightly)
		// This fixed the issue.
		//if (totalWidth % 2 == 1)
		//{
		//	totalWidth++;
		//}

		// total width is sum of glyph's advance, which includes extra space for next character. Remove it
		maxWidth = glm::max(maxX, maxWidth);

		totalLines += lineSize.subLineSizes.size();
	
		// end of current line size
	}
	// loop ends

	return true;
}

void Voxel::UI::Text::computePenPositions(std::vector<LineSize>& lineSizes, const float yAdvance)
{
	// Current Y. Because we are using horizontal layout, we advance y in negative direction (Down), starting from height point, which is half of max height
	float curY = boundingBox.size.y * 0.5f;

	//curY -= lineSizes.front().maxBearingY;
	curY -= font->getSize();

	// Iterate line sizes to find out each line's pen position from origin
	for (auto& lineSize : lineSizes)
	{
		// has line break width. possible multiple sublinesize in linesize
		for (auto& sls : lineSize.subLineSizes)
		{
			glm::vec2 penPos = glm::vec2(0);

			if (align == ALIGN::LEFT)
			{
				// Align text to left. x is always the most left pos of longest line
				penPos.x = static_cast<float>(boundingBox.size.x) * -0.5f;
			}
			else if (align == ALIGN::RIGHT)
			{
				// Aling text to right.
				penPos.x = (boundingBox.size.x * 0.5f) - static_cast<float>(sls.width);
			}
			else // center
			{
				// Aling text to center.
				penPos.x = static_cast<float>(sls.width) * -0.5f;
			}

			penPos.y = curY;
			curY -= yAdvance;
			sls.penPosition = penPos;
		}
	}
}

bool Voxel::UI::Text::buildMesh(const bool reallocate)
{
	// Check font
	if (font)
	{
		// Step 0. Prepare ------------

		// Check text
		if (text.empty())
		{
			return true;
		}

		// set texture
		texture = font->getTexture();

		totalLines = 0;
		
		// Step 0 done.



		// Step 1. Split text by new line ------------

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

		// Step 1 done.




		// Step 2. Compute line sizes, max width and total height ------------

		// This is where we store each line's size so we can properly reposition all character based on align type
		int maxWidth = 0;

		// Compute line sizes
		lineSizes.clear();

		bool result = computeLineSizes(split, lineSizes, maxWidth);

		if (!result)
		{
			// abort
			return false;
		}

		// get outline size
		const float outlineSize = static_cast<float>(font->getOutlineSize());
		// get line space
		const int lineSpace = font->getLineSpace();
		// count line. Exclude last line
		//const int lineSizeCount = lineSizes.size() - 1;
		int totalLineCount = 0;

		for (auto& lineSize : lineSizes)
		{
			totalLineCount += lineSize.subLineSizes.size();
		}

		if (totalLineCount == 0)
		{
			//abort
			return false;
		}

		// compute total height
		//int totalHeight = (lineSizeCount * lineSpace) + font->getSize();
		int totalHeight = totalHeight = ((totalLineCount - 1) * lineSpace) + font->getSize();

		/*
		if (totalHeight % 2 == 1)
		{
			totalHeight++;
		}
		*/

		// Step 2 done.



		// Step 3. Compute text bounding box and content size ------------

		int lineGapHeight = static_cast<int>(outlineSize);
		//int lineGap = lineSizeCount * lineGapHeight;
		int lineGap = (totalLineCount - 1) * lineGapHeight;

		/*
		// Set size of Text object. Ignore last line's line gap
		const float h = static_cast<float>(totalHeight + lineGap);
		auto boxMin = glm::vec2(static_cast<float>(maxWidth) * -0.5f, h * -0.5f);
		auto boxMax = glm::vec2(static_cast<float>(maxWidth) * 0.5f, h * 0.5f);
		*/

		boundingBox.center = position;
		boundingBox.size = glm::vec2(maxWidth, totalHeight + ((lineSizes.size() == 1) ? 0 : lineGap));

		contentSize = boundingBox.size;

		// Step 3 done.



		// Step 4. Compute pen positions ------------

		// Pen position. Also called as origin or base line in y pos.
		computePenPositions(lineSizes, static_cast<float>(lineSpace + lineGapHeight));

		// Step 4 done.



		// We have pen position for each line. Iterate line and build vertices based on 
		int penPosIndex = 0;

		std::vector<float> vertices;
		// colors, indices and uv doesn't have to be tralsated later.
		std::vector<float> colors;
		std::vector<unsigned int> indices;
		std::vector<float> uvVertices;
		unsigned int indicesIndex = 0;

		textSize = 0;	// Doesn't include escaped keys like new line

		for (auto& lineSize : lineSizes)
		{
			for (auto& subLineSize : lineSize.subLineSizes)
			{
				float penPosX = subLineSize.penPosition.x;
				float penPosY = subLineSize.penPosition.y;

				unsigned int len = subLineSize.text.size();

				textSize += len;

				for (unsigned int i = 0; i < len; i++)
				{
					// Build quad for each character
					const char c = subLineSize.text[i];
					Glyph* glyph = font->getGlyph((int)c);

					// Empty pos. p1 = left bottom, p2 = right top. z == 0
					glm::vec2 leftBottom(0);
					glm::vec2 rightTop(0);
					float x = penPosX;

					// Advance x for bearing x
					x += static_cast<float>(glyph->bearingX);

					// outline
					leftBottom.x = (x - outlineSize);

					// Advance x again for width
					x += static_cast<float>(glyph->width);
					rightTop.x = (x + outlineSize);

					// Calculate Y based on pen position
					leftBottom.y = penPosY - static_cast<float>(glyph->botY) - outlineSize;
					rightTop.y = penPosY + static_cast<float>(glyph->bearingY) + outlineSize;

					// Advnace pen pos x to next char
					penPosX += glyph->advance;

					// Store left bttom and right top vertices pos
					vertices.push_back(leftBottom.x); vertices.push_back(leftBottom.y); vertices.push_back(0);	// left bottom
					vertices.push_back(leftBottom.x); vertices.push_back(rightTop.y); vertices.push_back(0);	// left top
					vertices.push_back(rightTop.x); vertices.push_back(leftBottom.y); vertices.push_back(0);		// right bottom
					vertices.push_back(rightTop.x); vertices.push_back(rightTop.y); vertices.push_back(0);		// right top

					// add char bounding box relative to origin
					// Todo: Support character bounding box check for input field. 
					//subLineSize.characterBoundingBoxes.push_back(Voxel::Shape::Rect(glm::vec2(leftBottom.x + (rightTop.x * 0.5f)), glm::abs(rightTop - leftBottom)));

					// color
					for (int j = 0; j < 4; j++)
					{
						colors.push_back(color.r);
						colors.push_back(color.g);
						colors.push_back(color.b);
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
		}
		
		// load buffer
		loadBuffers(vertices, colors, uvVertices, indices, reallocate);

		updateModelMatrix();
		updateBoundingBox();

		if (parent)
		{
			updateBoundingBox();
		}

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

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX && V_DEBUG_DRAW_TEXT_BOUNDING_BOX
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

	lineIndicesSize = 8;

#if V_DEBUG_DRAW_TEXT_LINE_DIVIDER

#endif

#if V_DEBUG_DRAW_TEXT_CHARACTER_DIVIDER

#endif

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
			Glyph* glyph = font->getGlyph((int)c);
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
	if (textSize == 0) return;

	program->use(true);
	program->setUniformMat4("modelMat", glm::scale(modelMat, glm::vec3(scale, 1)));
	program->setUniformFloat("opacity", opacity);
	program->setUniformVec3("color", color);

	texture->activate(GL_TEXTURE0);
	texture->bind();
	texture->enableTexLoc();

	if (outlined)
	{
		program->setUniformBool("outlined", true);
		program->setUniformInt("outlineSize", 2);
		program->setUniformVec3("outlineColor", outlineColor);

		auto textureSize = texture->getTextureSize();
		program->setUniformFloat("textureWidth", static_cast<float>(textureSize.x));
		program->setUniformFloat("textureHeight", static_cast<float>(textureSize.y));
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

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX && V_DEBUG_DRAW_TEXT_BOUNDING_BOX
	if (bbVao)
	{
		auto lineProgram = ProgramManager::getInstance().getProgram(Voxel::ProgramManager::PROGRAM_NAME::LINE_SHADER);
		lineProgram->use(true);
		lineProgram->setUniformMat4("modelMat", modelMat);
		lineProgram->setUniformMat4("viewMat", glm::mat4(1.0f));
		lineProgram->setUniformVec4("lineColor", glm::vec4(1.0f));

		glBindVertexArray(bbVao);
		glDrawArrays(GL_LINES, 0, lineIndicesSize);
	}
#endif
}

//====================================================================================================================================