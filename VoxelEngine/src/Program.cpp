#include "Program.h"
#include "Shader.h"

Program::~Program()
{
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

GLint Program::attrib(const GLchar * attributeName)
{
	if (!attributeName)
	{
		throw std::runtime_error("Attribute name was null");
	}

	GLint attrib = glGetAttribLocation(programObject, attributeName);
	if (attrib == -1)
	{
		throw std::runtime_error(std::string("Program attribute not found: ") + attributeName);
	}

	return attrib;
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