// pch
#include "PreCompiled.h"

#include "InputField.h"

// voxel
#include "SpriteSheet.h"
#include "InputHandler.h"
#include "ProgramManager.h"
#include "Program.h"
#include "Quad.h"
#include "Font.h"
#include "Utility.h"

Voxel::UI::InputField::InputField(const std::string& name)
	: Text(name)
	, cursorVao(0)
	, cursorModelMat(1.0f)
	, cursorTexture(nullptr)
	, cursorBlinkSpeed(0.5f)
	, cursorBlinkElapsedTime(0.0f)
	, cursorVisible(false)
	, defaultTextMode(true)
	, prevDefaultTextMode(true)
	, editting(false)
	, textMaxLength(-1)
{}

Voxel::UI::InputField::~InputField()
{
	if (cursorVao)
	{
		glDeleteVertexArrays(1, &cursorVao);
	}

	if (editting)
	{
		InputHandler::getInstance().redirectKeyInputToText(nullptr);
	}
}

bool Voxel::UI::InputField::init(const std::string & defaultText, const int fontId, const std::string & spriteSheetName, const std::string & cursorImageName, const Voxel::UI::Text::ALIGN align, const int textMaxLength, const unsigned int lineBreakWidth)
{
	// check cursor
	cursorProgram = Voxel::ProgramManager::getInstance().getProgram(Voxel::ProgramManager::PROGRAM_NAME::UI_TEXTURE_SHADER);

	if (cursorProgram == nullptr) return false;

	auto ssm = &Voxel::SpriteSheetManager::getInstance();

	if (ssm == nullptr) return false;

	auto ss = ssm->getSpriteSheetByKey(spriteSheetName);

	if (ss == nullptr) return false;

	auto cursorImageEntry = ss->getImageEntry(cursorImageName);

	if (cursorImageEntry == nullptr) return false;

	cursorTexture = ss->getTexture();

	if (cursorTexture == nullptr) return false;
	
	bool textResult = Voxel::UI::Text::init(defaultText, fontId, align, lineBreakWidth);

	if (textResult)
	{
		this->defaultText = defaultText;
		this->textMaxLength = textMaxLength;
		this->align = align;

		auto size = glm::vec2(cursorImageEntry->width, cursorImageEntry->height);
		float widthHalf = size.x * 0.5f;
		float heightHalf = size.y * 0.5f;

		std::array<float, 12> vertices = { -widthHalf, -heightHalf, 0.0f, -widthHalf, heightHalf, 0.0f, widthHalf, -heightHalf, 0.0f, widthHalf, heightHalf, 0.0f };

		auto& uvOrigin = cursorImageEntry->uvOrigin;
		auto& uvEnd = cursorImageEntry->uvEnd;

		std::array<float, 8> uvs = { uvOrigin.x, uvOrigin.y, uvOrigin.x, uvEnd.y, uvEnd.x, uvOrigin.y, uvEnd.x, uvEnd.y };

		auto indices = Quad::indices;

		// Successfully created text.
		if (cursorVao)
		{
			glDeleteVertexArrays(1, &cursorVao);
			cursorVao = 0;
		}

		glGenVertexArrays(1, &cursorVao);
		glBindVertexArray(cursorVao);

		GLint vertLoc = cursorProgram->getAttribLocation("vert");

		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices.front(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(vertLoc);
		glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		GLint uvVertLoc = cursorProgram->getAttribLocation("uvVert");

		GLuint uvbo;
		glGenBuffers(1, &uvbo);
		glBindBuffer(GL_ARRAY_BUFFER, uvbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * uvs.size(), &uvs.front(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(uvVertLoc);
		glVertexAttribPointer(uvVertLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

		GLuint ibo;
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices.front(), GL_STATIC_DRAW);

		glBindVertexArray(0);

		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &uvbo);
		glDeleteBuffers(1, &ibo);

		setInteractable();

		return true;
	}

	return false;
}

Voxel::UI::InputField * Voxel::UI::InputField::create(const std::string & name, const std::string & defaultText, const std::string & spriteSheetName, const int fontId, const std::string & cursorImageName, const Voxel::UI::Text::ALIGN align, const int textMaxLength, const unsigned int lineBreakWidth)
{
	auto newInputField = new InputField(name);

	if (newInputField->init(defaultText, fontId, spriteSheetName, cursorImageName, align, textMaxLength, lineBreakWidth))
	{
		return newInputField;
	}
	else
	{
		delete newInputField;
		return nullptr;
	}
}

void Voxel::UI::InputField::setDefaultText(const std::string & defaultText)
{
	this->defaultText = defaultText;
}

std::string Voxel::UI::InputField::getDefaultText() const
{
	return defaultText;
}

void Voxel::UI::InputField::setToDefaultText()
{
	defaultTextMode = true;
	prevDefaultTextMode = false;

	modifyText(defaultText);
}

bool Voxel::UI::InputField::updateInputFieldMouseMove(const glm::vec2 & mousePosition, const glm::vec2 & mouseDelta)
{
	// check if button is interacble
	if (isInteractable())
	{
		// check bb
		if (boundingBox.containsPoint(mousePosition))
		{
			if (state == State::IDLE)
			{
				// idle. hover it
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
			// else, nothing to do
			return true;
		}
		else
		{
			updateMouseMoveFalse();
		}
		// else, nothing to do
	}
	// else, not interactable

	return false;
}

void Voxel::UI::InputField::update(const float delta)
{
	Voxel::UI::TransformNode::update(delta);

	cursorBlinkElapsedTime += delta;

	if (cursorBlinkElapsedTime >= cursorBlinkSpeed)
	{
		cursorBlinkElapsedTime -= cursorBlinkSpeed;

		cursorVisible = !cursorVisible;
	}
}

bool Voxel::UI::InputField::updateMouseMove(const glm::vec2 & mousePosition, const glm::vec2 & mouseDelta)
{
	if (visibility)
	{
		// visible
		if (children.empty())
		{
			// Has no children. update self
			return updateInputFieldMouseMove(mousePosition, mouseDelta);
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
				childHovered = updateInputFieldMouseMove(mousePosition, mouseDelta);
			}

			return childHovered;
		}
	}
	else
	{
		return false;
	}
}

bool Voxel::UI::InputField::updateMousePress(const glm::vec2 & mousePosition, const int button)
{
	if (visibility)
	{
		bool pressed = false;

		// check if button is interacble
		if (isInteractable())
		{
			// it's interactable
			if (button == GLFW_MOUSE_BUTTON_1)
			{
				// clicked with left mouse button
				if (boundingBox.containsPoint(mousePosition))
				{
					// mouse is in bounding box
					if (state == State::HOVERED)
					{
						// was hovering. click
						state = State::CLICKED;

						pressed = true;
						
						if (onMousePressed)
						{
							onMousePressed(this, button);
						}
					}
					// else, disabled or not hovering
				}
				else
				{
					// didn't press text
					if (editting)
					{
						finishEdit();
					}
				}
			}
		}

		if (!pressed)
		{
			// button was not clicked. check if there is another button in children that might possibly clicked
			pressed = Voxel::UI::TransformNode::updateMousePress(mousePosition, button);
		}

		return pressed;
	}
	else
	{
		return false;
	}
}

bool Voxel::UI::InputField::updateMouseRelease(const glm::vec2 & mousePosition, const int button)
{
	if (visibility)
	{
		bool released = false;

		// check if button is interacble
		if (isInteractable())
		{
			// it's interactable
			if (button == GLFW_MOUSE_BUTTON_1)
			{
				// clicked with left mouse button
				if (boundingBox.containsPoint(mousePosition))
				{
					// mouse is in bounding box
					if (state == State::CLICKED)
					{
						state = State::IDLE;
						
						released = true;

						if (onMouseReleased)
						{
							onMouseReleased(this, button);
						}

						startEdit();

						if (onEditStart)
						{
							onEditStart(this);
						}
					}
					// else, disabled or not hovering
				}
			}
		}

		if (!released)
		{
			// button was not clicked. check if there is another button in children that might possibly released
			released = Voxel::UI::TransformNode::updateMouseRelease(mousePosition, button);
		}

		return released;
	}
	else
	{
		return false;
	}
}

void Voxel::UI::InputField::updateCursorModelMatrix()
{
	if (lineBreakWidth == 0)
	{
		if (lineSizes.empty())
		{
			// empty text.
			cursorModelMat = modelMat;
		}
		else
		{
			cursorModelMat = modelMat * glm::translate(glm::mat4(1.0f), glm::vec3((lineSizes.front().maxX * 0.5f) + 1.0f, 0.0f, 0.0f));
		}
	}
	else
	{
		if (totalLines == 1)
		{
			if (text.empty())
			{
				cursorModelMat = modelMat * glm::translate(glm::mat4(1.0f), glm::vec3((boundingBox.size.x * 0.5f) + 1.0f, 0.0f, 0.0f));
			}
			else
			{
				cursorModelMat = modelMat * glm::translate(glm::mat4(1.0f), glm::vec3((lineSizes.front().maxX * 0.5f) + 1.0f, 0.0f, 0.0f));
			}
		}
		else
		{
			float y = lineSizes.back().subLineSizes.back().penPosition.y + lineSizes.back().maxBearingY * 0.5f;
			cursorModelMat = modelMat * glm::translate(glm::mat4(1.0f), glm::vec3(lineSizes.back().subLineSizes.back().penPosition.x + (lineSizes.back().subLineSizes.back().width) + 1.0f, y, 0.0f));
		}
	}
}

void Voxel::UI::InputField::modifyText(const std::string & text)
{
	setText(text);
	
	//updateTextPosition();

	updateCursorModelMatrix();

	if (onEdit)
	{
		onEdit(this, text);
	}
}

void Voxel::UI::InputField::removeLastCharacter()
{
	if (text == defaultText)
	{
		if (defaultTextMode)
		{
			modifyText("");
			defaultTextMode = false;
			return;
		}
	}

	auto textSize = text.size();
	if (textSize == 0)
	{
		// do nothing
	}
	else if (textSize == 1)
	{
		modifyText("");
	}
	else
	{
		modifyText(text.substr(0, textSize - 1));
	}
}

void Voxel::UI::InputField::removeLastWord()
{
	std::string strcopy = text;

	if (strcopy.back() == ' ')
	{
		Utility::String::rtrim(strcopy);
	}

	auto pos = strcopy.find_last_of(' ');
	if (pos == std::string::npos)
	{
		modifyText("");
	}
	else
	{
		strcopy = strcopy.substr(0, pos);
		Utility::String::rtrim(strcopy);
		modifyText(strcopy);
	}
}

void Voxel::UI::InputField::startEdit()
{
	if (editting == false)
	{
		editting = true;

		prevText = text;

		InputHandler::getInstance().redirectKeyInputToText(this);

		if (text == defaultText)
		{
			if (defaultTextMode)
			{
				modifyText("");
				defaultTextMode = false;
				prevDefaultTextMode = true;
			}
		}

		if (onEditStart)
		{
			onEditStart(this);
		}
		
		cursorBlinkElapsedTime = 0.0f;
		cursorVisible = true;
	}
}

void Voxel::UI::InputField::finishEdit()
{
	if (editting)
	{
		if (onEditFinished)
		{
			onEditFinished(this, text);
		}

		// finish modifying
		state = State::IDLE;
		
		InputHandler::getInstance().redirectKeyInputToText(nullptr);

		if (text.empty())
		{
			modifyText(defaultText);
			defaultTextMode = true;
			prevDefaultTextMode = false;
		}

		editting = false;

		cursorBlinkElapsedTime = 0.0f;
		cursorVisible = false;
	}
}

void Voxel::UI::InputField::cancelEdit()
{
	if (editting)
	{
		state = State::IDLE;
		
		InputHandler::getInstance().redirectKeyInputToText(nullptr);

		if (prevDefaultTextMode)
		{
			defaultTextMode = true;
			prevDefaultTextMode = false;
			modifyText(defaultText);
		}
		else
		{
			modifyText(prevText);
		}

		if (onEditCancelled)
		{
			onEditCancelled(this);
		}

		editting = false;

		cursorBlinkElapsedTime = 0.0f;
		cursorVisible = false;
	}
}

void Voxel::UI::InputField::appendStr(const std::string & str)
{
	if (textMaxLength >= 0)
	{
		if (text.size() >= textMaxLength)
		{
			return;
		}
	}

	if (text == defaultText)
	{
		if (defaultTextMode)
		{
			modifyText(str);
		}
		else
		{
			modifyText(text + str);
		}
	}
	else
	{
		modifyText(text + str);
	}

	cursorBlinkElapsedTime = 0.0f;
	cursorVisible = true;
}

void Voxel::UI::InputField::setOnEditCallback(const std::function<void(Voxel::UI::InputField*, const std::string)>& func)
{
	onEdit = func;
}

void Voxel::UI::InputField::setOnEditFinishedCallback(const std::function<void(Voxel::UI::InputField*, const std::string)>& func)
{
	onEditFinished = func;
}

void Voxel::UI::InputField::setOnEditCancelledCallback(const std::function<void(Voxel::UI::InputField*)>& func)
{
	onEditCancelled = func;
}

void Voxel::UI::InputField::setOnEditStartCallback(const std::function<void(Voxel::UI::InputField*)>& func)
{
	onEditStart = func;
}

void Voxel::UI::InputField::renderSelf()
{
	Voxel::UI::Text::renderSelf();

	if (cursorVisible && editting && cursorVao)
	{
		// only render self
		if (cursorTexture == nullptr) return;
		if (cursorProgram == nullptr) return;

		cursorProgram->use(true);
		cursorProgram->setUniformMat4("modelMat", glm::scale(cursorModelMat, glm::vec3(scale, 1)));
		cursorProgram->setUniformFloat("opacity", opacity);
		cursorProgram->setUniformVec3("color", color);

		cursorTexture->activate(GL_TEXTURE0);
		cursorTexture->bind();

		glBindVertexArray(cursorVao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
}
