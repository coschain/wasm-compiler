#pragma once

#include "system.h"
#include "types.hpp"

namespace cosio {
    
    inline uint64_t current_block_number() {
        return ::current_block_number();
    }
    
    inline uint64_t current_timestamp() {
        return ::current_timestamp();
    }
    
    inline account_name current_witness() {
        char buf[COSIO_MAX_ACCOUNT_NAME_SIZE + 1];
        memset(buf, 0, COSIO_MAX_ACCOUNT_NAME_SIZE + 1);
        ::current_witness(buf, COSIO_MAX_ACCOUNT_NAME_SIZE + 1);
        return account_name((const char*)buf);
    }
    
    inline checksum256 sha256(const bytes& data) {
        checksum256 checksum;
        ::sha256((char*)data.data(), (int)data.size(), (char*)checksum.hash, 32);
        return checksum;
    }
    
    inline void require_auth(const account_name& name) {
        return ::require_auth((char*)name.c_str(), (int)name.size());
    }
    
    inline coin_amount get_balance_by_name(const account_name& name) {
        return ::get_balance_by_name((char*)name.c_str(), (int)name.size());
    }
    
    inline void transfer(const account_name& to, coin_amount amount, const std::string& memo) {
        ::transfer((char*)to.c_str(), (int)to.size(), amount, (char*)memo.c_str(), (int)memo.size());
    }

}
