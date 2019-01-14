#include <cosiolib/contract.hpp>
#include <cosiolib/print.hpp>

struct course {
    std::string name;
    uint32_t credit;
    
    COSIO_SERIALIZE(course, (name)(credit))
};

struct student {
    cosio::name name;
    uint32_t age;
    bool gender;
    std::vector<course> courses;
    
    COSIO_SERIALIZE(student, (name)(age)(gender)(courses))
};

struct player: public student {
    std::string game;
    
    COSIO_SERIALIZE_DERIVED(player, student, (game))
};

using team = std::vector<player>;

class test : public cosio::contract {
public:
    using cosio::contract::contract;

    void test_print() {
        cosio::print('h');cosio::print('e');cosio::print('l');cosio::print('l');cosio::print('o');cosio::print('\n');
        cosio::print("hello\n");
        cosio::print("hel", "lo", '\n');
        cosio::print_f("hello\n");
        cosio::print_f("%e%%%", 'h', "ll", "o", "\n");
        cosio::print(1); cosio::print(2); cosio::print(3); cosio::print(4); cosio::print(5); cosio::print("\n");
        cosio::print(1,2,3,4,5); cosio::print("\n");
        cosio::print(12345); cosio::print("\n");
        cosio::print((short)12345); cosio::print("\n");
        cosio::print((long)12345); cosio::print("\n");
        cosio::print((unsigned long)12345); cosio::print("\n");
        cosio::print((long long)12345); cosio::print("\n");
        cosio::print_f("%\n", 12345);
        cosio::print_f("%%%%%%", 1, (short)2, (long)3, (long long)4, (unsigned char)5, '\n');
        cosio::print(true, false); cosio::print("\n");
    }
    
    void test_chain_info() {
        cosio::print(cosio::current_block_number(), '\n');
        cosio::print(cosio::current_timestamp(), '\n');
        cosio::print(cosio::current_witness(), '\n');
        cosio::cosio_assert(cosio::current_block_number() == cosio::current_block_number(), "inconsistent block number");
        cosio::cosio_assert(cosio::current_timestamp() == cosio::current_timestamp(), "inconsistent block timestamp");
        cosio::cosio_assert(cosio::current_witness().size() > 0, "invalid witness name");
    }
    
    void test_sha256(std::string s) {
        cosio::print_f("sha256 of % is %\n", s, cosio::sha256(cosio::bytes(s.begin(), s.end())));
    }
    
    void contract_info(std::string owner, std::string name) {
        cosio::print_f("my name: %\nmy owner: %\nmy balance: %\ncalled by user: %\ncaller: %\nmethod: %\nowner balance: %\ncaller balance: %\n",
                       this->get_name().contract(),
                       this->get_name().account(),
                       cosio::get_balance(this->get_name()),
                       cosio::is_contract_called_by_user(),
                       cosio::get_contract_caller().string(),
                       cosio::get_contract_method(),
                       cosio::get_balance(this->get_name().account()),
                       cosio::get_balance(cosio::get_contract_caller())
                       );
        cosio::print("\n");
        cosio::name cn(owner, name);
        cosio::print_f("name: %\nowner: %\nbalance: %\n", cn.contract(), cn.account(), cosio::get_balance(cn));
        
        cosio::execute_contract(this->get_name(), "inline_target", 0);
    }
    
    void inline_target() {
        cosio::print_f("my name: %\nmy owner: %\nmy balance: %\ncalled by user: %\ncaller: %\nmethod: %\nowner balance: %\ncaller balance: %\n",
                       this->get_name().contract(),
                       this->get_name().account(),
                       cosio::get_balance(this->get_name()),
                       cosio::is_contract_called_by_user(),
                       cosio::get_contract_caller().string(),
                       cosio::get_contract_method(),
                       cosio::get_balance(this->get_name().account()),
                       cosio::get_balance(cosio::get_contract_caller())
                       );
    }
    
    // if using wallet-cli, try following method parameters,
    // - empty team: [[]]
    // - a team consisting of alice and bob: [[[[\"alice\",20,false,[[\"maths\",10],[\"chemistry\",8]]],\"swim\"],[[\"bob\",21,true,[[\"physics\",9]]],\"baseball\"]]]
    void test_arg(team t) {
        for (auto it = t.begin(); it != t.end(); it++) {
            cosio::print_f("%, %, %, % (", it->name, it->age, it->gender? "m":"f", it->game);
            for (auto c = it->courses.begin(); c != it->courses.end(); c++) {
                cosio::print_f("(%, %)", c->name, c->credit);
            }
            cosio::print(")\n");
        }
    }
    
private:
    
};

COSIO_ABI(test,
          (test_print)
          (test_chain_info)
          (test_sha256)
          (contract_info)
          (inline_target)
          (test_arg)
)
