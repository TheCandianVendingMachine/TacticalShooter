#include "component.hpp"

fe::str component::hashedName() const
    {
        return FE_STR(name());
    }
