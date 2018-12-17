#include "serialize.hpp"
#include "storage.hpp"
#include "contract.hpp"
#include <iostream>

struct foo {
    int x, y;
    std::string s;
    
    COSLIB_SERIALIZE( foo, (x)(y)(s) )
};

struct bar : public foo {
    float a, b;
    
    COSLIB_SERIALIZE_DERIVED(bar, foo, (a)(b))
};

class demo_contract : public cosio::contract {
public:
    demo_contract(const cosio::account_name& owner, const cosio::contract_name& name, const cosio::account_name& caller): cosio::contract(owner, name, caller) {
        
    }
    void add(int32_t x, int32_t y) {
        
    }
    void donate(cosio::account_name& whom, uint32_t value) {
        
    }
    int sub(int32_t x, int32_t y) {
        return 0;
    }
};
COSIO_ABI( demo_contract, (add)(donate) )

int main() {
    auto s = cosio::storage();
    
    s["alice"] = foo{10, 20, "hello, alice"};
    foo f = s["alice"];
    return 0;
}
