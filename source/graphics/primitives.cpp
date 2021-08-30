#include "primitives.hpp"
#include "glad/glad.h"

std::vector<unsigned int> primitive::plane::indices = {};
std::vector<vertex> primitive::plane::vertices = {};

unsigned int primitive::plane::vbo = 0;
unsigned int primitive::plane::ebo = 0;

bool primitive::plane::isBound = false;

void primitive::plane::bind(unsigned int vao)
	{
		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		glBindVertexArray(0);

		isBound = true;
	}

primitive::plane::plane()
	{
		glm::vec3 p0(-1.f, 0.f, -1.f);
		glm::vec3 p1( 1.f, 0.f, -1.f);
		glm::vec3 p2( 1.f, 0.f,  1.f);
		glm::vec3 p3(-1.f, 0.f,  1.f);

		glm::vec2 uv0(0.f, 0.f);
		glm::vec2 uv1(0.f, 1.f);
		glm::vec2 uv2(1.f, 1.f);
		glm::vec2 uv3(1.f, 0.f);

		glm::vec3 nm(0.f, 1.f, 0.f);

		indices = { 0, 1, 2, 2, 3, 0 };
		vertices = {
			vertex{ p0, nm, { 1.f, 0.f, 0.f }, uv0, { 1.f, 1.f, 1.f } },
			vertex{ p1, nm, { 1.f, 0.f, 0.f }, uv1, { 1.f, 1.f, 1.f } },
			vertex{ p2, nm, { 1.f, 0.f, 0.f }, uv2, { 1.f, 1.f, 1.f } },
			vertex{ p3, nm, { 1.f, 0.f, 0.f }, uv3, { 1.f, 1.f, 1.f } },
		};

		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);
	}

primitive::plane::~plane()
	{
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
	}
