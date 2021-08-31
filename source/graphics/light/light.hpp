// light.hpp
// information regarding the light itself: colour, intensity, etc
#pragma once
#include <glm/vec3.hpp>

struct light
	{
		glm::vec3 ambient = { 1.f, 1.f, 1.f };
		glm::vec3 diffuse = { 1.f, 1.f, 1.f };
		glm::vec3 specular = { 1.f, 1.f, 1.f };

		float constant = 1.f;
		float linear = 0.f;
		float quadratic = 0.f;
	};
