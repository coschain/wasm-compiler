#include <cstddef>
#include <algorithm>
#include <functional>
#include <cosiolib/contract.hpp>

//
// This contract implements management functionalities for Soulbound Tokens.
// It should be deployed once, and everyone can then issue, mint and burn their Soulbound Tokens by simply calling the contract.
//

// burn auths
#define BURN_AUTH_ISSUER_ONLY               "issuer"
#define BURN_AUTH_OWNER_ONLY                "owner"
#define BURN_AUTH_ISSUER_AND_OWNER          "issuer_owner"
#define BURN_AUTH_NONE                      "none"


// global config & stats
struct global_record :public cosio::singleton_record {   
    bool enabled = 0;                             // true: the contract is enabled, false: disabled
    cosio::coin_amount issue_fee = 0;             // the fee of a new token issue
    cosio::coin_amount mint_fee = 0;              // the fee of minting a single token
    cosio::coin_amount burn_fee = 0;              // the fee of burning a single token
    uint64_t issued_count = 0;                    // total number of token families issued
    uint64_t minted_count = 0;                    // total number of minted tokens of all families
    uint64_t burned_count = 0;                    // total number of burned tokens of all families
    
    COSIO_SERIALIZE_DERIVED( global_record, cosio::singleton_record, (enabled)(issue_fee)(mint_fee)(burn_fee)(issued_count)(minted_count)(burned_count) )
};

// a token family
struct token_record {
    std::string symbol;                 // the family symbol name, i.e. the family id, which must consist of upper-cased letters or digits with length between 3 and 8.
    std::string desc;                   // the description, which is 128 bytes long at most.
    std::string uri;                    // the uri of the family, which is 128 bytes long at most. people should be able to get token metas using the uri and the integer token id.
    uint64_t minted_count = 0;          // total number of minted tokens
    uint64_t burned_count = 0;          // total number of burned tokens
    cosio::name issuer;                 // account name of the issuer
    uint64_t issued_at = 0;             // the issue timestamp

    COSIO_SERIALIZE( token_record, (symbol)(desc)(uri)(minted_count)(burned_count)(issuer)(issued_at) )
};

// a token holding
struct holding_record {
    std::string global_id;              // a global token id = <symbol>:<token>, where <symbol> is the family symbol and <token> is the id of the token inside its family.
    std::string symbol;                 // the token family symbol
    std::string token;                  // the token id inside its family, which must consist of upper-cased letters or digits with max length of 128.
    cosio::name owner;                  // account name of the token owner
    std::string burn_auth;              // the burning auth of the token, must be one of BURN_AUTH_ISSUER_ONLY, BURN_AUTH_OWNER_ONLY, BURN_AUTH_ISSUER_AND_OWNER or BURN_AUTH_NONE.

    COSIO_SERIALIZE( holding_record, (global_id)(symbol)(token)(owner)(burn_auth) )
};


// the contract
class cossbt: public cosio::contract {
public:
    using cosio::contract::contract;

    /**
     * @brief Enable or disable the contract.
     * 
     * Only the contract owner can call this method. Once the contract is disabled, subsequent calls to 
     * issue(), mint() and burn() will be refused.
     * 
     * @param b true: enable; false: disable
     */
    void enable(bool b) {
        cosio::require_auth(get_name().account());
        global.get_or_create();
        global.update([&](global_record& g){
            g.enabled = b? 1 : 0; 
        });
    }

    /**
     * @brief Set the fees for operations.
     * 
     * Only the contract owner can call this method. The fees are zeros by default. After non-zero values are
     * set by the contract owner, calls to issue(), mint() and burn() must carry enough COS for the fees.
     * Note that a caller will not receive changes if he/she pays more COS than the required fee.
     * 
     * @param issue_fee minimal fee to issue a new token family
     * @param mint_fee minimal fee to mint a new token
     * @param burn_fee minimal fee to burn a token
     */
    void set_fee(cosio::coin_amount issue_fee, cosio::coin_amount mint_fee, cosio::coin_amount burn_fee) {
        cosio::require_auth(get_name().account());
        global.get_or_create();
        global.update([&](global_record& g){
            g.issue_fee = issue_fee;
            g.mint_fee = mint_fee;
            g.burn_fee = burn_fee;
        });
    }

    /**
     * @brief Withdraw the contract balance.
     * 
     * Only the contract owner can call this method. This is the only way to withdraw contract balance (received fees).
     * 
     * @param receiver the receiver account
     * @param amount amount of COS to withdraw
     * @param memo the memo
     */
    void withdraw_fee(const cosio::name& receiver, cosio::coin_amount amount, const std::string& memo) {
        cosio::require_auth(get_name().account());
        cosio::cosio_assert(!receiver.is_contract(), "non-contract receiver required");
        cosio::transfer_to(receiver, amount, memo);
    }

    /**
     * @brief Issue a new token family.
     * 
     * @param symbol the symbol of the family. a symbol must consist of 3 - 8 upper-case letters or digits
     * @param desc a brief description of the family within 128 bytes
     * @param uri a URI of the family with 128 bytes
     */
    void issue(const std::string& symbol, const std::string& desc, const std::string& uri) {
        check_enabled_and_fee([](global_record& g){ return g.issue_fee; });
        
        // validate the symbol
        check_string(symbol, 3, 8, "invalid symbol name");

        // validate the description and uri
        cosio::cosio_assert(desc.size() <= 128 , "description too long");
        cosio::cosio_assert(uri.size() <= 128 , "uri too long");
        // duplicate check
        cosio::cosio_assert(!tokens.has(symbol) , "symbol already exists");

        // add a new token family
        tokens.insert([&](token_record& r) {
            r.symbol = symbol;
            r.desc = desc;
            r.uri = uri;
            r.issuer = cosio::get_contract_caller();
            r.issued_at = cosio::current_timestamp();
            r.minted_count = 0;
            r.burned_count = 0;
        });
        // update the global stats
        global.update([&](global_record& g){
            g.issued_count++;
        });
    }

    /**
     * @brief Mint a new token.
     * 
     * Only the token family issuer can call this method to mint new tokens inside the family.
     * 
     * @param symbol symbol of the family to which the new token belongs
     * @param token_id the id of minted token.
     * @param burn_auth the burn auth of the minted token, must be one of BURN_AUTH_ISSUER_ONLY, BURN_AUTH_OWNER_ONLY, BURN_AUTH_ISSUER_AND_OWNER or BURN_AUTH_NONE.
     */
    void mint(const std::string& symbol, const std::string& token_id, const std::string& burn_auth, const cosio::name& receiver) {
        check_enabled_and_fee([](global_record& g){ return g.mint_fee; });

        // validate the token id
        check_string(token_id, 1, 128, "invalid token id");

        // validate the token symbol 
        cosio::cosio_assert(tokens.has(symbol) , "symbol not found");

        // the caller must be the issuer
        auto t = tokens.get(symbol);
        cosio::require_auth(t.issuer);

        // check if the token is already minted
        auto gid = global_token_id(symbol, token_id);
        cosio::cosio_assert(!holdings.has(gid) , "token already minted");

        // check burn auth string
        check_burn_auth(burn_auth);

        // check receiver existence
        cosio::get_balance(receiver);

        // create the token, and set the receiver as its owner
        holdings.insert([&](holding_record& r) {
            r.global_id = gid;
            r.symbol = symbol;
            r.token = token_id;
            r.owner = receiver;
            r.burn_auth = burn_auth;
        });
        tokens.update(symbol, [&](token_record& r){
            r.minted_count++;
        });
        global.update([&](global_record& g){
            g.minted_count++;
        });
    }

    /**
     * @brief Burn a token.
     * 
     * Only the token owner and/or the token issuer can possibly burn an issued token depending on the token's burn_auth setting.
     * Note that when a token got burned, it's lost forever. There's no way to get it back.
     * 
     * @param symbol the token family symbol
     * @param token_id the token id to be burned.
     */
    void burn(const std::string& symbol, const std::string& token_id) {
        check_enabled_and_fee([](global_record& g){ return g.burn_fee; });

        // check the existence of the token
        auto gid = global_token_id(symbol, token_id);
        cosio::cosio_assert(holdings.has(gid) , "token not found");

        // check burn auth
        auto t = tokens.get(symbol);
        auto h = holdings.get(gid);

        cosio::cosio_assert(h.burn_auth != BURN_AUTH_NONE , "the token is not burnable");

        if (h.burn_auth == BURN_AUTH_ISSUER_ONLY) {
            cosio::require_auth(t.issuer);
        } else if (h.burn_auth == BURN_AUTH_OWNER_ONLY) {
            cosio::require_auth(h.owner);
        } else if (h.burn_auth == BURN_AUTH_ISSUER_AND_OWNER) {
            auto caller = cosio::get_contract_caller();
            cosio::cosio_assert(caller == t.issuer || caller == h.owner, "the caller is not authorized to burn the token");
        }

        // remove the token
        tokens.update(h.symbol, [&](token_record& r){
            r.burned_count++;
        });
        global.update([&](global_record& g){
            g.burned_count++;
        });
        holdings.remove(gid);
    }


private:
    // check if the service is enabled and if the carried COS tokens are enough to pay the fee.
    void check_enabled_and_fee(std::function<uint64_t(global_record&)> get_fee_func) {
        auto g = global.get_or_default();
        cosio::cosio_assert(g.enabled, "contract disabled");
        cosio::cosio_assert(cosio::get_contract_sender_value() >= get_fee_func(g), "inadequate fee");
    }

    // check if the given string contains allowed chars and its length is valid.
    void check_string(const std::string& s, int min_size, int max_size, const std::string& msg) {
        cosio::cosio_assert(
            s.size() >= min_size && s.size() <= max_size && std::all_of(s.begin(), s.end(), [](char c) { 
                return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z');
            }),
            msg
        );
    }

    // return a global unique id based on token family symbol and token id.
    std::string global_token_id(const std::string& symbol, const std::string& token_id) {
        return symbol + ":" + token_id;
    }

    // check if the burn auth string is permitted.
    void check_burn_auth(const std::string& burn_auth) {
        cosio::cosio_assert(
            burn_auth == BURN_AUTH_ISSUER_ONLY || burn_auth == BURN_AUTH_OWNER_ONLY || burn_auth == BURN_AUTH_ISSUER_AND_OWNER || burn_auth == BURN_AUTH_NONE,
            "unknown burn auth: " + burn_auth
        );
    }
    
private:
    COSIO_DEFINE_NAMED_SINGLETON( global, "global", global_record );
    COSIO_DEFINE_TABLE( tokens, token_record, (symbol)(issuer) );
    COSIO_DEFINE_TABLE( holdings, holding_record, (global_id)(owner) );
};

// contract methods declaration
COSIO_ABI( cossbt, (enable)(set_fee)(withdraw_fee)(issue)(mint)(burn) )
