#pragma once

#ifdef __cplusplus
extern "C" {
#endif

    /**
     Get current block number.
     @return The block number.
     */
    unsigned long long current_block_number();
    
    /**
     Get current timestamp.
     @return The UTC timestamp of head block, in seconds.
     */
    unsigned long long current_timestamp();
    
    /**
     Get current witness account name.
     @param[in,out] buffer the buffer to which the account name string is stored.
     @param[in] size the capacity of @p buffer, in bytes.
     @return if @p size is positive, return the number of bytes written to @p buffer.
     if @p size is zero or negative, return the length of the account name in bytes without changing @p buffer.
     */
    int current_witness(char* buffer, int size);
    
    /**
     Get SHA256 hash digest of a blob.
     @param[in] buffer the data to be hashed.
     @param[in] size size of data in bytes.
     @param[in,out] hash the buffer to which digest bytes are stored.
     @param[in] hash_size the capacity of @p hash, in bytes.
     @return if @p hash_size is positive, return the number of bytes written to @p hash.
     if @p hash_size is zero or negative, return the length of digest in bytes without changing @p hash.
     */
    int sha256(char* buffer, int size, char* hash, int hash_size);

    /**
     Print a string.
     @param[in] s the string to be printed.
     @param[in] l the length of @p s.
     */
    void print_str(char*s, int l);
    
    /**
     Print a signed 64-bit integer.
     @param[in] n the integer.
     */
    void print_int(long long n);
    
    /**
     Print an unsigned 64-bit integer.
     @param[in] n the integer.
     */
    void print_uint(unsigned long long n);
    
    /**
     Assert that contract has authority of specific account.
     @param[in] name the account name string.
     @param[in] length the length of @p name.
     @remarks
     This function aborts execution of contract if the authority check fails.
     */
    void require_auth(char* name, int length);
    
    /**
     Get balance of specific account.
     @param[in] name the account name string.
     @param[in] length the length of @p name.
     @return the balance of account @p name, in coins. If the account doesn't exist, abort execution.
     */
    unsigned long long get_balance_by_name(char* name, int length);
    
    /**
     Get balance of specific contract.
     @param[in] owner the account name who owns the contract.
     @param[in] owner_len the length of @p owner.
     @param[in] contract the name of the contract.
     @param[in] contract_len the length of @p contract.
     @return the balance of the contract, in coins. If the contract doesn't exist, abort execution.
     */
    unsigned long long get_contract_balance(char* owner, int owner_len, char* contract, int contract_len);
    
    /**
     Write a key-value pair to database.
     @param[in] key the buffer storing bytes of the key.
     @param[in] key_size number of bytes of the key.
     @param[in] value the buffer storing bytes of the value.
     @param[in] value_size number of bytes of the value.
     */
    void save_to_storage(char* key, int key_size, char* value, int value_size);
    
    /**
     Read value from database for specific key.
     @param[in] key the buffer storing bytes of the querying key.
     @param[in] key_size number of bytes of the querying key.
     @param[in,out] value the buffer to which value bytes are stored.
     @param[in] value_size the capacity of @p value, in bytes.
     @return if @p value_size is positive, return the number of bytes written to @p value.
     if @p value_size is zero or negative, return the actual length of value without changing @p value.
     */
    int read_from_storage(char* key, int key_size, char* value, int value_size);

    /**
     Query a record in a database table.
     @param[in] table_name name of the table.
     @param[in] table_name_length length of @p table_name.
     @param[in] primary the primary key for query.
     @param[in] primary_len length of @p primary.
     @param[in,out] value the buffer to which record data are stored.
     @param[in] value_len capacity of @p value, in bytes
     @return if @p value_len is positive, return the number of bytes written in @p value.
     if @p value_len is zero or negative, return the size of value.
     */
    int table_get_record(char *table_name, int table_name_len, char* primary, int primary_len, char* value, int value_len);
    
    /**
     Create a record in a database table.
     @param[in] table_name name of the table.
     @param[in] table_name_length length of @p table_name.
     @param[in] value the record value.
     @param[in] value_len length of @p value.
     */
    void table_new_record(char *table_name, int table_name_len, char* value, int value_len);
    
    /**
     Update a record in a database table.
     @param[in] table_name name of the table.
     @param[in] table_name_length length of @p table_name.
     @param[in] primary the primary key of the record.
     @param[in] primary_len length of @p primary.
     @param[in] value the updated record value.
     @param[in] value_len length of @p value.
     */
    void table_update_record(char *table_name, int table_name_len, char* primary, int primary_len, char* value, int value_len);
    
    /**
     Delete a record in a database table.
     @param[in] table_name name of the table.
     @param[in] table_name_length length of @p table_name.
     @param[in] primary the primary key of the record.
     @param[in] primary_len length of @p primary.
     */
    void table_delete_record(char *table_name, int table_name_len, char* primary, int primary_len);
    
    /**
     Assert function
     @param[in] pred a boolean predicate.
     @param[in] msg the error message string.
     @param[in] msg_len the length of @p msg.
     @remarks
     This function aborts execution of contract if @p pred is zero. Otherwise, do nothing.
     */
    void cos_assert(int pred, char* msg, int msg_len);
    
    /**
     Abort execution of contract.
     */
    void abort();
    
    /**
     Get parameters data of current contract.
     @param[in,out] buf the buffer to which parameter data are stored.
     @param[in] capacity of @p buf.
     @return if @p size is positive, return the number of bytes written to @p buf.
     if @p size is zero or negative, return the actual length of parameter data without changing @p buf.
     */
    int read_contract_op_params(char* buf, int size);
    
    /**
     Get amount of coins the caller has sent with current contract calling.
     @return the amount of coins.
     */
    unsigned long long read_contract_sender_value();
    
    /**
     Get name of current contract.
     @param[in,out] buf the buffer to which name is stored.
     @param[in] size capacity of @p buf, in bytes.
     @return if @p size is positive, return the number of bytes written to @p buf.
     if @p size is zero or negative, return the length of contract name.
     */
    int read_contract_name(char* buf, int size);
    
    /**
     Get name of current contract method.
     @param[in,out] buf the buffer to which name is stored.
     @param[in] size capacity of @p buf, in bytes.
     @return if @p size is positive, return the number of bytes written to @p buf.
     if @p size is zero or negative, return the length of method name.
     */
    int read_contract_method(char* buf, int size);
    
    /**
     Get name of the account who owns current contract.
     @param[in,out] buf the buffer to which name is stored.
     @param[in] size capacity of @p buf, in bytes.
     @return if @p size is positive, return the number of bytes written to @p buf.
     if @p size is zero or negative, return the length of owner account name.
     */
    int read_contract_owner(char* buf, int size);
    
    /**
     Get name of the account who is calling current contract.
     @param[in,out] buf the buffer to which name is stored.
     @param[in] size capacity of @p buf, in bytes.
     @return if @p size is positive, return the number of bytes written to @p buf.
     if @p size is zero or negative, return the length of calling account name.
     */
    int read_contract_caller(char* buf, int size);

    /**
     Transfer coins
     @param[in] from account name of payer.
     @param[in] from_len length of @p from.
     @param[in] to account name of receiver.
     @param[in] to_len length of @p to.
     @param[in] amount number of coins to transfer.
     @param[in] memo a memo string.
     @param[in] memo_len length of @p memo.
     */
    void transfer( char* from, int from_len, char* to, int to_len, unsigned long long amount, char* memo, int memo_len);
    
    
#ifdef __cplusplus
}
#endif
