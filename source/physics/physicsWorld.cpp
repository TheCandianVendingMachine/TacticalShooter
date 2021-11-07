#include "physicsWorld.hpp"
#include <spdlog/spdlog.h>

physx::PxFilterFlags SampleSubmarineFilterShader(
    physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
    physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
    physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
    {
        // let triggers through
        if(physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
            {
                pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
                return physx::PxFilterFlag::eDEFAULT;
            }
        // generate contacts for all that were not filtered above
        pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT | physx::PxPairFlag::eNOTIFY_CONTACT_POINTS;

        return physx::PxFilterFlag::eDEFAULT;
    }

physicsWorld::physicsWorld()
    {
        const_cast<physx::PxTolerancesScale&>(c_tolerances).length = c_length;
        const_cast<physx::PxTolerancesScale&>(c_tolerances).speed = c_gravity;

        m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_allocator, m_errorCallback);

        if (!m_foundation)
            {
                spdlog::error("PhysX Foundation creation failed");
                return;
            }

        m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, c_tolerances, true);
        if (!m_physics)
            {
                spdlog::error("Could not create physics");
                return;
            }

        physx::PxSceneDesc sceneDescription(m_physics->getTolerancesScale());
        sceneDescription.gravity = physx::PxVec3(0, c_gravity, 0.f);
        sceneDescription.broadPhaseType = physx::PxBroadPhaseType::eSAP;

        sceneDescription.cpuDispatcher = &m_dispatcher;
        sceneDescription.filterShader = SampleSubmarineFilterShader;

        m_scene = m_physics->createScene(sceneDescription);
        if (!m_scene)
            {
                spdlog::error("Could not create scene");
                return;
            }

        m_rigidBodies.reserve(sizeof(rigidBody) * c_expectedRigidBodies);
    }

physicsWorld::~physicsWorld()
    {
        m_scene->release();
        m_physics->release();
        m_foundation->release();
    }

void physicsWorld::fixedUpdate(double deltaTime)
    {
        m_scene->simulate(deltaTime);
        m_scene->fetchResults(true);
    }

rigidBody &physicsWorld::createBody(rigidType simulationType, rigidBody::types shapeType, physx::PxTransform transform)
    {
        rigidBody &body = *m_rigidBodies.emplace();
        body.material = m_physics->createMaterial(c_defaultStaticFriction, c_defaultDynamicFriction, c_defaultRestitution);

        switch (simulationType)
            {
                case rigidType::STATIC:
                    body.actor = m_physics->createRigidStatic(transform);
                    break;
                case rigidType::DYNAMIC:
                    body.actor = m_physics->createRigidDynamic(transform);
                    break;
                default:
                    break;
            }

        physx::PxShape *shape = nullptr;
        switch (shapeType)
            {
                case rigidBody::types::UNKNOWN:
                    spdlog::warn("Creating rigid body with unknown rigid type");
                    break;
                case rigidBody::types::PLANE:
                    shape = m_physics->createShape(physx::PxPlaneGeometry(), *body.material);
                    break;
                case rigidBody::types::SPHERE:
                    shape = m_physics->createShape(physx::PxSphereGeometry(1.f), *body.material);
                    break;
                case rigidBody::types::BOX:
                    shape = m_physics->createShape(physx::PxBoxGeometry(0.5f, 0.5f, 0.5f), *body.material);
                    break;
                default:
                    break;
            }

        if (shape) 
            {
                body.actor->attachShape(*shape);
            }

        return body;
    }

void physicsWorld::addToScene(rigidBody &body)
    {
        m_scene->addActor(*body.actor);
    }
