#include "Shader.h"

// cpp
#include <fstream>	//file
#include <sstream>	//string stream

using namespace Voxel;

Voxel::Shader::~Shader()
{
	glDeleteShader(shaderObject);
}

Shader * Shader::create(const std::string & filePath, GLenum shaderType)
{
	Shader* newShader = new Shader();
	if (newShader->init(filePath, shaderType))
	{
		return newShader;
	}
	else
	{
		delete newShader;
		return nullptr;
	}
}

bool Shader::init(const std::string & filePath, GLenum shaderType)
{
	// read file from path
	std::ifstream fs;
	fs.open(filePath.c_str(), std::ios::in | std::ios::binary);

	if (!fs.is_open())
	{
		throw std::runtime_error(std::string("Failed to open shader file: ") + filePath);
	}

	std::stringstream buffer;
	buffer << fs.rdbuf();

	fs.close();

	std::string shaderCode = buffer.str();

	if (shaderCode.empty())
	{
		throw std::runtime_error(std::string("Shader file \"") + filePath + std::string("\" is empty."));
	}

	shaderObject = glCreateShader(shaderType);

	const char* code = shaderCode.c_str();

	glShaderSource(shaderObject, 1, (const GLchar **) &code, nullptr);

	glCompileShader(shaderObject);

	checkCompileError();

	return true;
}

GLuint Shader::getObject()
{
	return shaderObject;
}

void Shader::checkCompileError()
{
	GLint status;
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		std::string msg("Compile failure in vertex shader:\n");

		GLint infoLogLength;
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);

		char* strInfoLog = new char[infoLogLength + 1];

		glGetShaderInfoLog(shaderObject, infoLogLength, NULL, strInfoLog);

		msg += strInfoLog;

		delete[] strInfoLog;

		throw std::runtime_error(msg);
	}
}