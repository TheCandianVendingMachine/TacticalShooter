/*
    ENGINE: OpenGL Renderer
    ENGINE: Shadows
    ENGINE: Abstracted framebuffers
    ENGINE: Font rendering
    ENGINE: IBL
    ENGINE: Paralax Mapping
    ENGINE: Bloom
    ENGINE: Billboarded Sprites
    ENGINE: Decals
    ENGINE: Mesh bone animations
    ENGINE: Multiplayer
    ENGINE: PhysX
    ENGINE: Game state machine
    ENGINE: FMOD integration
    ENGINE: Steam Audio
    ENGINE: Generic asset referal (hash of file in header as UID?)
    ENGINE: ECS
    ENGINE: Front end UI
    ENGINE: Resource handler
    ENGINE: Observer event handler
    FEATURE: 3d level editor
*/
#include "graphics/window.hpp"
#include "graphics/primitives.hpp"
#include "graphics/shader.hpp"
#include "graphics/camera.hpp"
#include "graphics/texture.hpp"
#include "graphics/light/directionalLight.hpp"
#include "graphics/light/pointLight.hpp"
#include "graphics/light/spotLight.hpp"
#include "graphics/transformable.hpp"
#include "graphics/graphicsEngine.hpp"
#include "graphics/renderObject.hpp"

#include "random.hpp"
#include "clock.hpp"
#include "time.hpp"
#include "str.hpp"
#include "inputHandler.hpp"

#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "graphics/mesh.hpp"

#include "editor/editor.hpp"

#include "physics/physicsWorld.hpp"

#include "ecs/graphicsSystem.hpp"
#include "ecs/physicsSystem.hpp"
#include "ecs/graphicsComponent.hpp"
#include "ecs/physicsComponent.hpp"
#include "ecs/entity.hpp"

int main()
    {
        spdlog::set_level(spdlog::level::debug);
        
        physicsWorld physics;

        fe::randomImpl c_generator;
        c_generator.startUp();
        c_generator.seed(1337);

        window app(1280, 720, "Tactical Shooter Prototype");
        glfwSetInputMode(app.getWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

        // generate all primitives onto stack. Needs to be called after OpenGL initialisation
        primitive::plane c_plane;
        primitive::sphere c_sphere;
        primitive::cube c_cube;

        graphicsEngine graphicsEngine(app);

        physicsSystem ecsPhysics(physics);
        graphicsSystem ecsGraphics(graphicsEngine);

        inputHandler c_inputHandler(app.getWindow(), "inputs.ini");
        globals::g_inputs = &c_inputHandler;

        c_inputHandler.addDefaultKey("movement", "forward", GLFW_KEY_W);
        c_inputHandler.addDefaultKey("movement", "backward", GLFW_KEY_S);
        c_inputHandler.addDefaultKey("movement", "left", GLFW_KEY_A);
        c_inputHandler.addDefaultKey("movement", "right", GLFW_KEY_D);

        c_inputHandler.addDefaultKey("debug", "close", GLFW_KEY_ESCAPE);
        c_inputHandler.addDefaultKey("debug", "toggleCursor", GLFW_KEY_F1);

        editor editor(app, graphicsEngine);

        c_inputHandler.save("inputs.ini");

        texture albedo("rustediron2_basecolor.png", true);
        texture metallic("rustediron2_metallic.png", false);
        texture normal("rustediron2_normal.png", true);
        texture roughness("rustediron2_roughness.png", false);
        texture ao("rustediron2_ao.png", false);

        material temp = {
            .albedoMap = albedo,
            .normalMap = normal,
            .metallicMap = metallic,
            .roughnessMap = roughness,
            .ambientOcclusionMap = ao
        };

        entity ecsSphere;
        {
            ecsSphere.addComponent(ecsGraphics.createComponent(graphicsSystem::graphicData{
                .vao = primitive::sphere::generate(vertex::attributes::POSITION | vertex::attributes::NORMAL | vertex::attributes::TANGENT | vertex::attributes::TEXTURE),
                .material = temp
                }));
            ecsSphere.addComponent(ecsPhysics.createComponent(physicsSystem::rigidBodyData{
                .simulationType = physicsWorld::rigidType::DYNAMIC,
                .rigidType = rigidBody::types::SPHERE
                }));
            graphicsEngine.render(static_cast<graphicsComponent *>(ecsSphere.getComponent("graphics"))->renderObject);
        }

        entity ecsPlane;
        {
            component &planeGraphics = ecsPlane.addComponent(ecsGraphics.createComponent(graphicsSystem::graphicData{
                .vao = primitive::plane::generate(vertex::attributes::POSITION | vertex::attributes::NORMAL | vertex::attributes::TANGENT | vertex::attributes::TEXTURE),
                .material = temp
            }));
            component &planePhysics = ecsPlane.addComponent(ecsPhysics.createComponent(physicsSystem::rigidBodyData{
                .simulationType = physicsWorld::rigidType::STATIC,
                .rigidType = rigidBody::types::PLANE,
                .transform = physx::PxTransform(physx::PxVec3(0, 5.f, 0), physx::PxQuat(physx::PxHalfPi, { 0, 0, -1 }))
            }));
            graphicsEngine.render(static_cast<graphicsComponent*>(ecsPlane.getComponent("graphics"))->renderObject);

            static_cast<graphicsComponent&>(planeGraphics).renderObject.transform.position = { 0, 5.f, 0 };
            static_cast<graphicsComponent&>(planeGraphics).renderObject.transform.scale = { 50.f, 0.f, 50.f };

            graphicsEngine.render(static_cast<graphicsComponent&>(planeGraphics).renderObject);
        }

        entity ecsCube;
        {
            component &cubeGraphics = ecsPlane.addComponent(ecsGraphics.createComponent(graphicsSystem::graphicData{
                .vao = primitive::cube::generate(vertex::attributes::POSITION | vertex::attributes::NORMAL | vertex::attributes::TANGENT | vertex::attributes::TEXTURE),
                .material = temp
            }));

            static_cast<graphicsComponent&>(cubeGraphics).renderObject.transform.position = { 2.f, 3.f, 0.f };
            graphicsEngine.render(static_cast<graphicsComponent&>(cubeGraphics).renderObject);
        }

        pointLight &pl = graphicsEngine.createPointLight();
        pl.position = glm::vec3(-5.f, 4.f, 0.f);
        pl.info.ambient = glm::vec3(0.1f);
        pl.info.diffuse = glm::vec3(1.4f);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGui_ImplGlfw_InitForOpenGL(app.getWindow(), true);
        ImGui_ImplOpenGL3_Init("#version 330");

        constexpr fe::time simulationRate = fe::seconds(1.f / 60.f);
        constexpr fe::time maxDeltaTime = fe::seconds(3);
        fe::clock runClock;
        fe::time lastTime = runClock.getTime();
        fe::time accumulator;

        while (app.isOpen())
            {
                fe::time currentTime = runClock.getTime();
                fe::time deltaTime = currentTime - lastTime;
                if (deltaTime >= maxDeltaTime)
                    {
                        deltaTime = maxDeltaTime;
                    }
                lastTime = currentTime;
                accumulator += deltaTime;

                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();

                if (globals::g_inputs->keyState("debug", "close") == inputHandler::inputState::PRESS)
                    {
                        app.close();
                    }

                editor.update();

                while (accumulator >= simulationRate)
                    {
                        accumulator -= simulationRate;

                        physics.fixedUpdate(deltaTime.asSeconds());

                        editor.fixedUpdate(static_cast<float>(deltaTime.asSeconds()));
                    }

                physicsComponent &p = *static_cast<physicsComponent*>(ecsSphere.getComponent("physics"));
                graphicsComponent &g = *static_cast<graphicsComponent*>(ecsSphere.getComponent("graphics"));

                physx::PxTransform t = p.rigidBody.actor->getGlobalPose();
                g.renderObject.transform.position = {
                    t.p.x, t.p.y, t.p.z
                };

                editor.draw();

                ImGui::Render();

                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

                app.display();
                app.pollEvents();
            }

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        return 0;
    }