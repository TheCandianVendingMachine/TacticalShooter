// material.hpp
// defines a PBR material
#pragma once
#include "texture.hpp"

struct material
    {
        texture albedoMap;
        texture normalMap;
        texture metallicMap;
        texture roughnessMap;
        texture ambientOcclusionMap;
    };
