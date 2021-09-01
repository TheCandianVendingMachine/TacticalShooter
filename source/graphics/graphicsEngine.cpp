#include "graphicsEngine.hpp"
#include "vertexArray.hpp"
#include "transformable.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "glad/glad.h"
#include "window.hpp"
#include "str.hpp"
#include "primitives.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

constexpr unsigned int c_gBufferCount = 3;

void graphicsEngine::createFramebuffers()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_deferredFramebuffer);

		// position buffer
		glBindTexture(GL_TEXTURE_2D, m_gPosition);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, static_cast<int>(m_screenWidth), static_cast<int>(m_screenHeight), 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gPosition, 0);

		// normal buffer
		glBindTexture(GL_TEXTURE_2D, m_gNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, static_cast<int>(m_screenWidth), static_cast<int>(m_screenHeight), 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gNormal, 0);

		// colour + specular buffer
		glBindTexture(GL_TEXTURE_2D, m_gColourSpecular);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<int>(m_screenWidth), static_cast<int>(m_screenHeight), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_gColourSpecular, 0);

		unsigned int attachments[c_gBufferCount] = {
			GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1,
			GL_COLOR_ATTACHMENT2
		};
		glDrawBuffers(c_gBufferCount, attachments);

		glBindTexture(GL_TEXTURE_2D, m_gDepth);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, static_cast<int>(m_screenWidth), static_cast<int>(m_screenHeight), 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_gDepth, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
				spdlog::error("[Deferred Rendering] Framebuffer is not complete");
			}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

graphicsEngine::graphicsEngine(window &app) :
	m_forwardRenderShader("shaders/forward_lighting.vs", "shaders/forward_lighting.fs"),
	m_deferredRenderShader("shaders/forward_lighting.vs", "shaders/deferred_rendering.fs"),
	m_deferredLightingShader("shaders/deferred_lighting.vs", "shaders/deferred_lighting.fs"),
	m_lightDebugShader("shaders/basic3d.vs", "shaders/basic3d.fs")
	{
		m_screenWidth = app.width;
		m_screenHeight = app.height;

		m_forwardRenderShader.use();
		m_forwardRenderShader.setFloat("gamma", 2.2);

		m_forwardRenderShader.setInt("material.diffuse", 0);
		m_forwardRenderShader.setInt("material.specular", 1);
		m_forwardRenderShader.setFloat("material.shininess", 128.f);

		m_deferredRenderShader.use();
		m_deferredRenderShader.setInt("material.diffuse", 0);
		m_deferredRenderShader.setInt("material.specular", 1);
		m_deferredRenderShader.setFloat("material.shininess", 128.f);

		glGenFramebuffers(1, &m_deferredFramebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_deferredFramebuffer);

		unsigned int gBuffers[c_gBufferCount] = {};
		glGenTextures(c_gBufferCount, gBuffers);

		m_gColourSpecular = gBuffers[0];
		m_gNormal = gBuffers[1];
		m_gPosition = gBuffers[2];

		glGenTextures(1, &m_gDepth);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		createFramebuffers();
		app.subscribe(FE_STR("framebufferResize"), [this] (message &event) {
			m_screenWidth = event.arguments[0].variable.INT;
			m_screenHeight = event.arguments[1].variable.INT;

			createFramebuffers();
		});

		m_quadVAO = primitive::plane::generate(vertex::attributes::POSITION | vertex::attributes::TEXTURE);

		m_pointLightVAO = primitive::sphere::generate(vertex::attributes::POSITION | vertex::attributes::COLOUR);
		m_directionalLightVAO = primitive::plane::generate(vertex::attributes::POSITION);
	}

void graphicsEngine::addLight(const spotLight &light)
	{
		m_spotLights.push_back(light);
	}

void graphicsEngine::addLight(const pointLight &light)
	{
		m_pointLights.push_back(light);
	}

void graphicsEngine::render(const renderObject &object)
	{
		m_renderObjects.push_back(&object);
	}

void graphicsEngine::draw(const camera &camera) const
	{
		// TODO: Implement light volumes via rendering spheres that represent light radius
		glCullFace(GL_BACK);

		m_forwardRenderShader.use();

		m_forwardRenderShader.setVec3("lightInfo.direction", directionalLight.direction);
		//m_forwardRenderShader.setVec3("lightInfo.position", testLight.position);
		//m_forwardRenderShader.setFloat("lightInfo.cutoff", testLight.cutoffAngleCos);
		//m_forwardRenderShader.setFloat("lightInfo.cutoffOuter", testLight.outerCutoffAngleCos);
		m_forwardRenderShader.setInt("lightInfo.type", 0);

		m_forwardRenderShader.setVec3("light.ambient", directionalLight.info.ambient);
		m_forwardRenderShader.setVec3("light.diffuse", directionalLight.info.diffuse);
		m_forwardRenderShader.setVec3("light.specular", directionalLight.info.specular);

		m_forwardRenderShader.setFloat("light.constant", directionalLight.info.constant);
		m_forwardRenderShader.setFloat("light.linear", directionalLight.info.linear);
		m_forwardRenderShader.setFloat("light.quadratic", directionalLight.info.quadratic);

		glBindFramebuffer(GL_FRAMEBUFFER, m_deferredFramebuffer);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_deferredRenderShader.use();

		m_deferredRenderShader.setMat4("view", camera.view());
		m_deferredRenderShader.setMat4("projection", camera.projection());

		m_deferredRenderShader.setVec3("ViewPos", camera.position);

		for (const auto &renderObject : m_renderObjects) 
			{
				m_deferredRenderShader.setMat4("model", renderObject->transform.transform());

				renderObject->diffuse.bind(GL_TEXTURE0);
				renderObject->specular.bind(GL_TEXTURE1);

				glBindVertexArray(renderObject->vao.vao);
				glDrawElements(GL_TRIANGLES, renderObject->vao.indexCount, GL_UNSIGNED_INT, 0);
			}
		glBindVertexArray(0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_deferredLightingShader.use();
		m_deferredLightingShader.setVec2("FramebufferSize", glm::vec2(m_screenWidth, m_screenHeight));

		m_deferredLightingShader.setInt("gPosition", 0);
		m_deferredLightingShader.setInt("gNormal", 1);
		m_deferredLightingShader.setInt("gColourSpecular", 2);

		m_deferredLightingShader.setVec3("lightInfo.direction", directionalLight.direction);
		//m_deferredLightingShader.setVec3("lightInfo.position", testLight.position);
		//m_deferredLightingShader.setFloat("lightInfo.cutoff", testLight.cutoffAngleCos);
		//m_deferredLightingShader.setFloat("lightInfo.cutoffOuter", testLight.outerCutoffAngleCos);
		m_deferredLightingShader.setInt("lightInfo.type", 0);

		m_deferredLightingShader.setVec3("light.ambient", directionalLight.info.ambient);
		m_deferredLightingShader.setVec3("light.diffuse", directionalLight.info.diffuse);
		m_deferredLightingShader.setVec3("light.specular", directionalLight.info.specular);

		m_deferredLightingShader.setFloat("light.constant", directionalLight.info.constant);
		m_deferredLightingShader.setFloat("light.linear", directionalLight.info.linear);
		m_deferredLightingShader.setFloat("light.quadratic", directionalLight.info.quadratic);

		m_deferredLightingShader.setMat4("view", camera.view());
		m_deferredLightingShader.setMat4("projection", camera.projection());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_gColourSpecular);

		glCullFace(GL_FRONT);
		for (const auto &pointLight : m_pointLights)
			{
				//m_deferredLightingShader.setVec3("lightInfo.direction", pointLight.direction);
				m_deferredLightingShader.setVec3("lightInfo.position", pointLight.position);
				//m_deferredLightingShader.setFloat("lightInfo.cutoff", pointLight.cutoffAngleCos);
				//m_deferredLightingShader.setFloat("lightInfo.cutoffOuter", pointLight.outerCutoffAngleCos);
				m_deferredLightingShader.setInt("lightInfo.type", 1);

				m_deferredLightingShader.setVec3("light.ambient", pointLight.info.ambient);
				m_deferredLightingShader.setVec3("light.diffuse", pointLight.info.diffuse);
				m_deferredLightingShader.setVec3("light.specular", pointLight.info.specular);

				m_deferredLightingShader.setFloat("light.constant", pointLight.info.constant);
				m_deferredLightingShader.setFloat("light.linear", pointLight.info.linear);
				m_deferredLightingShader.setFloat("light.quadratic", pointLight.info.quadratic);

				float scale = pointLight.info.radius(camera.zFar / 2.f);
				glm::mat4 model = glm::translate(glm::mat4(1.f), pointLight.position);
				model = glm::scale(model, glm::vec3(scale));
				m_deferredLightingShader.setMat4("model", model);

				glBindVertexArray(m_pointLightVAO.vao);
				glDrawElements(GL_TRIANGLES, m_pointLightVAO.indexCount, GL_UNSIGNED_INT, 0);
			}

		glBindVertexArray(0);

		if (m_debugDrawLight) 
			{
				m_lightDebugShader.use();
				m_lightDebugShader.setMat4("view", camera.view());
				m_lightDebugShader.setMat4("projection", camera.projection());

				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDisable(GL_CULL_FACE);
				for (const auto &pointLight : m_pointLights)
				{
					float scale = pointLight.info.radius(camera.zFar / 2.f);
					glm::mat4 model = glm::translate(glm::mat4(1.f), pointLight.position);
					model = glm::scale(model, glm::vec3(scale));
					m_lightDebugShader.setMat4("model", model);

					glBindVertexArray(m_pointLightVAO.vao);
					glDrawElements(GL_TRIANGLES, m_pointLightVAO.indexCount, GL_UNSIGNED_INT, 0);
				}
				glBindVertexArray(0);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glEnable(GL_CULL_FACE);
			}
	}
