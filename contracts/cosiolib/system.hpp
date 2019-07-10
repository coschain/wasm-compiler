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
    
    inline std::vector<std::string> block_producers() {
        std::vector<std::string> result;
        std::string::size_type prev_pos = 0, pos = 0;
        std::string names = _read_string(::get_block_producers);
        while((pos = names.find(' ', pos)) != std::string::npos) {
            std::string substring( names.substr(prev_pos, pos-prev_pos) );
            result.push_back(substring);
            prev_pos = ++pos;
        }
        if (pos != std::string::npos && pos > prev_pos) {
            result.push_back(names.substr(prev_pos, pos-prev_pos));
        }
        return result;
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

    inline std::string reputation_admin() {
        return _read_string(::get_reputation_admin);
    }

    inline void reputation_admin(const std::string& name) {
        ::set_reputation_admin((char *)name.c_str(), (int)name.size());
    }

    inline void update_reputations(const std::vector<std::string>& names, const std::vector<int32_t>& reputations, const std::vector<std::string>& memos) {
        size_t count = names.size();
        cosio_assert(count == reputations.size() && count == memos.size(), "illegal parameters");

        std::vector<char*> name;
        std::vector<int> name_len;
        std::vector<char*> memo;
        std::vector<int> memo_len;
        std::vector<int> rep;
        for (size_t i = 0; i < count; i++) {
            name.push_back((char*)names[i].c_str());
            name_len.push_back((int)names[i].size());
            memo.push_back((char*)memos[i].c_str());
            memo_len.push_back((int)memos[i].size());
            rep.push_back((int)reputations[i]);
        }
        
        ::set_reputation(
            name.data(), sizeof(char*) * name.size(),
            name_len.data(), sizeof(int) * name_len.size(),
            rep.data(), sizeof(int) * reputations.size(),
            memo.data(), sizeof(char*) * memo.size(),
            memo_len.data(), sizeof(int) * memo_len.size());
    }

    inline void update_reputation(const std::string& name, int32_t reputation, const std::string& memo) {
        update_reputations( std::vector<std::string>{name}, std::vector<int32_t>{reputation}, std::vector<std::string>{memo});
    }

    inline void set_copyright_admin(const std::string& name) {
        ::set_copyright_admin((char*)name.c_str(),(int)name.size());
    }

    inline void update_copyrights(const std::vector<uint64_t>& postids, const std::vector<int32_t>& copyrights, const std::vector<std::string>& memos) {
        size_t count = postids.size();
        cosio_assert(count == copyrights.size() && count == memos.size(), "illegal parameters");

        std::vector<unsigned long long> pids;
        std::vector<char*> memo;
        std::vector<int> memo_len;
        std::vector<int> cps;
        for (size_t i = 0; i < count; i++) {
            pids.push_back(postids[i]);
            memo.push_back((char*)memos[i].c_str());
            memo_len.push_back((int)memos[i].size());
            cps.push_back((int)copyrights[i]);
        }
        
        ::set_copyright(
            (int*)pids.data(), sizeof(unsigned long long) * pids.size(),
            cps.data(), sizeof(int) * copyrights.size(),
            memo.data(), sizeof(char*) * memo.size(),
            memo_len.data(), sizeof(int) * memo_len.size());
    }

    inline void update_copyright(uint64_t postid, int32_t copyright, const std::string& memo) {
        update_copyrights( std::vector<uint64_t>{postid}, std::vector<int32_t>{copyright}, std::vector<std::string>{memo});
    }

    inline void update_freeze(const std::vector<std::string>& names, int32_t op, const std::vector<std::string>& memos) {
        size_t count = names.size();

        std::vector<char*> name;
        std::vector<int> name_len;
        std::vector<char*> memo;
        std::vector<int> memo_len;
        for (size_t i = 0; i < count; i++) {
            name.push_back((char*)names[i].c_str());
            name_len.push_back((int)names[i].size());
            memo.push_back((char*)memos[i].c_str());
            memo_len.push_back((int)memos[i].size());
        }

        ::set_freeze(
                name.data(), sizeof(char*) * name.size(),
                name_len.data(), sizeof(int) * name_len.size(),
                (int)op,
                memo.data(), sizeof(char*) * memo.size(),
                memo_len.data(), sizeof(int) * memo_len.size());
    }
}
