#include "../cosiolib/contract.hpp"
#include "../cosiolib/print.hpp"

using timestamp_t = uint64_t;

// the database table record type
struct greeting {
    cosio::account_name name;
    uint32_t count;
    timestamp_t last_seen;
    
    // record type must support serialization.
    COSIO_SERIALIZE(greeting, (name)(count)(last_seen))
};

// the contract class
class hello : public cosio::contract {
public:
    using cosio::contract::contract;

    void hi( cosio::account_name user ) {
        // check if we have greeted this guy before
        if(!table_greetings.has(user)) {
            // first time meeting
            table_greetings.insert([&](greeting& r) {
                r.name = user;
                r.count = 1;
                r.last_seen = cosio::current_timestamp();
            });
        } else {
            // increase the greeting counter
            table_greetings.update(user, [&](greeting& r) {
                r.count++;
                r.last_seen = cosio::current_timestamp();
            });
        }
        // print something
        auto r = table_greetings.get(user);
        cosio::print("Hello %, we have met % times.", user, r.count);
    }
private:
    //
    // define a class data member named "table_greetings" representing the database table which,
    // - has name "table_greetings", the same as variable name
    // - has a record type of greeting
    // - takes greeting::name as primary key
    // - has 2 secondary indices taking greeting::count and greeting::last_seen as indexing fields respectively
    //
    COSIO_DEFINE_TABLE( table_greetings, greeting, (name)(count)(last_seen) );
    
    //
    // define a class data member named "table_hello" representing the database table which,
    // - has name "hello"
    // - has a record type of greeting
    // - takes greeting::name as primary key
    // - has no secondary indices
    //
    COSIO_DEFINE_NAMED_TABLE( table_hello, "hello", greeting, (name) );
    
};

// declare that this contract is named "hello", and has 1 method named "hi".
COSIO_ABI(hello, (hi))
