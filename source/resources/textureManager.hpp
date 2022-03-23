// textureManager.hpp
// loads textures from disk, serialises them, gives them a UID, and allows them to be refered
#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include "robin_hood.h"
#include "typeDefines.hpp"
#include "graphics/texture.hpp"

class textureManager
    {
        private:
            struct metaFileData
                {
                    unsigned char version = 0;
                    std::vector<char> hash;
                    std::vector<char> internalName;
                    std::vector<unsigned char> imageData;
                };

            friend void parseMetaFileV1(std::FILE *stream, textureManager::metaFileData &metaFile);

            robin_hood::unordered_map<std::string, std::string> m_nameUIDMap;
            robin_hood::unordered_map<std::string, texture> m_textures;

        public:
            const robin_hood::unordered_map<std::string, std::string> &nameUIDMap = m_nameUIDMap;

            void loadFromMetaImage(const char *path);
            texture get(const char *name);
    };

