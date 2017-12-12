#include "Button.h"

// voxel
#include "Quad.h"
#include "ProgramManager.h"
#include "Program.h"
#include "Utility.h"
#include "InputHandler.h"

// glm
#include <glm/gtx/transform.hpp>

Voxel::UI::Button::Button(const std::string & name)
	: RenderNode(name)
	, buttonState(State::IDLE)
	, currentIndex(0)
	, onButtonClicked(nullptr)
{}

Voxel::UI::Button* Voxel::UI::Button::create(const std::string & name, const std::string & spriteSheetName, const std::string & buttonImageFileName)
{
	auto newButton = new Voxel::UI::Button(name);

	auto& ssm = SpriteSheetManager::getInstance();

	auto ss = ssm.getSpriteSheetByKey(spriteSheetName);

	if (ss)
	{
		if (newButton->init(ss, buttonImageFileName))
		{
			return newButton;
		}
	}

	delete newButton;
	return nullptr;
}

bool Voxel::UI::Button::init(SpriteSheet * ss, const std::string & buttonImageFileName)
{
	setInteractable();

	texture = ss->getTexture();

	if (texture == nullptr)
	{
		return false;
	}

	texture->setLocationOnProgram(ProgramManager::PROGRAM_NAME::UI_TEXTURE_SHADER);

	std::vector<float> vertices;
	std::vector<float> uvs;
	std::vector<unsigned int> indices;

	std::string fileName;
	std::string fileExt;

	Utility::String::fileNameToNameAndExt(buttonImageFileName, fileName, fileExt);

	auto quadIndices = Quad::indices;

	std::array<std::string, 4> fileNames = { fileName + "_idle" + fileExt, fileName + "_hovered" + fileExt, fileName + "_clicked" + fileExt, fileName + "_disabled" + fileExt };

	for (unsigned int i = 0; i < fileNames.size(); i++)
	{
		auto imageEntry = ss->getImageEntry(fileNames.at(i));

		if (imageEntry)
		{
			auto size = glm::vec2(imageEntry->width, imageEntry->height);
			auto curVertices = Quad::getVertices(size);

			vertices.insert(vertices.end(), curVertices.begin(), curVertices.end());

			auto& uvOrigin = imageEntry->uvOrigin;
			auto& uvEnd = imageEntry->uvEnd;

			uvs.push_back(uvOrigin.x);
			uvs.push_back(uvOrigin.y);
			uvs.push_back(uvOrigin.x);
			uvs.push_back(uvEnd.y);
			uvs.push_back(uvEnd.x);
			uvs.push_back(uvOrigin.y);
			uvs.push_back(uvEnd.x);
			uvs.push_back(uvEnd.y);

			for (auto index : quadIndices)
			{
				indices.push_back(index + (4 * i));
			}

			frameSizes.at(i) = size;
		}
		else
		{
			return false;
		}
	}

	boundingBox.center = position;
	boundingBox.size = frameSizes.front();

	contentSize = boundingBox.size;

	build(vertices, uvs, indices);

	return true;
}

void Voxel::UI::Button::build(const std::vector<float>& vertices, const std::vector<float>& uvs, const std::vector<unsigned int>& indices)
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

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX
	createDebugBoundingBoxLine();
#endif
}

void Voxel::UI::Button::enable()
{
	buttonState = State::IDLE;
	currentIndex = 0;
}

void Voxel::UI::Button::disable()
{
	buttonState = State::DISABLED;
	currentIndex = 18;
}

bool Voxel::UI::Button::updateMouseMove(const glm::vec2 & mousePosition)
{
	// check if button is interacble
	if (isInteractable())
	{
		// check bb
		if (boundingBox.containsPoint(mousePosition))
		{
			if (buttonState == State::IDLE)
			{
				// idle. hover it
				buttonState = State::HOVERED;
				currentIndex = 6;
			}
			// else, nothing to do
			return true;
		}
		else
		{
			// mouse is not in bounding box. Hover it
			if (buttonState == State::HOVERED || buttonState == State::CLICKED)
			{
				// Hovered or clicked. Set back to idle
				buttonState = State::IDLE;
				currentIndex = 0;
			}
		}
		// else, nothing to do
	}
	// else, not interactable

	return false;
}

void Voxel::UI::Button::setOnButtonClickCallbackFunc(const std::function<void()>& func)
{
	onButtonClicked = func;
}

bool Voxel::UI::Button::updateMouseMove(const glm::vec2 & mousePosition, const glm::vec2& mouseDelta)
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
				if (buttonState == State::HOVERED || buttonState == State::CLICKED)
				{
					// Hovered or clicked. Set back to idle
					buttonState = State::IDLE;
					currentIndex = 0;
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

bool Voxel::UI::Button::updateMousePress(const glm::vec2 & mousePosition, const int button)
{
	if (visibility)
	{
		bool clicked = false;

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
					if (buttonState == State::HOVERED)
					{
						// was hovering. click
						buttonState = State::CLICKED;
						currentIndex = 12;
						clicked = true;
					}
					// else, disabled or not hovering
				}
			}
		}

		if (!clicked)
		{
			// button was not clicked. check if there is another button in children that might possibly clicked
			clicked = Voxel::UI::TransformNode::updateMousePress(mousePosition, button);
		}

		return clicked;
	}
	else
	{
		return false;
	}
}

bool Voxel::UI::Button::updateMouseRelease(const glm::vec2 & mousePosition, const int button)
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
					if (buttonState == State::CLICKED)
					{
						buttonState = State::IDLE;
						currentIndex = 0;
						released = true;

						// button clicked!
						if (onButtonClicked)
						{
							onButtonClicked();
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

void Voxel::UI::Button::renderSelf()
{
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
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(currentIndex * sizeof(GLuint)));
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