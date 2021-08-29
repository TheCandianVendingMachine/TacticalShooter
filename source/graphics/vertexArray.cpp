#include "vertexArray.hpp"
#include "glad/glad.h"

void vertexArray::generateBuffers()
	{
		glGenVertexArrays(1, &m_vao);
		glGenBuffers(1, &m_vbo);
		glGenBuffers(1, &m_ebo);

		glBindVertexArray(m_vao);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<void*>(offsetof(vertex, position)));
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<void*>(offsetof(vertex, normal)));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<void*>(offsetof(vertex, tangent)));
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<void*>(offsetof(vertex, textureCoordinate)));

		glBindVertexArray(0);
	}

vertexArray::vertexArray()
	{
		generateBuffers();
	}

vertexArray::vertexArray(vertex::attributes attributes)
	{
		generateBuffers();
		use(attributes);
	}

vertexArray::~vertexArray()
	{
		glDeleteBuffers(1, &m_ebo);
		glDeleteBuffers(1, &m_vbo);
		glDeleteVertexArrays(1, &m_vao);
	}

void vertexArray::bindVertices(const std::vector<vertex> &vertices) const
	{
		glBindVertexArray(m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), vertices.data(), GL_STATIC_DRAW);

		glBindVertexArray(0);
	}

void vertexArray::bindIndices(const std::vector<unsigned int> &indices) const
	{
		glBindVertexArray(m_vao);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		glBindVertexArray(0);
	}

void vertexArray::use(vertex::attributes attributes) const
	{
		glBindVertexArray(m_vao);

		if ((attributes & vertex::attributes::POSITION) == vertex::attributes::POSITION)
			{
				glEnableVertexAttribArray(0);
			}

		if ((attributes & vertex::attributes::NORMAL) == vertex::attributes::NORMAL)
			{
				glEnableVertexAttribArray(1);
			}

		if ((attributes & vertex::attributes::TANGENT) == vertex::attributes::TANGENT)
			{
				glEnableVertexAttribArray(2);
			}

		if ((attributes & vertex::attributes::TEXTURE) == vertex::attributes::TEXTURE)
			{
				glEnableVertexAttribArray(3);
			}

		glBindVertexArray(0);
	}

