#include "Program.h"
#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>

using namespace Voxel;
using namespace glm;

Program::~Program()
{
	glDeleteProgram(programObject);
}

Program * Program::create(Shader * vertexShader, Shader * fragmentShader)
{
	if (vertexShader == nullptr)
	{
		throw std::runtime_error("Vertex shader is null");
	}

	if (fragmentShader == nullptr)
	{
		throw std::runtime_error("Fragment shader is null");
	}

	Program* newProgram = new Program();
	if (newProgram->init(vertexShader, fragmentShader))
	{
		return newProgram;
	}
	else
	{
		delete newProgram;
		return nullptr;
	}
}

bool Program::init(Shader * vertexShader, Shader * fragmentShader)
{
	programObject = glCreateProgram();

	if (programObject == 0)
	{
		throw std::runtime_error("Failed to create glProgram");
	}

	glAttachShader(programObject, vertexShader->getObject());
	glAttachShader(programObject, fragmentShader->getObject());

	glLinkProgram(programObject);

	checkLinkError();

	return true;
}

GLuint Program::getObject()
{
	return programObject;
}

GLint Program::getAttribLocation(const GLchar * attributeName)
{
	if (!attributeName)
	{
		throw std::runtime_error("Attribute name was null");
	}

	GLint location = glGetAttribLocation(programObject, attributeName);
	if (location == -1)
	{
		throw std::runtime_error(std::string("Program attribute not found: ") + attributeName);
	}

	return location;
}

GLint Program::getUniformLocation(const GLchar * uniformName)
{
	if (!uniformName)
	{
		throw std::runtime_error("Uniform name was null");
	}

	GLint location = glGetUniformLocation(programObject, uniformName);
	if (location == -1)
	{
		throw std::runtime_error(std::string("Program uniform not found: ") + uniformName);
	}

	return location;
}

void Program::setUniformMat4(const GLint location, const mat4 & mat)
{
	glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(mat));
}

void Program::setUniformMat4(const std::string & name, const mat4 & mat)
{
	auto location = getUniformLocation(name.c_str());
	return setUniformMat4(location, mat);
}

void Program::checkLinkError()
{
	GLint status;
	glGetProgramiv(programObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		std::string msg("Program linking failure: ");

		GLint infoLogLength;
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* strInfoLog = new char[infoLogLength + 1];
		glGetProgramInfoLog(programObject, infoLogLength, NULL, strInfoLog);
		msg += strInfoLog;
		delete[] strInfoLog;

		glDeleteProgram(programObject);
		programObject = 0;
		throw std::runtime_error(msg);
	}
}