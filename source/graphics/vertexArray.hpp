// vertexArray.hpp
// An OpenGL vertex array object. Contains all needed information to draw a shape
#pragma once
#include <vector>
#include "vertex.hpp"

class vertexArray
	{
		private:
			unsigned int m_vao = 0;
			unsigned int m_vbo = 0;
			unsigned int m_ebo = 0;

			void generateBuffers();

		public:
			const unsigned int &vao = m_vao;
			const unsigned int &vbo = m_vbo;
			const unsigned int &ebo = m_ebo;

			vertexArray();
			vertexArray(vertex::attributes attributes);
			~vertexArray();

			void bindVertices(const std::vector<vertex> &vertices) const;
			void bindIndices(const std::vector<unsigned int> &indices) const;

			void use(vertex::attributes attributes) const;

	};
