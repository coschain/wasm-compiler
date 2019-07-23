#pragma once

#include <string>
#include <vector>

/* macro to align/overalign a type to ensure calls to intrinsics with pointers/references are properly aligned */
#define ALIGNED(X) __attribute__ ((aligned (16))) X

namespace cosio {
    constexpr char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                           '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    using coin_amount = uint64_t;
    using bytes = std::vector<char>;

    std::string uint8_to_hex(uint8_t *data, int len)
    {
        std::string s(len * 2, ' ');
        for (int i = 0; i < len; ++i) {
            s[2 * i]     = hexmap[(data[i] & 0xF0) >> 4];
            s[2 * i + 1] = hexmap[data[i] & 0x0F];
        }
        return s;
    }
    
    struct ALIGNED(checksum256) {
        uint8_t hash[32];

        std::string to_string() {
            return uint8_to_hex(hash, 32);
        }
    };
    
    struct ALIGNED(checksum160) {
        uint8_t hash[20];

        std::string to_string() {
            return uint8_to_hex(hash, 20);
        }
    };
    
    struct ALIGNED(checksum512) {
        uint8_t hash[64];

        std::string to_string() {
            return uint8_to_hex(hash, 64);
        }
    };
    
    class name {
        constexpr static char COSIO_CONTRACT_NAME_PREFIX_CHAR = '$';
        constexpr static char COSIO_CONTRACT_NAME_SPLIT_CHAR = '@';
        
    public:
        name() { }
        
        name(const name& other) {
            set_name(other);
        }
    
        name(const std::string& s) {
            set_string(s);
        }
        
        name(const char* s) {
            set_string(s);
        }
        
        name(const std::string& owner, const std::string& contract) {
            set_contract(owner, contract);
        }
    
        void set_name(const name& other) {
            _name = other._name;
            _split = other._split;
        }
        
        void set_account(const std::string& account) {
            _name = account;
            _split = std::string::npos;
        }
        
        void set_contract(const std::string& owner, const std::string& contract) {
            _name = COSIO_CONTRACT_NAME_PREFIX_CHAR + contract + COSIO_CONTRACT_NAME_SPLIT_CHAR + owner;
            _split = 1 + contract.size();
        }
        
        void set_string(const std::string& s) {
            if (s.size() > 1 && s[0] == COSIO_CONTRACT_NAME_PREFIX_CHAR) {
                auto split = s.find(COSIO_CONTRACT_NAME_SPLIT_CHAR);
                if ( split != std::string::npos ) {
                    _name = s;
                    _split = split;
                    return;
                }
            }
            _name = s;
            _split = std::string::npos;
        }
        
        bool is_contract() const {
            return _split != std::string::npos
            && _name.size() > 1 + _split
            && _name[0] == COSIO_CONTRACT_NAME_PREFIX_CHAR
            && _name[_split] == COSIO_CONTRACT_NAME_SPLIT_CHAR;
        }
        
        std::string account() const {
            return is_contract()? _name.substr(_split + 1) : _name;
        }
        
        std::string contract() const {
            return is_contract()? _name.substr(1, _split - 1) : "";
        }
        
        const std::string& string() const {
            return _name;
        }
        
        operator const std::string() const {
            return _name;
        }
        
        name& operator = (const name& other) {
            set_name(other);
            return *this;
        }
        
        name& operator = (const std::string& s) {
            set_string(s);
            return *this;
        }
        
        bool operator == (const name& other) const {
            return _name == other._name;
        }
        
        template<typename DataStream>
        friend DataStream& operator << (DataStream& ds, const name& n) {
            return ds << n.string();
        }
        
        template<typename DataStream>
        friend DataStream& operator >> (DataStream& ds, name& n) {
            std::string s;
            ds >> s;
            n.set_string(s);
            return ds;
        }
        
    private:
        std::string _name;
        std::size_t _split = std::string::npos;
    };
    
}
