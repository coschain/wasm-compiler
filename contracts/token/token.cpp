#include <cosiolib/contract.hpp>
#include <cosiolib/print.hpp>
#include <cosiolib/system.hpp>

using namespace std;
/*
struct TokenInterface {
    virtual uint32_t totalSupply() = 0;
    virtual uint32_t banlanceOf(cosio::name tokenOwner) = 0;
    virtual void transfer(cosio::name from,cosio::name to, uint32_t amount) = 0;
};
*/

struct balance {
    cosio::name tokenOwner;
    uint32_t amount;
    COSIO_SERIALIZE(balance, (tokenOwner)(amount))
};

struct stat : public cosio::singleton_record {
    string name;
    string symbol;
    uint32_t total_supply;
    COSIO_SERIALIZE_DERIVED(stat, cosio::singleton_record, (name)(symbol)(total_supply))
};

struct cosToken : public cosio::contract {
    using cosio::contract::contract;

    void create(string name,string symbol, uint32_t total_supply) {
        cosio::require_auth(get_name().account());

        stats.get_or_create();
        stats.update([&](stat& s){
                s.name = name;
                s.symbol = symbol;
                s.total_supply = total_supply;
                });

        // give init supply to contract owner
        auto owner = get_name();
        cosio::print_f("create call 1 : name %, symbol %, total_supply %, owner % \n", name,symbol,total_supply,owner.account());
        balances.insert([&](balance& b){
            auto account_name = owner.account();
            b.tokenOwner.set_string(account_name);
            b.amount = total_supply;
        });

        auto b = balances.get(owner.account());
        cosio::print_f("create call 2 : user % have % amount. \n", b.tokenOwner.string(), b.amount);
    }

    void transfer(cosio::name from,cosio::name to, uint32_t amount) {
        cosio::require_auth(from);

        cosio::cosio_assert(balances.has(from), std::string("balance not exist:") + from.string());
        cosio::cosio_assert(balances.get(from).amount >= amount, std::string("balance not enough:") + from.string());
        cosio::cosio_assert(balances.get_or_default(to).amount + amount > balances.get_or_default(to).amount, std::string("over flow"));

        auto previousBalances = balances.get_or_default(from).amount + balances.get_or_default(to).amount;

        balances.update(from,[&](balance& b){
                    b.amount -= amount;
                    });
        if(!balances.has(to)) {
            balances.insert([&](balance& b){
                        b.tokenOwner = to;
                        b.amount = amount;
                    });
        } else {
            balances.update(to,[&](balance& b){
                       b.amount += amount;
                    });
        }

        cosio::cosio_assert(balances.get(from).amount + balances.get(to).amount == previousBalances, std::string("balance not equal after transfer"));
    }

    COSIO_DEFINE_TABLE( balances, balance, (tokenOwner) );
    COSIO_DEFINE_NAMED_SINGLETON( stats, "stats", stat );
};

COSIO_ABI(cosToken, (create)(transfer))
