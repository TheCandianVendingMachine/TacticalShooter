// graphicsComponent.hpp
// A wrapper around a render object. Contains information pertaining to how its rendered
#pragma once
#include "component.hpp"
#include "graphics/renderObject.hpp"

struct graphicsComponent : public component
    {
        renderObject renderObject;

        virtual const char *name() const override final { return "graphics"; }
    };
