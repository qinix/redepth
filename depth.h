extern "C" {
#include "redismodule.h"
}

#include <vector>
#include <string>
#include <map>
#include <set>
#include <ctime>
#include "decimal.h"
#include "price_level.h"
#include "depth.pb.h"
#include "boost/date_time/posix_time/posix_time.hpp"

class Depth {
public:
    typedef std::map<Decimal, PriceLevelPtr, std::less<Decimal>> BidLevelMap;
    typedef std::map<Decimal, PriceLevelPtr, std::greater<Decimal>> AskLevelMap;

    Depth() {}
    Depth(std::string pbstr) { this->load_from_pb(pbstr); }

    void add(Decimal price, Decimal amount, bool is_bid);
    void fill(Decimal price, Decimal filled_amount, bool is_bid, bool is_fully_filled);
    void close(Decimal price, Decimal amount, bool is_bid);
    void merge(Depth newdepth);
    std::string newchanges(); // returns pb encoded Depth
    std::string to_protobuf(uint64_t limit = 0); // returns pb encoded Depth

private:
    BidLevelMap bids_;
    AskLevelMap asks_;
    std::set<PriceLevelPtr> changes_;
    boost::posix_time::ptime updated_at_ = boost::posix_time::microsec_clock::universal_time();

    void serialize_to_protobuf(redepth::Depth& pb, uint64_t limit = 0);
    void load_from_pb(std::string pbstr);

    PriceLevelPtr at(Decimal price, bool is_bid);
    void update_at(Decimal price, bool is_bid, const std::function<void(PriceLevelPtr)>& block);
    void remove_price_level_on_empty(PriceLevelPtr pl);
};
