// graphicsEngine.hpp
// High level API to render arbitrary VAO's with associated transforms
#pragma once
#include <vector>
#include "shader.hpp"
#include "renderObject.hpp"
#include "light/directionalLight.hpp"
#include "light/spotLight.hpp"
#include "light/pointLight.hpp"

class shader;
class camera;
class graphicsEngine
	{
		private:
			std::vector<const renderObject*> m_renderObjects;

			std::vector<spotLight> m_spotLights;
			std::vector<pointLight> m_pointLights;

			shader m_forwardRenderShader;

		public:
			directionalLight directionalLight;

			graphicsEngine();

			void render(const renderObject &object);
			void draw(const camera &camera) const;
	};
