#include <cosiolib/contract.hpp>
#include <cosiolib/print.hpp>
#include <cosiolib/system.hpp>

using namespace std;

// the contract is just a demo for testing transfer

struct stat: public cosio::singleton_record {
    cosio::name admin;

    // specify the sequence of fields for serialization.
    COSIO_SERIALIZE_DERIVED(stat, cosio::singleton_record, (admin))
};

class mybet: public cosio::contract {
public:
    using cosio::contract::contract;

    void init(cosio::name admin) {
        cosio::require_auth(get_caller());
        stats.get_or_create();
        stats.update([&](stat& s){
            s.admin = admin;
        });
        cosio::print_f("create success with admin % \n", admin);
    }

    void bet(bool guess) {
        cosio::name contract_name = get_name();
        cosio::name caller = get_caller();
        cosio::coin_amount contract_balance = cosio::get_contract_balance(contract_name);
        cosio::coin_amount stake = cosio::get_contract_sender_value();
        cosio::cosio_assert(stats.exists(), "should init first");
        cosio::cosio_assert( contract_balance >= 2 * stake, std::string("contract balance not enough"));
        uint64_t block_number = cosio::current_block_number();
        if (block_number % 2 == 0 && guess) {
            cosio::transfer_to_user(caller, 2* stake, "");
            cosio::print_f("user % win % . \n", caller, stake);
            return;
        }
        if (block_number % 2 == 1 && !guess) {
            cosio::transfer_to_user(caller, 2 * stake, "");
            cosio::print_f("user % win % . \n", caller, stake);
            return;
        }
        cosio::print_f("user % loss\n", caller);
    }

    void withdraw() {
        cosio::cosio_assert(stats.exists(), "should init first");
        cosio::name caller = get_caller();
        cosio::cosio_assert(stats.get().admin == caller, "only admin can withdraw");
        auto contract_balance = cosio::get_contract_balance(get_name());
        cosio::transfer_to_user(caller, contract_balance, "");   
        cosio::print_f("user % withdraw % success. \n", caller, contract_balance);
    }

    void charge() {
        cosio::cosio_assert(stats.exists(), "should init first");
        cosio::name caller = get_caller();
        cosio::cosio_assert(stats.get().admin == caller, "only admin can charge"); 
        cosio::print_f("admin % charged % success. \n", caller, cosio::get_contract_sender_value());
        // cosio::print_f("haha: %", get_caller());
    }

    void balance() {
         auto contract_balance = cosio::get_contract_balance(get_name()); 
         cosio::print_f("contract % balance %. \n", get_name(), contract_balance);
    }

    COSIO_DEFINE_NAMED_SINGLETON(stats, "stat", stat);
};

// declare the class and methods of contract
COSIO_ABI(mybet, (init)(bet)(withdraw)(charge)(balance))