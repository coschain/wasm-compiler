#pragma once

#include "types.hpp"
#include "assert.hpp"
#include "system.h"
#include "datastream.hpp"
#include <type_traits>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/include/std_tuple.hpp>
#include <boost/mp11/tuple.hpp>

namespace cosio {
    
    inline account_name get_contract_owner() {
        char buf[COSIO_MAX_ACCOUNT_NAME_SIZE + 1];
        memset(buf, 0, COSIO_MAX_ACCOUNT_NAME_SIZE + 1);
        ::read_contract_owner(buf, COSIO_MAX_ACCOUNT_NAME_SIZE + 1);
        return account_name((const char*)buf);
    }
    
    inline contract_name get_contract_name() {
        char buf[COSIO_MAX_CONTRACT_NAME_SIZE + 1];
        memset(buf, 0, COSIO_MAX_CONTRACT_NAME_SIZE + 1);
        ::read_contract_name(buf, COSIO_MAX_CONTRACT_NAME_SIZE + 1);
        return contract_name((const char*)buf);
    }
    
    inline account_name get_contract_caller() {
        char buf[COSIO_MAX_ACCOUNT_NAME_SIZE + 1];
        memset(buf, 0, COSIO_MAX_ACCOUNT_NAME_SIZE + 1);
        ::read_contract_caller(buf, COSIO_MAX_ACCOUNT_NAME_SIZE + 1);
        return account_name((const char*)buf);
    }
    
    inline method_name get_contract_method() {
        char buf[COSIO_MAX_METHOD_NAME_SIZE + 1];
        memset(buf, 0, COSIO_MAX_METHOD_NAME_SIZE + 1);
        ::read_contract_method(buf, COSIO_MAX_METHOD_NAME_SIZE + 1);
        return method_name((const char*)buf);
    }
    
    inline bytes get_contract_args() {
        bytes args;
        int min_size = ::read_contract_op_params_length();
        if (min_size > 0) {
            args.reserve(min_size);
        }
        ::read_contract_op_params(args.data(), (int)args.size());
        return args;
    }
    
    inline coin_amount get_contract_balance(account_name owner, contract_name contract) {
        return ::get_contract_balance((char*)owner.c_str(), (char*)contract.c_str());
    }
    
    inline coin_amount get_contract_sender_value() {
        return ::get_sender_value();
    }
    
    ///////////////////////
    
    class contract {
    public:
        contract(const account_name& owner, const contract_name& name, const account_name& caller):
            _owner(owner), _name(name), _caller(caller) { }
        const account_name& owner() const { return _owner; }
        const contract_name& name() const { return _name; }
        const account_name& caller() const { return _caller; }
    private:
        account_name _owner;
        contract_name _name;
        account_name _caller;
    };
    
    template<typename T, typename...Args>
    static void execute_contract_method( T* contract_ptr, void (T::*method)(Args...) ) {
        auto args = unpack<std::tuple<std::decay_t<Args>...>>( get_contract_args() );
        auto f = [&]( auto...a ) {
            (contract_ptr->*method)( a... );
        };
        boost::mp11::tuple_apply( f, args );
    }
    
    template<typename T, typename R, typename...Args>
    static constexpr bool valid_contract_method(R (T::*method)(Args...) ) {
        return std::is_void<R>::value;
    }

}

#define COSIO_CONTRACT_ENTRY_NAME   apply

#define COSIO_API_CHECK( r, TYPE, M ) static_assert( cosio::valid_contract_method( &TYPE::M ), \
    BOOST_PP_STRINGIZE(M) ": contract method must return void." );

#define COSIO_API_CALL( r, TYPE, M ) \
if ( method == BOOST_PP_STRINGIZE(M) ) { \
    cosio::execute_contract_method( &this_contract, &TYPE::M ); \
    return; \
}

#define COSIO_API( TYPE, MEMBERS )  BOOST_PP_SEQ_FOR_EACH( COSIO_API_CALL, TYPE, MEMBERS )

#define COSIO_ABI( TYPE, MEMBERS ) \
extern "C" void COSIO_CONTRACT_ENTRY_NAME () { \
    BOOST_PP_SEQ_FOR_EACH( COSIO_API_CHECK, TYPE, MEMBERS ) \
    TYPE this_contract( cosio::get_contract_owner(), cosio::get_contract_name(), cosio::get_contract_caller() ); \
    cosio::method_name method = cosio::get_contract_method(); \
    COSIO_API( TYPE, MEMBERS ) \
    cosio::cosio_assert(false, std::string("unknown contract method: ") + method); \
}

