#pragma once

#include <cosiolib/serialize.hpp>
#include <cosiolib/table.hpp>

#define _COSIO_SINGLETON_DATA_ID  1

namespace cosio {
    struct singleton_record {
        singleton_record(): id(_COSIO_SINGLETON_DATA_ID) {}
        int32_t id;
        
        COSIO_SERIALIZE(::cosio::singleton_record, (id))
    };
    
    template<typename Record, typename NameProvider>
    class singleton {
    public:
        using table_type = table<Record, int32_t, NameProvider>;
        
    public:
        bool exists() {
            return _table.has(_COSIO_SINGLETON_DATA_ID);
        }
        
        Record get() {
            return _table.get(_COSIO_SINGLETON_DATA_ID);
        }
        
        Record get_or_default(const Record& def = Record()) {
            return _table.get_or_default(_COSIO_SINGLETON_DATA_ID, def);
        }
        
        Record get_or_create(const Record& def = Record()) {
            return _table.get_or_create(_COSIO_SINGLETON_DATA_ID, def);
        }
        
        template<typename Modifier>
        void update(Modifier m) {
            _table.update(_COSIO_SINGLETON_DATA_ID, [&](Record& r) {
                m(r);
                r.id = _COSIO_SINGLETON_DATA_ID;
            });
        }
        
        void remove() {
            return _table.remove(_COSIO_SINGLETON_DATA_ID);
        }
        
    private:
        table_type _table;
    };

    template<typename Record, typename NameProvider>
    class singleton_ex {
    public:
        using table_type = table_ex<Record, int32_t, NameProvider>;
        
    public:
        bool exists() {
            return _table.has(_COSIO_SINGLETON_DATA_ID);
        }
        
        Record get() {
            return _table.get(_COSIO_SINGLETON_DATA_ID);
        }
        
        Record get_or_default(const Record& def = Record()) {
            return _table.get_or_default(_COSIO_SINGLETON_DATA_ID, def);
        }
        
    private:
        table_type _table;
    };
}

//
// macros for local singleton definition
//
#define _COSIO_NAMED_SINGLETON(NAMETYPE, NAME, RECORD) \
_COSIO_NAME_PROVIDER(NAMETYPE, NAME);\
cosio::singleton<RECORD, NAMETYPE>

#define COSIO_NAMED_SINGLETON(NAME, RECORD) \
_COSIO_NAMED_SINGLETON(BOOST_PP_SEQ_CAT((__cosio_name)(__COUNTER__)), NAME, RECORD)

#define COSIO_DEFINE_SINGLETON(VARNAME, RECORD)  COSIO_NAMED_SINGLETON(BOOST_PP_STRINGIZE(VARNAME), RECORD) VARNAME

#define COSIO_DEFINE_NAMED_SINGLETON(VARNAME, NAME, RECORD)  COSIO_NAMED_SINGLETON(NAME, RECORD) VARNAME

//
// macros for external singleton definition
//
#define _COSIO_NAMED_SINGLETON_EX(NAMETYPE, OWNER, CONTRACT, TABLE, RECORD) \
_COSIO_NAME_PROVIDER_EX(NAMETYPE, OWNER, CONTRACT, TABLE);\
cosio::singleton_ex<RECORD, NAMETYPE>

#define COSIO_NAMED_SINGLETON_EX(OWNER, CONTRACT, TABLE, RECORD) \
_COSIO_NAMED_SINGLETON_EX(BOOST_PP_SEQ_CAT((__cosio_name_ex)(__COUNTER__)), OWNER, CONTRACT, TABLE, RECORD)

#define COSIO_DEFINE_SINGLETON_EX(VARNAME, OWNER, CONTRACT, TABLE, RECORD)  COSIO_NAMED_SINGLETON_EX(OWNER, CONTRACT, TABLE, RECORD) VARNAME
