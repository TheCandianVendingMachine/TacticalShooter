#include "physxErrorCallback.hpp"
#include <spdlog/spdlog.h>

void physxErrorCallback::reportError(physx::PxErrorCode::Enum code, const char *message, const char *file, int line)
    {
        switch (code)
            {
                case physx::PxErrorCode::eNO_ERROR:
                    break;
                case physx::PxErrorCode::eDEBUG_INFO:
                    break;
                case physx::PxErrorCode::eDEBUG_WARNING:
                    spdlog::warn(message);
                    break;
                case physx::PxErrorCode::eINVALID_PARAMETER:
                    spdlog::warn(message);
                    break;
                case physx::PxErrorCode::eINVALID_OPERATION:
                    spdlog::warn(message);
                    break;
                case physx::PxErrorCode::eOUT_OF_MEMORY:
                    spdlog::error(message);
                    break;
                case physx::PxErrorCode::eINTERNAL_ERROR:
                    spdlog::error(message);
                    break;
                case physx::PxErrorCode::eABORT:
                    spdlog::error(message);
                    break;
                case physx::PxErrorCode::ePERF_WARNING:
                    spdlog::warn(message);
                    break;
                default:
                    break;
            }
    }
