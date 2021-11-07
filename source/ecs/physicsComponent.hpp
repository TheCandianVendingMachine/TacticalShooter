// physicsComponent.hpp
// Allows an entity to be affected by physics
#pragma once
#include "component.hpp"
#include "physics/rigidBody.hpp"

struct physicsComponent : public component
    {
        rigidBody &rigidBody;

        physicsComponent(::rigidBody &body) : rigidBody(body) {}
        virtual const char *name() const override final { return "physics"; }
    };
