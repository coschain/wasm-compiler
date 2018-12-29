#pragma once

#include <cosiolib/system.h>
#include <string>

namespace cosio {
    inline void cosio_assert(bool pred, const std::string& what) {
        cos_assert(pred, (char*)what.c_str(), (int)what.size());
    }
}
