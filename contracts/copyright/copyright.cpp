#include <cosiolib/contract.hpp>
#include <cosiolib/print.hpp>

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

};


COSIO_ABI(copyright, (setadmin)(setcopyright)(setcopyrights))
