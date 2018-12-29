#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/seq/seq.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <cosiolib/varint.hpp>
#include <cosiolib/assert.hpp>

#define COSIO_REFLECT_MEMBER_OP( r, OP, elem ) \
  OP t.elem 

#define COSIO_SERIALIZE( TYPE,  MEMBERS ) \
 template<typename DataStream> \
 friend DataStream& operator << ( DataStream& ds, const TYPE& t ){ \
    ds << cosio::unsigned_int( BOOST_PP_SEQ_SIZE( MEMBERS ) ); \
    return ds BOOST_PP_SEQ_FOR_EACH( COSIO_REFLECT_MEMBER_OP, <<, MEMBERS );\
 }\
 template<typename DataStream> \
 friend DataStream& operator >> ( DataStream& ds, TYPE& t ){ \
    cosio::unsigned_int s; \
    ds >> s; \
    cosio::cosio_assert( BOOST_PP_SEQ_SIZE( MEMBERS ) == s.value, "unpacking " BOOST_PP_STRINGIZE(TYPE) ": field count mismatched."); \
    return ds BOOST_PP_SEQ_FOR_EACH( COSIO_REFLECT_MEMBER_OP, >>, MEMBERS );\
 } \
public:\
static const char* _cosio_type_name() { return BOOST_PP_STRINGIZE(TYPE); }


#define COSIO_SERIALIZE_DERIVED( TYPE, BASE, MEMBERS ) \
 template<typename DataStream> \
 friend DataStream& operator << ( DataStream& ds, const TYPE& t ){ \
    ds << cosio::unsigned_int( BOOST_PP_SEQ_SIZE( MEMBERS ) + 1 ); \
    ds << static_cast<const BASE&>(t); \
    return ds BOOST_PP_SEQ_FOR_EACH( COSIO_REFLECT_MEMBER_OP, <<, MEMBERS );\
 }\
 template<typename DataStream> \
 friend DataStream& operator >> ( DataStream& ds, TYPE& t ){ \
    cosio::unsigned_int s; \
    ds >> s; \
    cosio::cosio_assert( BOOST_PP_SEQ_SIZE( MEMBERS ) + 1 == s.value, "unpacking " BOOST_PP_STRINGIZE(TYPE) ":field count mismatched."); \
    ds >> static_cast<BASE&>(t); \
    return ds BOOST_PP_SEQ_FOR_EACH( COSIO_REFLECT_MEMBER_OP, >>, MEMBERS );\
 } \
public:\
static const char* _cosio_type_name() { return BOOST_PP_STRINGIZE(TYPE); }

