#include "decimal.h"
#include "depth.pb.h"
#include <vector>
#include <string>

struct PriceLevel {
    PriceLevel(Decimal price, bool is_bid) : price(price), is_bid(is_bid) {}
    void serialize_to_protobuf(redepth::PriceLevel& pb);
    void set(Decimal amount, uint64_t count);
    void add_order(Decimal amount);
    void close_order(Decimal amount);
    void increase_amount(Decimal amount);
    void decrease_amount(Decimal amount);
    Decimal price, amount;
    uint64_t order_count = 0;
    bool is_bid;
};

typedef std::shared_ptr<PriceLevel> PriceLevelPtr;
