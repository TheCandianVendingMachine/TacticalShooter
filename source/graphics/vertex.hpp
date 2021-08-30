// vertex.hpp
// a vertex in 3d space
#pragma once
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

struct vertex
	{
		glm::vec3 position = { 0, 0, 0 };
		glm::vec3 normal = { 0, 0, 0 };
		glm::vec3 tangent = { 0, 0, 0 };
		glm::vec2 textureCoordinate = { 0, 0 };
		glm::vec3 colour = { 0, 0, 0 };

		enum class attributes
			{
				NONE		= 0,
				POSITION	= 1 << 0,
				NORMAL		= 1 << 1,
				TANGENT		= 1 << 2,
				COLOUR		= 1 << 3,
				TEXTURE		= 1 << 4
			};

		friend vertex::attributes operator|(const vertex::attributes &lhs, const vertex::attributes &rhs);
		friend vertex::attributes operator&(const vertex::attributes &lhs, const vertex::attributes &rhs);
	};

