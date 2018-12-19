#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/seq/seq.hpp>
#include <boost/preprocessor/stringize.hpp>
#include "varint.hpp"
#include "assert.hpp"

#define COSLIB_REFLECT_MEMBER_OP( r, OP, elem ) \
  OP t.elem 

#define COSLIB_SERIALIZE( TYPE,  MEMBERS ) \
 template<typename DataStream> \
 friend DataStream& operator << ( DataStream& ds, const TYPE& t ){ \
    ds << unsigned_int( BOOST_PP_SEQ_SIZE( MEMBERS ) ); \
    return ds BOOST_PP_SEQ_FOR_EACH( COSLIB_REFLECT_MEMBER_OP, <<, MEMBERS );\
 }\
 template<typename DataStream> \
 friend DataStream& operator >> ( DataStream& ds, TYPE& t ){ \
    unsigned_int s; \
    ds >> s; \
    cosio::cosio_assert( BOOST_PP_SEQ_SIZE( MEMBERS ) == s.value, "unpacking " BOOST_PP_STRINGIZE(TYPE) ": field count mismatched."); \
    return ds BOOST_PP_SEQ_FOR_EACH( COSLIB_REFLECT_MEMBER_OP, >>, MEMBERS );\
 }


#define COSLIB_SERIALIZE_DERIVED( TYPE, BASE, MEMBERS ) \
 template<typename DataStream> \
 friend DataStream& operator << ( DataStream& ds, const TYPE& t ){ \
    ds << unsigned_int( BOOST_PP_SEQ_SIZE( MEMBERS ) + 1 ); \
    ds << static_cast<const BASE&>(t); \
    return ds BOOST_PP_SEQ_FOR_EACH( COSLIB_REFLECT_MEMBER_OP, <<, MEMBERS );\
 }\
 template<typename DataStream> \
 friend DataStream& operator >> ( DataStream& ds, TYPE& t ){ \
    unsigned_int s; \
    ds >> s; \
    cosio::cosio_assert( BOOST_PP_SEQ_SIZE( MEMBERS ) + 1 == s.value, "unpacking " BOOST_PP_STRINGIZE(TYPE) ":field count mismatched."); \
    ds >> static_cast<BASE&>(t); \
    return ds BOOST_PP_SEQ_FOR_EACH( COSLIB_REFLECT_MEMBER_OP, >>, MEMBERS );\
 }
