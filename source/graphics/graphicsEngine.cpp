#include "graphicsEngine.hpp"
#include "vertexArray.hpp"
#include "transformable.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "glad/glad.h"
#include "window.hpp"
#include "str.hpp"
#include "primitives.hpp"
#include "mesh.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

constexpr unsigned int c_gBufferCount = 4;

void graphicsEngine::createFramebuffers()
	{
		/* BEGIN DEFERRED RENDERING */
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

			// albedo buffer
			glBindTexture(GL_TEXTURE_2D, m_gAlbedo);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, static_cast<int>(m_screenWidth), static_cast<int>(m_screenHeight), 0, GL_RGB, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_gAlbedo, 0);

			// metallic, roughness, and ambient occlusion buffer
			glBindTexture(GL_TEXTURE_2D, m_gMetallicRoughnessAO);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, static_cast<int>(m_screenWidth), static_cast<int>(m_screenHeight), 0, GL_RGB, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_gMetallicRoughnessAO, 0);

			unsigned int attachments[c_gBufferCount] = {
				GL_COLOR_ATTACHMENT0,
				GL_COLOR_ATTACHMENT1,
				GL_COLOR_ATTACHMENT2,
				GL_COLOR_ATTACHMENT3
			};
			glDrawBuffers(c_gBufferCount, attachments);

			glBindTexture(GL_TEXTURE_2D, m_gDepth);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, static_cast<int>(m_screenWidth), static_cast<int>(m_screenHeight), 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_gDepth, 0);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				{
					spdlog::error("[Deferred Rendering] Framebuffer is not complete");
				}
		}
		/* END DEFERRED RENDERING */
		/* BEGIN POST PROCESSING*/
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_ppFramebuffer);
			glBindTexture(GL_TEXTURE_2D, m_ppRenderTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, static_cast<int>(m_screenWidth), static_cast<int>(m_screenHeight), 0, GL_RGBA, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ppRenderTexture, 0);

			unsigned int attachments[] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers(1, attachments);

			glBindTexture(GL_TEXTURE_2D, m_ppDepth);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, static_cast<int>(m_screenWidth), static_cast<int>(m_screenHeight), 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_ppDepth, 0);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				{
					spdlog::error("[Post Processing] Framebuffer is not complete");
				}
		}
		/* END POST PROCESSING */

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

graphicsEngine::graphicsEngine(window &app) :
	m_forwardRenderShader("shaders/forward_lighting.vs", "shaders/forward_lighting.fs"),
	m_deferredRenderShader("shaders/forward_lighting.vs", "shaders/deferred_rendering_phong.fs"),
	m_deferredLightingShader("shaders/deferred_lighting_phong.vs", "shaders/deferred_lighting_pbr.fs"),
	m_lightDebugShader("shaders/basic3d.vs", "shaders/basic3d.fs"),
	m_postProcessingShader("shaders/post_processing.vs", "shaders/post_processing.fs")
	{
		m_screenWidth = app.width;
		m_screenHeight = app.height;

		m_forwardRenderShader.use();
		m_forwardRenderShader.setFloat("gamma", 2.2);

		m_forwardRenderShader.setInt("material.diffuse", 0);
		m_forwardRenderShader.setInt("material.specular", 1);
		m_forwardRenderShader.setFloat("material.shininess", 128.f);

		m_deferredRenderShader.use();
		m_deferredRenderShader.setInt("material.albedoMap", 0);
		m_deferredRenderShader.setInt("material.normalMap", 1);
		m_deferredRenderShader.setInt("material.metallicMap", 2);
		m_deferredRenderShader.setInt("material.roughnessMap", 3);
		m_deferredRenderShader.setInt("material.ambientOcclusionMap", 4);

		m_postProcessingShader.use();
		m_postProcessingShader.setInt("Image", 0);

		/* BEGIN DEFERRED RENDERING */
		glGenFramebuffers(1, &m_deferredFramebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_deferredFramebuffer);

		unsigned int gBuffers[c_gBufferCount + 1] = {};
		glGenTextures(c_gBufferCount + 1, gBuffers);

		m_gAlbedo =					gBuffers[0];
		m_gNormal =					gBuffers[1];
		m_gPosition =				gBuffers[2];
		m_gMetallicRoughnessAO =	gBuffers[3];
		m_gDepth =					gBuffers[4];

		/* END DEFERRED RENDERING */

		/* BEGIN POST PROCESSING */
		glGenFramebuffers(1, &m_ppFramebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_ppFramebuffer);
		glGenTextures(1, &m_ppRenderTexture);
		glGenTextures(1, &m_ppDepth);

		/* END POST PROCESSING*/

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

void graphicsEngine::render(const renderObject &object)
	{
		m_renderObjects.push_back(&object);
	}

void graphicsEngine::render(const mesh &object)
	{
		m_meshes.push_back(&object);
	}

void graphicsEngine::draw(const perspectiveCamera &camera) const
	{
		glCullFace(GL_BACK);

		/* BEGIN DEFERRED RENDERING */
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

				renderObject->material.albedoMap.bind(GL_TEXTURE0);
				renderObject->material.normalMap.bind(GL_TEXTURE1);
				renderObject->material.metallicMap.bind(GL_TEXTURE2);
				renderObject->material.roughnessMap.bind(GL_TEXTURE3);
				renderObject->material.ambientOcclusionMap.bind(GL_TEXTURE4);

				glBindVertexArray(renderObject->vao.vao);
				glDrawElements(GL_TRIANGLES, renderObject->vao.indexCount, GL_UNSIGNED_INT, 0);
			}

		for (const auto &mesh : m_meshes) 
			{
				glm::mat4 transform(1.f);
				m_deferredRenderShader.setMat4("model", transform);

				glBindVertexArray(mesh->m_vertices.vao);
				for (auto &submesh : mesh->m_subMeshes) 
					{
						submesh.materials.albedoMap.bind(GL_TEXTURE0);
						submesh.materials.normalMap.bind(GL_TEXTURE1);
						submesh.materials.metallicMap.bind(GL_TEXTURE2);
						submesh.materials.roughnessMap.bind(GL_TEXTURE3);
						submesh.materials.ambientOcclusionMap.bind(GL_TEXTURE4);

						glDrawElements(GL_TRIANGLES, submesh.indexCount, GL_UNSIGNED_INT, reinterpret_cast<void*>(submesh.indexStartIndex));
					}
			}
		/* END DEFERRED RENDERING */

		glBindFramebuffer(GL_FRAMEBUFFER, m_ppFramebuffer);
		/* BEGIN LIGHT EQUATION */
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glBlendEquation(GL_FUNC_ADD);
		glDisable(GL_DEPTH_TEST); // we want fragments to overdraw

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_deferredLightingShader.use();
		m_deferredLightingShader.setVec2("framebufferSize", glm::vec2(m_screenWidth, m_screenHeight));
		m_deferredLightingShader.setVec3("cameraPosition", camera.position);

		m_deferredLightingShader.setInt("gPosition", 0);
		m_deferredLightingShader.setInt("gNormal", 1);
		m_deferredLightingShader.setInt("gAlbedo", 2);
		m_deferredLightingShader.setInt("gMetallicRoughnessAO", 3);

		m_deferredLightingShader.setVec3("lightInfo.direction", directionalLight.direction);
		m_deferredLightingShader.setInt("lightInfo.type", 0);

		m_deferredLightingShader.setVec3("light.ambient", directionalLight.info.ambient);
		m_deferredLightingShader.setVec3("light.diffuse", directionalLight.info.diffuse);

		m_deferredLightingShader.setMat4("view", camera.view());
		m_deferredLightingShader.setMat4("projection", camera.projection());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_gAlbedo);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, m_gMetallicRoughnessAO);

		glCullFace(GL_FRONT);

		glBindVertexArray(m_pointLightVAO.vao);
		for (const auto &pointLight : m_pointLights)
			{
				m_deferredLightingShader.setVec3("lightSpatialInfo.position", pointLight.position);
				m_deferredLightingShader.setInt("lightSpatialInfo.type", 1);

				m_deferredLightingShader.setVec3("light.ambient", pointLight.info.ambient);
				m_deferredLightingShader.setVec3("light.diffuse", pointLight.info.diffuse);

				float scale = pointLight.info.radius();
				glm::mat4 model = glm::translate(glm::mat4(1.f), pointLight.position);
				model = glm::scale(model, glm::vec3(scale));
				m_deferredLightingShader.setMat4("model", model);

				glDrawElements(GL_TRIANGLES, m_pointLightVAO.indexCount, GL_UNSIGNED_INT, 0);
			}

		for (const auto &spotLight : m_spotLights)
			{
				m_deferredLightingShader.setVec3("lightSpatialInfo.direction", spotLight.direction);
				m_deferredLightingShader.setVec3("lightSpatialInfo.position", spotLight.position);
				m_deferredLightingShader.setFloat("lightSpatialInfo.cutoff", spotLight.cutoffAngleCos);
				m_deferredLightingShader.setFloat("lightSpatialInfo.cutoffOuter", spotLight.outerCutoffAngleCos);
				m_deferredLightingShader.setInt("lightSpatialInfo.type", 2);

				m_deferredLightingShader.setVec3("light.ambient", spotLight.info.ambient);
				m_deferredLightingShader.setVec3("light.diffuse", spotLight.info.diffuse);

				float scale = spotLight.info.radius();
				glm::mat4 model = glm::translate(glm::mat4(1.f), spotLight.position);
				model = glm::scale(model, glm::vec3(scale));
				m_deferredLightingShader.setMat4("model", model);

				glDrawElements(GL_TRIANGLES, m_pointLightVAO.indexCount, GL_UNSIGNED_INT, 0);
			}

		glEnable(GL_DEPTH_TEST); // we want fragments to overdraw
		glDisable(GL_BLEND);
		/* END LIGHT EQUATION */
		/* BEGIN DEBUG DRAWING */
		if (m_debugDrawLight) 
			{
				m_lightDebugShader.use();
				m_lightDebugShader.setMat4("view", camera.view());
				m_lightDebugShader.setMat4("projection", camera.projection());

				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDisable(GL_CULL_FACE);

				glBindVertexArray(m_pointLightVAO.vao);
				for (const auto &pointLight : m_pointLights)
					{
						float scale = pointLight.info.radius();
						glm::mat4 model = glm::translate(glm::mat4(1.f), pointLight.position);
						model = glm::scale(model, glm::vec3(scale));
						m_lightDebugShader.setMat4("model", model);

						glDrawElements(GL_TRIANGLES, m_pointLightVAO.indexCount, GL_UNSIGNED_INT, 0);
					}

				for (const auto &spotLight : m_spotLights)
				{
					float scale = spotLight.info.radius();
					glm::mat4 model = glm::translate(glm::mat4(1.f), spotLight.position);
					model = glm::scale(model, glm::vec3(scale));
					m_lightDebugShader.setMat4("model", model);

					glDrawElements(GL_TRIANGLES, m_pointLightVAO.indexCount, GL_UNSIGNED_INT, 0);
				}

				glBindVertexArray(0);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glEnable(GL_CULL_FACE);
			}
		/* END DEBUG DRAWING */

		/* BEGIN POST PROCESSING */
		glDisable(GL_CULL_FACE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_ppRenderTexture);

		m_postProcessingShader.use();
		m_postProcessingShader.setFloat("gamma", 2.2f);
		m_postProcessingShader.setFloat("exposure", 1.f);

		glBindVertexArray(m_quadVAO.vao);
		glDrawElements(GL_TRIANGLES, m_quadVAO.indexCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glEnable(GL_CULL_FACE);
		/* END POST PROCESSING */
	}

spotLight &graphicsEngine::createSpotLight()
	{
		return *m_spotLights.emplace();
	}

pointLight &graphicsEngine::createPointLight()
	{
		return *m_pointLights.emplace();
	}
