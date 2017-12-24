// pch
#include "PreCompiled.h"

#include "ColorPicker.h"

// voxel
#include "Program.h"
#include "ProgramManager.h"
#include "SpriteSheet.h"
#include "Quad.h"
#include "Color.h"

Voxel::UI::ColorPicker::ColorPicker(const std::string& name)
	: RenderNode(name)
	, h(0.0f)
	, s(0.5f)
	, b(0.5f)
	, palleteColor(1.0f, 0.0f, 0.0f, 1.0f)
	, palleteVao(0)
	, palleteSize(0.0f)
	, palleteProgram(nullptr)
	, iconModelMat(1.0f)
	, iconColor(1.0f)
	, iconPos(0.0f)
	, state(State::IDLE)
{}

Voxel::UI::ColorPicker * Voxel::UI::ColorPicker::create(const std::string & name, const glm::vec2 & palleteSize, const std::string & spriteSheetName, const std::string & palleteIconImageName)
{
	auto newColorPicker = new ColorPicker(name);
	if (newColorPicker->init(palleteSize, spriteSheetName, palleteIconImageName))
	{
		return newColorPicker;
	}
	else
	{
		delete newColorPicker;
		return nullptr;
	}
}

bool Voxel::UI::ColorPicker::init(const glm::vec2 & palleteSize, const std::string & spriteSheetName, const std::string & palleteIconImageName)
{
	if (palleteSize.x <= 0.0f || palleteSize.y <= 0.0f) return false;
	if (palleteIconImageName.empty()) return false;
	if (spriteSheetName.empty()) return false;

	auto ssm = &Voxel::SpriteSheetManager::getInstance();

	if (ssm == nullptr) return false;

	auto ss = ssm->getSpriteSheetByKey(spriteSheetName);

	if (ss == nullptr) return false;

	auto palleteIconIE = ss->getImageEntry(palleteIconImageName);

	if (palleteIconIE == nullptr) return false;

	palleteProgram = ProgramManager::getInstance().getProgram(Voxel::ProgramManager::PROGRAM_NAME::UI_COLOR_PICKER_SHADER);

	if (palleteProgram == nullptr) return false;

	program = ProgramManager::getInstance().getProgram(Voxel::ProgramManager::PROGRAM_NAME::UI_TEXTURE_SHADER);

	if (program == nullptr) return false;

	texture = ss->getTexture();
	texture->setLocationOnProgram(ProgramManager::PROGRAM_NAME::UI_TEXTURE_SHADER);

	if (texture == nullptr) return false;

	this->palleteSize = palleteSize;

	setInteractable();

	loadPalleteBuffer();

	loadIconBuffer(palleteIconIE);
	
	boundingBox.center = position;
	boundingBox.size = palleteSize;

	contentSize = palleteSize;
	
	return true;
}

void Voxel::UI::ColorPicker::loadPalleteBuffer()
{
	if (palleteVao)
	{
		glDeleteVertexArrays(1, &palleteVao);
		palleteVao = 0;
	}

	auto palleteX = palleteSize.x * 0.5f;
	auto palleteY = palleteSize.y * 0.5f;

	std::vector<float> palleteVert =
	{
		-palleteX, -palleteY, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-palleteX, palleteY, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		palleteX, -palleteY, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		palleteX, palleteY, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,

		-palleteX, -palleteY, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-palleteX, palleteY, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		palleteX, -palleteY, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		palleteX, palleteY, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	};

	std::vector<float> colorVert = std::vector<float>(12, 1.0f);

	GLint vertLoc = palleteProgram->getAttribLocation("vert");
	GLint colorLoc = palleteProgram->getAttribLocation("color");

	glGenVertexArrays(1, &palleteVao);
	glBindVertexArray(palleteVao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * palleteVert.size(), &palleteVert.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), nullptr);
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (const GLvoid*)(3 * sizeof(float)));

	std::vector<unsigned int> palleteIndices =
	{
		0, 1, 2, 1, 2, 3,
		4, 5, 6, 5, 6, 7,
	};

	GLuint ibo;

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * palleteIndices.size(), &palleteIndices.front(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
}

void Voxel::UI::ColorPicker::loadIconBuffer(const Voxel::ImageEntry * ie)
{
	auto size = glm::vec2(ie->width, ie->height);
	float widthHalf = size.x * 0.5f;
	float heightHalf = size.y * 0.5f;

	std::array<float, 12> vertices = { -widthHalf, -heightHalf, 0.0f, -widthHalf, heightHalf, 0.0f, widthHalf, -heightHalf, 0.0f, widthHalf, heightHalf, 0.0f };

	auto& uvOrigin = ie->uvOrigin;
	auto& uvEnd = ie->uvEnd;

	std::array<float, 8> uvs = { uvOrigin.x, uvOrigin.y, uvOrigin.x, uvEnd.y, uvEnd.x, uvOrigin.y, uvEnd.x, uvEnd.y };

	if (vao)
	{
		// Delte array
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	program = ProgramManager::getInstance().getProgram(ProgramManager::PROGRAM_NAME::UI_TEXTURE_SHADER);

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

	auto indices = Quad::indices;

	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices.front(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &uvbo);
	glDeleteBuffers(1, &ibo);
}

void Voxel::UI::ColorPicker::updateHSB(const glm::vec2 & mousePosition)
{
	if (boundingBox.containsPoint(mousePosition))
	{
		auto d = mousePosition - (boundingBox.center - (boundingBox.size * 0.5f));

		s = d.x / boundingBox.size.x;
		b = d.y / boundingBox.size.y;
	}
	else
	{
		auto min = boundingBox.getMin();
		auto max = boundingBox.getMax();

		if (mousePosition.x < min.x)
		{
			s = 0.0f;
		}
		else if (mousePosition.x > max.x)
		{
			s = 1.0f;
		}

		if (mousePosition.y < min.y)
		{
			b = 0.0f;
		}
		else if (mousePosition.y > max.y)
		{
			b = 1.0f;
		}
	}
}

void Voxel::UI::ColorPicker::updateIconPos(const glm::vec2 & mousePosition)
{
	auto min = boundingBox.getMin();
	auto max = boundingBox.getMax();
	auto resolvedPos = mousePosition;

	if (mousePosition.x < min.x)
	{
		resolvedPos.x = min.x;
	}
	else if (mousePosition.x > max.x)
	{
		resolvedPos.x = max.x;
	}

	if (mousePosition.y < min.y)
	{
		resolvedPos.y = min.y;
	}
	else if (mousePosition.y > max.y)
	{
		resolvedPos.y = max.y;
	}

	iconPos = resolvedPos - boundingBox.center;

	iconModelMat = modelMat * glm::translate(glm::mat4(1.0f), glm::vec3(iconPos, 0.0f));
}

void Voxel::UI::ColorPicker::updateColor()
{
	if (0.0f <= h && h < 60.0f)
	{
		// R <-> RG
		palleteColor.r = 1.0f;
		palleteColor.g = h / 60.0f;
		palleteColor.b = 0.0f;
	}
	else if (60.0f <= h && h < 120.0f)
	{
		// RG <-> G
		palleteColor.r = 1.0f - ((h - 60.0f) / 60.0f);
		palleteColor.g = 1.0f;
		palleteColor.b = 0.0f;
	}
	else if (120.0f <= h && h < 180.0f)
	{
		// G <-> GB
		palleteColor.r = 0.0f;
		palleteColor.g = 1.0f;
		palleteColor.b = (h - 120.0f) / 60.0f;
	}
	else if (180.0f <= h && h < 240.0f)
	{
		// GB <-> B
		palleteColor.r = 0.0f;
		palleteColor.g = 1.0f - ((h - 180.0f) / 60.0f);
		palleteColor.b = 1.0f;
	}
	else if (240.0f <= h && h < 300.0f)
	{
		// B <-> BR
		palleteColor.r = (h - 240.0f) / 60.0f;
		palleteColor.g = 0.0f;
		palleteColor.b = 1.0f;
	}
	else if (300.0f <= h && h <= 360.0f)
	{
		// BR <-> R
		palleteColor.r = 1.0f;
		palleteColor.g = 0.0f;
		palleteColor.b = 1.0f - ((h - 300.0f) / 60.0f);
	}
}

bool Voxel::UI::ColorPicker::updateColorPickerMouseMove(const glm::vec2 & mousePosition, const glm::vec2& mouseDelta)
{
	// check if color picker is interactable
	if (isInteractable())
	{
		if (state == State::CLICKED)
		{
			updateHSB(mousePosition);
			updateIconPos(mousePosition);

			return true;
		}
		else
		{
			// check bb
			if (boundingBox.containsPoint(mousePosition))
			{
				// check state
				if (state == State::IDLE)
				{
					// hovered
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
				if (state == State::HOVERED)
				{
					state = State::IDLE;

					if (onMouseExit)
					{
						onMouseExit(this);
					}
				}

				return false;
			}
		}
	}
	// Else, not interactable

	return false;
}

void Voxel::UI::ColorPicker::updateModelMatrix()
{
	Voxel::UI::TransformNode::updateModelMatrix();

	iconModelMat = modelMat * glm::translate(glm::mat4(1.0f), glm::vec3(iconPos, 0.0f));
}

bool Voxel::UI::ColorPicker::updateMouseMove(const glm::vec2 & mousePosition, const glm::vec2 & mouseDelta)
{
	if (visibility)
	{
		// visible
		if (children.empty())
		{
			// Has no children. update self
			return updateColorPickerMouseMove(mousePosition, mouseDelta);
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
				childHovered = updateColorPickerMouseMove(mousePosition, mouseDelta);
			}

			return childHovered;
		}
	}
	else
	{
		return false;
	}
}

bool Voxel::UI::ColorPicker::updateMousePress(const glm::vec2 & mousePosition, const int button)
{
	if (visibility)
	{
		bool pressed = false;

		// check if color picker is interactable
		if (isInteractable())
		{
			if (button == GLFW_MOUSE_BUTTON_1)
			{
				if (boundingBox.containsPoint(mousePosition))
				{
					if (state == State::HOVERED)
					{
						state = State::CLICKED;

						updateHSB(mousePosition);
						updateIconPos(mousePosition);

						if (onMousePressed)
						{
							onMousePressed(this, button);
						}
					}

					pressed = true;
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

bool Voxel::UI::ColorPicker::updateMouseRelease(const glm::vec2 & mousePosition, const int button)
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

void Voxel::UI::ColorPicker::setH(const float h)
{
	this->h = glm::clamp(h, 0.0f, 360.0f);

	updateColor();
}

glm::vec3 Voxel::UI::ColorPicker::getHSB() const
{
	return glm::vec3(h, s, b);
}

glm::vec3 Voxel::UI::ColorPicker::getRGB() const
{
	return Color::HSV2RGB(h / 360.0f, s, b);
}

void Voxel::UI::ColorPicker::renderSelf()
{
	// only render self
	if (texture == nullptr) return;
	if (palleteProgram == nullptr) return;

	palleteProgram->use(true);
	palleteProgram->setUniformMat4("modelMat", glm::scale(modelMat, glm::vec3(scale, 1)));
	palleteProgram->setUniformVec4("palleteColor", palleteColor);

	if (palleteVao)
	{
		glBindVertexArray(palleteVao);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
	}

	if (texture == nullptr) return;
	if (program == nullptr) return;

	program->use(true);
	program->setUniformMat4("modelMat", glm::scale(iconModelMat, glm::vec3(scale, 1)));
	program->setUniformFloat("opacity", opacity);
	program->setUniformVec3("color", iconColor);

	texture->activate(GL_TEXTURE0);
	texture->bind();

	if (vao)
	{
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
}
