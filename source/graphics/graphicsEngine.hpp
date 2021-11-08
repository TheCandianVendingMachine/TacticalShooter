// graphicsEngine.hpp
// High level API to render arbitrary VAO's with associated transforms
#pragma once
#include <vector>
#include <plf_colony.h>
#include "shader.hpp"
#include "renderObject.hpp"
#include "light/directionalLight.hpp"
#include "light/spotLight.hpp"
#include "light/pointLight.hpp"
#include "vertexArray.hpp"

class mesh;
class shader;
class camera;
class window;
class graphicsEngine
    {
        private:
            std::vector<const renderObject*> m_renderObjects;
            std::vector<const mesh*> m_meshes;

            plf::colony<spotLight> m_spotLights;
            plf::colony<pointLight> m_pointLights;

            shader m_forwardRenderShader;

            shader m_deferredRenderShader;
            shader m_deferredLightingShader;
            shader m_lightDebugShader;

            shader m_postProcessingShader;

            shader m_editorOrthoShader;

            vertexArray m_quadVAO;

            vertexArray m_pointLightVAO;
            vertexArray m_directionalLightVAO;

            // deferred rendering
            unsigned int m_deferredFramebuffer = 0;

            unsigned int m_gDepth = 0;

            unsigned int m_gPosition = 0;
            unsigned int m_gNormal = 0;
            unsigned int m_gAlbedo = 0;
            unsigned int m_gMetallicRoughnessAO = 0;

            // post processing
            unsigned int m_ppFramebuffer = 0;
            unsigned int m_ppRenderTexture = 0;
            unsigned int m_ppDepth = 0;

            unsigned int m_screenWidth = 0;
            unsigned int m_screenHeight = 0;

            void createFramebuffers();

            void drawDeferred(const camera &camera) const;
            void drawLight(const camera &camera) const;
            void drawDebug(const camera &camera) const;
            void postProcess(float exposure, unsigned int texture) const;

            void drawEditorOrtho(const camera &camera, unsigned int texture) const;

        public:
            enum class drawFlags
                {
                    NONE = 0,
                    DEBUG_LIGHT = 1 << 0,
                    NORMAL_PIPELINE = 1 << 1,
                    EDITOR_ORTHO_PIPELINE = 1 << 2,
                    ALL = 1 << 3,
                    COUNT
                };
            friend drawFlags operator&(const drawFlags &lhs, const drawFlags &rhs) { return static_cast<drawFlags>(static_cast<std::underlying_type<drawFlags>::type>(lhs) & static_cast<std::underlying_type<drawFlags>::type>(rhs)); }
            friend drawFlags operator|(const drawFlags &lhs, const drawFlags &rhs) { return static_cast<drawFlags>(static_cast<std::underlying_type<drawFlags>::type>(lhs) | static_cast<std::underlying_type<drawFlags>::type>(rhs)); }
            friend drawFlags operator^(const drawFlags &lhs, const drawFlags &rhs) { return static_cast<drawFlags>(static_cast<std::underlying_type<drawFlags>::type>(lhs) ^ static_cast<std::underlying_type<drawFlags>::type>(rhs)); }
            friend drawFlags &operator&=(drawFlags &lhs, const drawFlags &rhs) { lhs = lhs & rhs; return lhs; }
            friend drawFlags &operator|=(drawFlags &lhs, const drawFlags &rhs) { lhs = lhs | rhs; return lhs; }
            friend drawFlags &operator^=(drawFlags &lhs, const drawFlags &rhs) { lhs = lhs ^ rhs; return lhs; }

            directionalLight directionalLight;

            graphicsEngine(window &app);

            spotLight &createSpotLight();
            pointLight &createPointLight();

            void render(const renderObject &object);
            void render(const mesh &mesh);

            void draw(const camera &camera, unsigned int texture = 0, drawFlags flags = drawFlags::NORMAL_PIPELINE) const;
    };
