#include "Image.h"

// voxel
#include "SpriteSheet.h"
#include "Quad.h"
#include "ProgramManager.h"
#include "Program.h"

Voxel::UI::Image::Image(const std::string& name)
	: RenderNode(name)
	, imageState(State::IDLE)
{}

Voxel::UI::Image::~Image()
{
	//std::cout << "~Image()\n";
}

Voxel::UI::Image * Voxel::UI::Image::create(const std::string & name, std::string & imageFileName)
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

	auto ss = ssm.getSpriteSheet(spriteSheetName);

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

bool Voxel::UI::Image::updateMouseMove(const glm::vec2 & mousePosition, const glm::vec2& mouseDelta)
{
	if (isDraggable())
	{
		if (imageState == State::CLICKED)
		{
			addPosition(mouseDelta);
			return true;
		}
	}

	return false;
}

bool Voxel::UI::Image::updateMouseClick(const glm::vec2 & mousePosition, const int button)
{
	if (isDraggable())
	{
		if (imageState == State::IDLE)
		{
			// Check if mouse is in check box
			if (boundingBox.containsPoint(mousePosition))
			{
				imageState = State::CLICKED;

				return true;
			}
		}
	}

	return false;
}

bool Voxel::UI::Image::updateMouseRelease(const glm::vec2 & mousePosition, const int button)
{
	if (isDraggable())
	{
		if (imageState == State::CLICKED)
		{
			// Check if mouse is in check box
			if (boundingBox.containsPoint(mousePosition))
			{
				imageState = State::IDLE;

				return true;
			}
		}
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

	auto size = glm::vec2(imageEntry->width, imageEntry->height);

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

	auto& uvOrigin = imageEntry->uvOrigin;
	auto& uvEnd = imageEntry->uvEnd;

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

	return true;
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

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX
	createDebugBoundingBoxLine();
#endif
}

void Voxel::UI::Image::renderSelf()
{
	// only render self
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
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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