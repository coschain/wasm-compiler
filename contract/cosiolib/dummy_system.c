#include "system.h"

unsigned long long current_block_number() {
    return 0;
}

unsigned long long current_timestamp() {
    return 0;
}

void current_witness(char* name, int name_size) {
    
}

void sha256(char*data, int size, char*checksum, int checksum_len) {
    
}

void print_str(char*s) {
    
}
void print_str_l(char *s, int len) {
    
}
void print_int(long long) {
    
}
void print_uint(unsigned long long) {
    
}

void require_auth(char* name) {
    
}

unsigned long long get_balance_by_name(char* name)  {
    return 0;
}

unsigned long long get_contract_balance(char* owner, char* contract) {
    return 0;
}

void save_to_storage(char* key, int key_size, char* value, int value_size) {
    
}

int read_from_storage(char* key, int key_size, char* value, int value_size) {
    return 0;
}

void log_sort(unsigned int event_id, char* key, int key_size, char* sec_key, int sec_key_size) {
    
}

void cos_assert(int pred, char* msg) {
    
}

void read_contract_op_params(char* params, int params_size) {
    
}
int    read_contract_op_params_length()  {
    return 0;
}

void read_contract_method(char*method, int method_size) {
    
}

void read_contract_owner(char*owner, int owner_size) {
    
}
void read_contract_caller(char*caller, int caller_size) {
    
}

void transfer( char*from , char*to , unsigned long long amount, char* memo ) {
    
}
unsigned long long get_sender_value() {
    return 0;
}

