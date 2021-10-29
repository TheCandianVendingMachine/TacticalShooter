#include "eventSource.hpp"

void eventSource::signal(message event) const
    {
        if (m_observers.find(event.event) != m_observers.end())
            {
                for (auto &observer : m_observers.at(event.event))
                    {
                        observer.callback(event);
                    }
            }
    }

int eventSource::subscribe(fe::str event, handler callback)
    {
        observer o;
        o.callback = callback;
        o.id = ++m_ids[event];

        m_observers[event].push_back(o);

        return o.id;
    }

void eventSource::unsubscribe(fe::str event, int id)
    {
        if (m_observers.find(event) != m_observers.end())
            {
                std::vector<observer> &observers = m_observers.at(event);
                observers.erase(std::remove_if(observers.begin(), observers.end(), [id] (const observer &test) { return test.id == id; }), observers.end());
            }
    }
