#include "shader.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <glad/glad.h>
#include <iostream>
#include <spdlog/spdlog.h>

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

void shader::use()
	{
		glUseProgram(id);
	}

