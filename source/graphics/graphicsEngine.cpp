#include "graphicsEngine.hpp"
#include "vertexArray.hpp"
#include "transformable.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "glad/glad.h"

graphicsEngine::graphicsEngine() :
	m_forwardRenderShader("shaders/forward_lighting.vs", "shaders/forward_lighting.fs")
	{
		m_forwardRenderShader.use();
		m_forwardRenderShader.setFloat("gamma", 2.2);

		m_forwardRenderShader.setInt("material.diffuse", 0);
		m_forwardRenderShader.setInt("material.specular", 1);
		m_forwardRenderShader.setFloat("material.shininess", 128.f);
	}

void graphicsEngine::render(const renderObject &object)
	{
		m_renderObjects.push_back(&object);
	}

void graphicsEngine::draw(const camera &camera) const
	{
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



		m_forwardRenderShader.setMat4("view", camera.view());
		m_forwardRenderShader.setMat4("projection", camera.projection());

		m_forwardRenderShader.setVec3("ViewPos", camera.position);

		for (const auto &renderObject : m_renderObjects) 
			{
				m_forwardRenderShader.setMat4("model", renderObject->transform.transform());

				renderObject->diffuse.bind(GL_TEXTURE0);
				renderObject->specular.bind(GL_TEXTURE1);

				glBindVertexArray(renderObject->vao.vao);
				glDrawElements(GL_TRIANGLES, renderObject->vao.indexCount, GL_UNSIGNED_INT, 0);
			}

		glBindVertexArray(0);
	}
