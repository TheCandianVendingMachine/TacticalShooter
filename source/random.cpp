#include "random.hpp"
#ifdef _WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

fe::randomImpl *fe::randomImpl::m_instance = nullptr;

void fe::randomImpl::startUp()
    {
        if (!m_instance)
            {
                m_instance = this;
            }
    }

void fe::randomImpl::seed(fe::uint64 seed)
    {
        stateA = seed;
    }

#pragma intrinsic(__rdtsc)
void fe::randomImpl::randomSeed()
    {
        seed(__rdtsc());
    }

fe::randomImpl &fe::randomImpl::get()
    {
        return *m_instance;
    }

fe::uint64 fe::randomImpl::generate(fe::uint64 min, fe::uint64 max)
    {
        return min + (generate() % (max - min + 1));
    }

fe::uint64 fe::randomImpl::generate()
    {
        fe::uint64 x = stateA;
        x ^= x << 13;
        x ^= x >> 7;
        x ^= x << 17;
        stateA = x;
        return stateA;
    }

fe::uint64 fe::random(fe::uint64 min, fe::uint64 max)
    {
        return fe::randomImpl::get().generate(min, max);
    }

fe::uint64 fe::random()
    {
        return fe::randomImpl::get().generate();
    }
