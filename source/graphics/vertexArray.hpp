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

			bool m_ownBuffers = false;

			unsigned int m_indexCount = 0;
			unsigned int m_vertexCount = 0;

		public:
			const unsigned int &vao = m_vao;
			const unsigned int &vbo = m_vbo;
			const unsigned int &ebo = m_ebo;

			const unsigned int &vertexCount = m_vertexCount;
			const unsigned int &indexCount = m_indexCount;

			vertexArray();
			vertexArray(const vertexArray &rhs);
			~vertexArray();

			vertexArray &operator=(const vertexArray &rhs);

			void bindVertices(const std::vector<vertex> &vertices);
			void bindVertices(unsigned int vbo, unsigned int vertexCount);

			void bindIndices(const std::vector<unsigned int> &indices);
			void bindIndices(unsigned int ebo, unsigned int indexCount);

			void use(vertex::attributes attributes) const;

	};
