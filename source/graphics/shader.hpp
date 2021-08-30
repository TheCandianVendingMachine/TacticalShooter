// shader.hpp
// allows the loading of a shader program
#pragma once

class shader
	{
		private:
			unsigned int m_id = 0;

		public:
			const unsigned int &id = m_id;

			shader(const char *vertexPath, const char *fragmentPath);
			~shader();

			void use();
	};
