#include <cosiolib/contract.hpp>
#include <cosiolib/print.hpp>
#include <cosiolib/system.hpp>

using namespace std;

// the contract is just a demo for testing transfer

struct stat: public cosio::singleton_record {
    cosio::name creator;
    cosio::name challenger;
    cosio::coin_amount stake;
    cosio::coin_amount creator_stake;
    cosio::coin_amount challenger_stake; 
    string seed;
    string hash1;
    string hash2;
    uint64_t primary1;
    uint64_t primary2;
    uint64_t timeout;
    COSIO_SERIALIZE_DERIVED(stat, cosio::singleton_record, (creator)(challenger)(stake)(creator_stake)(challenger_stake)(seed)(hash1)(hash2)(primary1)(primary2)(timeout))
};

class odd_or_even: public cosio::contract {
public:
    using cosio::contract::contract;

    void init(cosio::name creator, cosio::name challenger, cosio::coin_amount stake, uint64_t timeout) {
        cosio::require_auth(get_caller());
        cosio::cosio_assert(cosio::get_contract_sender_value() == 0, "do not send cos to the function");
        stats.get_or_create();
        stats.update([&](stat& s){
            s.creator = creator;
            s.challenger = challenger;
            s.stake = stake;
            s.creator_stake = 0;
            s.challenger_stake = 0;
            s.seed = to_string(cosio::current_block_number());
            s.hash1 = "";
            s.hash2 = "";
            s.primary1 = 0;
            s.primary2 = 0;
            s.timeout = cosio::current_block_number() + timeout;
        });
        cosio::print_f("bet contract init successful, seed is %", stats.get().seed);
    }

    void stake() {
        cosio::cosio_assert(stats.exists(), "should init first"); 
        cosio::cosio_assert(cosio::get_contract_sender_value() == 0, "do not send cos to the function");
        auto db = stats.get();
        cosio::print_f("the stake is %", db.stake);
    }

    void seed() {
        cosio::cosio_assert(stats.exists(), "should init first"); 
        cosio::cosio_assert(cosio::get_contract_sender_value() == 0, "do not send cos to the function");
        auto db = stats.get();
        cosio::print_f("the seed is %", db.seed); 
    }

    void sha256(uint64_t primary) {
        cosio::cosio_assert(stats.exists(), "should init first");
        auto db = stats.get();
        cosio::name caller = get_caller();
        cosio::cosio_assert(db.creator == caller || db.challenger == caller, "only creator and challenger can set hash");
        cosio::cosio_assert(primary > 0, "primary should > 0");
        string o_str = to_string(primary) + db.seed;
        std::vector<char> data(o_str.begin(), o_str.end()); 
        cosio::checksum256 checksum = cosio::sha256(data);
        auto hash = checksum.to_string();
        cosio::print_f("the hash result is %", hash); 
    }

    void remain_seconds() {
        cosio::cosio_assert(stats.exists(), "should init first"); 
        cosio::cosio_assert(cosio::get_contract_sender_value() == 0, "do not send cos to the function");
        auto db = stats.get();
        uint64_t current_block = cosio::current_block_number();
        if (current_block > db.timeout) {
           cosio::print_f("the bet has finished, you can withdraw balance");
        } else {
            cosio::print_f("remains % seconds", db.timeout - current_block);
        }
    }

    void balance() {
        cosio::cosio_assert(cosio::get_contract_sender_value() == 0, "do not send cos to the function");
        auto contract_balance = cosio::get_contract_balance(get_name()); 
        cosio::print_f("contract % balance %. \n", get_name(), contract_balance);
    } 

    void who_set_hashed() {
        cosio::cosio_assert(stats.exists(), "should init first"); 
        cosio::cosio_assert(cosio::get_contract_sender_value() == 0, "do not send cos to the function");
        auto db = stats.get();
        if (db.hash1.length() > 0 && db.hash2.length() > 0){
            cosio::print_f("both creator and challenger have set hashed.");
        } else if (db.hash1.length() > 0) {
            cosio::print_f("creator has set hashed.");
        } else {
            cosio::print_f("chanllenger has set hashed.");
        }
    }

    void who_revealed() {
        cosio::cosio_assert(stats.exists(), "should init first"); 
        cosio::cosio_assert(cosio::get_contract_sender_value() == 0, "do not send cos to the function");
        auto stat = stats.get();
        if (stat.primary1 > 0 && stat.primary2 > 0){
            cosio::print_f("both creator and challenger have revealed primary values.");
        } else if (stat.primary1 > 0) {
            cosio::print_f("creator has revealed his primary value.");
        } else {
            cosio::print_f("chanllenger has revealed his primary value.");
        }
    }

    void set_hash(string hash) {
        cosio::cosio_assert(stats.exists(), "should init first"); 
        cosio::name caller = get_caller();
        auto db = stats.get();
        cosio::cosio_assert(db.creator == caller || db.challenger == caller, "only creator and challenger can set hash");
        cosio::cosio_assert(cosio::get_contract_sender_value() == db.stake, "should send stake to exec the function");
        if (db.creator == caller) {
            stats.update([&](stat& s){
                s.hash1 = hash;
                s.creator_stake = db.stake;
            }); 
        }
        if (db.challenger == caller) {
            stats.update([&](stat& s){
                s.hash2 = hash;
                s.challenger_stake = db.stake;
            }); 
        } 
       cosio::print_f("% set hash success", caller);
    }

    void reveal_hash(uint64_t primary) {
        cosio::cosio_assert(stats.exists(), "should init first"); 
        cosio::cosio_assert(cosio::get_contract_sender_value() == 0, "do not send cos to the function");
        cosio::name caller = get_caller();
        auto db = stats.get();
        cosio::cosio_assert(db.creator == caller || db.challenger == caller, "only creator and challenger can reveal hash");
        cosio::cosio_assert(primary > 0, "primary should > 0");
        string o_str = to_string(primary) + db.seed;
        std::vector<char> data(o_str.begin(), o_str.end());
        if (db.creator == caller) {
            cosio::cosio_assert(db.hash1.length() > 0, "should set hash1 first");
            cosio::checksum256 checksum = cosio::sha256(data);
            auto hash = checksum.to_string();
            if (hash == db.hash1) {
                stats.update([&](stat& s){
                s.primary1 = primary;
            });
          }
        }
        if (db.challenger == caller) {
            cosio::cosio_assert(db.hash2.length() > 0, "should set hash2 first");
            cosio::checksum256 checksum = cosio::sha256(data);
            auto hash = checksum.to_string();
            if (hash == db.hash2) {
                stats.update([&](stat& s){
                s.primary2 = primary;
            });
          }
        }
        cosio::print_f("% revealed hash", caller);
    }

    void bet() {
        cosio::cosio_assert(stats.exists(), "should init first"); 
        cosio::cosio_assert(cosio::get_contract_sender_value() == 0, "do not send cos to the function");
        auto db = stats.get();
        cosio::name caller = get_caller(); 
        cosio::cosio_assert(db.creator == caller || db.challenger == caller, "only creator and challenger can reveal hash");
        cosio::cosio_assert(db.primary1 > 0 && db.primary2 > 0, "both creator and challenger should have revealed it's primary value."); 
        auto sum = db.primary1 + db.primary2;
        if (sum % 2 == 1) {
        // odd
            cosio::transfer_to_user(db.creator, cosio::get_contract_balance(get_name()), "");  
            cosio::print_f("result is %, it's odd, creator win!", sum);
        } else {
        // even
            cosio::transfer_to_user(db.challenger, cosio::get_contract_balance(get_name()), "");  
            cosio::print_f("result is %, it's even, challenger win!", sum);
        }
        stats.remove();
    }

    void withdraw() {
        cosio::cosio_assert(stats.exists(), "should init first"); 
        cosio::cosio_assert(cosio::get_contract_sender_value() == 0, "do not send cos to the function");
        auto db = stats.get();
        cosio::name caller = get_caller();
        cosio::cosio_assert(db.creator == caller || db.challenger == caller, "only creator and challenger can reveal hash"); 
        uint64_t current_block = cosio::current_block_number();
        cosio::cosio_assert(current_block > db.timeout, "please waiting for finish"); 
        cosio::cosio_assert((db.primary1 == 0 && db.primary2 > 0) || (db.primary1 > 0 && db.primary2 == 0), "need one revealed his value and the other didn't."); 
        if(caller == db.creator && db.primary1 > 0) {
            cosio::transfer_to_user(caller, cosio::get_contract_balance(get_name()), ""); 
            cosio::print_f("% withdrawed success", caller);
            stats.remove();
            return;
        }
        if(caller == db.challenger && db.primary2 > 0) {
            cosio::transfer_to_user(caller, cosio::get_contract_balance(get_name()), ""); 
            cosio::print_f("% withdrawed success", caller);
            stats.remove();
            return;
        }
        cosio::print_f("% withdrawed failed", caller);
    }

    void close() {
        cosio::cosio_assert(stats.exists(), "should init first"); 
        cosio::cosio_assert(cosio::get_contract_sender_value() == 0, "do not send cos to the function");
        auto db = stats.get();
        cosio::name caller = get_caller();
        cosio::cosio_assert(db.creator == caller || db.challenger == caller, "only creator or challenger can reveal hash"); 
        uint64_t current_block = cosio::current_block_number();
        cosio::cosio_assert(current_block > db.timeout, "please waiting for finish"); 
        cosio::cosio_assert(db.primary1 == 0 && db.primary2 == 0, "someone revealed his primary number, waiting withdraw."); 
        if (caller == db.creator) {
            cosio::transfer_to_user(caller, db.creator_stake, "");   
        }
        if (caller == db.challenger) {
            cosio::transfer_to_user(caller, db.challenger_stake, "");    
        }
        stats.remove();
        cosio::print_f("% closed bet", caller);
    }

    COSIO_DEFINE_NAMED_SINGLETON(stats, "stat", stat);
};

// declare the class and methods of contract
COSIO_ABI(odd_or_even, (init)(stake)(seed)(sha256)(remain_seconds)(balance)(who_set_hashed)(who_revealed)(set_hash)(reveal_hash)(bet)(withdraw)(close))