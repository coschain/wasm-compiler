#pragma once

#include "system.h"
#include "types.hpp"
#include <utility>
#include <string>

namespace cosio {
    
    inline void print(const char *s) {
        ::print_str((char*)s);
    }
    
    inline void prints_l(const char *s, size_t len) {
        ::print_str_l((char*)s, (int)len);
    }
    
    inline void print(const std::string& str) {
        prints_l(str.c_str(), str.size());
    }
    
    inline void print(std::string& str) {
        prints_l(str.c_str(), str.size());
    }
    
    inline void print(const char c) {
        prints_l(&c, 1);
    }
    
    inline void print(int32_t n) {
        ::print_int(n);
    }
    
    inline void print(int64_t n) {
        ::print_int(n);
    }
    
    inline void print(uint32_t n) {
        ::print_uint(n);
    }
    
    inline void print(uint64_t n) {
        ::print_uint(n);
    }
    
    inline void print(bool b) {
        print(b? "true" : "false");
    }
    
    template <typename T>
    inline void print(T&& obj) {
        obj.print();
    }
    
    inline void print_f(const char *s) {
        print(s);
    }
    
    template <typename Arg, typename...Args>
    inline void print_f(const char *s, Arg val, Args...rest) {
        while (*s) {
            if (*s == '%') {
                print(val);
                print_f(s + 1, rest...);
                return;
            }
            prints_l(s, 1);
            s++;
        }
    }
    
    template <typename Arg, typename...Args>
    inline void print(Arg&& val, Args&&...rest) {
        print(std::forward<Arg>(val));
        print(std::forward<Args>(rest)...);
    }

}
