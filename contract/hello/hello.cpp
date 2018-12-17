#include "../cosiolib/contract.hpp"
#include "../cosiolib/print.hpp"

class hello : public cosio::contract {
public:
    using cosio::contract::contract;

    void hi( cosio::account_name user ) {
        cosio::print("Hello, ", user);
    }
};

COSIO_ABI(hello, (hi))
