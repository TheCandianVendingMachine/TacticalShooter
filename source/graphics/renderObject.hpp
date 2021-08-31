// renderObject.hpp
// Struct that contains a VAO and transform and anything else needed for rendering
#pragma once
#include "texture.hpp"
#include "vertexArray.hpp"
#include "transformable.hpp"

struct renderObject
	{
		texture diffuse;
		texture specular;
		vertexArray vao;
		transformable transform;
	};

