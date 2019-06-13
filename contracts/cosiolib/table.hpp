#pragma once

#include <boost/preprocessor/seq/seq.hpp>
#include <boost/preprocessor/seq/cat.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <cosiolib/types.hpp>
#include <cosiolib/system.hpp>

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
    
    template<typename Record>
    struct record_type_name {
        static const char *name() {
            return Record::_cosio_type_name();
        }
    };
    
    template<typename Record, typename Primary, typename NameProvider = record_type_name<Record> >
    class table {
    public:
        static const char* name() {
            return NameProvider::name();
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

    inline bool table_has_ex(const name& contract_name, const std::string& table_name, const bytes& primary_key) {
        if (!contract_name.is_contract()) {
            return false;
        }
        std::string owner = contract_name.account();
        std::string contract = contract_name.contract();
        int size = ::table_get_record_ex((char*)owner.c_str(), (int)owner.size(),
                                         (char*)contract.c_str(), (int)contract.size(),
                                         (char*)table_name.c_str(), (int)table_name.size(),
                                         (char*)primary_key.data(), (int)primary_key.size(),
                                         nullptr, 0);
        return size > 0;
    }
    
    inline void table_get_ex(const name& contract_name, const std::string& table_name, const bytes& primary_key, bytes& record) {
        std::string owner = contract_name.account();
        std::string contract = contract_name.contract();
        int size = ::table_get_record_ex((char*)owner.c_str(), (int)owner.size(),
                                         (char*)contract.c_str(), (int)contract.size(),
                                         (char*)table_name.c_str(), (int)table_name.size(),
                                         (char*)primary_key.data(), (int)primary_key.size(),
                                         nullptr, 0);
        record.clear();
        if(size > 0) {
            record.resize(size);
            ::table_get_record_ex((char*)owner.c_str(), (int)owner.size(),
                                  (char*)contract.c_str(), (int)contract.size(),
                                  (char*)table_name.c_str(), (int)table_name.size(),
                                  (char*)primary_key.data(), (int)primary_key.size(),
                                  (char*)record.data(), size);
        }
    }

    template<typename Record, typename Primary>
    class unbound_table_ex {
    public:
        bool has(const Primary& key) {
            return table_has_ex(_contract, _table_name, pack(key));
        }
        
        Record get(const Primary& key) {
            bytes enc;
            table_get_ex(_contract, _table_name, pack(key), enc);
            return unpack<Record>(enc);
        }
        
        Record get_or_default(const Primary& key, const Record& def = Record()) {
            return has(key)? get(key) : def;
        }

        void bind(const name& contract, const std::string& table) {
            cosio_assert(contract.is_contract(), "bind to contract only");
            _contract = contract;
            _table_name = table;
        }

        void bind(const std::string& owner, const std::string& contract, const std::string& table) {
            bind(name(owner, contract), table);
        }

    protected:
        name _contract;
        std::string _table_name;
    };
    
    template<typename Record, typename Primary, typename NameProvider>
    class table_ex : public unbound_table_ex<Record, Primary> {
    public:
        table_ex() {
            bind(NameProvider::contract(), NameProvider::table());
        }
    };

    template <class T, class M> M get_member_type(M T::*);
}

//
// macros for local table definition
//
#define _COSIO_MEMBER_TYPE(m) decltype(cosio::get_member_type(m))

#define _COSIO_TABLE(RECORD, INDICES, NAME) cosio::table<RECORD, _COSIO_MEMBER_TYPE(&(RECORD::BOOST_PP_SEQ_ELEM(0, INDICES))), NAME>

#define _COSIO_NAME_PROVIDER(TYPENAME, NAME) struct TYPENAME { static const char *name() { return NAME; } }

#define _COSIO_NAMED_TABLE(NAMETYPE, NAME, RECORD, INDICES) \
_COSIO_NAME_PROVIDER(NAMETYPE, NAME);\
_COSIO_TABLE(RECORD, INDICES, NAMETYPE)

#define COSIO_NAMED_TABLE(NAME, RECORD, INDICES) \
_COSIO_NAMED_TABLE(BOOST_PP_SEQ_CAT((__cosio_name)(__COUNTER__)), NAME, RECORD, INDICES)

#define COSIO_DEFINE_TABLE(VARNAME, RECORD, INDICES)  COSIO_NAMED_TABLE(BOOST_PP_STRINGIZE(VARNAME), RECORD, INDICES) VARNAME

#define COSIO_DEFINE_NAMED_TABLE(VARNAME, NAME, RECORD, INDICES)  COSIO_NAMED_TABLE(NAME, RECORD, INDICES) VARNAME

//
// macros for external table definition
//
#define _COSIO_TABLE_EX(RECORD, INDICES, NAME) cosio::table_ex<RECORD, _COSIO_MEMBER_TYPE(&(RECORD::BOOST_PP_SEQ_ELEM(0, INDICES))), NAME>

#define _COSIO_NAME_PROVIDER_EX(TYPENAME, OWNER, CONTRACT, TABLE) \
struct TYPENAME { \
    static cosio::name contract() { return cosio::name(OWNER, CONTRACT); } \
    static std::string table() { return TABLE; } \
}

#define _COSIO_NAMED_TABLE_EX(NAMETYPE, OWNER, CONTRACT, TABLE, RECORD, INDICES) \
_COSIO_NAME_PROVIDER_EX(NAMETYPE, OWNER, CONTRACT, TABLE);\
_COSIO_TABLE_EX(RECORD, INDICES, NAMETYPE)

#define COSIO_NAMED_TABLE_EX(OWNER, CONTRACT, TABLE, RECORD, INDICES) \
_COSIO_NAMED_TABLE_EX(BOOST_PP_SEQ_CAT((__cosio_name_ex)(__COUNTER__)), OWNER, CONTRACT, TABLE, RECORD, INDICES)

#define COSIO_DEFINE_TABLE_EX(VARNAME, OWNER, CONTRACT, TABLE, RECORD, INDICES)  COSIO_NAMED_TABLE_EX(OWNER, CONTRACT, TABLE, RECORD, INDICES) VARNAME

#define COSIO_UNBOUND_TABLE_EX(VARNAME, RECORD, INDICES)  cosio::unbound_table_ex<RECORD, _COSIO_MEMBER_TYPE(&(RECORD::BOOST_PP_SEQ_ELEM(0, INDICES)))> VARNAME
