#include "physxCPUDispatcher.hpp"

void physxCPUDispatcher::submitTask(physx::PxBaseTask &task)
    {
        task.run();
        task.release();
    }

uint32_t physxCPUDispatcher::getWorkerCount() const
    {
        return 0;
    }
