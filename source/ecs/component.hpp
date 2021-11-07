// component.hpp
// base class for all components. Has ability for components to talk to eachother
#pragma once
#include "typeDefines.hpp"
#include "str.hpp"

class entity;
struct component
    {
        entity *entity = nullptr;

        virtual const char *name() const = 0;
        fe::str hashedName() const;
    };

