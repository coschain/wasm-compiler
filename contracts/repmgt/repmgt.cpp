#include <cosiolib/contract.hpp>
#include <cosiolib/print.hpp>

class repmgt : public cosio::contract {
public:
    using cosio::contract::contract;

	void setadmin( const std::string& user ) {
		cosio::reputation_admin(user);
	}

	void setrep( const std::string& name, int32_t reputation, const std::string& memo ) {
		cosio::update_reputation(name, reputation, memo);
	}

	void setreps( const std::vector<std::string>& names, const std::vector<int32_t>& reputations, const std::vector<std::string>& memos ) {
		cosio::update_reputations(names, reputations, memos);
	}

};


COSIO_ABI(repmgt, (setadmin)(setrep)(setreps))
