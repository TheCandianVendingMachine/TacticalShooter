// texture.hpp
// a texture loaded into memory
#pragma once

class texture
	{
		private:
			int m_width = 0;
			int m_height = 0;
			int m_channels = 0;
			unsigned int m_id = 0;

		public:
			enum class type
				{
					NONE,
					SPECULAR,
					DIFFUSE,
					NORMAL,
					PARALLAX
				};

			type type = type::NONE;

			const int &width = m_width;
			const int &height = m_height;
			const int &channels = m_channels;
			const unsigned int &id = m_id;

			texture() = default;
			texture(const char *file, bool useSRGB);
			texture(const texture &rhs);
			texture(texture &&rhs);
			
			void loadFromFile(const char *file, bool useSRGB);

			void bind(int textureUnit) const;

			texture &operator=(const texture &rhs);
			texture &operator=(texture &&rhs);
	};
