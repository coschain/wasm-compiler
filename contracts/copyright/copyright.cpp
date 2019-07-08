#include <cosiolib/contract.hpp>
#include <cosiolib/print.hpp>

const uint32_t expire_blocks = 900;
//const uint32_t expire_blocks = 86400;

struct voter {
    voter():name(""),haveVoted(false){}
    std::string name;
    bool haveVoted;

    COSIO_SERIALIZE(voter,(name)(haveVoted))
};

struct item : public cosio::singleton_record {
    item(): admin(""), agree(0), begin_block(0), producers(std::vector<voter>()) {}

    std::string admin; // the proposal target
    uint32_t agree; // total agrees to the proposal
    uint64_t begin_block; // when expired, this item become invalid, we accept new proposal
    std::vector<voter> producers; // all producers when proposal

    COSIO_SERIALIZE_DERIVED(item,cosio::singleton_record,(admin)(agree)(begin_block)(producers))
};

class copyright : public cosio::contract {
public:
    using cosio::contract::contract;

	void setadmin( const std::string& user ) {
		cosio::set_copyright_admin(user);
	}

	void setcopyright( const uint64_t postid, int32_t copyright, const std::string& memo ) {
		cosio::update_copyright(postid, copyright, memo);
	}

	void setcopyrights( const std::vector<uint64_t>& postids, const std::vector<int32_t>& copyrights, const std::vector<std::string>& memos ) {
		cosio::update_copyrights(postids, copyrights, memos);
	}

    void proposal(const std::string& user) {
        auto caller = cosio::get_contract_caller();
        cosio::print("\nproposal() get_contract_caller:%",caller.string());
        auto producers = cosio::block_producers();

        std::vector<std::string>::const_iterator it = std::find(producers.begin(),producers.end(),caller.string());
        if(it == producers.end()){
            cosio::print("\nproposal() caller not producers");
            cosio::cosio_assert(false, std::string("caller is not producers, name:") + caller.string());
        }

        // check expire
        auto num = cosio::current_block_number();
        if (box.exists()) {
            auto v = box.get();
            if(v.begin_block + expire_blocks > num){
                cosio::cosio_assert(false, std::string("last proposal still available"));
            }
        }

        // a new proposal
        auto v = box.get_or_create();
        box.update([&](item &vt){
                vt.admin = user;
                vt.agree = 0;
                vt.begin_block = num;
                for(int i=0;i<producers.size();i++) {
                    voter v;
                    v.name = producers[i];
                    v.haveVoted = false;
                    vt.producers.push_back(v);
                }
        cosio::print("\nnew proposal begin_block:%", vt.begin_block);
        });
    }

    void vote() {
        cosio::print("\nvote() enter");
        auto caller = cosio::get_contract_caller();
        if(!box.exists()) {
        cosio::print("\nvote() box not exist");
            cosio::cosio_assert(false, std::string("no proposal yet"));
        }
        cosio::print("\nvote() box exist");

        // check if caller in the producer list when proposal created
        auto v = box.get();
        cosio::print("\n table producer size:%",v.producers.size());
        for(std::vector<voter>::const_iterator it = v.producers.begin();it!=v.producers.end();it++) {
            cosio::print("\nproducer:%",it->name);
        }

        auto name = caller.string();
        std::vector<voter>::const_iterator it = std::find_if(v.producers.begin(),v.producers.end(),[&name](const voter& vt){return vt.name == name;});
        if(it == v.producers.end()) {
        cosio::print("\ncaller not in producers");
            cosio::cosio_assert(false, std::string("caller is not in producers when proposal, caller:") + caller.string());
        }

        // check expire
        auto num = cosio::current_block_number();
        cosio::print("\nvote() current block num:%", num);
        if(v.begin_block + expire_blocks <= num) {
            cosio::cosio_assert(false, std::string("proposal expired, please commit a new proposal"));
        }

        cosio::print("\nvote() update");
        // add vote
        box.update([&](item &vt){
                vt.agree++;
                });
        auto all_producer_size = v.producers.size();
        auto limit = (all_producer_size/3)*2;
        
        cosio::print("\nvote() producer size:%",all_producer_size);
        // setadmin if most bp agree
        if(v.agree+1 == 1) {
        //if(v.agree+1 > limit) {
        cosio::print("\nset admin %", v.admin);
            setadmin(v.admin);
            box.remove();
        }
    }

private:

    COSIO_DEFINE_NAMED_SINGLETON( box, "electionbox", item);
};


COSIO_ABI(copyright, (setadmin)(setcopyright)(setcopyrights)(proposal)(vote))
