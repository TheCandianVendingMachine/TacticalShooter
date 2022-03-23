#include "textureManager.hpp"
#include "zip.h"
#include <vector>
#include <string>
#include <stdio.h>
#include <spdlog/spdlog.h>

void parseMetaFileV1(std::FILE *stream, textureManager::metaFileData &metaFile)
    {
        {
            unsigned int hashLength = 0;
            char *hashLengthBuffer = reinterpret_cast<char*>(&hashLength);

            for (int i = 0; i < sizeof(hashLength); i++)
                {
                    hashLengthBuffer[i] = std::getc(stream);
                }

            metaFile.hash = std::vector<char>(hashLength);
            for (unsigned int i = 0; i < hashLength; i++)
                {
                    metaFile.hash[i] = std::getc(stream);
                }
            metaFile.hash.push_back('\0');
        }

        {
            unsigned int nameLength = 0;
            char *nameLengthBuffer = reinterpret_cast<char*>(&nameLength);

            for (int i = 0; i < sizeof(nameLength); i++)
                {
                    nameLengthBuffer[i] = std::getc(stream);
                }

            metaFile.internalName = std::vector<char>(nameLength);
            for (unsigned int i = 0; i < nameLength; i++) 
                {
                    metaFile.internalName[i] = std::getc(stream);
                }
            metaFile.internalName.push_back('\0');
        }

        {
            unsigned long long imageLength = 0;
            char *imageLengthBuffer = reinterpret_cast<char*>(&imageLength);

            for (int i = 0; i < sizeof(imageLength); i++)
                {
                    imageLengthBuffer[i] = std::getc(stream);
                }

            metaFile.imageData = std::vector<unsigned char>(imageLength);
            for (unsigned long long i = 0; i < imageLength; i++)
                {
                    metaFile.imageData[i] = std::getc(stream);
                }
        }

        {
            bool isSRGB = false;
            char *isSRGBBuffer = reinterpret_cast<char*>(&isSRGB);
            for (int i = 0; i < sizeof(isSRGB); i++)
                {
                    isSRGBBuffer[i] = std::getc(stream);
                }

            metaFile.isSRGB = isSRGB;
        }
    }

void textureManager::loadFromMetaImage(const char *path)
    {
        metaFileData metaFile;
        std::FILE *stream = std::fopen(path, "rb");
        if (!stream || std::ferror(stream))
            {
                spdlog::error("Cannot read file {} Reason: {}", path, std::strerror(errno));
                return;
            }

        metaFile.version = std::getc(stream);
        switch (metaFile.version)
            {
                case 1:
                    parseMetaFileV1(stream, metaFile);
                    break;
                default:
                    spdlog::error("metafile version parsing not implemented: {}", metaFile.version);
                    break;
            }

        std::fclose(stream);

        texture t;
        t.loadFromMemory(metaFile.imageData.data(), metaFile.imageData.size(), metaFile.isSRGB);

        m_nameUIDMap.insert({ metaFile.internalName.data(), metaFile.hash.data() });
        m_textures.insert({ metaFile.hash.data(), std::move(t) });
    }

texture textureManager::get(const char *name)
    {
        if (m_nameUIDMap.find(name) == m_nameUIDMap.end()) 
            {
                spdlog::error("Cannot find texture with name in UID map {}", name);
                return texture();
            }

        if (m_textures.find(m_nameUIDMap.at(name)) == m_textures.end())
            {
                spdlog::error("Cannot find texture with name in texture map {}", name);
                return texture();
            }

        return m_textures.at(m_nameUIDMap.at(name));
    }

