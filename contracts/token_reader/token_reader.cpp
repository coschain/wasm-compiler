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


// the contract
struct tokenReader : public cosio::contract {
    using cosio::contract::contract;

    void read(string account) {
        stat s = _stats.get_or_default();
        cosio::print_f("stats: name=%, symbol=%, total_supply=%, decimals=%\n", s.name, s.symbol, s.total_supply, s.decimals);

        if (_balances.has(account)) {
            cosio::print_f("balance of %: %\n", account, _balances.get(account).amount);
        } else {
            cosio::print_f("balance of %: not found\n", account);
        }
    }

    void read2(string owner, string contract, string account) {
        _unbound_balances.bind(owner, contract, "balances");
        _unbound_stats.bind(owner, contract, "stats");

        stat s = _unbound_stats.get_or_default();
        cosio::print_f("stats: name=%, symbol=%, total_supply=%, decimals=%\n", s.name, s.symbol, s.total_supply, s.decimals);

        if (_unbound_balances.has(account)) {
            cosio::print_f("balance of %: %\n", account, _unbound_balances.get(account).amount);
        } else {
            cosio::print_f("balance of %: not found\n", account);
        }
    }

    // define a member _balances bound to the "balances" table of a fixed contract "token@initminer"
    COSIO_DEFINE_TABLE_EX( _balances, "initminer", "token", "balances", balance, (tokenOwner) );

    // define a member _stats bound to the "stats" singleton table of a fixed contract "token@initminer"
    COSIO_DEFINE_SINGLETON_EX( _stats, "initminer", "token", "stats", stat );

    // define a member _unbound_balances which can be bound to any contract table with record type of balance.
    COSIO_UNBOUND_TABLE_EX(_unbound_balances, balance, (tokenOwner));

    // define a member _unbound_stats which can be bound to any contract singleton table with record type of stat.
    COSIO_UNBOUND_SINGLETON_EX(_unbound_stats, stat);
};

// declare the class and methods of contract.
COSIO_ABI(tokenReader, (read)(read2))
