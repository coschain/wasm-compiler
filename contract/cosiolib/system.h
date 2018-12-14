#pragma once

#ifdef __cplusplus
extern "C" {
#endif

    unsigned long long current_block_number();
    
    unsigned long long current_timestamp();
    
    void current_witness(char* , int);
    
    void sha256(char*, int, char*, int);
    
    void print_str(char*);
    
    void print_uint32(unsigned int);
    void print_uint64(unsigned long long);
    void print_bool(unsigned int);
    
    void require_auth(char*);
    
    unsigned long long get_balance_by_name(char*);
    
    unsigned long long get_contract_balance(char* , char*);
    
    void save_to_storage(char*, int, char*, int);
    
    int read_from_storage(char*, int, char*, int);
    
    void log_sort(unsigned int, char*, int, char*, int);
    
    void cos_assert(int, char* );
    
    void read_contract_op_params(char*, int, char*, int);
    int    read_contract_op_params_length();
    
    void read_contract_owner(char*, int);
    void read_contract_caller(char*, int);
    
    void transfer( char* , char* , unsigned long long, char* );
    unsigned long long get_sender_value();
    
    
#ifdef __cplusplus
}
#endif
