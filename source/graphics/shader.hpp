// shader.hpp
// allows the loading of a shader program
#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class shader
	{
		private:
			unsigned int m_id = 0;

		public:
			const unsigned int &id = m_id;

			shader(const char *vertexPath, const char *fragmentPath);
			~shader();

			void use();

			void setInt(const char *uniform, int value) const;
			void setFloat(const char *uniform, float value) const;
			void setBool(const char *uniform, bool value) const;
			void setVec2(const char *uniform, const glm::vec2 &value) const;
			void setVec3(const char *uniform, const glm::vec3 &value) const;
			void setMat4(const char *uniform, const glm::mat4 &value) const;
	};
