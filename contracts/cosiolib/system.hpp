#pragma once

#include <cosiolib/system.h>
#include <cosiolib/types.hpp>
#include <cosiolib/assert.hpp>
#include <cosiolib/datastream.hpp>

namespace cosio {
    
    inline uint64_t current_block_number() {
        return ::current_block_number();
    }
    
    inline uint64_t current_timestamp() {
        return ::current_timestamp();
    }
    
    inline bytes _read_bytes( int(*reader)(char*,int) ) {
        bytes result;
        int min_size = reader(nullptr, 0);
        if (min_size > 0) {
            result.resize(min_size);
            reader((char*)result.data(), min_size);
        }
        return result;
    }
    
    inline std::string _read_string( int(*reader)(char*,int) ) {
        bytes b = _read_bytes(reader);
        return std::string(b.begin(), b.end());
    }
    
    inline std::string current_witness() {
        return _read_string(::current_witness);
    }
    
    inline checksum256 sha256(const bytes& data) {
        checksum256 checksum;
        ::sha256((char*)data.data(), (int)data.size(), (char*)checksum.hash, 32);
        return checksum;
    }

    inline name get_contract_name() {
        return name(_read_string(::read_contract_owner), _read_string(::read_contract_name));
    }
    
    inline bool is_contract_called_by_user() {
        return ::contract_called_by_user() != 0;
    }
    
    inline name get_contract_caller() {
        return is_contract_called_by_user()?
            name(_read_string(::read_contract_caller)) :
            name(_read_string(::read_calling_contract_owner), _read_string(::read_calling_contract_name));
    }
    
    inline std::string get_contract_method() {
        return _read_string(::read_contract_method);
    }
    
    inline bytes get_contract_args() {
        return _read_bytes(read_contract_op_params);
    }
    
    inline coin_amount get_contract_balance(const name& contract) {
        cosio_assert(contract.is_contract(), "invalid contract name: " + contract.string());
        std::string owner = contract.account();
        std::string name = contract.contract();
        return ::get_contract_balance((char*)name.c_str(), (int)name.size(), (char*)owner.c_str(), (int)owner.size());
    }
    
    inline coin_amount get_user_balance(const name& user) {
        cosio_assert(!user.is_contract(), "invalid account name: " + user.string());
        return ::get_user_balance((char*)user.string().c_str(), (int)user.string().size());
    }
    
    inline coin_amount get_balance(const name& who) {
        return who.is_contract()? get_contract_balance(who) : get_user_balance(who);
    }
    
    inline coin_amount get_contract_sender_value() {
        return ::read_contract_sender_value();
    }
    
    inline void require_auth(const name& who) {
        return who.is_contract()?
            cosio_assert( who == get_contract_name() || who == get_contract_caller(), "no authority of contract: " + who.string()):
            ::require_auth((char*)who.string().c_str(), (int)who.string().size());
    }

    inline void transfer_to_user(const name& to, coin_amount amount, const std::string& memo) {
        cosio_assert(!to.is_contract(), "invalid user name: " + to.string());
        ::transfer_to_user((char*)to.string().c_str(), (int)to.string().size(), amount, (char*)memo.c_str(), (int)memo.size());
    }
    
    inline void transfer_to_contract(const name& to, coin_amount amount, const std::string& memo) {
        cosio_assert(to.is_contract(), "invalid contract name: " + to.string());
        std::string owner = to.account();
        std::string name = to.contract();
        ::transfer_to_contract((char*)owner.c_str(), (int)owner.size(), (char*)name.c_str(), (int)name.size(), amount, (char*)memo.c_str(), (int)memo.size());
    }
    
    inline void transfer_to(const name& to, coin_amount amount, const std::string& memo) {
        return to.is_contract()? transfer_to_contract(to, amount, memo) : transfer_to_user(to, amount, memo);
    }
    
    inline void execute_contract(const name& contract, const std::string& method, const bytes& params, coin_amount coins) {
        cosio_assert(contract.is_contract(), "invalid contract name: " + contract.string());
        std::string owner = contract.account();
        std::string name = contract.contract();
        return ::contract_call(
                               (char*)owner.c_str(), (int)owner.size(),
                               (char*)name.c_str(), (int)name.size(),
                               (char*)method.c_str(), (int)method.size(),
                               (char*)params.data(), (int)params.size(),
                               coins );
    }
    
    template<typename...Args>
    static void execute_contract( const name& contract, const std::string& method, coin_amount coins, Args...args ) {
        return execute_contract(contract, method, pack(std::make_tuple(args...)), coins);
    }
    
}
