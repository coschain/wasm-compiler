/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

//#include <contento/abi_generator/types.hpp>
//#include <fc/io/varint.hpp>
//#include <fc/reflect/variant.hpp>
//#include <fc/variant_object.hpp>
//#include <fc/exception/exception.hpp>

#include <map>
#include <string>
#include <vector>
#include <set>
//#include <optional>
#include <boost/assert.hpp>
#include <iostream>
#include <nlohmann/json.hpp>


#define FC_ASSERT(x, ...) BOOST_ASSERT(x);
#define FC_CAPTURE_AND_RETHROW(...)  \
   catch( ... ) {  \
      throw std::current_exception(); \
   }
#define wlog(x, ...) std::cout << x << std::endl
#define FC_CAPTURE_AND_LOG(...)   \
   catch( ... ) {  \
   }

namespace contento { namespace chain {
    using json = nlohmann::json;

   using std::vector;
   using std::string;
   //using std::map;

   using type_name      = std::string;
   using field_name     = std::string;
   using table_name = string;
   using action_name = string;
   using name = string;

struct type_def {
   type_def() = default;
   type_def(const type_name& new_type_name, const type_name& type)
   :new_type_name(new_type_name), type(type)
   {}

   type_name   new_type_name;
   type_name   type;
    
    void to_json2(json& out) {
        out["new_type_name"] = new_type_name.c_str();
        out["type"] = type;
    }
};

struct field_def {
   field_def() = default;
   field_def(const field_name& name, const type_name& type)
   :name(name), type(type)
   {}

   field_name name;
   type_name  type;

   bool operator==(const field_def& other) const {
      return std::tie(name, type) == std::tie(other.name, other.type);
   }

    void to_json2(json& out){
        out["name"] = name;
        out["type"] = type;
    }
};

struct struct_def {
   struct_def() = default;
   struct_def(const type_name& name, const type_name& base, const vector<field_def>& fields)
   :name(name), base(base), fields(fields)
   {}

   type_name            name;
   type_name            base;
   vector<field_def>    fields;

   bool operator==(const struct_def& other) const {
      return std::tie(name, base, fields) == std::tie(other.name, other.base, other.fields);
   }
    
    void to_json2(json& out){
        out["name"] = name;
        out["base"] = base;
        
        json ofields;
        for(auto i : fields){
            json ofield;
            i.to_json2(ofield);
            ofields.push_back(ofield);
        }
        out["fields"] = ofields;
    }
};

struct action_def {
   action_def() = default;
   action_def(const action_name& name, const type_name& type)
   :name(name), type(type)
   {}

   action_name name;
   type_name   type;
    
    void to_json2(json& out){
        out["name"] = name;
        out["type"] = type;
    }
};
    
    struct table_def {
        table_def() = default;
        table_def(const table_name& name, const type_name& type, const vector<field_name>& key_names)
        :name(name), type(type), keys(key_names)
        {}
        
        table_name         name;        // the name of the table
        type_name          type;  // the kind of index, i64, i128i128, etc
        vector<field_name> keys;   // names for the keys
        
        void to_json2(json& out){
            out["name"] = name;
            out["type"] = type;
            
            {
                out["secondary"] = json::array();
                for(int i=0;i<keys.size();i++){
                    if (i == 0) {
                        out["primary"] = keys[i];
                    } else {
                        //secondary remove first
                        keys.erase(keys.begin());
                        out["secondary"] = keys;
                        break;
                    }
                }
            }
        }
        
    };

struct error_message {
   error_message() = default;
   error_message( uint64_t error_code, const string& error_msg )
   : error_code(error_code), error_msg(error_msg)
   {}

   uint64_t error_code;
   string   error_msg;
};

struct abi_def {
   abi_def() = default;
   abi_def(const vector<type_def>& types, const vector<struct_def>& structs, const vector<action_def>& actions, const vector<error_message>& error_msgs, const vector<table_def>& tables)
   :version("contento::abi/1.0")
   ,types(types)
   ,structs(structs)
   ,actions(actions)
   ,error_messages(error_msgs)
   ,tables(tables)
   {}

   string                version = "contentos::abi-1.0";
   vector<type_def>      types;
   vector<struct_def>    structs;
   vector<action_def>    actions;
   vector<error_message> error_messages;
   vector<table_def> tables;
   //extensions_type       abi_extensions;
    
    void to_json2(json& out){
        out["version"] = version.c_str();
        
        {
            json obtrees;
            for(auto i : types){
                json otype;
                i.to_json2(otype);
                obtrees.push_back(otype);
            }
            out["types"] = obtrees;
        }
        {
            json obtrees;
            for(auto i : structs){
                json obtree;
                i.to_json2(obtree);
                obtrees.push_back(obtree);
            }
            out["structs"] = obtrees;
        }
        {
            json obtrees;
            for(auto i : actions){
                json obtree;
                i.to_json2(obtree);
                obtrees.push_back(obtree);
            }
            out["actions"] = obtrees;
        }
        {
            json obtrees;
            for(auto i : tables){
                json obtree;
                i.to_json2(obtree);
                obtrees.push_back(obtree);
            }
            out["tables"] = obtrees;
        }
    }
};

abi_def contento_contract_abi(const abi_def& contento_system_abi);
vector<type_def> common_type_defs();

} } /// namespace contento::chain
