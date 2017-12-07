#include "CheckBox.h"

// voxel
#include "SpriteSheet.h"
#include "Quad.h"
#include "ProgramManager.h"
#include "Program.h"
#include "Utility.h"

Voxel::UI::CheckBox::CheckBox(const std::string& name)
	: RenderNode(name)
	, currentIndex(0)
	, checkBoxState(State::DESELECTED)
{}

Voxel::UI::CheckBox * Voxel::UI::CheckBox::create(const std::string & name, const std::string & spriteSheetName, const std::string & checkBoxImageFileName)
{
	auto newCheckBox = new Voxel::UI::CheckBox(name);

	auto& ssm = SpriteSheetManager::getInstance();

	auto ss = ssm.getSpriteSheet(spriteSheetName);

	if (ss)
	{
		if (newCheckBox->init(ss, checkBoxImageFileName))
		{
			return newCheckBox;
		}
	}

	delete newCheckBox;
	return nullptr;
}

bool Voxel::UI::CheckBox::init(SpriteSheet * ss, const std::string & checkBoxImageFileName)
{
	texture = ss->getTexture();

	if (texture == nullptr)
	{
		return false;
	}

	texture->setLocationOnProgram(ProgramManager::PROGRAM_NAME::UI_TEXTURE_SHADER);

	std::string fileName;
	std::string fileExt;

	Utility::String::fileNameToNameAndExt(checkBoxImageFileName, fileName, fileExt);

	std::vector<float> vertices;
	std::vector<float> uvs;
	std::vector<unsigned int> indices;
	auto quadIndices = Quad::indices;

	std::array<std::string, 7> fileNames =
	{
		fileName + "_deselected" + fileExt,
		fileName + "_hovered" + fileExt,
		fileName + "_clicked" + fileExt,
		fileName + "_selected" + fileExt,
		fileName + "_hoveredSelected" + fileExt,
		fileName + "_clickedSelected" + fileExt,
		fileName + "_disabled" + fileExt
	};

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

			//frameSizes.at(i) = size;
		}
		else
		{
			return false;
		}
	}

	boundingBox.center = position;
	auto size = ss->getImageEntry(fileNames.at(0));
	boundingBox.size = glm::vec2(size->width, size->height);

	contentSize = boundingBox.size;

	build(vertices, uvs, indices);

	return true;
}

void Voxel::UI::CheckBox::build(const std::vector<float>& vertices, const std::vector<float>& uvs, const std::vector<unsigned int>& indices)
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

void Voxel::UI::CheckBox::updateCurrentIndex()
{
	switch (checkBoxState)
	{
	case Voxel::UI::CheckBox::State::DESELECTED:
		currentIndex = 0;
		break;
	case Voxel::UI::CheckBox::State::HOVERED:
		currentIndex = 6;
		break;
	case Voxel::UI::CheckBox::State::CLICKED:
		currentIndex = 12;
		break;
	case Voxel::UI::CheckBox::State::SELECTED:
		currentIndex = 18;
		break;
	case Voxel::UI::CheckBox::State::HOVERED_SELECTED:
		currentIndex = 24;
		break;
	case Voxel::UI::CheckBox::State::CLICKED_SELECTED:
		currentIndex = 30;
		break;
	case Voxel::UI::CheckBox::State::DISABLED:
		currentIndex = 36;
		break;
	default:
		currentIndex = 0;
		break;
	}
}

void Voxel::UI::CheckBox::enable()
{
	if (checkBoxState == State::DISABLED)
	{
		return;
	}
	else
	{
		checkBoxState = prevCheckBoxState;

		updateCurrentIndex();
	}
}

void Voxel::UI::CheckBox::disable()
{
	prevCheckBoxState = checkBoxState;
	checkBoxState = State::DISABLED;

	updateCurrentIndex();
}

void Voxel::UI::CheckBox::select()
{
	if (checkBoxState == State::DISABLED)
	{
		return;
	}
	else
	{
		checkBoxState = State::SELECTED;

		updateCurrentIndex();
	}
}

void Voxel::UI::CheckBox::deselect()
{
	if (checkBoxState == State::DISABLED)
	{
		return;
	}
	else
	{
		checkBoxState = State::DESELECTED;

		updateCurrentIndex();
	}
}

void Voxel::UI::CheckBox::updateMouseMove(const glm::vec2 & mousePosition)
{
	// mouse moved
	if (checkBoxState == State::DISABLED)
	{
		// check box is disabled. do nothing
		return;
	}
	else
	{
		// Check if mouse is in check box
		if (boundingBox.containsPoint(mousePosition))
		{
			// Mouse is in check box
			if (checkBoxState == State::DESELECTED)
			{
				// check box was deselected. hover it
				checkBoxState = State::HOVERED;
			}
			else if (checkBoxState == State::SELECTED)
			{
				// check box was selected. hover it
				checkBoxState = State::HOVERED_SELECTED;
			}
		}
		else
		{
			// Mouse is not in check box
			if (checkBoxState == State::HOVERED)
			{
				// was hovering check box
				checkBoxState = State::DESELECTED;
			}
			else if (checkBoxState == State::HOVERED_SELECTED)
			{
				// was hovering selected check box
				checkBoxState = State::SELECTED;
			}
			else if (checkBoxState == State::CLICKED)
			{
				// was clikcing box. cancel
				checkBoxState = State::DESELECTED;
			}
			else if (checkBoxState == State::CLICKED_SELECTED)
			{
				// was clicking selected box. cancel
				checkBoxState = State::SELECTED;
			}
		}

		updateCurrentIndex();
	}
}

bool Voxel::UI::CheckBox::updateMouseClick(const glm::vec2 & mousePosition, const int button)
{
	// Check if mouse is in check box
	if (boundingBox.containsPoint(mousePosition))
	{
		// mouse clicked
		if (checkBoxState == State::DISABLED)
		{
			// check box is disabled. do nothing
			return true;
		}
		else
		{
			// Clicked the check box
			if (checkBoxState == State::HOVERED)
			{
				// Was hovering check box. click
				checkBoxState = State::CLICKED;
			}
			else if (checkBoxState == State::HOVERED_SELECTED)
			{
				// was hovering selected box. click
				checkBoxState = State::CLICKED_SELECTED;
			}

			return true;
		}

		updateCurrentIndex();
	}
	else
	{
		return false;
	}
}

bool Voxel::UI::CheckBox::updateMouseRelease(const glm::vec2 & mousePosition, const int button)
{
	// Check if mouse is in check box
	if (boundingBox.containsPoint(mousePosition))
	{
		// mouse release
		if (checkBoxState == State::DISABLED)
		{
			// check box is disabled. do nothing
			return true;
		}
		else
		{
			// Released in the check box
			if (checkBoxState == State::CLICKED)
			{
				// was clicking. select
				checkBoxState = State::SELECTED;
			}
			else if (checkBoxState == State::CLICKED_SELECTED)
			{
				// was clikcing selected. deslect
				checkBoxState = State::DESELECTED;
			}

			updateCurrentIndex();

			return true;
		}
	}
	else
	{
		return false;
	}
}

void Voxel::UI::CheckBox::renderSelf()
{
	if (texture == nullptr) return;
	if (!visibility) return;
	if (program == nullptr) return;

	program->use(true);
	program->setUniformMat4("modelMat", modelMat);
	program->setUniformFloat("opacity", opacity);

	texture->activate(GL_TEXTURE0);
	texture->bind();

	if (vao)
	{
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(currentIndex * sizeof(GLuint)));
	}
}