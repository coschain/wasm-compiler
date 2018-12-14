#include "serialize.hpp"
#include "storage.hpp"
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

int main() {
    auto s = cosio::storage();
    
    s["alice"] = foo{10, 20, "hello, alice"};
    foo f = s["alice"];
    return 0;
}
