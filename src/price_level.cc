#include "price_level.h"

void PriceLevel::serialize_to_protobuf(redepth::PriceLevel& pb) {
    pb.set_price(price.str(16));
    pb.set_amount(amount.str(16));
    pb.set_order_count(order_count);
}

void PriceLevel::set(Decimal amount, uint64_t count) {
    this->amount = amount;
    this->order_count = count;
}

void PriceLevel::add_order(Decimal amount) {
    this->order_count += 1;
    this->amount += amount;
}

void PriceLevel::close_order(Decimal amount) {
    this->order_count -= 1;
    this->amount -= amount;
}

void PriceLevel::increase_amount(Decimal amount) {
    this->amount += amount;
}

void PriceLevel::decrease_amount(Decimal amount) {
    this->amount -= amount;
}
