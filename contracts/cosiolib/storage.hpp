#pragma once

#include <cosiolib/types.hpp>
#include <cosiolib/system.h>
#include <cosiolib/datastream.hpp>
#include <string>

namespace cosio {
    
    inline void storage_put(const std::string& key, const bytes& value) {
        ::save_to_storage((char*)key.c_str(), (int32_t)key.size(), (char*)value.data(), (int)value.size());
    }
    
    inline void storage_get(const std::string& key, bytes& value) {
        int size = ::read_from_storage((char*)key.c_str(), (int32_t)key.size(), nullptr, 0);
        value.clear();
        if (size > 0) {
            value.reserve(size);
            ::read_from_storage((char*)key.c_str(), (int32_t)key.size(), (char*)value.data(), size);
        }
    }
    
    class storage_value {
    public:
        template<typename T>
        void put(const T& obj) {
            storage_put(_key, pack(obj));
        }
        
        template<typename T>
        void operator = (const T& obj) {
            this->put(obj);
        }
        
        template<typename T>
        T get() {
            bytes buffer;
            storage_get(_key, buffer);
            return unpack<T>(buffer);
        }
        
        template<typename T>
        operator T() {
            return this->get<T>();
        }
        
        static storage_value value_of(const std::string& key) {
            return storage_value(key);
        }

    private:
        storage_value() {}
        storage_value(const std::string& key): _key(key) {}
    
    private:
        std::string _key;
    };
    
    inline storage_value value_of(const std::string& key) {
        return storage_value::value_of(key);
    }
    
    class storage {
    public:
        storage_value value_of(const std::string& key) {
            return cosio::value_of(key);
        }
        storage_value operator[](const std::string& key) {
            return this->value_of(key);
        }
    };
    
}
