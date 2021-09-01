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

		float radius(float max = std::numeric_limits<float>::max()) const
			{
				constexpr float iMaxAtten = 256.f / 5.f;

				float lightMaxComponent = std::fmaxf(std::fmaxf(diffuse.r, diffuse.g), diffuse.b);
				float radical = linear * linear - 4 * quadratic * (constant - iMaxAtten * lightMaxComponent);

				float radius = (-linear + std::sqrtf(radical)) / (2 * quadratic);

				if (radius != radius)
					{
						radius = max;
					}
				return radius;
			}
	};
