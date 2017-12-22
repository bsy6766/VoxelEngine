// pch
#include "PreCompiled.h"

#include "Slider.h"

// voxel
#include "Quad.h"
#include "SpriteSheet.h"
#include "ProgramManager.h"
#include "Program.h"
#include "Utility.h"
#include "InputHandler.h"

Voxel::UI::Slider::Slider(const std::string & name)
	: RenderNode(name)
	, reversedValue(false)
	, state(State::IDLE)
	, type(Type::HORIZONTAL)
	, buttonVao(0)
	, barVao(0)
	, minValue(0)
	, maxValue(0)
	, currentValue(0)
	, buttonSize(0.0f)
	, buttonBoundingBox(glm::vec2(0.0f), glm::vec2(0.0f))
	, barSize(0.0f)
	, barBoundingBox(glm::vec2(0.0f), glm::vec2(0.0f))
	, buttonIndexOffset(0)
	, onButtonPressed(nullptr)
	, onBarPressed(nullptr)
	, onFinished(nullptr)
	, onValueChange(nullptr)
{}

Voxel::UI::Slider::~Slider()
{
	if (buttonVao)
	{
		glDeleteVertexArrays(1, &buttonVao);
	}

	if (barVao)
	{
		glDeleteVertexArrays(1, &barVao);
	}
}

bool Voxel::UI::Slider::init(const std::string & spriteSheetName, const std::string & barImageName, const std::string & buttonImageName, const Type type, const float minValue, const float maxValue)
{
	auto& ssm = Voxel::SpriteSheetManager::getInstance();

	auto ss = ssm.getSpriteSheetByKey(spriteSheetName);

	if (ss)
	{
		setInteractable();

		texture = ss->getTexture();

		if (texture == nullptr)
		{
			return false;
		}

		texture->setLocationOnProgram(ProgramManager::PROGRAM_NAME::UI_TEXTURE_SHADER);

		this->type = type;

		this->minValue = minValue;
		this->maxValue = maxValue;
		this->currentValue = minValue;

		std::array<float, 12> barVertices;
		std::array<float, 8> barUVs;
		std::array<unsigned int, 6> barIndices;

		auto quadIndices = Quad::indices;

		// initialize bar
		auto barImageEntry = ss->getImageEntry(barImageName);

		if (barImageEntry)
		{
			auto size = glm::vec2(barImageEntry->width, barImageEntry->height);
			auto curVertices = Quad::getVertices(size);

			std::move(curVertices.begin(), curVertices.end(), barVertices.begin());

			auto& uvOrigin = barImageEntry->uvOrigin;
			auto& uvEnd = barImageEntry->uvEnd;

			barUVs.at(0) = uvOrigin.x;
			barUVs.at(1) = uvOrigin.y;
			barUVs.at(2) = uvOrigin.x;
			barUVs.at(3) = uvEnd.y;
			barUVs.at(4) = uvEnd.x;
			barUVs.at(5) = uvOrigin.y;
			barUVs.at(6) = uvEnd.x;
			barUVs.at(7) = uvEnd.y;

			barIndices = quadIndices;

			barSize = size;

			barBoundingBox.center = position;
			barBoundingBox.size = size;
		}
		else
		{
			return false;
		}

		std::string buttonFileName;
		std::string fileExt;

		Utility::String::fileNameToNameAndExt(buttonImageName, buttonFileName, fileExt);

		std::array<std::string, 4> fileNames = { buttonFileName + "_idle" + fileExt, buttonFileName + "_hovered" + fileExt, buttonFileName + "_clicked" + fileExt, buttonFileName + "_disabled" + fileExt };

		std::vector<float> buttonVertices;
		std::vector<float> buttonUVs;
		std::vector<unsigned int> buttonIndices;

		for (unsigned int i = 0; i < fileNames.size(); i++)
		{
			auto imageEntry = ss->getImageEntry(fileNames.at(i));

			if (imageEntry)
			{
				auto size = glm::vec2(imageEntry->width, imageEntry->height);
				auto curVertices = Quad::getVertices(size);

				buttonVertices.insert(buttonVertices.end(), curVertices.begin(), curVertices.end());

				auto& uvOrigin = imageEntry->uvOrigin;
				auto& uvEnd = imageEntry->uvEnd;

				buttonUVs.push_back(uvOrigin.x);
				buttonUVs.push_back(uvOrigin.y);
				buttonUVs.push_back(uvOrigin.x);
				buttonUVs.push_back(uvEnd.y);
				buttonUVs.push_back(uvEnd.x);
				buttonUVs.push_back(uvOrigin.y);
				buttonUVs.push_back(uvEnd.x);
				buttonUVs.push_back(uvEnd.y);

				for (auto index : quadIndices)
				{
					buttonIndices.push_back(index + (4 * i));
				}

				if (i == 0)
				{
					buttonSize = size;

					if (type == Type::HORIZONTAL)
					{
						buttonBoundingBox.center.x = barBoundingBox.size.x * -0.5f;
					}
					else
					{
						buttonBoundingBox.center.y = barBoundingBox.size.y * -0.5f;
					}

					buttonBoundingBox.size = size;
				}
			}
			else
			{
				return false;
			}
		}

		program = ProgramManager::getInstance().getProgram(ProgramManager::PROGRAM_NAME::UI_TEXTURE_SHADER);
		
		loadBarBuffer(barVertices, barUVs, barIndices);

		loadButtonBuffer(buttonVertices, buttonUVs, buttonIndices);

		boundingBox.center = position;
		//boundingBox.size = glm::vec2(glm::max(barBoundingBox.size.x, buttonBoundingBox.size.x), glm::max(barBoundingBox.size.y, buttonBoundingBox.size.y));
		boundingBox.size = glm::vec2(0.0f);

		contentSize = glm::vec2(glm::max(barBoundingBox.size.x, buttonBoundingBox.size.x), glm::max(barBoundingBox.size.y, buttonBoundingBox.size.y));

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX && V_DEBUG_DRAW_SLIDER_BOUNDING_BOX
		createDebugBoundingBoxLine();
#endif
		return true;
	}
	else
	{
		return false;
	}
}

void Voxel::UI::Slider::loadBarBuffer(const std::array<float, 12>& vertices, const std::array<float, 8>& uvs, const std::array<unsigned int, 6>& indices)
{
	if (barVao)
	{
		glDeleteVertexArrays(1, &barVao);
	}

	glGenVertexArrays(1, &barVao);
	glBindVertexArray(barVao);
	
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

	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices.front(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &uvbo);
	glDeleteBuffers(1, &ibo);
}

void Voxel::UI::Slider::loadButtonBuffer(const std::vector<float>& vertices, const std::vector<float>& uvs, const std::vector<unsigned int>& indices)
{
	if (buttonVao)
	{
		glDeleteVertexArrays(1, &buttonVao);
	}

	glGenVertexArrays(1, &buttonVao);
	glBindVertexArray(buttonVao);


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

	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices.front(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &uvbo);
	glDeleteBuffers(1, &ibo);
}

Voxel::UI::Slider * Voxel::UI::Slider::create(const std::string & name, const std::string & spriteSheetName, const std::string & barImageName, const std::string & buttonImageName, const Type type, const float minValue, const float maxValue)
{
	auto newSlider = new Slider(name);

	if (newSlider->init(spriteSheetName, barImageName, buttonImageName, type, minValue, maxValue))
	{
		return newSlider;
	}
	else
	{
		delete newSlider;
		return nullptr;
	}
}

void Voxel::UI::Slider::enable()
{
	state = State::IDLE;
	updateButtonIndexOffset();
}

void Voxel::UI::Slider::disable()
{
	state = State::BUTTON_DISABLED;
	updateButtonIndexOffset();
}

void Voxel::UI::Slider::updateButtonIndexOffset()
{
	switch (state)
	{
	case Voxel::UI::Slider::State::BUTTON_HOVERED:
		buttonIndexOffset = 6;
		break;
	case Voxel::UI::Slider::State::BAR_CLICKED:
	case Voxel::UI::Slider::State::BUTTON_CLICKED:
		buttonIndexOffset = 12;
		break;
	case Voxel::UI::Slider::State::BUTTON_DISABLED:
		buttonIndexOffset = 18;
		break;
	case Voxel::UI::Slider::State::IDLE:
	case Voxel::UI::Slider::State::BAR_HOVERED:
	default:
		buttonIndexOffset = 0;
		break;
	}
}

float Voxel::UI::Slider::getValueOnMousePosition(const glm::vec2 & mousePosition)
{
	float newValue = 0.0f;

	if (type == Type::HORIZONTAL)
	{
		if (mousePosition.x <= barBoundingBox.getMin().x)
		{
			buttonBoundingBox.center.x = (barBoundingBox.size.x * -0.5f) + barBoundingBox.center.x;

			if (reversedValue)
			{
				newValue = maxValue;
			}
			else
			{
				newValue = minValue;
			}
		}
		else if (mousePosition.x >= barBoundingBox.getMax().x)
		{
			buttonBoundingBox.center.x = (barBoundingBox.size.x * 0.5f) + barBoundingBox.center.x;

			if (reversedValue)
			{
				newValue = minValue;
			}
			else
			{
				newValue = maxValue;
			}
		}
		else if (mousePosition.x == barBoundingBox.center.x)
		{
			// center
			buttonBoundingBox.center.x = barBoundingBox.center.x;
			newValue = maxValue * 0.5f;
		}
		else
		{
			const float distFromCenterToMp = mousePosition.x - barBoundingBox.center.x;

			buttonBoundingBox.center.x = distFromCenterToMp + barBoundingBox.center.x;

			const float mpRange = mousePosition.x - barBoundingBox.getMin().x;

			const float ratio = mpRange / barBoundingBox.size.x;
			
			newValue = (ratio * (maxValue - minValue)) + minValue;

			if (reversedValue)
			{
				newValue = maxValue - newValue;
			}
		}
	}
	else
	{
		// vertical
		if (mousePosition.y <= barBoundingBox.getMin().y)
		{
			buttonBoundingBox.center.y = (barBoundingBox.size.y * -0.5f) + barBoundingBox.center.y;

			if (reversedValue)
			{
				newValue = maxValue;
			}
			else
			{
				newValue = minValue;
			}
		}
		else if (mousePosition.y >= barBoundingBox.getMax().y)
		{
			buttonBoundingBox.center.y = (barBoundingBox.size.y * 0.5f) + barBoundingBox.center.y;

			if (reversedValue)
			{
				newValue = minValue;
			}
			else
			{
				newValue = maxValue;
			}
		}
		else if (mousePosition.y == barBoundingBox.center.y)
		{
			// center
			buttonBoundingBox.center.y = barBoundingBox.center.y;
			newValue = maxValue * 0.5f;
		}
		else
		{
			const float distFromCenterToMp = mousePosition.y - barBoundingBox.center.y;

			buttonBoundingBox.center.y = distFromCenterToMp + barBoundingBox.center.y;

			const float mpRange = mousePosition.y - barBoundingBox.getMin().y;

			const float ratio = mpRange / barBoundingBox.size.y;

			newValue = (ratio * (maxValue - minValue)) + minValue;

			if (reversedValue)
			{
				newValue = maxValue - newValue;
			}
		}
	}

	return newValue;
}

void Voxel::UI::Slider::updateButtonPos()
{
	buttonBoundingBox.center = barBoundingBox.center;

	if (type == Type::HORIZONTAL)
	{
		buttonBoundingBox.center.x -= (barSize.x * scale.x * 0.5f);
		buttonBoundingBox.center.x += (((currentValue - minValue) / (maxValue - minValue)) * barBoundingBox.size.x);
	}
	else
	{
		buttonBoundingBox.center.y -= (barSize.y * scale.y * 0.5f);
		buttonBoundingBox.center.y += (((currentValue - minValue) / (maxValue - minValue)) * barBoundingBox.size.y);
	}
}

void Voxel::UI::Slider::updateBoundingBox()
{
	// Update bounding box
	Voxel::UI::TransformNode::updateBoundingBox();

	// Update both bounding box
	if (parent)
	{
		auto screenPos = glm::vec2(getParentMatrix() * glm::vec4(position, 1.0f, 1.0f));
		auto shiftPos = screenPos + (pivot * contentSize * scale * -1.0f);

		barBoundingBox.center = shiftPos;
	}
	else
	{
		barBoundingBox.center = position;
	}

	barBoundingBox.size = barSize * scale;

	updateButtonPos();

	buttonBoundingBox.size = buttonSize * scale;
}

void Voxel::UI::Slider::updateModelMatrix()
{
	Voxel::UI::TransformNode::updateModelMatrix();
}

void Voxel::UI::Slider::updateMouseMoveFalse()
{
	// If it was hovering, set back to idle
	if (state == State::BAR_HOVERED || state == State::BUTTON_HOVERED)
	{
		//std::cout << "Was hovering, out of slider. back to idle\n";
		state = State::IDLE;

		updateButtonIndexOffset();

		if (onMouseExit)
		{
			onMouseExit(this);
		}
	}
}

bool Voxel::UI::Slider::updateSliderMouseMove(const glm::vec2 & mousePosition, const glm::vec2 & mouseDelta)
{
	// check if slider is interacble
	if (isInteractable())
	{
		// Check state first
		if (state == State::BUTTON_CLICKED || state == State::BAR_CLICKED)
		{
			// clicking button. Move button
			// clicking bar, move button
			auto newValue = getValueOnMousePosition(mousePosition);

			if (newValue != currentValue)
			{
				currentValue = newValue;

				if (onValueChange)
				{
					onValueChange(this);
				}
			}

			return true;
		}
		else
		{
			//not clicked. Check if mouse is on button
			if (buttonBoundingBox.containsPoint(mousePosition))
			{
				if (state == State::IDLE || state == State::BAR_HOVERED)
				{
					if (state == State::IDLE)
					{
						if (onMouseEnter)
						{
							onMouseEnter(this);
						}
					}

					state = State::BUTTON_HOVERED;

					updateButtonIndexOffset();
				}

				if (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f)
				{
					if (onMouseMove)
					{
						onMouseMove(this);
					}
				}

				return true;
			}
			else
			{
				// Then check if bar has
				if (barBoundingBox.containsPoint(mousePosition))
				{
					if (state == State::IDLE || state == State::BUTTON_HOVERED)
					{
						if (state == State::IDLE)
						{
							if (onMouseEnter)
							{
								onMouseEnter(this);
							}
						}

						state = State::BAR_HOVERED;

						updateButtonIndexOffset();
					}

					if (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f)
					{
						if (onMouseMove)
						{
							onMouseMove(this);
						}
					}

					return true;
				}
				else
				{
					updateMouseMoveFalse();
				}
			}

			return Voxel::UI::TransformNode::updateMouseMove(mousePosition, mouseDelta);
		}
	}
	// Else, not interactable

	return false;
}

bool Voxel::UI::Slider::updateMouseMove(const glm::vec2 & mousePosition, const glm::vec2 & mouseDelta)
{
	if (visibility)
	{
		// check state first. If slider was clicked and changing value, ignore all child update
		if (state == State::BUTTON_CLICKED || state == State::BAR_CLICKED)
		{
			return updateSliderMouseMove(mousePosition, mouseDelta);
		}
		else
		{
			// not clicking
			if (children.empty())
			{
				// Has no children. update self
				return updateSliderMouseMove(mousePosition, mouseDelta);
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
						// child hovered.
						// Known issue: When sliders have other ui object as sibling, changing value gets blocked by slibling's update mouse event.
						// Todo: fix it?
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
					childHovered = updateSliderMouseMove(mousePosition, mouseDelta);
				}

				return childHovered;
			}
		}
	}
	else
	{
		return false;
	}
}

bool Voxel::UI::Slider::updateMousePress(const glm::vec2 & mousePosition, const int button)
{
	if (visibility)
	{
		bool pressed = false;

		// check if button is interactable
		if (isInteractable())
		{
			// interactable
			if (button == GLFW_MOUSE_BUTTON_1)
			{
				// clicked with left mouse
				if (buttonBoundingBox.containsPoint(mousePosition))
				{
					// clicked button
					if (state == State::BUTTON_HOVERED)
					{
						state = State::BUTTON_CLICKED;

						auto newValue = getValueOnMousePosition(mousePosition);

						if (newValue != currentValue)
						{
							currentValue = newValue;

							if (onValueChange)
							{
								onValueChange(this);
							}
						}

						updateButtonIndexOffset();
						pressed = true;

						if (onMousePressed)
						{
							onMousePressed(this, button);
						}

						if (onButtonPressed)
						{
							onButtonPressed(this);
						}
					}
				}
				else if (barBoundingBox.containsPoint(mousePosition))
				{
					// clicked bar
					if (state == State::BAR_HOVERED)
					{
						state = State::BAR_CLICKED;
						auto newValue = getValueOnMousePosition(mousePosition);

						if (newValue != currentValue)
						{
							currentValue = newValue;

							if (onValueChange)
							{
								onValueChange(this);
							}
						}

						updateButtonIndexOffset();
						pressed = true;

						if (onMousePressed)
						{
							onMousePressed(this, button);
						}

						if (onBarPressed)
						{
							onBarPressed(this);
						}
					}
				}
				// Else, didn't click either bar or button
			}
			// Else, not a left mouse button
		}
		// Else, not interactable

		if (!pressed)
		{
			// slider wasn't pressed. Check if there is another ui in children that might possibly press
			pressed = Voxel::UI::TransformNode::updateMousePress(mousePosition, button);
		}

		return pressed;
	}
	// Else, not visible

	return false;
}

bool Voxel::UI::Slider::updateMouseRelease(const glm::vec2 & mousePosition, const int button)
{
	if (visibility)
	{
		bool released = false;

		// Check if slider is interactable
		if (isInteractable())
		{
			// interactable
			if (button == GLFW_MOUSE_BUTTON_1)
			{
				// No matter where mouse was released, if it was clicking button, finish sliding
				if (state == State::BAR_CLICKED || state == State::BUTTON_CLICKED)
				{
					// finish
					state = State::IDLE;
					updateButtonIndexOffset();
					released = true;

					if (onMouseReleased)
					{
						onMouseReleased(this, button);
					}

					if (onFinished)
					{
						onFinished(this);
					}
				}
			}
			// Else, mouse button wasn't left mouse button
		}
		// Else, not iteractable

		if (!released)
		{
			// didn't release. Check children.
			released = Voxel::UI::TransformNode::updateMouseRelease(mousePosition, button);
		}

		return released;
	}
	// Else, not visible

	return false;
}

void Voxel::UI::Slider::setValue(const float value)
{
	if (minValue <= value && value <= maxValue)
	{
		currentValue = value;

		updateButtonPos();
	}
}

float Voxel::UI::Slider::getValue() const
{
	return currentValue;
}

void Voxel::UI::Slider::setOnButtonPressed(const std::function<void(Voxel::UI::Slider*)>& func)
{
	onButtonPressed = func;
}

void Voxel::UI::Slider::setOnBarPressed(const std::function<void(Voxel::UI::Slider*)>& func)
{
	onBarPressed = func;
}

void Voxel::UI::Slider::setOnValueChange(const std::function<void(Voxel::UI::Slider*)>& func)
{
	onValueChange = func;
}

void Voxel::UI::Slider::setOnFinished(const std::function<void(Voxel::UI::Slider*)>& func)
{
	onFinished = func;
}

void Voxel::UI::Slider::renderSelf()
{
	if (texture == nullptr) return;
	if (program == nullptr) return;

	program->use(true);
	program->setUniformMat4("modelMat", glm::scale(modelMat, glm::vec3(scale, 1)));
	program->setUniformFloat("opacity", opacity);
	program->setUniformVec3("color", color);

	texture->activate(GL_TEXTURE0);
	texture->bind();

	if (barVao)
	{
		glBindVertexArray(barVao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	if (type == Type::HORIZONTAL)
	{
		program->setUniformMat4("modelMat", glm::scale(modelMat * glm::translate(glm::mat4(1.0f), glm::vec3(buttonBoundingBox.center.x - barBoundingBox.center.x, 0.0f, 0.0f)), glm::vec3(scale, 1)));
	}
	else
	{
		program->setUniformMat4("modelMat", glm::scale(modelMat * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, buttonBoundingBox.center.y - barBoundingBox.center.y, 0.0f)), glm::vec3(scale, 1)));
	}

	if (buttonVao)
	{
		glBindVertexArray(buttonVao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(buttonIndexOffset * sizeof(GLuint)));
	}

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX && V_DEBUG_DRAW_SLIDER_BOUNDING_BOX
	if (bbVao)
	{
		auto lineProgram = ProgramManager::getInstance().getProgram(Voxel::ProgramManager::PROGRAM_NAME::LINE_SHADER);
		lineProgram->use(true);
		lineProgram->setUniformMat4("modelMat", modelMat);
		lineProgram->setUniformMat4("viewMat", glm::mat4(1.0f));

		glBindVertexArray(bbVao);
		glDrawArrays(GL_LINES, 0, 24);
	}
#endif
}

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX && V_DEBUG_DRAW_SLIDER_BOUNDING_BOX
void Voxel::UI::Slider::createDebugBoundingBoxLine()
{
	if (bbVao)
	{
		glDeleteVertexArrays(1, &bbVao);
		bbVao = 0;
	}

	glGenVertexArrays(1, &bbVao);
	glBindVertexArray(bbVao);

	// unlike other ui, use contensize because slider itself have 0 sized bounding box
	auto bbMin = -contentSize * scale * 0.5f;
	auto bbMax = contentSize * scale * 0.5f;

	auto barMin = -barBoundingBox.size * 0.5f;
	auto barMax = barBoundingBox.size * 0.5f;

	auto btnMin = -buttonBoundingBox.size * 0.5f;
	auto btnMax = buttonBoundingBox.size * 0.5f;

	std::vector<float> lineVertices =
	{
		// outer bounding box. Red
		bbMin.x, bbMin.y, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		bbMin.x, bbMax.y, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,

		bbMin.x, bbMax.y, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		bbMax.x, bbMax.y, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,

		bbMax.x, bbMax.y, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		bbMax.x, bbMin.y, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,

		bbMax.x, bbMin.y, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		bbMin.x, bbMin.y, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,

		// bar. Orange
		barMin.x, barMin.y, 0.0f, 1.0f, 0.6f, 0.0f, 1.0f,
		barMin.x, barMax.y, 0.0f, 1.0f, 0.6f, 0.0f, 1.0f,

		barMin.x, barMax.y, 0.0f, 1.0f, 0.6f, 0.0f, 1.0f,
		barMax.x, barMax.y, 0.0f, 1.0f, 0.6f, 0.0f, 1.0f,

		barMax.x, barMax.y, 0.0f, 1.0f, 0.6f, 0.0f, 1.0f,
		barMax.x, barMin.y, 0.0f, 1.0f, 0.6f, 0.0f, 1.0f,

		barMax.x, barMin.y, 0.0f, 1.0f, 0.6f, 0.0f, 1.0f,
		barMin.x, barMin.y, 0.0f, 1.0f, 0.6f, 0.0f, 1.0f,

		// bar. Blue
		btnMin.x, btnMin.y, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		btnMin.x, btnMax.y, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,

		btnMin.x, btnMax.y, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		btnMax.x, btnMax.y, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,

		btnMax.x, btnMax.y, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		btnMax.x, btnMin.y, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,

		btnMax.x, btnMin.y, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		btnMin.x, btnMin.y, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
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