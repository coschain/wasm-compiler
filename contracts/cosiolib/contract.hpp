#pragma once

#include <cosiolib/types.hpp>
#include <cosiolib/assert.hpp>
#include <cosiolib/system.hpp>
#include <cosiolib/serialize.hpp>
#include <cosiolib/datastream.hpp>
#include <cosiolib/table.hpp>
#include <cosiolib/singleton.hpp>
#include <type_traits>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/include/std_tuple.hpp>
#include <boost/mp11/tuple.hpp>

namespace cosio {
    
    class contract {
    public:
        contract(const name& name, const name& caller):
            _name(name), _caller(caller) { }
        const name& name() const { return _name; }
        const name& caller() const { return _caller; }
    private:
        name _name;
        name _caller;
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
    return 0; \
}

#define COSIO_API( TYPE, MEMBERS )  BOOST_PP_SEQ_FOR_EACH( COSIO_API_CALL, TYPE, MEMBERS )

#define COSIO_ABI( TYPE, MEMBERS ) \
extern "C" uint32_t COSIO_CONTRACT_ENTRY_NAME () { \
    BOOST_PP_SEQ_FOR_EACH( COSIO_API_CHECK, TYPE, MEMBERS ) \
    TYPE this_contract( cosio::get_contract_name(), cosio::get_contract_caller() ); \
    auto method = cosio::get_contract_method(); \
    COSIO_API( TYPE, MEMBERS ) \
    cosio::cosio_assert(false, std::string("unknown contract method: ") + method); \
    return 1; \
}

