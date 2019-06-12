#include <cosiolib/contract.hpp>
#include <cosiolib/print.hpp>
#include <cosiolib/system.hpp>

using namespace std;

// must be exact the same as defined in token contract source.
struct balance {
    cosio::name tokenOwner;         ///< name of account who owns the token
    uint64_t amount;                ///< balance of the account

    // specify the sequence of fields for serialization.
    COSIO_SERIALIZE(balance, (tokenOwner)(amount))
};

// must be exact the same as defined in token contract source.
struct stat : public cosio::singleton_record {
    string name;                    ///< name of the token
    string symbol;                  ///< symbol name of the token
    uint64_t total_supply;          ///< total number of tokens issued
    uint32_t decimals;              ///< number of digits after decimal point

    // specify the sequence of fields for serialization.
    COSIO_SERIALIZE_DERIVED(stat, cosio::singleton_record, (name)(symbol)(total_supply)(decimals))
};

#define TOKEN_OWNER_NAME      "initminer"
#define TOKEN_CONTRACT_NAME   "token"

struct tokenReader : public cosio::contract {
    using cosio::contract::contract;

    void read(string account) {
        stat s = stats.get_or_default();
        cosio::print_f("stats: name=%, symbol=%, total_supply=%, decimals=%\n", s.name, s.symbol, s.total_supply, s.decimals);

        balance b = balances.get(account);
        if (b.tokenOwner.account() != account) {
            cosio::print_f("balance of %: not found\n", account);
        } else {
            cosio::print_f("balance of %: %\n", account, b.amount);
        }
    }

    COSIO_DEFINE_TABLE_EX( balances, TOKEN_OWNER_NAME, TOKEN_CONTRACT_NAME, "balances", balance, (tokenOwner) );
    COSIO_DEFINE_SINGLETON_EX( stats, TOKEN_OWNER_NAME, TOKEN_CONTRACT_NAME, "stats", stat );
};

// declare the class and methods of contract.
COSIO_ABI(tokenReader, (read))
