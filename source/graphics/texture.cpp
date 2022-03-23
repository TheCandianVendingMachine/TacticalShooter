#include "texture.hpp"
#include <stb_image.h>
#include <glad/glad.h>
#include <utility>

void texture::loadFromMemoryInternal(unsigned char *pixels, bool useSRGB)
    {
        glGenTextures(1, &m_id);
        glBindTexture(GL_TEXTURE_2D, m_id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (pixels) 
            {
                constexpr GLint possibleFormats[] = {
                    GL_NONE,
                    GL_RED,
                    GL_RG,
                    GL_RGB,
                    GL_RGBA
                };

                GLint sourceFormat = possibleFormats[m_channels];
                GLint destFormat = possibleFormats[m_channels];

                if (useSRGB) 
                    {
                        sourceFormat = GL_SRGB;
                        if (m_channels == 4)
                            {
                                sourceFormat = GL_SRGB_ALPHA;
                                destFormat = GL_RGBA;
                            }
                    }

                glTexImage2D(GL_TEXTURE_2D, 0, sourceFormat, width, height, 0, destFormat, GL_UNSIGNED_BYTE, pixels);
                glGenerateMipmap(GL_TEXTURE_2D);
            }
    }

texture::texture(const char *file, bool useSRGB)
    {
        loadFromFile(file, useSRGB);
    }

texture::texture(const texture &rhs)
    {
        *this = rhs;
    }

texture::texture(texture &&rhs)
    {
        *this = std::move(rhs);
    }

void texture::loadFromFile(const char *file, bool useSRGB)
    {
        stbi_set_flip_vertically_on_load(true);
        unsigned char *pixels = stbi_load(file, &m_width, &m_height, &m_channels, 0);
        loadFromMemoryInternal(pixels, useSRGB);
        stbi_image_free(pixels);
    }

void texture::loadFromMemory(unsigned char *pixels, std::size_t length, bool useSRGB)
    {
        unsigned char *memoryPixels = stbi_load_from_memory(pixels, length, &m_width, &m_height, &m_channels, 0);
        loadFromMemoryInternal(memoryPixels, useSRGB);
        stbi_image_free(memoryPixels);
    }

void texture::bind(int textureUnit) const
    {
        glActiveTexture(textureUnit);
        glBindTexture(GL_TEXTURE_2D, id);
    }

texture &texture::operator=(const texture &rhs)
    {
        if (&rhs != this)
            {
                m_width = rhs.width;
                m_height = rhs.height;
                m_channels = rhs.channels;
                m_id = rhs.id;
                type = rhs.type;
            }
        return *this;
    }

texture &texture::operator=(texture &&rhs)
    {
        if (&rhs != this)
            {
                m_width = std::move(rhs.m_width);
                m_height = std::move(rhs.m_height);
                m_channels = std::move(rhs.m_channels);
                m_id = std::move(rhs.m_id);
                type = std::move(rhs.type);
            }
        return *this;
    }
