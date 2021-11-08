// entity.hpp
// A bag of components. Effectively used as a handle for a bunch of components
#pragma once
#include <robin_hood.h>
#include <string>
#include "typeDefines.hpp"

struct component;
class entity
    {
        private:
            robin_hood::unordered_map<fe::str, component*> m_components;

        public:
            std::string name = "";

            component &addComponent(component &component);

            bool hasComponent(const char *name) const;
            bool hasComponent(fe::str name) const;

            component *getComponent(const char *name) const;
            component *getComponent(fe::str name) const;

            void removeComponent(const char *name);
            void removeComponent(fe::str name);

    };
