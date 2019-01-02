#include "decimal.h"
#include "depth.pb.h"
#include <vector>
#include <string>

struct PriceLevel {
public:
    PriceLevel(Decimal price, bool is_bid) : price_(price), is_bid_(is_bid) {}
    void serialize_to_protobuf(redepth::PriceLevel& pb);
    void set(Decimal price, Decimal amount, uint64_t count);
    void add_order(Decimal amount);
    void close_order(Decimal amount);
    void increase_amount(Decimal amount);
    void decrease_amount(Decimal amount);

private:
    Decimal price_, amount_;
    uint64_t order_count_ = 0;
    bool is_bid_;
};

typedef std::shared_ptr<PriceLevel> PriceLevelPtr;
