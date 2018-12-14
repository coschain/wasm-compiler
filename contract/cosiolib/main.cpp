#include "serialize.hpp"
#include "datastream.hpp"
#include <iostream>

struct foo {
    int x, y;
    std::string s;
    
    COSLIB_SERIALIZE( foo, (x)(y)(s) )
};

int main() {
    foo f = {10, 20, "hello"};
    auto data = cosio::pack(f);
    auto f2 = cosio::unpack<foo>(data);
    
    std::cout << data.size() << "\n";
}
