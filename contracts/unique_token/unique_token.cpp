#include <cosiolib/contract.hpp>
#include <cosiolib/print.hpp>
#include <cosiolib/system.hpp>

using namespace std;

//
// unique token is a ERC721 like contract maintains 4 database tables.
// 1."tokens" table in which each row records a token id and its data.
// 2."idToOwner" table in which record token id to owner mapping
// 3."ownerCount" table in which record owner to token count mapping
// 4."stats" table, which contains a single row recording properties of the token.
//

/**
 * @brief record type of "tokens" table.
 */
struct token {
    uint64_t id;        ///< token unique id
    string data;        ///< token context data
    uint32_t expireBlocks;        ///< token will expired after expireBlocks

    COSIO_SERIALIZE(token, (id)(data)(expireBlocks))
};

/**
 * @brief record type of "idToOwner" table.
 */
struct idOwnerItem {
    uint64_t id;                ///< idOwnerItem unique id
    cosio::name tokenOwner;         ///< name of account who owns the idOwnerItem

    COSIO_SERIALIZE(idOwnerItem, (id)(tokenOwner))
};

/**
 * @brief record type of "ownerCount" table.
 */
struct ownerCountItem {
    cosio::name tokenOwner;         ///< name of account
    uint64_t count;             ///< token count of owner

    COSIO_SERIALIZE(ownerCountItem, (tokenOwner)(count))
};

/**
 * @brief record type of "stats" table.
 */
struct stat : public cosio::singleton_record {
    string name;                    ///< name of the token
    string symbol;                  ///< symbol name of the token
    uint64_t currentId;             ///< current token id
    uint64_t totalSupply;           ///< all token has been mint

    COSIO_SERIALIZE_DERIVED(stat, cosio::singleton_record, (name)(symbol)(currentId)(totalSupply))
};

/**
 * @brief the token contract class
 */
struct uniqueToken : public cosio::contract {
    using cosio::contract::contract;

    /**
     * @brief init contract table.
     * 
     * @param name          name of the token, e.g. "Native token of Contentos".
     * @param symbol        symbol name of the token, e.g. "COS".
     */
    void init(string name,string symbol) {
        auto owner = get_name();
        cosio::require_auth(owner.account());

        // create the stats record in database with default record member values.
        stats.get_or_create();
        // update the stats record
        stats.update([&](stat& s){
                s.name = name;
                s.symbol = symbol;
                s.totalSupply = 0;
                s.currentId = 0;
                });
    }

    /**
     * @brief contract method to create a erc721 token.
     * 
     * @param data          token's context data
     */
    void create(string data, uint32_t blocks) {
        // make sure that only the contract owner can create her token
        auto owner = get_name();
        cosio::require_auth(owner.account());

        // stats table must be created
        cosio::cosio_assert(stats.exists(), std::string("stats table not exist"));
        
        // mint new token
        auto statObj = stats.get();
        auto id = statObj.currentId;

        // update id -> token mapping
        idToOwner.insert([&](idOwnerItem& i){
            i.id = id;
            i.tokenOwner.set_string(owner.account());
        });
 
        // update owner's token count
        update_ownerCount(owner.account(),true);
        
        auto current_block = cosio::current_block_number();
        // add token to table
        tokens.insert([&](token& t){
                t.id = id;
                t.data = data;
                t.expireBlocks = current_block + blocks;
                });

        // update token id and totalSupply
        id++;
        stats.update([&](stat& s){
                s.currentId = id;
                s.totalSupply++;
                });
    }

    /**
     * @brief contract method to transfer tokens.
     *
     * @param from      the account who sends tokens.
     * @param to        the account who receives tokens.
     * @param id        token id.
     */
    void transfer(cosio::name from,cosio::name to, uint64_t id) {
        cosio::require_auth(from.account());
        // token id must has been exist
        cosio::cosio_assert(idToOwner.has(id), "id not exist");
        // token must belong to from user
        cosio::cosio_assert(idToOwner.get(id).tokenOwner == from, "id not belong to user");

        auto current_block = cosio::current_block_number();
        cosio::cosio_assert(tokens.get(id).expireBlocks < current_block, "token has expired");
        
        // transfer token's owner
        idToOwner.update(id,[&](idOwnerItem& i){
                i.tokenOwner.set_string(to.account());
                });
        
        // update origin owner's token count
        update_ownerCount(from,false);
        
        // update new owner's token count
        update_ownerCount(to,true);
    }

    void update_ownerCount(const cosio::name& user,bool add) {
        // update user's token count
        if (ownerCount.has(user)) {
            ownerCount.update(user,[&](ownerCountItem& o){
                    add ? o.count++:o.count--;
                    });
        } else {
            if(!add) cosio::cosio_assert(false, "can not decrease when count not exist");
            ownerCount.insert([&](ownerCountItem& o){
                    o.tokenOwner = user;
                    o.count = 1;
                    });
        }
    }

    // define a class member named "idToOwner" representing a database table which,
    // - has name of "idToOwner", the same as variable name,
    // - has a record type of idOwnerItem
    // - takes idOwnerItem::id as primary key
    COSIO_DEFINE_TABLE( idToOwner, idOwnerItem, (id) );

    // define a class member named "ownerCount" representing a database table which,
    // - has name of "ownerCount", the same as variable name,
    // - has a record type of ownerCountItem
    // - takes ownerCountItem::tokenOwner as primary key
    COSIO_DEFINE_TABLE( ownerCount, ownerCountItem, (tokenOwner) );

    // define a class member named "tokens" representing a database table which,
    // - has name of "tokens", the same as variable name,
    // - has a record type of token
    // - takes token::id as primary key
    COSIO_DEFINE_TABLE( tokens, token, (id) );

    // define a class data member named "stats" representing a singleton table which,
    // - has name of "stats"
    // - has a record type of stat
    COSIO_DEFINE_NAMED_SINGLETON( stats, "stats", stat );
};

// declare the class and methods of contract.
COSIO_ABI(uniqueToken, (init)(create)(transfer))
