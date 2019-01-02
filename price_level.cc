#include "price_level.h"

void PriceLevel::serialize_to_protobuf(redepth::PriceLevel& pb) {
    pb.set_price(price_.str(16));
    pb.set_amount(amount_.str(16));
    pb.set_order_count(order_count_);
}

void PriceLevel::set(Decimal price, Decimal amount, uint64_t count) {
    price_ = price;
    amount_ = amount;
    order_count_ = count;
}

void PriceLevel::add_order(Decimal amount) {
    order_count_ += 1;
    amount_ += amount;
}

void PriceLevel::close_order(Decimal amount) {
    order_count_ -= 1;
    amount_ -= amount;
}

void PriceLevel::increase_amount(Decimal amount) {
    amount_ += amount;
}

void PriceLevel::decrease_amount(Decimal amount) {
    amount_ -= amount;
}
