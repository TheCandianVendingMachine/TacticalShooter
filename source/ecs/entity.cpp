#include "entity.hpp"
#include "str.hpp"
#include "component.hpp"

component &entity::addComponent(component &component)
    {
        m_components.insert({ component.hashedName(), &component});
        component.entity = this;
        return component;
    }

component *entity::getComponent(const char *name) const
    {
        return getComponent(FE_STR(name));
    }

component *entity::getComponent(fe::str name) const
    {
        if (m_components.find(name) == m_components.end())
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
        if (m_components.find(name) != m_components.end())
            {
                m_components.erase(name);
            }
    }
