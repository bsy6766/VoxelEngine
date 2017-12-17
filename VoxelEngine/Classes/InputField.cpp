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
	, onEditStart(nullptr)
	, onEdit(nullptr)
	, onEditFinished(nullptr)
	, onEditCancelled(nullptr)
	, onEditSubmitted(nullptr)
	, align(Align::CENTER)
	, prevText("")
	, input(&InputHandler::getInstance())
	, textDefaultMode(true)
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
	auto blinkSeq = Voxel::UI::RepeatForever::create(Voxel::UI::Sequence::create({ Voxel::UI::Visibility::create(0.0f, true), Voxel::UI::Visibility::create(0.4f, false), Voxel::UI::Delay::create(0.4f) }));
	cursor->runAction(blinkSeq);
	cursor->pauseAction();

	setInteractable();

	return true;
}

void Voxel::UI::InputField::updateTextPosition()
{
	auto textBB = text->getBoundingBox();

	switch (align)
	{
	case Voxel::UI::InputField::Align::LEFT:
	{
		text->setPosition(textBB.size.x * 0.5f, 0.0f, false);
	}
		break;
	case Voxel::UI::InputField::Align::CENTER:
	{
		text->setPosition(0.0f, 0.0f, false);
	}
		break;
	case Voxel::UI::InputField::Align::RIGHT:
	{
		text->setPosition(textBB.size.x * 0.5f * -1.0f, 0.0f, false);
	}
		break;
	default:
		return;
		break;
	}

	text->updateModelMatrix(modelMat);
	text->updateBoundingBox(modelMat);
}

void Voxel::UI::InputField::updateCursorPosition()
{
	auto textBB = text->getBoundingBox();

	switch (align)
	{
	case Voxel::UI::InputField::Align::LEFT:
	{
		cursor->setPosition(textBB.size.x + 1.0f, 0.0f, false);
	}
		break;
	case Voxel::UI::InputField::Align::CENTER:
	{
		cursor->setPosition(textBB.size.x * 0.5f, 0.0f, false);
	}
		break;
	case Voxel::UI::InputField::Align::RIGHT:
	{
		cursor->setPosition(0.0f, 0.0f, false);
	}
		break;
	default:
		return;
		break;
	}

	cursor->updateModelMatrix(modelMat);
	cursor->updateBoundingBox(modelMat);
}

void Voxel::UI::InputField::modifyText(const std::string & text)
{
	this->text->setText(text);

	this->updateModelMatrix();

	this->updateBoundingBox();

	updateTextPosition();
	updateCursorPosition();

	if (onEdit)
	{
		onEdit(text);
	}
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

void Voxel::UI::InputField::setAlign(const Voxel::UI::InputField::Align align)
{
	this->align = align;

	updateTextPosition();
	updateCursorPosition();
}

void Voxel::UI::InputField::setOnEditStartCallback(const std::function<void()>& func)
{
	onEditStart = func;
}

void Voxel::UI::InputField::setOnEditCallback(const std::function<void(const std::string)>& func)
{
	onEdit = func;
}

void Voxel::UI::InputField::setOnEditFinished(const std::function<void(const std::string)>& func)
{
	onEditFinished = func;
}

void Voxel::UI::InputField::setOnEditSubmitted(const std::function<void(const std::string)>& func)
{
	onEditSubmitted = func;
}

void Voxel::UI::InputField::setOnEditCancelled(const std::function<void()>& func)
{
	onEditCancelled = func;
}

void Voxel::UI::InputField::setScale(const float scale)
{
	text->setScale(scale);
	cursor->setScale(scale);

	// Update self
	Voxel::UI::TransformNode::updateModelMatrix();

	// Update model mat is public for now. 
	text->updateModelMatrix(modelMat);

	cursor->updateModelMatrix(modelMat);
}

void Voxel::UI::InputField::updateModelMatrix()
{
	// Unlike other ui, inputfield is a class that wraps text and image together.
	// so we have to update model matrix for both text and image manually because text and cursor doesn't have parent directly
	// (If only Inputfield is not field of Text) Trick here is simply add text and cursor to inputfield, which updates model matrix and bounding box

	// Inputfield's bounding box is same as text.
	contentSize = text->getContentSize();
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

	// Don't forget to call this to update self and children bounding box.
	Voxel::UI::TransformNode::updateBoundingBox();
}

void Voxel::UI::InputField::update(const float delta)
{
	if (text)
	{
		text->update(delta);
	}
	
	if (cursor)
	{
		cursor->update(delta);
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
					//std::cout << "Hovering\n";
				}

				return true;
			}
			else
			{
				if (state == State::HOVERED || state == State::CLICKED)
				{
					state = State::IDLE;
					//std::cout << "Idle\n";
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
						//std::cout << "Clicked\n";
					}
					else if (state == State::EDITTING)
					{
						// todo: change cursor position.
					}
				}
				else
				{
					// didn't pressed text
					if (state == State::EDITTING)
					{
						finishEdit();
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
						//std::cout << "Released\n";
						startEdit();
					}
					// else, disabled or not hovering

					released = true;
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

void Voxel::UI::InputField::startEdit()
{
	if (state != State::EDITTING)
	{
		//std::cout << "Starts editing\n";

		// start text input
		state = State::EDITTING;

		prevText = text->getText();

		cursor->resumeAction();

		InputHandler::getInstance().redirectKeyInputToText(this);

		if (text->getText() == defaultText)
		{
			if (textDefaultMode)
			{
				modifyText("");
				textDefaultMode = false;
			}
		}

		if (onEditStart)
		{
			onEditStart();
		}
	}
}

void Voxel::UI::InputField::finishEdit()
{
	if (state == State::EDITTING)
	{
		//std::cout << "Finishes editing\n";

		if (onEditFinished)
		{
			onEditFinished(text->getText());
		}

		// finish modifying
		state = State::IDLE;

		cursor->restartAllActions();
		cursor->pauseAction();

		InputHandler::getInstance().redirectKeyInputToText(nullptr);

		if (text->getText().empty())
		{
			modifyText(defaultText);
			textDefaultMode = true;
		}
	}
}

void Voxel::UI::InputField::submitEdit()
{
	if (state == State::EDITTING)
	{
		//std::cout << "Finishes editing\n";

		if (onEditSubmitted)
		{
			onEditSubmitted(text->getText());
		}

		// finish modifying
		state = State::IDLE;

		cursor->restartAllActions();
		cursor->pauseAction();

		InputHandler::getInstance().redirectKeyInputToText(nullptr);

		if (text->getText().empty())
		{
			modifyText(defaultText);
			textDefaultMode = true;
		}
	}
}

void Voxel::UI::InputField::cancelEdit()
{
	//std::cout << "Cancelled editing\n";

	// finish modifying
	state = State::IDLE;

	cursor->restartAllActions();
	cursor->pauseAction();

	InputHandler::getInstance().redirectKeyInputToText(nullptr);

	modifyText(prevText);

	if (onEditCancelled)
	{
		onEditCancelled();
	}
}

void Voxel::UI::InputField::appendStr(const std::string & str)
{
	auto curText = text->getText();

	if (curText == defaultText)
	{
		if (textDefaultMode)
		{
			modifyText(str);
		}
		else
		{
			modifyText(curText + str);
		}
	}
	else
	{
		modifyText(curText + str);
	}
}

void Voxel::UI::InputField::removeLastCharacter()
{
	auto curText = text->getText();
	
	if (curText == defaultText)
	{
		if (textDefaultMode)
		{
			modifyText("");
			textDefaultMode = false;
			return;
		}
	}


	auto curSize = curText.size();
	if (curSize == 0)
	{
		// do nothing
	}
	else if (curSize == 1)
	{
		curText = "";
	}
	else
	{
		curText = curText.substr(0, curSize - 1);
	}

	modifyText(curText);
}

void Voxel::UI::InputField::setToDefaultText()
{
	modifyText(defaultText);
	textDefaultMode = true;
}

void Voxel::UI::InputField::setText(const std::string & text)
{
	modifyText(text);
}

std::string Voxel::UI::InputField::getText() const
{
	if (text)
	{
		return text->getText();
	}
	else
	{
		return "";
	}
}

void Voxel::UI::InputField::render()
{
	if (children.empty())
	{
		if (visibility)
		{
			// Render self
			if (text)
			{
				text->renderSelf();
			}

			if (state == State::EDITTING && cursor && cursor->getVisibility())
			{
				cursor->renderSelf();
			}
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
			if (text)
			{
				text->renderSelf();
			}

			if (state == State::EDITTING && cursor && cursor->getVisibility())
			{
				cursor->renderSelf();
			}

			// Render positive 
			for (; children_it != children.end(); children_it++)
			{
				((children_it)->second)->render();
			}
		}
	}
}
