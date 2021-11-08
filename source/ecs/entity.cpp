#include "entity.hpp"
#include "str.hpp"
#include "component.hpp"
#include <spdlog/spdlog.h>

component &entity::addComponent(component &component)
    {
        if (hasComponent(component.hashedName()))
            {
#if _DEBUG
                spdlog::warn("Adding component to entity which already possesses component [Entity: {} Hash: {}, Real: {}]", name, component.hashedName(), fe::impl::g_debugStrings.strs.at(component.hashedName()));
#else
                spdlog::warn("Adding component to entity which already possesses component [Entity: {} Hash: {}]", name, component.hashedName());
#endif
            }

        m_components.insert({ component.hashedName(), &component});
        component.entity = this;
        return component;
    }

bool entity::hasComponent(const char *name) const
    {
        return hasComponent(FE_STR(name));
    }

bool entity::hasComponent(fe::str name) const
    {
        return m_components.find(name) != m_components.end();
    }

component *entity::getComponent(const char *name) const
    {
        return getComponent(FE_STR(name));
    }

component *entity::getComponent(fe::str name) const
    {
        if (!hasComponent(name))
            {
                return nullptr;
            }

        return m_components.at(name);
    }

void entity::removeComponent(const char *name)
    {
        removeComponent(FE_STR(name));
    }

void entity::removeComponent(fe::str name)
    {
        if (hasComponent(name))
            {
                m_components.erase(name);
            }
    }
