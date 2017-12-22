// pch
#include "PreCompiled.h"

#include "Image.h"

// voxel
#include "Quad.h"
#include "ProgramManager.h"
#include "Program.h"

Voxel::UI::Image::Image(const std::string& name)
	: RenderNode(name)
	, state(State::IDLE)
{}

Voxel::UI::Image::~Image()
{
	//std::cout << "~Image()\n";
}

Voxel::UI::Image * Voxel::UI::Image::create(const std::string & name, const std::string & imageFileName)
{
	auto newImage = new Image(name);

	if (newImage->init(imageFileName))
	{
		return newImage;
	}
	else
	{
		delete newImage;
		return nullptr;
	}
}

Voxel::UI::Image * Voxel::UI::Image::createFromSpriteSheet(const std::string & name, const std::string & spriteSheetName, const std::string & imageFileName)
{
	auto& ssm = SpriteSheetManager::getInstance();

	auto ss = ssm.getSpriteSheetByKey(spriteSheetName);

	if (ss)
	{
		auto newImage = new Image(name);

		if (newImage->initFromSpriteSheet(ss, imageFileName))
		{
			return newImage;
		}
		else
		{
			delete newImage;
			return nullptr;
		}
	}
	else
	{
		return nullptr;
	}
}

Voxel::UI::Image * Voxel::UI::Image::createFromSpriteSheet(const std::string & name, const std::string & spriteSheetName, const std::string & imageFileName, const glm::vec2 & size)
{
	auto& ssm = SpriteSheetManager::getInstance();

	auto ss = ssm.getSpriteSheetByKey(spriteSheetName);

	if (ss)
	{
		auto newImage = new Image(name);

		if (newImage->initFromSpriteSheet(ss, imageFileName, size))
		{
			return newImage;
		}
		else
		{
			delete newImage;
			return nullptr;
		}
	}
	else
	{
		return nullptr;
	}
}

bool Voxel::UI::Image::updateImageMouseMove(const glm::vec2 & mousePosition, const glm::vec2 & mouseDelta)
{
	// Update mouse move for image
	if (isInteractable())
	{
		// interactable
		if (boundingBox.containsPoint(mousePosition))
		{
			// mouse is in ui's bb
			if (state == State::IDLE)
			{
				// was idle. hovered.
				state = State::HOVERED;

				if (onMouseEnter)
				{
					onMouseEnter(this);
				}
			}
			else if (state == State::HOVERED)
			{
				// was hovering.
				if (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f)
				{
					if (onMouseMove)
					{
						onMouseMove(this);
					}
				}
			}
			else if (state == State::CLICKED)
			{
				// was clicking.
				if (isDraggable())
				{
					// drag
					addPosition(mouseDelta);
				}
			}

			return true;
		}
		else
		{
			// mouse is not in ui's bb
			if (state == State::HOVERED)
			{
				// back to idle
				state = State::IDLE;

				if (onMouseExit)
				{
					onMouseExit(this);
				}
			}
		}
	}
	// Else, not interactable

	return false;
}

bool Voxel::UI::Image::updateMouseMove(const glm::vec2 & mousePosition, const glm::vec2& mouseDelta)
{
	// Update mouse move for this ui
	if (visibility)
	{
		// visible
		if (children.empty())
		{
			// No children. update self.
			return updateImageMouseMove(mousePosition, mouseDelta);
		}
		else
		{
			// check state
			if (state == State::CLICKED && isDraggable())
			{
				// image is clicked and dragging. ignore evetns from children
				return true;
			}
			else
			{
				// Image is either not clicked or not dragging.
				bool childHovered = false;

				// Check if one of child has mouse move event
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
					// mouse is hovering one of children. Back to idle state.
					if (state == State::HOVERED)
					{
						// was hovering. back to idle
						state = State::IDLE;
					}
					else if (state == State::CLICKED)
					{
						// was clicking, was not dragging. back to idle
						state = State::IDLE;
					}
				}
				else
				{
					// There was no mouse move event on child
					childHovered = updateImageMouseMove(mousePosition, mouseDelta);
				}

				return childHovered;
			}
		}
	}
	// Else, not visible
	
	return false;
}

bool Voxel::UI::Image::updateMousePress(const glm::vec2 & mousePosition, const int button)
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
						if (isDraggable())
						{
							state = State::CLICKED;
						}

						if (onMousePressed)
						{
							onMousePressed(this, button);
						}

						pressed = true;
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
	// Else, not visible

	return false;
}

bool Voxel::UI::Image::updateMouseRelease(const glm::vec2 & mousePosition, const int button)
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
				// released with left mouse button
				if (state != State::IDLE)
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
			// button was not clicked. check if there is another button in children that might possibly released
			released = Voxel::UI::TransformNode::updateMouseRelease(mousePosition, button);
		}

		return released;
	}

	return false;
}

bool Voxel::UI::Image::init(const std::string& textureName)
{
	texture = Texture2D::create(textureName, GL_TEXTURE_2D);

	if (texture == nullptr)
	{
		return false;
	}

	texture->setLocationOnProgram(ProgramManager::PROGRAM_NAME::UI_TEXTURE_SHADER);

	auto size = glm::vec2(texture->getTextureSize());

	std::array<float, 12> vertices = { 0.0f };

	float widthHalf = size.x * 0.5f;
	float heightHalf = size.y * 0.5f;

	// Add vertices from 0 to 4
	// 0
	vertices.at(0) = -widthHalf;
	vertices.at(1) = -heightHalf;
	vertices.at(2) = 0.0f;

	//1
	vertices.at(3) = -widthHalf;
	vertices.at(4) = heightHalf;
	vertices.at(5) = 0.0f;

	//2
	vertices.at(6) = widthHalf;
	vertices.at(7) = -heightHalf;
	vertices.at(8) = 0.0f;

	//3
	vertices.at(9) = widthHalf;
	vertices.at(10) = heightHalf;
	vertices.at(11) = 0.0f;

	boundingBox.center = position;
	boundingBox.size = size;

	contentSize = size;

	build(vertices, Quad::uv, Quad::indices);

	return true;
}

bool Voxel::UI::Image::initFromSpriteSheet(SpriteSheet* ss, const std::string& textureName)
{
	texture = ss->getTexture();

	if (texture == nullptr)
	{
		return false;
	}

	texture->setLocationOnProgram(ProgramManager::PROGRAM_NAME::UI_TEXTURE_SHADER);

	auto imageEntry = ss->getImageEntry(textureName);

	if (imageEntry)
	{
		initImage(imageEntry, glm::vec2(imageEntry->width, imageEntry->height));

		return true;
	}
	else
	{
		return false;
	}
}

bool Voxel::UI::Image::initFromSpriteSheet(SpriteSheet * ss, const std::string & textureName, const glm::vec2 & size)
{
	texture = ss->getTexture();

	if (texture == nullptr)
	{
		return false;
	}

	texture->setLocationOnProgram(ProgramManager::PROGRAM_NAME::UI_TEXTURE_SHADER);

	auto imageEntry = ss->getImageEntry(textureName);

	if (imageEntry)
	{
		initImage(imageEntry, size);

		return true;
	}
	else
	{
		return false;
	}
}

void Voxel::UI::Image::initImage(const ImageEntry * ie, const glm::vec2 & size)
{
	std::array<float, 12> vertices = { 0.0f };

	float widthHalf = size.x * 0.5f;
	float heightHalf = size.y * 0.5f;

	// Add vertices from 0 to 4
	// 0
	vertices.at(0) = -widthHalf;
	vertices.at(1) = -heightHalf;
	vertices.at(2) = 0.0f;

	//1
	vertices.at(3) = -widthHalf;
	vertices.at(4) = heightHalf;
	vertices.at(5) = 0.0f;

	//2
	vertices.at(6) = widthHalf;
	vertices.at(7) = -heightHalf;
	vertices.at(8) = 0.0f;

	//3
	vertices.at(9) = widthHalf;
	vertices.at(10) = heightHalf;
	vertices.at(11) = 0.0f;

	auto& uvOrigin = ie->uvOrigin;
	auto& uvEnd = ie->uvEnd;

	std::array<float, 8> uvs = { 0.0f };

	uvs.at(0) = uvOrigin.x;
	uvs.at(1) = uvOrigin.y;
	uvs.at(2) = uvOrigin.x;
	uvs.at(3) = uvEnd.y;
	uvs.at(4) = uvEnd.x;
	uvs.at(5) = uvOrigin.y;
	uvs.at(6) = uvEnd.x;
	uvs.at(7) = uvEnd.y;

	boundingBox.center = position;
	boundingBox.size = size;

	contentSize = size;

	build(vertices, uvs, Quad::indices);
}

void Voxel::UI::Image::build(const std::array<float, 12>& vertices, const std::array<float, 8>& uvs, const std::array<unsigned int, 6>& indices)
{
	if (vao)
	{
		// Delte array
		glDeleteVertexArrays(1, &vao);
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

	//============= find error here. error count: 14. Only during using sprite sheet
	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices.front(), GL_STATIC_DRAW);
	//========================

	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &uvbo);
	glDeleteBuffers(1, &ibo);

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX && V_DEBUG_DRAW_IMAGE_BOUNDING_BOX
	createDebugBoundingBoxLine();
#endif
}

void Voxel::UI::Image::renderSelf()
{
	// only render self
	if (texture == nullptr) return;
	if (program == nullptr) return;

	program->use(true);
	program->setUniformMat4("modelMat", glm::scale(modelMat, glm::vec3(scale, 1)));
	program->setUniformFloat("opacity", opacity);
	program->setUniformVec3("color", color);

	texture->activate(GL_TEXTURE0);
	texture->bind();

	if (vao)
	{
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX && V_DEBUG_DRAW_IMAGE_BOUNDING_BOX
	if (bbVao)
	{
		auto lineProgram = ProgramManager::getInstance().getProgram(Voxel::ProgramManager::PROGRAM_NAME::LINE_SHADER);
		lineProgram->use(true);

		auto mat = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0));

		if (pivot.x != 0.0f || pivot.y != 0.0f)
		{
			mat = glm::translate(mat, glm::vec3(pivot * getContentSize() * -1.0f, 0));
		}

		if (parent)
		{
			mat = getParentMatrix() * mat;
		}

		lineProgram->setUniformMat4("modelMat", mat);
		lineProgram->setUniformMat4("viewMat", glm::mat4(1.0f));

		glBindVertexArray(bbVao);
		glDrawArrays(GL_LINES, 0, 8);
	}
#endif
}