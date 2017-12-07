#include "Button.h"

// voxel
#include "SpriteSheet.h"
#include "Quad.h"
#include "ProgramManager.h"
#include "Program.h"
#include "Utility.h"
#include "InputHandler.h"

Voxel::UI::Button::Button(const std::string & name)
	: RenderNode(name)
	, buttonState(State::IDLE)
	, currentIndex(0)
{}

Voxel::UI::Button* Voxel::UI::Button::create(const std::string & name, const std::string & spriteSheetName, const std::string & buttonImageFileName)
{
	auto newButton = new Voxel::UI::Button(name);

	auto& ssm = SpriteSheetManager::getInstance();

	auto ss = ssm.getSpriteSheet(spriteSheetName);

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

void Voxel::UI::Button::updateMouseMove(const glm::vec2 & mousePosition)
{
	if (buttonState == State::DISABLED)
	{
		return;
	}
	else
	{
		if (buttonState == State::IDLE)
		{
			if (boundingBox.containsPoint(mousePosition))
			{
				buttonState = State::HOVERED;
				currentIndex = 6;
			}
		}
		else if (buttonState == State::HOVERED || buttonState == State::CLICKED)
		{
			if (!boundingBox.containsPoint(mousePosition))
			{
				buttonState = State::IDLE;
				currentIndex = 0;
			}
		}
	}
}

void Voxel::UI::Button::updateMouseClick(const glm::vec2 & mousePosition, const int button)
{
	if (button == GLFW_MOUSE_BUTTON_1)
	{
		if (buttonState == State::DISABLED)
		{
			return;
		}
		else
		{
			if (buttonState == State::HOVERED)
			{
				if (boundingBox.containsPoint(mousePosition))
				{
					buttonState = State::CLICKED;
					currentIndex = 12;
				}
			}
		}
	}
}

void Voxel::UI::Button::updateMouseRelease(const glm::vec2 & mousePosition, const int button)
{
	if (button == GLFW_MOUSE_BUTTON_1)
	{
		if (buttonState == State::DISABLED)
		{
			return;
		}
		else
		{
			if (buttonState == State::CLICKED)
			{
				if (boundingBox.containsPoint(mousePosition))
				{
					buttonState = State::IDLE;
					currentIndex = 0;

					// button clicked!
					std::cout << "Button " << name << " clicked\n";
				}
			}
		}
	}
}

void Voxel::UI::Button::renderSelf()
{
	if (texture == nullptr) return;
	if (!visibility) return;
	if (program == nullptr) return;

	program->use(true);
	program->setUniformMat4("modelMat", modelMat);
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