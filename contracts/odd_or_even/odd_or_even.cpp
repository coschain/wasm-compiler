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
    string hash1;
    string hash2;
    uint64_t primary1;
    uint64_t primary2;
    uint64_t timeout;
    COSIO_SERIALIZE_DERIVED(stat, cosio::singleton_record, (creator)(challenger)(stake)(creator_stake)(challenger_stake)(hash1)(hash2)(primary1)(primary2)(timeout))
};

class odd_or_even: public cosio::contract {
public:
    using cosio::contract::contract;

    void init(cosio::name creator, cosio::name challenger, cosio::coin_amount stake, uint64_t timeout) {
        cosio::require_auth(get_caller());
        cosio::cosio_assert(cosio::get_contract_sender_value() == 0, "do not send cos to the function");
        cosio::cosio_assert(!(creator == challenger), "creator and challenger should be different");
        stats.get_or_create();
        stats.update([&](stat& s){
            s.creator = creator;
            s.challenger = challenger;
            s.stake = stake;
            s.creator_stake = 0;
            s.challenger_stake = 0;
            s.hash1 = "";
            s.hash2 = "";
            s.primary1 = 0;
            s.primary2 = 0;
            s.timeout = cosio::current_block_number() + timeout;
        });
        cosio::print_f("bet contract init successful, finished after % seconds", timeout);
    }

    void balance() {
        cosio::cosio_assert(cosio::get_contract_sender_value() == 0, "do not send cos to the function");
        auto contract_balance = cosio::get_contract_balance(get_name()); 
        cosio::print_f("contract % balance %. \n", get_name(), contract_balance);
    } 

    void has_finished() {
        cosio::cosio_assert(stats.exists(), "should init first"); 
        cosio::cosio_assert(cosio::get_contract_sender_value() == 0, "do not send cos to the function");
        uint64_t current_block = cosio::current_block_number();
        auto db = stats.get();
        if (current_block > db.timeout) {
            cosio::print_f("the bet has finished.");
        } else {
            cosio::print_f("the bet is processing."); 
        }  
    }

    void set_hash(string hash) {
        cosio::cosio_assert(stats.exists(), "should init first"); 
        cosio::name caller = get_caller();
        auto db = stats.get();
        cosio::cosio_assert(db.creator == caller || db.challenger == caller, "only creator and challenger can set hash");
        cosio::cosio_assert(cosio::get_contract_sender_value() == db.stake, "should send stake to exec the function");
        uint64_t current_block = cosio::current_block_number();
        cosio::cosio_assert(current_block < db.timeout, "the bet has finished."); 
        if (db.creator == caller) {
            if (db.hash2.length() > 0) {
                cosio::cosio_assert(!(hash == db.hash2), "same hash with challenger."); 
            }
            stats.update([&](stat& s){
                s.hash1 = hash;
                s.creator_stake += db.stake;
            }); 
        }
        if (db.challenger == caller) {
            if (db.hash1.length() > 0) {
                cosio::cosio_assert(!(hash == db.hash1), "same hash with creator."); 
            }
            stats.update([&](stat& s){
                s.hash2 = hash;
                s.challenger_stake += db.stake;
            }); 
        } 
       cosio::print_f("% set hash success", caller);
    }

    void reveal_hash(uint64_t primary, string seed) {
        cosio::cosio_assert(stats.exists(), "should init first"); 
        cosio::cosio_assert(cosio::get_contract_sender_value() == 0, "do not send cos to the function");
        cosio::name caller = get_caller();
        auto db = stats.get();
        cosio::cosio_assert(db.creator == caller || db.challenger == caller, "only creator and challenger can reveal hash");
        cosio::cosio_assert(primary > 0, "primary should > 0");
        cosio::cosio_assert(seed.length() == 64, "the length of seed should exactly be 64");
        uint64_t current_block = cosio::current_block_number();
        cosio::cosio_assert(current_block < db.timeout, "the bet has finished."); 
        string o_str = to_string(primary) + seed;
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
        cosio::cosio_assert(db.primary1 > 0 && db.primary2 > 0, "both creator and challenger should have revealed its primary value."); 
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

    // only creator or challenger reveal revealed its primary values
    void claim() {
        cosio::cosio_assert(stats.exists(), "should init first"); 
        cosio::cosio_assert(cosio::get_contract_sender_value() == 0, "do not send cos to the function");
        auto db = stats.get();
        cosio::name caller = get_caller();
        cosio::cosio_assert(db.creator == caller || db.challenger == caller, "only creator or challenger can claim."); 
        uint64_t current_block = cosio::current_block_number();
        cosio::cosio_assert(current_block > db.timeout, "please waiting for the bet finishing."); 
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

    // both creator and challenger did not reveal their primary values.
    void liquidation() {
        cosio::cosio_assert(stats.exists(), "should init first"); 
        cosio::cosio_assert(cosio::get_contract_sender_value() == 0, "do not send cos to the function");
        auto db = stats.get();
        cosio::name caller = get_caller();
        cosio::cosio_assert(db.creator == caller || db.challenger == caller, "only creator or challenger can reveal hash"); 
        uint64_t current_block = cosio::current_block_number();
        cosio::cosio_assert(current_block > db.timeout, "please waiting for finish"); 
        cosio::cosio_assert(db.primary1 == 0 && db.primary2 == 0, "someone revealed his primary number, using claim insteads."); 
        cosio::transfer_to_user(caller, db.creator_stake, "");   
        cosio::transfer_to_user(caller, db.challenger_stake, "");
        stats.remove();
        cosio::print_f("% closed bet", caller);
    }

    COSIO_DEFINE_NAMED_SINGLETON(stats, "stat", stat);
};

// declare the class and methods of contract
COSIO_ABI(odd_or_even, (init)(balance)(has_finished)(set_hash)(reveal_hash)(bet)(claim)(liquidation))