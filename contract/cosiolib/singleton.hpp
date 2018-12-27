#pragma once

#include "serialize.hpp"
#include "table.hpp"

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
        static constexpr int32_t the_key = _COSIO_SINGLETON_DATA_ID;
        
    public:
        bool exists() {
            return _table.has(the_key);
        }
        
        Record get() {
            return _table.get(the_key);
        }
        
        Record get_or_default(const Record& def = Record()) {
            return _table.get_or_default(the_key, def);
        }
        
        Record get_or_create(const Record& def = Record()) {
            return _table.get_or_create(the_key, def);
        }
        
        template<typename Modifier>
        void update(Modifier m) {
            _table.update(the_key, [&](Record& r) {
                m(r);
                r.id = the_key;
            });
        }
        
        void remove() {
            return _table.remove(the_key);
        }
        
    private:
        table_type _table;
    };
}

#define _COSIO_NAMED_SINGLETON(NAMETYPE, NAME, RECORD) \
_COSIO_NAME_PROVIDER(NAMETYPE, NAME);\
cosio::singleton<RECORD, NAMETYPE>

#define COSIO_NAMED_SINGLETON(NAME, RECORD) \
_COSIO_NAMED_SINGLETON(BOOST_PP_SEQ_CAT((__cosio_name)(__COUNTER__)), NAME, RECORD)

#define COSIO_DEFINE_SINGLETON(VARNAME, RECORD)  COSIO_NAMED_SINGLETON(BOOST_PP_STRINGIZE(VARNAME), RECORD) VARNAME

#define COSIO_DEFINE_NAMED_SINGLETON(VARNAME, NAME, RECORD)  COSIO_NAMED_SINGLETON(NAME, RECORD) VARNAME
