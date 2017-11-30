#include "Program.h"

// voxel
#include "Shader.h"

// cpp
#include <iostream>

// glm
#include <glm/gtc/type_ptr.hpp>

using namespace Voxel;
using namespace glm;

GLuint Program::lastProgramObject = 0;

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

	findAllUniforms();

	return true;
}

void Voxel::Program::findAllUniforms()
{
	GLint activeUniforms;
	glGetProgramiv(programObject, GL_ACTIVE_UNIFORMS, &activeUniforms);

	if (activeUniforms > 0)
	{
		GLint len;
		glGetProgramiv(programObject, GL_ACTIVE_UNIFORM_MAX_LENGTH, &len);

		if (len > 0)
		{
			char* uniformName = new char[len + 1];

			for (int i = 0; i < activeUniforms; i++)
			{
				int nameLength = 0;
				int num = 0;
				GLenum type = GL_ZERO;
				glGetActiveUniform(programObject, i, len, &nameLength, &num, &type, uniformName);
				uniformName[len] = '\0';

				GLint location = glGetUniformLocation(programObject, uniformName);

				//std::cout << "Uniform: " << uniformName << ", location: " << location << std::endl;

				UniformValue uv;
				uv.type = type;

				switch (type)
				{
				case GL_BOOL:
					uv.value.boolValue = false;
					break;
				case GL_INT:
					uv.value.intValue = 0;
					break;
				case GL_FLOAT:
					uv.value.floatValue = 0;
					break;
				case GL_FLOAT_VEC2:
					uv.value.v2Value[0] = 0.0f;
					uv.value.v2Value[1] = 0.0f;
					break;
				case GL_FLOAT_VEC3:
					uv.value.v3Value[0] = 0.0f;
					uv.value.v3Value[1] = 0.0f;
					uv.value.v3Value[2] = 0.0f;
					break;
				case GL_FLOAT_VEC4:
					uv.value.v4Value[0] = 0.0f;
					uv.value.v4Value[1] = 0.0f;
					uv.value.v4Value[2] = 0.0f;
					uv.value.v4Value[3] = 0.0f;
					break;
				case GL_FLOAT_MAT4:
					for (int j = 0; j < 16; j++)
					{
						uv.value.mat4Value[j] = 0.0f;
					}
					break;
				default:
					break;
				}

				uniformLocations.emplace(std::string(uniformName), location);
				uniformValues.emplace(location, uv);
			}

			delete[] uniformName;
		}
	}
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

void Voxel::Program::setUniformBool(const GLint location, const bool boolean)
{
	auto find_it = uniformValues.find(location);
	if (find_it == uniformValues.end())
	{
		std::cout << "[Program] Failed to find uniform boolean location: " << location << std::endl;
		return;
	}
	else
	{
		auto& uniformValue = find_it->second;
		if (uniformValue.type == GL_BOOL)
		{
			if (uniformValue.value.boolValue != boolean)
			{
				glUniform1i(location, boolean);
				uniformValue.value.boolValue = boolean;
			}
			else
			{
				return;
			}
		}
		else
		{
			return;
		}
	}
}

void Voxel::Program::setUniformBool(const std::string & name, const bool boolean)
{
	auto find_it = uniformLocations.find(name);
	if (find_it == uniformLocations.end())
	{
		return;
	}
	else
	{
		setUniformBool(find_it->second, boolean);
	}
}

void Voxel::Program::setUniformInt(const GLint location, const int integer)
{
	auto find_it = uniformValues.find(location);
	if (find_it == uniformValues.end())
	{
		std::cout << "[Program] Failed to find uniform int location: " << location << std::endl;
		return;
	}
	else
	{
		auto& uniformValue = find_it->second;
		if (uniformValue.type == GL_INT)
		{
			if (uniformValue.value.intValue != integer)
			{
				glUniform1i(location, integer);
				uniformValue.value.intValue = integer;
			}
			else
			{
				return;
			}
		}
		else
		{
			return;
		}
	}
}

void Voxel::Program::setUniformInt(const std::string & name, const int integer)
{
	auto find_it = uniformLocations.find(name);
	if (find_it == uniformLocations.end())
	{
		return;
	}
	else
	{
		setUniformInt(find_it->second, integer);
	}
}

void Voxel::Program::setUniformFloat(const GLint location, const float val)
{
	auto find_it = uniformValues.find(location);
	if (find_it == uniformValues.end())
	{
		std::cout << "[Program] Failed to find uniform float location: " << location << std::endl;
		return;
	}
	else
	{
		auto& uniformValue = find_it->second;
		if (uniformValue.type == GL_FLOAT)
		{
			if (uniformValue.value.floatValue != val)
			{
				glUniform1f(location, val);
				uniformValue.value.floatValue = val;
			}
			else
			{
				return;
			}
		}
		else
		{
			return;
		}
	}
}

void Voxel::Program::setUniformFloat(const std::string & name, const float val)
{
	auto find_it = uniformLocations.find(name);
	if (find_it == uniformLocations.end())
	{
		return;
	}
	else
	{
		setUniformFloat(find_it->second, val);
	}
}

void Voxel::Program::setUniformVec2(const GLint location, const glm::vec2 & val)
{
	auto find_it = uniformValues.find(location);
	if (find_it == uniformValues.end())
	{
		std::cout << "[Program] Failed to find uniform vec2 location: " << location << std::endl;
		return;
	}
	else
	{
		auto& uniformValue = find_it->second;
		if (uniformValue.type == GL_FLOAT_VEC2)
		{
			glm::vec2 v2 = glm::make_vec2(uniformValue.value.v2Value);
			if (v2 != val)
			{
				glUniform2f(location, val.x, val.y);
				uniformValue.value.v2Value[0] = val.x;
				uniformValue.value.v2Value[1] = val.y;
			}
			else
			{
				return;
			}
		}
		else
		{
			return;
		}
	}
}

void Voxel::Program::setUniformVec2(const std::string & name, const glm::vec2 & val)
{
	auto find_it = uniformLocations.find(name);
	if (find_it == uniformLocations.end())
	{
		return;
	}
	else
	{
		setUniformVec2(find_it->second, val);
	}
}

void Voxel::Program::setUniformVec3(const GLint location, const glm::vec3 & val)
{
	auto find_it = uniformValues.find(location);
	if (find_it == uniformValues.end())
	{
		std::cout << "[Program] Failed to find uniform vec3 location: " << location << std::endl;
		return;
	}
	else
	{
		auto& uniformValue = find_it->second;
		if (uniformValue.type == GL_FLOAT_VEC3)
		{
			glm::vec3 v3 = glm::make_vec3(uniformValue.value.v3Value);
			if (v3 != val)
			{
				glUniform3f(location, val.x, val.y, val.z);
				uniformValue.value.v3Value[0] = val.x;
				uniformValue.value.v3Value[1] = val.y;
				uniformValue.value.v3Value[2] = val.z;
			}
			else
			{
				return;
			}
		}
		else
		{
			return;
		}
	}
}

void Voxel::Program::setUniformVec3(const std::string & name, const glm::vec3 & val)
{
	auto find_it = uniformLocations.find(name);
	if (find_it == uniformLocations.end())
	{
		return;
	}
	else
	{
		setUniformVec3(find_it->second, val);
	}
}

void Voxel::Program::setUniformVec4(const GLint location, const glm::vec4 & val)
{
	auto find_it = uniformValues.find(location);
	if (find_it == uniformValues.end())
	{
		std::cout << "[Program] Failed to find uniform vec4 location: " << location << std::endl;
		return;
	}
	else
	{
		auto& uniformValue = find_it->second;
		if (uniformValue.type == GL_FLOAT_VEC4)
		{
			glm::vec4 v4 = glm::make_vec4(uniformValue.value.v4Value);
			if (v4 != val)
			{
				glUniform4f(location, val.x, val.y, val.z, val.w);
				uniformValue.value.v4Value[0] = val.x;
				uniformValue.value.v4Value[1] = val.y;
				uniformValue.value.v4Value[2] = val.z;
				uniformValue.value.v4Value[3] = val.w;
			}
			else
			{
				return;
			}
		}
		else
		{
			return;
		}
	}
}

void Voxel::Program::setUniformVec4(const std::string & name, const glm::vec4 & val)
{
	auto find_it = uniformLocations.find(name);
	if (find_it == uniformLocations.end())
	{
		return;
	}
	else
	{
		setUniformVec4(find_it->second, val);
	}
}

void Program::setUniformMat4(const GLint location, const mat4 & mat)
{
	auto find_it = uniformValues.find(location);
	if (find_it == uniformValues.end())
	{
		std::cout << "[Program] Failed to find uniform vec4 location: " << location << std::endl;
		return;
	}
	else
	{
		auto& uniformValue = find_it->second;
		if (uniformValue.type == GL_FLOAT_MAT4)
		{
			glm::mat4 mat4 = glm::make_mat4(uniformValue.value.mat4Value);

			if (mat4 != mat)
			{
				glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(mat));

				memcpy(uniformValue.value.mat4Value, glm::value_ptr(mat), sizeof(float) * 16);
			}
			else
			{
				return;
			}
		}
		else
		{
			return;
		}
	}
}

void Program::setUniformMat4(const std::string & name, const mat4 & mat)
{
	auto find_it = uniformLocations.find(name);
	if (find_it == uniformLocations.end())
	{
		return;
	}
	else
	{
		setUniformMat4(find_it->second, mat);
	}
}

void Voxel::Program::use(const bool use)
{
	if (use)
	{
		if (lastProgramObject != programObject)
		{
			lastProgramObject = programObject;
		}
		// already using
		glUseProgram(programObject);
	}
	else
	{
		lastProgramObject = 0;
		//glUseProgram(0);
	}
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