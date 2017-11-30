#include "ProgressTimer.h"

// voxel
#include "SpriteSheet.h"
#include "Quad.h"
#include "ProgramManager.h"
#include "Program.h"
#include "Utility.h"

Voxel::UI::ProgressTimer::ProgressTimer(const std::string & name)
	: RenderNode(name)
	, percentage(100)
	, currentIndex(0)
{}

Voxel::UI::ProgressTimer * Voxel::UI::ProgressTimer::create(const std::string & name, const std::string & spriteSheetName, const std::string & progressTimerImageFileName, const Type type, const Direction direction)
{
	auto newProgressTimer = new Voxel::UI::ProgressTimer(name);

	auto& ssm = SpriteSheetManager::getInstance();

	auto ss = ssm.getSpriteSheet(spriteSheetName);

	if (ss)
	{
		if (newProgressTimer->init(ss, progressTimerImageFileName, type, direction))
		{
			return newProgressTimer;
		}
	}

	delete newProgressTimer;
	return nullptr;
}

bool Voxel::UI::ProgressTimer::init(SpriteSheet * ss, const std::string & progressTimerImageFileName, const Type type, const Direction direction)
{
	texture = ss->getTexture();

	if (texture == nullptr)
	{
		return false;
	}

	this->type = type;

	texture->setLocationOnProgram(ProgramManager::PROGRAM_NAME::UI_TEXTURE_SHADER);

	std::vector<float> vertices;
	std::vector<float> uvs;
	std::vector<unsigned int> indices;

	auto imageEntry = ss->getImageEntry(progressTimerImageFileName);

	if (imageEntry)
	{
		auto size = glm::vec2(imageEntry->width, imageEntry->height);

		auto& uvOrigin = imageEntry->uvOrigin;
		auto& uvEnd = imageEntry->uvEnd;

		//frameSizes.at(i) = size;

		buildMesh(size * -0.5f, size * 0.5f, uvOrigin, uvEnd, vertices, uvs, indices, direction);

		build(vertices, uvs, indices);

		return true;
	}
	else
	{
		return false;
	}
}

void Voxel::UI::ProgressTimer::buildMesh(const glm::vec2& verticesOrigin, const glm::vec2& verticesEnd, const glm::vec2& uvOrigin, const glm::vec2& uvEnd, std::vector<float>& vertices, std::vector<float>& uvs, std::vector<unsigned int>& indices, const Direction direction)
{
	const float width = verticesEnd.x - verticesOrigin.x;
	const float height = verticesEnd.y - verticesOrigin.y;

	if (type == Type::HORIZONTAL)
	{
		// Horizontal bar type

		float stepX = width * 0.01f;

		float startX = 0.0f;
		const float yTop = verticesEnd.y;
		const float yBot = verticesOrigin.y;

		float uvStepX = (uvEnd.x - uvOrigin.x) * 0.01f;

		float uvStartX = 0.0f;

		auto quadIndices = Quad::indices;

		// Build vertices from left to right.
		startX = verticesOrigin.x;
		uvStartX = uvOrigin.x;

		int curIndex = 0;

		if (direction == Direction::COUNTER_CLOCK_WISE)
		{
			curIndex = 99;
		}

		for (int i = 0; i <= 100; i++)
		{
			if (i == 50)
			{
				vertices.push_back(0);
				vertices.push_back(yBot);
				vertices.push_back(0);

				vertices.push_back(0);
				vertices.push_back(yTop);
				vertices.push_back(0);
			}
			else if (i == 100)
			{
				vertices.push_back(verticesEnd.x);
				vertices.push_back(yBot);
				vertices.push_back(0);

				vertices.push_back(verticesEnd.x);
				vertices.push_back(yTop);
				vertices.push_back(0);
			}
			else
			{
				vertices.push_back(startX);
				vertices.push_back(yBot);
				vertices.push_back(0);

				vertices.push_back(startX);
				vertices.push_back(yTop);
				vertices.push_back(0);
			}

			startX += stepX;

			uvs.push_back(uvStartX);
			uvs.push_back(uvOrigin.y);

			uvs.push_back(uvStartX);
			uvs.push_back(uvEnd.y);

			uvStartX += uvStepX;

			if (i < 100)
			{
				for (auto index : quadIndices)
				{
					indices.push_back(index + (2 * curIndex));
				}

				if (direction == Direction::CLOCK_WISE)
				{
					curIndex++;
				}
				else
				{
					curIndex--;
				}
			}
		}
	}
	else if (type == Type::VERTICAL)
	{
		// Vertical bar type

		float stepY = height * 0.01f;

		float startY = 0.0f;
		const float xLeft = verticesOrigin.x;
		const float xRight = verticesEnd.x;

		float uvStepY = (uvOrigin.y - uvEnd.y) * 0.01f;

		float uvStartY = 0.0f;

		auto quadIndices = std::array<unsigned int, 6>{0, 2, 1, 2, 1, 3};

		// Build vertices from bottom to top
		startY = verticesOrigin.y;
		uvStartY = uvOrigin.y;

		uvStepY *= -1.0f;

		int curIndex = 0;

		if (direction == Direction::COUNTER_CLOCK_WISE)
		{
			curIndex = 99;
		}

		for (int i = 0; i <= 100; i++)
		{
			if (i == 50)
			{
				vertices.push_back(xLeft);
				vertices.push_back(0);
				vertices.push_back(0);

				vertices.push_back(xRight);
				vertices.push_back(0);
				vertices.push_back(0);
			}
			else if (i == 100)
			{
				vertices.push_back(xLeft);
				vertices.push_back(verticesEnd.y);
				vertices.push_back(0);

				vertices.push_back(xRight);
				vertices.push_back(verticesEnd.y);
				vertices.push_back(0);
			}
			else
			{
				vertices.push_back(xLeft);
				vertices.push_back(startY);
				vertices.push_back(0);

				vertices.push_back(xRight);
				vertices.push_back(startY);
				vertices.push_back(0);
			}

			startY += stepY;

			uvs.push_back(uvOrigin.x);
			uvs.push_back(uvStartY);

			uvs.push_back(uvEnd.x);
			uvs.push_back(uvStartY);

			uvStartY += uvStepY;

			if (i < 100)
			{
				for (auto index : quadIndices)
				{
					indices.push_back(index + (2 * curIndex));
				}

				if (direction == Direction::CLOCK_WISE)
				{
					curIndex++;
				}
				else
				{
					curIndex--;
				}
			}
		}
	}
	else if (type == Type::RADIAL)
	{
		// Radial bar type.

		// Radial type is different compared to bar type.
		// We have to divide quad to 100 triangles in radial.
		// However, dividing quad in to 100 triangles with 3.6 degrees each will have problems in corner.

		/*

		c4   7*      0   c1
		*------*------*
		|      |     /|
		6 |      |   /  | 1*
		|      | /    |
		*      *      *
		|             |
		5* |             | 2
		|             |
		*------*------*
		c3   4     3*   c2

		*/

		/*
		widthIntervalWithCorner	cornerWidth
		-----|--------*
		/|
		/  |  cornerHeight
		/    |
		/      -
		/        |  heightIntervalWithCorner
		*/

		// vertices x
		const float widthHalf = width * 0.5f;
		const float cornerWidth = (widthHalf / 13.0f) * 0.5f;
		//const float widthInterval = widthHalf / 12.0f;
		const float widthIntervalWithCorner = (widthHalf - cornerWidth) / 12.0f;

		// vertices y
		const float heightHalf = height * 0.5f;
		const float cornerHeight = (heightHalf / 13.0f) * 0.5f;
		//const float heightInterval = heightHalf / 12.0f;
		const float heightIntervalWithCorner = (heightHalf - cornerHeight) / 12.0f;

		// uv x
		const float uvWidthHalf = (uvEnd.x - uvOrigin.x) * 0.5f;
		const float uvCornerWidth = (uvWidthHalf / 13.0f) * 0.5f;
		//const float uvWidthInterval = uvWidthHalf / 12.0f;
		const float uvWidthIntervalWithCorner = (uvWidthHalf - uvCornerWidth) / 12.0f;

		// uv y
		const float uvHeightHalf = (uvOrigin.y - uvEnd.y) * 0.5f;
		const float uvCornerHeight = (uvHeightHalf / 13.0f) * 0.5f;
		//const float uvHeightInterval = uvHeightHalf / 12.0f;
		const float uvHeightIntervalWithCorner = (uvHeightHalf - uvCornerHeight) / 12.0f;

		// add origin
		vertices.push_back(0.0f);
		vertices.push_back(0.0f);
		vertices.push_back(0.0f);

		const float uvWidth = (uvEnd.x - uvOrigin.x);
		const float uvHeight = (uvOrigin.y - uvEnd.y);
		const float uvXCenter = uvOrigin.x + (uvWidth * 0.5f);
		const float uvYCenter = uvEnd.y + (uvHeight * 0.5f);

		// add origin uv
		uvs.push_back(uvXCenter);
		uvs.push_back(uvYCenter);

		// add first point
		vertices.push_back(0.0f);
		vertices.push_back(verticesEnd.y);
		vertices.push_back(0.0f);

		// add first uvs
		uvs.push_back(uvXCenter);
		uvs.push_back(uvEnd.y);

		int index = 0;
		const int lastIndex = 103;	// 100 + 4 corners - 1

		if (direction == Direction::COUNTER_CLOCK_WISE)
		{
			index = lastIndex;
		}

		float step = 1.0f;

		for (int i = 1; i <= 100; i++)
		{
			if (i == 13)
			{
				// c1

				// add vertices
				vertices.push_back(verticesEnd.x);
				vertices.push_back(verticesEnd.y);
				vertices.push_back(0.0f);

				// add uvs
				uvs.push_back(uvEnd.x);
				uvs.push_back(uvEnd.y);

				// add indices
				indices.push_back(0);
				indices.push_back(index + 1);
				indices.push_back(index + 2);

				if (direction == Direction::CLOCK_WISE)
				{
					index++;
				}
				else
				{
					index--;
				}

				float y = verticesEnd.y - cornerHeight;

				// add vertices
				vertices.push_back(verticesEnd.x);
				vertices.push_back(y);
				vertices.push_back(0.0f);

				float uvY = uvEnd.y + uvCornerHeight;

				// add uvs
				uvs.push_back(uvEnd.x);
				uvs.push_back(uvY);

				// add indices
				indices.push_back(0);
				indices.push_back(index + 1);
				indices.push_back(index + 2);

				step = 1.0f;
			}
			else if (i == 38)
			{
				// c2

				// add vertices
				vertices.push_back(verticesEnd.x);
				vertices.push_back(verticesOrigin.y);
				vertices.push_back(0.0f);

				// add uvs
				uvs.push_back(uvEnd.x);
				uvs.push_back(uvOrigin.y);

				// add indices
				indices.push_back(0);
				indices.push_back(index + 1);
				indices.push_back(index + 2);

				if (direction == Direction::CLOCK_WISE)
				{
					index++;
				}
				else
				{
					index--;
				}

				float x = verticesEnd.x - cornerWidth;

				// add vertices
				vertices.push_back(x);
				vertices.push_back(verticesOrigin.y);
				vertices.push_back(0.0f);

				float uvX = uvEnd.x - uvCornerWidth;

				// add uvs
				uvs.push_back(uvX);
				uvs.push_back(uvOrigin.y);

				// add indices
				indices.push_back(0);
				indices.push_back(index + 1);
				indices.push_back(index + 2);

				step = 1.0f;
			}
			else if (i == 63)
			{
				// c3

				// add vertices
				vertices.push_back(verticesOrigin.x);
				vertices.push_back(verticesOrigin.y);
				vertices.push_back(0.0f);

				// add uvs
				uvs.push_back(uvOrigin.x);
				uvs.push_back(uvOrigin.y);

				// add indices
				indices.push_back(0);
				indices.push_back(index + 1);
				indices.push_back(index + 2);

				if (direction == Direction::CLOCK_WISE)
				{
					index++;
				}
				else
				{
					index--;
				}

				float y = verticesOrigin.y + cornerHeight;

				// add vertices
				vertices.push_back(verticesOrigin.x);
				vertices.push_back(y);
				vertices.push_back(0.0f);

				float uvY = uvOrigin.y - uvCornerWidth;

				// add uvs
				uvs.push_back(uvOrigin.x);
				uvs.push_back(uvY);

				// add indices
				indices.push_back(0);
				indices.push_back(index + 1);
				indices.push_back(index + 2);

				step = 1.0f;
			}
			else if (i == 88)
			{
				// c4

				// add vertices
				vertices.push_back(verticesOrigin.x);
				vertices.push_back(verticesEnd.y);
				vertices.push_back(0.0f);

				// add uvs
				uvs.push_back(uvOrigin.x);
				uvs.push_back(uvEnd.y);

				// add indices
				indices.push_back(0);
				indices.push_back(index + 1);
				indices.push_back(index + 2);

				if (direction == Direction::CLOCK_WISE)
				{
					index++;
				}
				else
				{
					index--;
				}

				float x = verticesOrigin.x + cornerWidth;

				// add vertices
				vertices.push_back(x);
				vertices.push_back(verticesEnd.y);
				vertices.push_back(0.0f);

				float uvX = uvOrigin.x + uvCornerWidth;

				// add uvs
				uvs.push_back(uvX);
				uvs.push_back(uvEnd.y);

				// add indices
				indices.push_back(0);
				indices.push_back(index + 1);
				indices.push_back(index + 2);

				step = 1.0f;
			}
			else
			{
				if (i <= 12)
				{
					// section 0. 12 section + corner 1
					float x = step * widthIntervalWithCorner;

					// add vertices
					vertices.push_back(x);
					vertices.push_back(verticesEnd.y);
					vertices.push_back(0.0f);

					float uvX = uvXCenter + (step * uvWidthIntervalWithCorner);

					// add uvs
					uvs.push_back(uvX);
					uvs.push_back(uvEnd.y);

					step++;
				}
				else if (i > 13 && i <= 25)
				{
					// section 1. 12 section + corner 1
					float y = 0.0f;
					float uvY = 0.0f;

					if (i == 25)
					{
						// Quater
						y = 0.0f;
						uvY = uvYCenter;
					}
					else
					{
						y = (verticesEnd.y - cornerHeight) - (heightIntervalWithCorner * step);
						uvY = (uvEnd.y + uvCornerHeight) + (uvHeightIntervalWithCorner * step);
					}

					// add vertices
					vertices.push_back(verticesEnd.x);
					vertices.push_back(y);
					vertices.push_back(0.0f);

					// add uvs
					uvs.push_back(uvEnd.x);
					uvs.push_back(uvY);

					if (i == 25)
					{
						step = 1.0f;
					}
					else
					{
						step++;
					}
				}
				else if (i > 25 && i <= 37)
				{
					// section 2. 12 section + corner 2
					float y = 0.0f - (heightIntervalWithCorner * step);

					// add vertices
					vertices.push_back(verticesEnd.x);
					vertices.push_back(y);
					vertices.push_back(0.0f);

					float uvY = uvYCenter + (uvHeightIntervalWithCorner * step);

					// add uvs
					uvs.push_back(uvEnd.x);
					uvs.push_back(uvY);

					step++;
				}
				else if (i > 38 && i <= 50)
				{
					// section 3. 12 sections + corner 2
					float x = 0.0f;
					float uvX = 0.0f;

					if (i == 50)
					{
						// half
						x = 0.0f;
						uvX = uvXCenter;
					}
					else
					{
						x = (verticesEnd.x - cornerWidth) - (widthIntervalWithCorner * step);
						uvX = (uvEnd.x - uvCornerWidth) - (uvWidthIntervalWithCorner * step);
					}

					// add vertices
					vertices.push_back(x);
					vertices.push_back(verticesOrigin.y);
					vertices.push_back(0.0f);

					// add uvs
					uvs.push_back(uvX);
					uvs.push_back(uvOrigin.y);

					if (i == 50)
					{
						step = 1.0f;
					}
					else
					{
						step++;
					}
				}
				else if (i > 50 && i <= 62)
				{
					// section 4. 12 sections + corner 3
					float x = -(widthIntervalWithCorner * step);

					// add vertices
					vertices.push_back(x);
					vertices.push_back(verticesOrigin.y);
					vertices.push_back(0.0f);

					float uvX = uvXCenter - (uvWidthIntervalWithCorner * step);

					// add uvs
					uvs.push_back(uvX);
					uvs.push_back(uvOrigin.y);

					step++;
				}
				else if (i > 63 && i <= 75)
				{
					// section 5. 12 sections + corner 3
					float y = 0.0f;
					float uvY = 0.0f;

					if (i == 75)
					{
						// third quarter
						y = 0.0f;
						uvY = uvYCenter;
					}
					else
					{
						y = (verticesOrigin.y + cornerHeight) + (heightIntervalWithCorner * step);
						uvY = (uvOrigin.y - uvCornerHeight) - (uvHeightIntervalWithCorner * step);
					}

					// add vertices
					vertices.push_back(verticesOrigin.x);
					vertices.push_back(y);
					vertices.push_back(0.0f);

					// add uvs
					uvs.push_back(uvOrigin.x);
					uvs.push_back(uvY);

					if (i == 75)
					{
						step = 1.0f;
					}
					else
					{
						step++;
					}
				}
				else if (i > 75 && i <= 87)
				{
					// section 6. 12 sections + corner 4
					float y = (heightIntervalWithCorner * step);

					// add vertices
					vertices.push_back(verticesOrigin.x);
					vertices.push_back(y);
					vertices.push_back(0.0f);

					float uvY = uvYCenter - (uvHeightIntervalWithCorner * step);

					// add uvs
					uvs.push_back(uvOrigin.x);
					uvs.push_back(uvY);

					step++;
				}
				else if (i > 88 && i <= 100)
				{
					// section 7
					if (i == 100)
					{
						// back to first 0 percent. Don't need to add
					}
					else
					{
						float x = (verticesOrigin.x + cornerWidth) + (widthIntervalWithCorner * step);

						// add vertices
						vertices.push_back(x);
						vertices.push_back(verticesEnd.y);
						vertices.push_back(0);

						float uvX = (uvOrigin.x + uvCornerWidth) + (uvWidthIntervalWithCorner * step);

						// add uvs
						uvs.push_back(uvX);
						uvs.push_back(uvEnd.y);
					}

					step++;
				}

				// add indices
				indices.push_back(0);

				if (index == lastIndex)
				{
					// wrap up the last one
					indices.push_back(index + 1);
					indices.push_back(1);
				}
				else
				{
					indices.push_back(index + 1);
					indices.push_back(index + 2);
				}
			}

			if (direction == Direction::CLOCK_WISE)
			{
				index++;
			}
			else
			{
				index--;
			}
		}
	}
}

void Voxel::UI::ProgressTimer::build(const std::vector<float>& vertices, const std::vector<float>& uvs, const std::vector<unsigned int>& indices)
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

void Voxel::UI::ProgressTimer::updateCurrentIndex()
{
	if (type == Voxel::UI::ProgressTimer::Type::RADIAL)
	{
		int addition = 0;

		if (percentage >= 88)
		{
			addition = 4;
		}
		else if (percentage < 88 && percentage >= 63)
		{
			addition = 3;
		}
		else if (percentage < 63 && percentage >= 38)
		{
			addition = 2;
		}
		else if (percentage < 38 && percentage >= 13)
		{
			addition = 1;
		}

		std::cout << "p = " << percentage << ", a = " << addition;

		currentIndex = (3 * (percentage + addition));

		std::cout << ", c = " << currentIndex << "\n";
	}
	else
	{
		currentIndex = (6 * percentage);
	}
}

void Voxel::UI::ProgressTimer::setPercentage(const int percentage)
{
	this->percentage = glm::clamp(percentage, 0, 100);

	updateCurrentIndex();
}

int Voxel::UI::ProgressTimer::getPercentage() const
{
	return percentage;
}

void Voxel::UI::ProgressTimer::renderSelf()
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
		glDrawElements(GL_TRIANGLES, currentIndex, GL_UNSIGNED_INT, 0);
	}
}

//====================================================================================================================================