#pragma once

#include <boost/preprocessor/seq/seq.hpp>
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
        static const char* record_type_name() {
            return Record::_cosio_type_name();
        }
        
        bool has(const Primary& key) {
            return table_has(record_type_name(), pack(key));
        }
        
        Record get(const Primary& key) {
            bytes enc;
            table_get(record_type_name(), pack(key), enc);
            return unpack<Record>(enc);
        }
        
        template<typename Modifier>
        void insert(Modifier m) {
            Record r;
            m(r);
            table_insert(record_type_name(), pack(r));
        }
        
        template<typename Modifier>
        void update(const Primary& key, Modifier m) {
            Record r = get(key);
            m(r);
            table_update(record_type_name(), pack(key), pack(r));
        }
        
        void remove(const Primary& key) {
            table_delete(record_type_name(), pack(key));
        }
    };
    
    template <class T, class M> M get_member_type(M T::*);
}

#define _COSIO_MEMBER_TYPE(m) decltype(cosio::get_member_type(m))

#define COSIO_DEFINE_TABLE(VARNAME, RECORD, INDICES)  cosio::table<RECORD, _COSIO_MEMBER_TYPE(&(RECORD::BOOST_PP_SEQ_ELEM(0, INDICES)))> VARNAME

