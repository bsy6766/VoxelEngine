// pch
#include "PreCompiled.h"

#include "Line.h"

// voxel
#include "Program.h"
#include "ProgramManager.h"
#include "Camera.h"

Voxel::UI::Line::Line(const std::string & name)
	: TransformNode(name)
	, vao(0)
	, program(nullptr)
	, lineColor(1.0f)
{}

Voxel::UI::Line::~Line()
{
	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}
}

bool Voxel::UI::Line::init(const glm::vec2 & start, const glm::vec2 & end, const glm::vec4& lineColor)
{
	std::vector<float> vertices = { start.x, start.y, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, end.x, end.y, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f };

	if(vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices.front(), GL_STATIC_DRAW);

	program = ProgramManager::getInstance().getProgram(ProgramManager::PROGRAM_NAME::LINE_SHADER);

	GLint vertLoc = program->getAttribLocation("vert");
	GLint colorLoc = program->getAttribLocation("color");

	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), nullptr);

	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));

	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);

	this->lineColor = lineColor;

	return true;
}

Voxel::UI::Line * Voxel::UI::Line::create(const std::string & name, const glm::vec2 & start, const glm::vec2 & end, const glm::vec4& lineColor)
{
	auto newLine = new Line(name);

	if (newLine->init(start, end, lineColor))
	{
		return newLine;
	}
	else
	{
		delete newLine;
		return nullptr;
	}
}

void Voxel::UI::Line::setColor(const glm::vec4 & lineColor)
{
	this->lineColor = lineColor;
}

void Voxel::UI::Line::renderSelf()
{
	if (visibility)
	{
		if (vao)
		{
			glBindVertexArray(vao);

			program->use(true);
			program->setUniformMat4("projMat", Camera::mainCamera->getProjection());
			program->setUniformMat4("viewMat", glm::mat4(1.0f));
			program->setUniformMat4("modelMat", modelMat);
			program->setUniformVec4("lineColor", lineColor);
			glDrawArrays(GL_LINES, 0, 2);
		}
	}
}

void Voxel::UI::Line::render()
{
	if (children.empty())
	{
		renderSelf();
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
			renderSelf();

			// Render positive 
			for (; children_it != children.end(); children_it++)
			{
				((children_it)->second)->render();
			}
		}
	}
}
