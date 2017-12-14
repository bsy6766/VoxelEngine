#include "InputField.h"

// voxel
#include "UI.h"
#include "UIActions.h"
#include "InputHandler.h"

Voxel::UI::InputField::InputField(const std::string& name)
	: TransformNode(name)
	, text(nullptr)
	, cursor(nullptr)
	, state(State::IDLE)
	, onInputFieldClicked(nullptr)
	, onTextChanged(nullptr)
	, onTextFinished(nullptr)
{}

Voxel::UI::InputField::~InputField()
{
	if (text)
	{
		delete text;
	}

	if (cursor)
	{
		delete cursor;
	}
}

bool Voxel::UI::InputField::init(const std::string & defaultText, const int fontId, const std::string& spriteSheetName, const std::string & cursorImageName)
{
	this->defaultText = defaultText;

	text = Voxel::UI::Text::create(name + "Text", defaultText, fontId);

	if (!text)
	{
		return false;
	}

	cursor = Voxel::UI::Image::createFromSpriteSheet(name + "Cursor", spriteSheetName, cursorImageName);

	if (!cursor)
	{
		return false;
	}

	cursor->setVisibility(false);
	auto blinkSeq = Voxel::UI::RepeatForever::create(Voxel::UI::Sequence::create({ Voxel::UI::Visibility::create(0.0f, true), Voxel::UI::Visibility::create(0.5f, false), Voxel::UI::Delay::create(0.5f) }));
	cursor->runAction(blinkSeq);
	cursor->pauseAction();

	setInteractable();

	return true;
}

Voxel::UI::InputField * Voxel::UI::InputField::create(const std::string & name, const std::string & defaultText, const std::string& spriteSheetName, const int fontId, const std::string & cursorImageName)
{
	auto newInputField = new InputField(name);

	if (newInputField->init(defaultText, fontId, spriteSheetName, cursorImageName))
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

void Voxel::UI::InputField::updateModelMatrix()
{
	// Unlike other ui, inputfield is a class that wraps text and image together.
	// so we have to update model matrix for both text and image manually because text and cursor doesn't have parent directly
	// (If only Inputfield is not field of Text) Trick here is simply add text and cursor to inputfield, which updates model matrix and bounding box

	// Update self
	Voxel::UI::TransformNode::updateModelMatrix();

	// Update model mat is public for now. 
	text->updateModelMatrix(modelMat);
	cursor->updateModelMatrix(modelMat);
}

void Voxel::UI::InputField::updateBoundingBox()
{
	text->updateBoundingBox(modelMat);
	cursor->updateBoundingBox(modelMat);

	// Inputfield's bounding box is same as text.
	contentSize = text->getContentSize();
	// Don't forget to call this to update self and children bounding box.
	Voxel::UI::TransformNode::updateBoundingBox();
}

bool Voxel::UI::InputField::updateKeyboardInput(const std::string & str)
{
	if (!str.empty())
	{
		std::string strCpy = str;

		while (strCpy.empty() == false)
		{
			auto size = strCpy.size();

			if (size >= 20)
			{
				std::string token = strCpy.substr(0, 21);

				if (token == "VOXEL_GLFW_KEY_ENTER")
				{
					// end input
					state = State::IDLE;

					cursor->restartAllActions();
					cursor->pauseAction();

					if (onTextFinished)
					{
						onTextFinished();
					}

					return true;
				}
			}
			
			auto curText = text->getText();
			curText += strCpy.substr(0, 1);
			strCpy = strCpy.substr(1);
		}

		return true;
	}
	else
	{
		return false;
	}
}

bool Voxel::UI::InputField::updateMouseMove(const glm::vec2 & mousePosition)
{
	if (isInteractable())
	{
		if (text)
		{
			if (text->getBoundingBox().containsPoint(mousePosition))
			{
				if (state == State::IDLE)
				{
					state = State::HOVERED;
				}

				return true;
			}
			else
			{
				if (state == State::HOVERED || state == State::CLICKED)
				{
					state = State::IDLE;
				}
			}
		}
	}

	return false;
}

bool Voxel::UI::InputField::updateMouseMove(const glm::vec2 & mousePosition, const glm::vec2 & mouseDelta)
{
	if (visibility)
	{
		if (children.empty())
		{
			return updateMouseMove(mousePosition);
		}
		else
		{
			bool childHovered = false;

			// first check if there was mouse move event on children
			for (auto& child : children)
			{
				bool result = (child.second)->updateMouseMove(mousePosition, mouseDelta);
				if (result)
				{
					childHovered = true;
				}
			}

			// check if there was event
			if (childHovered)
			{
				// Mouse hvoers child. 
				if (state == State::HOVERED || state == State::CLICKED)
				{
					// Hovered or clicked. Set back to idle
					state = State::IDLE;
				}
			}
			else
			{
				// There was no mouse move event on child
				childHovered = updateMouseMove(mousePosition);
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

		// check if inputfield is interactable
		if (isInteractable())
		{
			// it's interactable
			if (button == GLFW_MOUSE_BUTTON_1)
			{
				// pressed with left mouse button
				if (text->getBoundingBox().containsPoint(mousePosition))
				{
					// pressed text
					if (state == State::HOVERED)
					{
						state = State::CLICKED;
						pressed = true;
					}
					else if (state == State::MODIFYING)
					{
						// todo: change cursor position.
					}
				}
				else
				{
					// didn't pressed text
					if (state == State::MODIFYING)
					{
						// finish modifying
						state = State::IDLE;

						cursor->restartAllActions();
						cursor->pauseAction();
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
				if (text->getBoundingBox().containsPoint(mousePosition))
				{
					// mouse is in bounding box
					if (state == State::CLICKED)
					{
						// start text input
						state = State::MODIFYING;

						cursor->resumeAction();

						InputHandler::getInstance().setBufferMode(true);

						released = true;

						// button clicked!
						if (onInputFieldClicked)
						{
							onInputFieldClicked();
						}
					}
					// else, disabled or not hovering
				}
			}
		}

		if (!released)
		{
			// text was not clicked. check if there is another ui in children that might possibly released
			released = Voxel::UI::TransformNode::updateMouseRelease(mousePosition, button);
		}

		return released;
	}
	else
	{
		return false;
	}
}

void Voxel::UI::InputField::render()
{
	if (children.empty())
	{
		if (visibility)
		{
			// Render self
			text->renderSelf();
			cursor->renderSelf();
		}
	}
	else
	{
		if (visibility)
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
			text->renderSelf();
			cursor->renderSelf();

			// Render positive 
			for (; children_it != children.end(); children_it++)
			{
				((children_it)->second)->render();
			}
		}
	}
}
