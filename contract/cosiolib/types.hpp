#pragma once

#include <string>
#include <vector>

/* macro to align/overalign a type to ensure calls to intrinsics with pointers/references are properly aligned */
#define ALIGNED(X) __attribute__ ((aligned (16))) X

namespace cosio {
    using account_name = std::string;
    using method_name = std::string;
    
    struct ALIGNED(checksum256) {
        uint8_t hash[32];
    };
    
    struct ALIGNED(checksum160) {
        uint8_t hash[20];
    };
    
    struct ALIGNED(checksum512) {
        uint8_t hash[64];
    };
    
    using bytes = std::vector<char>;
}

#define COSIO_MAX_ACCOUNT_NAME_SIZE     16
#define COSIO_MAX_METHOD_NAME_SIZE      128
