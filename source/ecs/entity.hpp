// entity.hpp
// A bag of components. Effectively used as a handle for a bunch of components
#pragma once
#include <robin_hood.h>
#include "typeDefines.hpp"

struct component;
class entity
    {
        private:
            robin_hood::unordered_map<fe::str, component*> m_components;

        public:
            component &addComponent(component &component);
            component *getComponent(const char *name) const;
            component *getComponent(fe::str name) const;

            void removeComponent(const char *name);
            void removeComponent(fe::str name);
    };
