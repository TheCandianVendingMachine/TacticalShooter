// physxCPUDispatcher.hpp
// Takes PhysX tasks and executes them
#pragma once
#include <PxPhysicsAPI.h>

class physxCPUDispatcher : public physx::PxCpuDispatcher 
    {
        public:
            virtual void submitTask(physx::PxBaseTask &task) override final;
            virtual uint32_t getWorkerCount() const override final;
    };
