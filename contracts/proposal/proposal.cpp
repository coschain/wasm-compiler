#include <algorithm>
#include <cosiolib/contract.hpp>

// a proposal
struct proposal_record {
    std::string id;             // proposal id, must be non-empty
    std::string contents;       // contents
    cosio::name proposer;       // proposer account name
    uint64_t created_at;        // creation timestamp

    COSIO_SERIALIZE( proposal_record, (id)(contents)(proposer)(created_at) )
};

// a vote
struct vote_record {
    std::string vote_id;        // vote id = "<proposal_id>:<voter_name>"
    std::string id;             // proposal id
    cosio::name voter;          // voter account name
    std::string agreed;         // agree: "yes"; disagree: "no"
    uint64_t voted_at;          // vote timestamp

    COSIO_SERIALIZE( vote_record, (vote_id)(id)(voter)(agreed)(voted_at) )
};

// the contract
class proposal: public cosio::contract {
public:
    using cosio::contract::contract;

    // make a proposal with @id and @contents.
    void propose(const std::string& id, const std::string& contents) {
        // proposal id check
        check_proposal_id(id);
        // only block producers can make a proposal
        cosio::name proposer = check_bp_caller();
        // duplicate proposal check
        cosio::cosio_assert(!proposals.has(id), "proposal id already exists");
        // insert a new proposal record
        proposals.insert([&](proposal_record& r) {
            r.id = id;
            r.contents = contents;
            r.proposer = proposer;
            r.created_at = cosio::current_timestamp();
        });
    }

    // agree with the proposal identified by @id.
    void agree(const std::string& id) {
        vote(id, true);
    }

    // disagree with the proposal identified by @id.
    void disagree(const std::string& id) {
        vote(id, false);
    }

private:
    // vote for a proposal
    void vote(const std::string& id, bool agreed) {
        // proposal id check
        check_proposal_id(id);
        cosio::cosio_assert(proposals.has(id), "unknown proposal id");
        // only block producers can vote
        cosio::name voter = check_bp_caller();
        // duplicate vote check
        std::string vote_id = id + ":" + voter.string();
        cosio::cosio_assert(!votes.has(vote_id), "duplicate votes not allowed");
        // insert a new vote record
        votes.insert([&](vote_record& r) {
            r.vote_id = vote_id;
            r.id = id;
            r.voter = voter;
            r.agreed = agreed? "yes" : "no";
            r.voted_at = cosio::current_timestamp();
        });
    }

    // returns the contract caller if the caller is one of our block producers.
    // otherwise, abort.
    cosio::name check_bp_caller() {
        cosio::name caller = cosio::get_contract_caller();
        auto producers = cosio::block_producers();
        cosio::cosio_assert(
            std::find(producers.begin(), producers.end(), caller.string()) != producers.end(),
            "block producers only"
        );
        return caller;
    }

    // checks if the given proposal id is valid.
    void check_proposal_id(const std::string& id) {
        cosio::cosio_assert(
            id.size() > 0 && id.size() < 128 &&
            std::all_of(id.begin(), id.end(), [](char c) { 
                return c == '_' || (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
            }),
            "invalid proposal id"
        );
    }

private:
    // database table for proposals
    COSIO_DEFINE_TABLE( proposals, proposal_record, (id)(proposer) );

    // database table for votes
    COSIO_DEFINE_TABLE( votes, vote_record, (vote_id) );
};

// contract methods declaration
COSIO_ABI(proposal, (propose)(agree)(disagree))
