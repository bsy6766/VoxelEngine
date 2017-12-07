#include "NinePatchImage.h"

// voxel
#include "SpriteSheet.h"
#include "Quad.h"
#include "ProgramManager.h"
#include "Program.h"

Voxel::UI::NinePatchImage::NinePatchImage(const std::string& name)
	: RenderNode(name)
{}

bool Voxel::UI::NinePatchImage::init(SpriteSheet * ss, const std::string & textureName, const float leftPadding, const float rightPadding, const float topPadding, const float bottomPadding, const glm::vec2 & bodySize)
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
		boundingBox.center = position;
		boundingBox.size = glm::vec2(bodySize.x + leftPadding + rightPadding, bodySize.y + topPadding + bottomPadding);

		contentSize = boundingBox.size;

		// A, C, G, I. 4 quad * 12 flaot = 48
		std::array<float, 48> vertices = { 0.0f };
		// 4 qauds * 8 float = 32 
		std::array<float, 32> uvs = { 0.0f };
		// 9 quads * 6 indices = 54
		std::array<unsigned int, 54> indices = { 0 };

		float widthHalf = contentSize.x * 0.5f;
		float heightHalf = contentSize.y * 0.5f;

		/*
		           x0           x1
				*---*------------*---*
				| A |     B      | C |
			 y0	*---*------------*---*
				|   |            |   |
				|   |            |   |
				| D |     E      | F |
				|   |   (body)   |   |
				|   |            |   |
			 y1	*---*------------*---*
				| G |     H      | I |
				*---*------------*---*
			
		*/

		const float x0 = -(bodySize.x * 0.5f);
		const float x1 = bodySize.x * 0.5f;
		const float y0 = bodySize.y * 0.5f;
		const float y1 = -(bodySize.y * 0.5f);

		auto& uvOrigin = imageEntry->uvOrigin;
		auto& uvEnd = imageEntry->uvEnd;
		
		const glm::vec2 spriteSheetSize = texture->getTextureSize();
		const float uvx0 = uvOrigin.x + (leftPadding / spriteSheetSize.x);
		const float uvx1 = uvEnd.x - (rightPadding / spriteSheetSize.x);
		const float uvy0 = uvEnd.y + (topPadding / spriteSheetSize.y);
		const float uvy1 = uvOrigin.y - (bottomPadding / spriteSheetSize.y);

		// A
		buildQuadSection(glm::vec2(-widthHalf, y0), glm::vec2(x0, heightHalf), 0, vertices);
		buildUVs(glm::vec2(uvOrigin.x, uvy0), glm::vec2(uvx0, uvEnd.y), 0, uvs);
		buildIndices(indices, 0, 0);

		// B
		buildIndices(indices, 1, 2);

		// C
		buildQuadSection(glm::vec2(x1, y0), glm::vec2(widthHalf, heightHalf), 1, vertices);
		buildUVs(glm::vec2(uvx1, uvy0), uvEnd, 1, uvs);
		buildIndices(indices, 2, 4);
		
		// D
		indices.at(18) = 9;
		indices.at(19) = 0;
		indices.at(20) = 11;
		indices.at(21) = 0;
		indices.at(22) = 11;
		indices.at(23) = 2;
		
		// E (body)
		indices.at(24) = 11;
		indices.at(25) = 2;
		indices.at(26) = 13;
		indices.at(27) = 2;
		indices.at(28) = 13;
		indices.at(29) = 4;
		
		// F
		indices.at(30) = 13;
		indices.at(31) = 4;
		indices.at(32) = 15;
		indices.at(33) = 4;
		indices.at(34) = 15;
		indices.at(35) = 6;
		
		// G
		buildQuadSection(glm::vec2(-widthHalf, -heightHalf), glm::vec2(x0, y1), 2, vertices);
		buildUVs(uvOrigin, glm::vec2(uvx0, uvy1), 2, uvs);
		buildIndices(indices, 6, 8);
		
		// H
		buildIndices(indices, 7, 10);
		
		// I
		buildQuadSection(glm::vec2(x1, -heightHalf), glm::vec2(widthHalf, y1), 3, vertices);
		buildUVs(glm::vec2(uvx1, uvOrigin.y), glm::vec2(uvEnd.x, uvy1), 3, uvs);
		buildIndices(indices, 8, 12);
		
		build(vertices, uvs, indices);

		return true;
	}
	else
	{
		return false;
	}
}

void Voxel::UI::NinePatchImage::buildQuadSection(const glm::vec2 & min, const glm::vec2 & max, const int offset, std::array<float, 48>& vertices)
{
	const int shift = offset * 12;

	vertices.at(0 + shift) = min.x;
	vertices.at(1 + shift) = min.y;
	vertices.at(2 + shift) = 0.0f;

	vertices.at(3 + shift) = min.x;
	vertices.at(4 + shift) = max.y;
	vertices.at(5 + shift) = 0.0f;

	vertices.at(6 + shift) = max.x;
	vertices.at(7 + shift) = min.y;
	vertices.at(8 + shift) = 0.0f;

	vertices.at(9 + shift) = max.x;
	vertices.at(10 + shift) = max.y;
	vertices.at(11 + shift) = 0.0f;
}

void Voxel::UI::NinePatchImage::buildUVs(const glm::vec2 & min, const glm::vec2 & max, const int offset, std::array<float, 32>& vertices)
{
	const int shift = offset * 8;

	vertices.at(0 + shift) = min.x;
	vertices.at(1 + shift) = min.y;

	vertices.at(2 + shift) = min.x;
	vertices.at(3 + shift) = max.y;

	vertices.at(4 + shift) = max.x;
	vertices.at(5 + shift) = min.y;

	vertices.at(6 + shift) = max.x;
	vertices.at(7 + shift) = max.y;
}

void Voxel::UI::NinePatchImage::buildIndices(std::array<unsigned int, 54>& indices, const int offset, const int shift)
{
	const int shiftIndex = offset * 6;

	indices.at(0 + shiftIndex) = Quad::indices.at(0) + shift;
	indices.at(1 + shiftIndex) = Quad::indices.at(1) + shift;
	indices.at(2 + shiftIndex) = Quad::indices.at(2) + shift;
	indices.at(3 + shiftIndex) = Quad::indices.at(3) + shift;
	indices.at(4 + shiftIndex) = Quad::indices.at(4) + shift;
	indices.at(5 + shiftIndex) = Quad::indices.at(5) + shift;
}

void Voxel::UI::NinePatchImage::build(const std::array<float, 48>& vertices, const std::array<float, 32>& uvs, const std::array<unsigned int, 54>& indices)
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

	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices.front(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &uvbo);
	glDeleteBuffers(1, &ibo);

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX
	createDebugBoundingBoxLine();
#endif
}

Voxel::UI::NinePatchImage * Voxel::UI::NinePatchImage::create(const std::string & name, const std::string & spriteSheetName, const std::string & imageFileName, const float leftPadding, const float rightPadding, const float topPadding, const float bottomPadding, const glm::vec2 & bodySize)
{
	auto& ssm = SpriteSheetManager::getInstance();

	auto ss = ssm.getSpriteSheet(spriteSheetName);

	if (ss)
	{
		auto newImage = new NinePatchImage(name);

		if (newImage->init(ss, imageFileName, leftPadding, rightPadding, topPadding, bottomPadding, bodySize))
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

void Voxel::UI::NinePatchImage::renderSelf()
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
		glDrawElements(GL_TRIANGLES, 54, GL_UNSIGNED_INT, 0);
	}
}
