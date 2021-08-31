#include "shader.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <glad/glad.h>
#include <iostream>
#include <spdlog/spdlog.h>
#include <glm/gtc/type_ptr.hpp>

shader::shader(const char *vertexPath, const char *fragmentPath)
	{
		int success;
		char infoLog[512];

		std::string vertexCode;
		std::string fragmentCode;

		std::ifstream in(vertexPath);
		std::stringstream vertexFile;
		vertexFile << in.rdbuf();
		vertexCode = vertexFile.str();
		in.close();

		in.open(fragmentPath);
		std::stringstream fragmentFile;
		fragmentFile << in.rdbuf();
		fragmentCode = fragmentFile.str();
		in.close();

		const char *vertexSource = vertexCode.c_str();
		const char *fragmentSource = fragmentCode.c_str();

		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexSource, nullptr);
		glCompileShader(vertexShader);

		unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
		glCompileShader(fragmentShader);

		m_id = glCreateProgram();
		glAttachShader(id, vertexShader);
		glAttachShader(id, fragmentShader);
		glLinkProgram(id);

		glGetProgramiv(id, GL_LINK_STATUS, &success);
		if (!success)
			{
				glGetProgramInfoLog(id, 512, NULL, infoLog);
				spdlog::error("Failed to link shader:\n{}", infoLog);
			}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

shader::~shader()
	{
		glDeleteProgram(id);
	}

void shader::use() const
	{
		glUseProgram(id);
	}

void shader::setInt(const char *uniform, int value) const
	{
		glUniform1i(glGetUniformLocation(m_id, uniform), value);
	}

void shader::setFloat(const char *uniform, float value) const
	{
		glUniform1f(glGetUniformLocation(m_id, uniform), value);
	}

void shader::setBool(const char *uniform, bool value) const
	{
		glUniform1i(glGetUniformLocation(m_id, uniform), value);
	}

void shader::setVec2(const char *uniform, const glm::vec2 &value) const
	{
		glUniform2fv(glGetUniformLocation(m_id, uniform), 1, glm::value_ptr(value));
	}

void shader::setVec3(const char *uniform, const glm::vec3 &value) const
	{
		glUniform3fv(glGetUniformLocation(m_id, uniform), 1, glm::value_ptr(value));
	}

void shader::setMat4(const char *uniform, const glm::mat4 &value) const
	{
		glUniformMatrix4fv(glGetUniformLocation(m_id, uniform), 1, GL_FALSE, glm::value_ptr(value));
	}

