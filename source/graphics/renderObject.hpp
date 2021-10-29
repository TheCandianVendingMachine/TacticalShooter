// renderObject.hpp
// Struct that contains a VAO and transform and anything else needed for rendering
#pragma once
#include "vertexArray.hpp"
#include "transformable.hpp"
#include "material.hpp"

struct renderObject
    {
        material material;
        vertexArray vao;
        transformable transform;
    };

