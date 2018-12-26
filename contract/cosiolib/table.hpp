#pragma once

#include <boost/preprocessor/seq/seq.hpp>
#include <boost/preprocessor/seq/cat.hpp>
#include <boost/preprocessor/stringize.hpp>
#include "types.hpp"
#include "system.hpp"

namespace cosio {
    
    inline bool table_has(const std::string& table_name, const bytes& primary_key) {
        int size = ::table_get_record((char*)table_name.c_str(), (int)table_name.size(),
                                      (char*)primary_key.data(), (int)primary_key.size(),
                                      nullptr, 0);
        return size > 0;
    }
    
    inline void table_get(const std::string& table_name, const bytes& primary_key, bytes& record) {
        int size = ::table_get_record((char*)table_name.c_str(), (int)table_name.size(),
                                      (char*)primary_key.data(), (int)primary_key.size(),
                                      nullptr, 0);
        record.clear();
        if(size > 0) {
            record.resize(size);
            ::table_get_record((char*)table_name.c_str(), (int)table_name.size(),
                               (char*)primary_key.data(), (int)primary_key.size(),
                               (char*)record.data(), size);
        }
    }
    
    inline void table_insert(const std::string& table_name, const bytes& record) {
        ::table_new_record((char*)table_name.c_str(), (int)table_name.size(),
                           (char*)record.data(), (int)record.size());
    }
    
    inline void table_update(const std::string& table_name, const bytes& primary_key, const bytes& record) {
        ::table_update_record((char*)table_name.c_str(), (int)table_name.size(),
                              (char*)primary_key.data(), (int)primary_key.size(),
                              (char*)record.data(), (int)record.size());
    }
    
    inline void table_delete(const std::string& table_name, const bytes& primary_key) {
        ::table_delete_record((char*)table_name.c_str(), (int)table_name.size(),
                       (char*)primary_key.data(), (int)primary_key.size());
    }
    
    template<typename Record, typename Primary>
    class table {
    public:
        virtual std::string name() {
            return Record::_cosio_type_name();
        }
        
        bool has(const Primary& key) {
            return table_has(name(), pack(key));
        }
        
        Record get(const Primary& key) {
            bytes enc;
            table_get(name(), pack(key), enc);
            return unpack<Record>(enc);
        }
        
        Record get_or_default(const Primary& key, const Record& def = Record()) {
            return has(key)? get(key) : def;
        }
        
        Record get_or_create(const Primary& key, const Record& def = Record()) {
            if (has(key)) {
                return get(key);
            }
            insert([&](Record& r) {
                r = def;
            });
            return def;
        }
        
        template<typename Modifier>
        void insert(Modifier m) {
            Record r;
            m(r);
            table_insert(name(), pack(r));
        }
        
        template<typename Modifier>
        void update(const Primary& key, Modifier m) {
            Record r = get(key);
            m(r);
            table_update(name(), pack(key), pack(r));
        }
        
        void remove(const Primary& key) {
            table_delete(name(), pack(key));
        }
    };
    
    template <class T, class M> M get_member_type(M T::*);
}

#define _COSIO_MEMBER_TYPE(m) decltype(cosio::get_member_type(m))

#define _COSIO_TABLE(RECORD, INDICES) cosio::table<RECORD, _COSIO_MEMBER_TYPE(&(RECORD::BOOST_PP_SEQ_ELEM(0, INDICES)))>

#define COSIO_NAMED_TABLE(NAME, RECORD, INDICES) \
struct BOOST_PP_SEQ_CAT((__cosio_table_)(__COUNTER__)): public _COSIO_TABLE(RECORD, INDICES) {\
    std::string name() { return NAME; }\
}

#define COSIO_DEFINE_TABLE(VARNAME, RECORD, INDICES)  COSIO_NAMED_TABLE(BOOST_PP_STRINGIZE(VARNAME), RECORD, INDICES) VARNAME

#define COSIO_DEFINE_NAMED_TABLE(VARNAME, NAME, RECORD, INDICES)  COSIO_NAMED_TABLE(NAME, RECORD, INDICES) VARNAME
