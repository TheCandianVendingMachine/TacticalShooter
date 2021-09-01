// graphicsEngine.hpp
// High level API to render arbitrary VAO's with associated transforms
#pragma once
#include <vector>
#include "shader.hpp"
#include "renderObject.hpp"
#include "light/directionalLight.hpp"
#include "light/spotLight.hpp"
#include "light/pointLight.hpp"
#include "vertexArray.hpp"

class shader;
class camera;
class window;
class graphicsEngine
	{
		private:
			std::vector<const renderObject*> m_renderObjects;

			std::vector<spotLight> m_spotLights;
			std::vector<pointLight> m_pointLights;

			shader m_forwardRenderShader;

			shader m_deferredRenderShader;
			shader m_deferredLightingShader;

			vertexArray m_quadVAO;

			vertexArray m_pointLightVAO;
			vertexArray m_directionalLightVAO;

			// deferred rendering
			unsigned int m_deferredFramebuffer = 0;

			unsigned int m_gDepth = 0;

			unsigned int m_gPosition = 0;
			unsigned int m_gNormal = 0;
			unsigned int m_gColourSpecular = 0;

			unsigned int m_screenWidth = 0;
			unsigned int m_screenHeight = 0;

			void createFramebuffers();

		public:
			directionalLight directionalLight;

			graphicsEngine(window &app);

			void addLight(const spotLight &light);
			void addLight(const pointLight &light);

			void render(const renderObject &object);
			void draw(const camera &camera) const;
	};
