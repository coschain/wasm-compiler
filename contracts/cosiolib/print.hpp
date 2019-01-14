#pragma once

#include <cosiolib/system.h>
#include <cosiolib/types.hpp>
#include <utility>
#include <string>

namespace cosio {
    
    inline void prints_l(const char *s, size_t len) {
        ::print_str((char*)s, (int)len);
    }
    
    inline void print(const std::string& str) {
        prints_l(str.c_str(), str.size());
    }
    
    inline void print(const char *s) {
        print(std::string(s));
    }
    
    inline void print(std::string& str) {
        prints_l(str.c_str(), str.size());
    }
    
    inline void print(char c) {
        prints_l(&c, 1);
    }
    
    inline void print(uint8_t n) {
        ::print_uint(n);
    }
    
    inline void print(int16_t n) {
        ::print_int(n);
    }
    
    inline void print(uint16_t n) {
        ::print_uint(n);
    }
    
    inline void print(int32_t n) {
        ::print_int(n);
    }
    
    inline void print(uint32_t n) {
        ::print_uint(n);
    }
    
    inline void print(int64_t n) {
        ::print_int(n);
    }
    
    inline void print(uint64_t n) {
        ::print_uint(n);
    }
    
    template <typename = std::enable_if_t< !std::is_same<int, int64_t>::value
                                        && !std::is_same<int, int32_t>::value
                                        && !std::is_same<int, int16_t>::value
                                        && !std::is_same<int, int8_t>::value
    > >
    inline void print(int n) {
        ::print_int(n);
    }
    
    template <typename = std::enable_if_t< !std::is_same<unsigned int, uint64_t>::value
                                        && !std::is_same<unsigned int, uint32_t>::value
                                        && !std::is_same<unsigned int, uint16_t>::value
                                        && !std::is_same<unsigned int, uint8_t>::value
    > >
    inline void print(unsigned int n) {
        ::print_uint(n);
    }
    
    inline void print(bool b) {
        print(b? "true" : "false");
    }
    
    template <typename T, typename = decltype(std::declval<T>().string())>
    inline void print(T&& obj) {
        print(obj.string());
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
    
    template <typename Arg, typename...Args, typename = std::enable_if_t<0 != sizeof...(Args)> >
    inline void print(Arg&& val, Args&&...rest) {
        print(std::forward<Arg>(val));
        print(std::forward<Args>(rest)...);
    }
    
    inline void print(const bytes& data) {
        static const char *hexchars = "0123456789ABCDEF";
        print('<');
        for (auto it = data.cbegin(); it != data.cend(); it++) {
            uint8_t b = (uint8_t)*it;
            print( hexchars[b >> 4], hexchars[b & 0x0f] );
        }
        print('>');
    }
    
    template <typename Hash, typename = decltype(std::declval<Hash>().hash)>
    inline void print(const Hash& h) {
        static const char *hexchars = "0123456789ABCDEF";
        print('<');
        for (int i = 0; i < (int)sizeof(h.hash); i++) {
            uint8_t b = (uint8_t)h.hash[i];
            print( hexchars[b >> 4], hexchars[b & 0x0f] );
        }
        print('>');
    }

}
